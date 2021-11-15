//
// Created by mxmdb on 21. 5. 3..
//
#include "include/Connection/mysql_connector.h"
#include "include/Connection/mongodb_connector.h"
#include "include/Connection/ScidbConnection.h"
#include "include/Connection/neo4j_connector.h"


#include <nlohmann/json.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <tuple>
#include <string>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using Json = nlohmann::json;

#define SCIDB_HOST_ECOMMERCE    "192.168.0.1"

template<typename T>
bool cmp(const pair<T, double> &a, const pair<T, double> &b) {
    return a.second > b.second;
};

#define BUFFER 1000

/**
 *  [Task1] Sales Performance ([R, D]=>R).
 *  Find the Top_selling brand in last year
 *  and
 *  Compute the ratio of total revenue to revenue by item class for the Top_selling brand.
 *
  A: SELECT order_line.product_id , SUM(order_line.price) AS order_price
        FROM Order  // Relational
        UNNEST order_line
        WHERE order_date = current_date - 1 year
        GROUP BY order_line.product_id // Document

    B: SELECT brand_id, Product.product_id as product_id, title as product_name, order_price
        FROM A, Product // Relational
        WHERE Product.product_id = A.product_id

    C:  SELECT B.brand_id AS top_brand, SUM(B.order_price) AS revenue
        From B
        GROUP BY B.brand_id
        ORDER BY revenue DESC LIMIT 1 // Relational

    D: SELECT brand_name, product_name,  (B.order_price/C.revenue)*100 AS percent_of_revenue
        FROM  Brand, B, C
        WHERE
            Brand.brand_id = B.brand_id
            and B.brand_id = C.top_brand
        ORDER By percent_of_revenue // Relational
 */
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;
using std::chrono::microseconds;

void T1(string curdate) {
    auto t1 = high_resolution_clock::now();


    auto mongodb = mongodb_connector("Ecommerce");
    auto orders = mongodb.db["Order"];

    auto mysql = mysql_connector();
    auto Products = mysql.mysess->getSchema("Ecommerce").getTable("Product");

    mysql.mysess->sql("use Ecommerce").execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE Brand_revenue("
                      "brand_id INT,"
                      "product_id CHAR(10),"
                      "product_name VARCHAR(1000),"
                      "order_price double)").execute();


    int a, b, c;
    char buf[11];
    sscanf(curdate.c_str(), "%d-%d-%d", &a, &b, &c);
    sprintf(buf, "%d-%02d-%02d", a-1, b, c);
    string OneYearAgo = buf;

    cout << OneYearAgo << "~" << curdate << endl;


    mongocxx::pipeline p{};
    p.unwind("$order_line");
    p.match(make_document(kvp("order_date", make_document(kvp("$lte",curdate),kvp("$gte", OneYearAgo)))));
    p.group(make_document(kvp("_id", "$order_line.product_id"),
                          kvp("order_price", make_document(kvp("$sum", "$order_line.price")))));
    p.project(make_document(kvp("product_id", "$_id"),
                            kvp("order_price", "$order_price")));

    //    p.coll_stats(make_document(kvp("latencyStats", make_document(kvp("histograms", true)))));


    auto cursor = orders.aggregate(p, mongocxx::options::aggregate{});


    int buffer = 0;

    auto insert2brand_revenue = mysql.mysess->getSchema("Ecommerce")
            .getTable("Brand_revenue")
            .insert("brand_id", "product_id", "product_name", "order_price");


    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<microseconds>(t2 - t1);
    cout <<  ms_int.count() <<  endl;

    auto t3 = high_resolution_clock::now();

    int c1=0, c2=0, c3=0, c4=0, c5=0;
    int line = 0;


    for (auto order_json : cursor) {
        line ++;
        auto t5 = high_resolution_clock::now();
        auto order = Json::parse(bsoncxx::to_json(order_json));
        auto pid = order["product_id"].get<std::string>();
        auto t6 = high_resolution_clock::now();
        c1 += duration_cast<microseconds>(t6 - t5).count();


        auto t7 = high_resolution_clock::now();
        auto res = Products.select("brand_id", "title as product_name").where(
                "product_id = \'" + pid + "\'").execute();

        auto t8 = high_resolution_clock::now();
//        cout << duration_cast<microseconds>(t8 - t7).count() << "\t" << pid  << endl;
        c2 += duration_cast<microseconds>(t8 - t7).count();

        auto t9 = high_resolution_clock::now();

        for (auto brand_id_and_product : res) {

            auto t11 = high_resolution_clock::now();
            insert2brand_revenue.values(brand_id_and_product[0].get<int>(),
                                        order["product_id"].get<std::string>(),
                                        brand_id_and_product[1].get<std::string>(),
                                        order["order_price"].get<double>());


            buffer++;

            if (buffer >= BUFFER) {
                insert2brand_revenue.execute();
                insert2brand_revenue = mysql.mysess->getSchema("Ecommerce").getTable("Brand_revenue").insert("brand_id",
                                                                                                             "product_id",
                                                                                                             "product_name",
                                                                                                             "order_price");
                buffer = 0;
            }

            auto t12 = high_resolution_clock::now();
            c4 += duration_cast<microseconds>(t12 - t11).count();
        }
        auto t10 = high_resolution_clock::now();
        c3 += duration_cast<microseconds>(t10 - t9).count();

    }
    auto t4 = high_resolution_clock::now();
    auto ms_int2 = duration_cast<microseconds>(t4 - t3);


    cout <<  ms_int2.count() <<  endl;

    auto t11 = high_resolution_clock::now();

    if (buffer != 0) insert2brand_revenue.execute();

    auto t12 = high_resolution_clock::now();
    c4 += duration_cast<microseconds>(t12 - t11).count();


    cout << line << "\t cursor:" << ms_int2.count()- (c1+c2+c3) <<"\t parsing" << c1 << "\t selection:" << c2 << "\t insertion+fetch:" << c3 << "\tinsertion:" << c4 << "\t" << endl;


    auto t15 = high_resolution_clock::now();

    auto brand_revenue = mysql.mysess->getSchema("Ecommerce").getTable("Brand_revenue");
    auto top_brand = brand_revenue
            .select("brand_id as Top_Brand", "sum(order_price) as revenue")
            .groupBy("brand_id")
            .orderBy("revenue desc")
            .limit(1)
            .execute();


    auto top_brand_values = top_brand.fetchOne();


    int top_brand_id = top_brand_values[0].get<int>();
    double revenue = top_brand_values[1].get<double>();

    auto t16 = high_resolution_clock::now();
    cout << duration_cast<microseconds>(t16 - t15).count() << endl;

    auto t17 = high_resolution_clock::now();
    std::vector<pair<std::string, double>> RevenuePerProduct;
    std::string query =
            "select name as brand_name, product_name, (Brand_revenue.order_price*100/" + to_string(revenue) +
            ") as percent_of_revenue "
            "from Brand, Brand_revenue "
            "where Brand.brand_id = Brand_revenue.brand_id "
            "and Brand.brand_id=" + to_string(top_brand_id) + " "
                                                              "order by  percent_of_revenue DESC";

    auto res = mysql.mysess->sql(query).execute();

    auto t18 = high_resolution_clock::now();
    cout << duration_cast<microseconds>(t18 - t17).count() << endl;

    int nrows = 0;
    for (auto row : res) {
//        cout << row[0].get<std::string>()+","+row[1].get<std::string>()+","+ to_string(row[2].get<double>()) <<"%" << endl;
        nrows++;
    }

    cout << "[TASK1]: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
}


/**
 *  [Task2] Product Recommendation ([D, A]=>A).
 *  Perform product recommendation using "Factorization" based on the past customer ratings.
 *
 *      A: SELECT Order.cid AS cid, Review.pid AS pid, Review.rating AS rating
 *          FROM Review, Order // Document
 *          WHERE Review.oid=Order.oid // Relational
 *
 *      B, C: A.toArray(<val(=A.rating)>[cid,pid]).Factorization // 2 Arrays
 *
 *      D: B(<val: latent_factor>[cid,k]) * C(<val: latent_factor>[k,pid]) // Array•E: SELECT pid FROM D WHERE cid=‘x’ AND val > 4 // Relational
 *
 */
void T2() {
    auto mongodb = mongodb_connector("Ecommerce");
    auto orders = mongodb.db["Order"];
    auto reviews = mongodb.db["Review"];


    auto mysql = mysql_connector();
    mysql.mysess->sql("use Ecommerce").execute();
    mysql.mysess->sql("CREATE TEMPORARY TABLE Rating_history("
                      "customer_id varchar(20),"
                      "product_id CHAR(10),"
                      "rating int)").execute();

    auto Rating_history = mysql.mysess->getSchema("Ecommerce").getTable("Rating_history").insert("customer_id",
                                                                                                 "product_id",
                                                                                                 "rating");
    int buffer = 0;

//    auto cursor = orders.find({},
//                              mongocxx::options::find{}.projection(
//                                      make_document(
//                                              kvp("order_id", 1),
//                                              kvp("customer_id", 1),
//                                              kvp("_id", 0))));
//    for (auto order : cursor) {
//        bsoncxx::document::element stored_element1{order["order_id"]};
//        bsoncxx::document::element stored_element2{order["customer_id"]};
//        if (stored_element1 && stored_element2) {
//            stdx::string_view view = order["order_id"].get_utf8().value;
//            std::string order_id = view.to_string();
//            stdx::string_view view2 = order["customer_id"].get_utf8().value;
//            std::string customer_id = view2.to_string();
//
//            auto cursor2 = reviews.find(
//                    make_document(kvp("order_id", order_id)),
//                    mongocxx::options::find{}.projection(
//                            make_document(
//                                    kvp("rating", 1),
//                                    kvp("product_id", 1),
//                                    kvp("_id", 0))));
//
//
//            for (auto review : cursor2) {
//                bsoncxx::document::element stored_element3{review["rating"]};
//                bsoncxx::document::element stored_element4{review["product_id"]};
//                int rating = stored_element3.get_int32();
//                std::string product_id = stored_element4.get_utf8().value.to_string();
//                Rating_history.values(customer_id, product_id, rating);
//                buffer++;
//                if (buffer >= BUFFER) {
//                    Rating_history.execute();
//                    Rating_history =
//                            mysql.mysess->getSchema("Ecommerce").getTable("Rating_history")
//                                    .insert("customer_id", "product_id", "rating");
//                    buffer = 0;
//                }
//            }
//
//        }
//    }
//    if (buffer != 0) Rating_history.execute();
    mongocxx::pipeline p{};
    p.lookup( make_document(
            kvp("from", "Order"),
            kvp("localField", "order_id"),
            kvp("foreignField", "order_id"),
            kvp("as", "res")
    ));
    p.group(make_document(
            kvp("_id",  make_document(
                         kvp("customer_id","$res.customer_id"),
                         kvp("product_id","$product_id")
                         )
             ),
            kvp( "val", make_document(
                        kvp( "$avg", "$rating")))
    ));

    auto cursor = reviews.aggregate(p, mongocxx::options::aggregate{});

    for (auto history : cursor){
                bsoncxx::document::element stored_element1{history["_id"]["customer_id"]};
                bsoncxx::document::element stored_element2{history["_id"]["product_id"]};
                bsoncxx::document::element stored_element3{history["val"]};

                int rating = stored_element3.get_double();
                std::string product_id = stored_element2.get_utf8().value.to_string();
                auto customer_id =  stored_element1.get_array().value[0].get_utf8().value.to_string();
                Rating_history.values(customer_id, product_id, rating);
                buffer++;
                if (buffer >= BUFFER) {
                    Rating_history.execute();
                    Rating_history =
                            mysql.mysess->getSchema("Ecommerce").getTable("Rating_history")
                                    .insert("customer_id", "product_id", "rating");
                    buffer = 0;
                }
    }
    if (buffer != 0) Rating_history.execute();


    mysql.mysess->sql( "CREATE INDEX Rating_history_idx1 on Rating_history(customer_id)" ).execute();
    mysql.mysess->sql( "CREATE INDEX Rating_history_idx2 on Rating_history(product_id)" ).execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE Rcustomer as "
                      "(SELECT t.customer_id, ROW_NUMBER() OVER () -1 as customer_id_d from "
                      "(Select distinct(customer_id) as customer_id "
                      "from Rating_history) as t )").execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE Rproduct as "
                      "(SELECT t.product_id, ROW_NUMBER() OVER () -1 as product_id_d from "
                      "(Select distinct(product_id) as product_id "
                      "from Rating_history) as t )").execute();



    mysql.mysess->sql("CREATE INDEX Rcustomer_idx on Rcustomer(customer_id)").execute();
    mysql.mysess->sql("CREATE INDEX Rproduct_idx on Rproduct(product_id)").execute();


    auto rows = mysql.mysess->sql("SELECT customer_id_d as person, product_id_d as product, rating "
                                  "From Rcustomer, Rproduct, Rating_history "
                                  "Where Rating_history.customer_id = Rcustomer.customer_id "
                                  "and Rating_history.product_id = Rproduct.product_id").execute();


    int nrows = 0;

    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST_ECOMMERCE + ":8080"));


    int dim1 = mysql.mysess->getSchema("Ecommerce").getTable("Rcustomer").count();
    int dim2 = mysql.mysess->getSchema("Ecommerce").getTable("Rproduct").count();

    // execute array

    cout << dim1 << " " <<  dim2 << endl;
//    conn->exec("create array coo<rating:double, person:int64, product:int64> [i=0:"+to_string(dim1*dim2)+":0:1000000]");

    conn->exec("remove(temp)");
    conn->exec("create array temp<val:double, x:int32, y:int32> [i=0:" + to_string(dim1 * dim2 - 1) +
               ":0:100000000]");
    int ncell = 0;

    ScidbSchema schema;
    schema.attrs.push_back(ScidbAttr("val", DOUBLE));
    schema.attrs.push_back(ScidbAttr("x", INT32));
    schema.attrs.push_back(ScidbAttr("y", INT32));

    shared_ptr<ScidbArrFile> coo(new ScidbArrFile(schema));

    for (auto row : rows) {
        int customer_id = row[0].get<int>();
        int product_id = row[1].get<int>();
        double rating = (double) row[2].get<int>();
        /***
         *
         * Pass to SCIDB
         *
         */
        ScidbLineType line;
        line.push_back(rating);
        line.push_back(customer_id);
        line.push_back(product_id);

        coo->add(line);
        ncell++;
    }
    cout << ncell << endl;

   // upload array. Note that SciDB only accepts 1-D array.
    cout << "upload" << endl;
    conn->upload("temp", coo);

    conn->exec("remove(V)");

    conn->exec("store(redimension(temp, <val:double>[x=0:" + to_string(dim1 - 1) +
               ":0:1000; y=0:" + to_string(dim2 - 1) + ":0:1000], false),  V)");



    /**
     *  INIT W H
     */

    conn->exec("load_library('dense_linear_algebra')");

    conn->exec("remove(initW)");
    conn->exec("remove(W)");

    int feature_size = 50;
    string create_W = "store(build(<val:double>[i=0:"+to_string(dim1 - 1)+":0:1000; j=0:"+to_string(feature_size - 1)+":0:1000], random()/2147483647.0), initW)";
    conn->exec(create_W);
    cout << create_W << endl;

    conn->exec("remove(initH)");
    conn->exec("remove(H)");
    string create_H = "store(build(<val:double>[i=0:"+to_string(feature_size - 1)+":0:1000; j=0:"+to_string(dim2 - 1)+":0:1000], random()/2147483647.0), initH)";
    conn->exec(create_H);
    cout << create_H << endl;

    string newH = "store(initH, H)";
    string newW = "store(initW, W)";
    conn->exec(newH);
    conn->exec(newW);

    /**
     *  ZERO MATRRIX
     */
    conn->exec("remove(zeroV)");
    string zeroV = "create array zeroV<val:double> [i=0:"+to_string(dim1 - 1)+":0:1000; j=0:"+to_string(dim2 - 1)+":0:1000]";
    conn->exec(zeroV);

    conn->exec("remove(zeroWtV)");
    string zeroWtV = "create array zeroWtV<val:double> [i=0:"+to_string(feature_size - 1)+":0:1000; j=0:"+to_string(dim2 - 1)+":0:1000]";
    conn->exec(zeroWtV);

    conn->exec("remove(zeroWtW)");
    string zeroWtW = "create array zeroWtW<val:double> [i=0:"+to_string(feature_size - 1)+":0:1000; j=0:"+to_string(feature_size - 1)+":0:1000]";
    conn->exec(zeroWtW);

    conn->exec("remove(zeroFH)");
    string zeroFH = "create array zeroFH<val:double> [i=0:"+to_string(feature_size - 1)+":0:1000; j=0:"+to_string(dim2 - 1)+":0:1000]";
    conn->exec(zeroFH);



    conn->exec("remove(zeroVHt)");
    string zeroVHt = "create array zeroVHt<val:double> [i=0:"+to_string(dim1 - 1)+":0:1000; j=0:"+to_string(feature_size - 1)+":0:1000]";
    conn->exec(zeroVHt);

    conn->exec("remove(zeroHHt)");
    string zeroHHt = "create array zeroHHt<val:double> [i=0:"+to_string(feature_size - 1)+":0:1000; j=0:"+to_string(feature_size - 1)+":0:1000]";
    conn->exec(zeroHHt);

    conn->exec("remove(zeroWF)");
    string zeroWF = "create array zeroWF<val:double> [i=0:"+to_string(dim1 - 1)+":0:1000; j=0:"+to_string(feature_size - 1)+":0:1000]";
    conn->exec(zeroWF);


    conn->exec("remove(WtV)");
    conn->exec("remove(WtWH)");
    conn->exec("remove(Hdiv)");
    conn->exec("remove(factorH)");
    conn->exec("remove(Hmul)");

    conn->exec("remove(VHt)");
    conn->exec("remove(WHHt)");
    conn->exec("remove(Wdiv)");
    conn->exec("remove(factorW)");
    conn->exec("remove(Wmul)");

    string WtV = "store( gemm(W,V,zeroWtV, transa: True), WtV)";
    string WtWH = "store( gemm(gemm(W,W,zeroWtW, transa: True),  H, zeroFH), WtWH)";
    string Hdiv =  "store( join( WtV , WtWH ), Hdiv)";
    string factorH =  "store( project( apply( Hdiv, div , gemm/gemm_2 ), div), factorH)";
    string Hmul =  "store( join( H, factorH), Hmul)";
    string renewH =  "store( project( apply( Hmul, newh , val*div), newh), H)";


    string VHt = "store(gemm(V,H,zeroVHt,transb:true), VHt)";
    string WHHt = "store(gemm(W, gemm(H,H,zeroHHt,transb:true), zeroWF),WHHt)";
    string Wdiv = "store(join(VHt, WHHt), Wdiv)";
    string factorW = "store(project( apply( Wdiv, div, gemm/gemm_2), div), factorW)";
    string Wmul = "store(join( W, factorW), Wmul)";
    string renewW = "store(project( apply( Wmul, neww, val*div), neww), W)";

    string newV = "store(join(gemm(W,H, zeroV), V), newV)";
    string agg = "store(aggregate( apply(newV, diff, abs(gemm-val)), sum(diff)), L)";

    for ( int iter = 0; iter < 1 ; iter++){
        conn->exec(WtV);
        conn->exec(WtWH);
        conn->exec(Hdiv);
        conn->exec(factorH);
        conn->exec(Hmul);
        conn->exec(renewH);

        conn->exec(VHt);
        conn->exec(WHHt);
        conn->exec(Wdiv);
        conn->exec(factorW);
        conn->exec(Wmul);
        conn->exec(Hmul);
        conn->exec(renewW);

//        conn->exec(newV);
//        conn->exec(agg);

//        auto result = conn->download("W");
//        auto cell = result->readcell();
//        while( cell.size()!= 0){
//            nrows++;
//            cell = result->readcell();
//        }
    }

//    auto result = conn->download("V");


    cout << "[TASK2]: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
}

void scidb_test() {
    unique_ptr<ScidbConnection> conn(new ScidbConnection(""));

    conn->exec("load_library(\'dense_linear_algebra\')");
    conn->exec("CREATE ARRAY A <val:double>[i=0:9; j=0:9]");
    conn->exec("store(build (A, i), A)");
    conn->exec("store(build(<val:double>[i=0:9; j=0:9],iif(i=j,1,0)),B)");
    conn->exec("CREATE ARRAY C <val:double>[i=0:9; j=0:9]");
    conn->exec("store(build (C, 1), C)");
    conn->exec("gemm(A,B,C)");

}

/**
 *  [Task3] Product Purchase Propensities ([R, D, G]=>R).
 *  Given a certain special day, find the customer who spent the highest amount of money in orders, 
 *  and analyze the purchase propensities of people within 3-hop relationships.
 *
 *      A: SELECT cid, SUM(total_price) AS order_price FROM Order
 *         WHERE  order_date = 2021/12/25
 *         GROUP BY cid ORDER BY order_price DESC LIMIT 1 // Document
 *
 *      B: SELECT SNS.p2.person_id AS 2hop_cid FROM A, SNS
 *         WHERE (p1:Person) - [r:FOLLOWS*2] - > (p2:Person) AND SNS.p2.person_id=A.cid // Relational
 * 
 *      C: SELECT Order.cid AS cid, Order.order_line.pid AS pid, Product.brand_id AS brand_id FROM  B, Order, Product
 *         UNNEST Order.order_line WHERE Order.cid=B.2hop_cid AND Product.pid=Order.order_line.pid // Document
 * 
 *      D: SELECT Brand.industry, COUNT(*) AS customer_count FROM C, Brand WHERE C.brand_id=Brand.brand_id GROUP BY Brand.industry // Relational
 *
 */


void T3(string curdate) {
    auto mongodb = mongodb_connector("Ecommerce");
        auto orders = mongodb.db["Order"];

        auto mysql = mysql_connector();
        auto Products = mysql.mysess->getSchema("Ecommerce").getTable("Product");
        auto Brands = mysql.mysess->getSchema("Ecommerce").getTable("Brand");

        std::vector<Json> A;

        mysql.mysess->sql("use Ecommerce").execute();

        mysql.mysess->sql("CREATE TEMPORARY TABLE customerId("
                      "customer_id VARCHAR(1000),"
                      "product_id VARCHAR(1000),"
                      "price FLOAT)").execute();

        auto insert2customerId =mysql.mysess->getSchema("Ecommerce")
                .getTable("customerId")
                .insert("customer_id","product_id","price");
        int iterator=0;

        mongocxx::pipeline p{};
        p.match(make_document(kvp("order_date",curdate)));
        p.group(make_document(kvp("_id","$customer_id"),kvp("order_price", make_document(kvp("$sum", "$total_price")))));
        p.project(make_document(kvp("cid","$_id"),kvp("order_price","$order_price")));
        p.sort(make_document(kvp("order_price",-1))).limit(1);
        auto cursor = orders.aggregate(p, mongocxx::options::aggregate{});
        int norder = 0;
        for (auto order : cursor) {
            auto json = Json::parse(bsoncxx::to_json(order));
            A.push_back(json);
        }
        std::string cid = A[0]["cid"];
        auto query1 = "select distinct person_id from "
                      "Customer "
                      "where Customer.customer_id = \"" + cid + "\"";
        auto res1 = mysql.mysess->sql(query1).execute();

        auto neo4j = new neo4j_connector();

        std::string pmatch = "(p:Person)-[:follows*2]->(m:Person)";
        std::string preturn = "p.person_id";
        std::string pwhere = "m.person_id=\"";
        
        mysql.mysess->sql("use Ecommerce").execute();

        mysql.mysess->sql("CREATE TEMPORARY TABLE 2hop_pid("
                      "pid VARCHAR(1000))").execute();

        auto insert2_2hop_pid =mysql.mysess->getSchema("Ecommerce")
            .getTable("2hop_pid")
            .insert("pid");
        int buff = 0;
        for (auto order:res1){
            std::string query_str = ("MATCH " + pmatch + " WHERE " + pwhere + to_string(order[0].get<int>())+ "\" RETURN " + preturn);
            auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
            char buffer[5000];
            while (1) {
                neo4j_result_t *result = neo4j_fetch_next(results);
                if (result == NULL) break;
                int pos = 0;
                while (1) {
                    auto n = neo4j_result_field(result, pos);
                    if (!neo4j_is_null(n)) {
                        std::string str = neo4j_tostring(n, buffer, sizeof(buffer));
                        str = str.substr(1,str.length()-2);
                        insert2_2hop_pid.values(str);
                        buff++;
                    } else {
                        break;
                    }
                    pos++;
                }if( buff>=BUFFER){
                    insert2_2hop_pid.execute();
                    insert2_2hop_pid = mysql.mysess->getSchema("Ecommerce").getTable("2hop_pid").insert("pid");
                    buff=0;
                }
            }
        }
        if( buff !=0 ) insert2_2hop_pid.execute();
        mysql.mysess->sql("use Ecommerce").execute();
        auto query2 = "select Customer.customer_id "
                      "from Customer,2hop_pid "
                      "where Customer.person_id = 2hop_pid.pid ";
        auto hop_cid = mysql.mysess->sql(query2).execute();
        mysql.mysess->sql("CREATE TEMPORARY TABLE hop_pid("
                        "customer_id VARCHAR(1000),"
                        "product_id VARCHAR(1000))").execute();
        auto insert2hoppid =mysql.mysess->getSchema("Ecommerce")
                .getTable("hop_pid")
                .insert("customer_id","product_id");

        for ( auto row : hop_cid){
            mongocxx::pipeline pp{};
            pp.match(make_document(kvp("customer_id", row[0].get<std::string>())));
            pp.unwind("$order_line");
            pp.project(make_document(kvp("cid","$customer_id"),kvp("pid","$order_line.product_id")));
            auto cursor1 = orders.aggregate(pp, mongocxx::options::aggregate{});
            int norder = 0;
            buff = 0;
            std::string pid;
            std:: string cid;
            for (auto order : cursor1) {
                auto json = Json::parse(bsoncxx::to_json(order));
                pid = json["pid"];
                cid = json["cid"];
                insert2hoppid.values(cid,pid);
                buff++;
            }
            if( buff>=BUFFER){
                insert2hoppid.execute();
                insert2hoppid = mysql.mysess->getSchema("Ecommerce")
                        .getTable("hop_pid")
                        .insert("customer_id","product_id");
                buff=0;}
        }
        if( buff !=0 ) insert2hoppid.execute();
        mysql.mysess->sql("use Ecommerce").execute();
        mysql.mysess->sql("CREATE TEMPORARY TABLE BrandId("
                      "cid CHAR(20),"
                      "pid CHAR(10),"
                      "brand_id INT,"
                      "industry VARCHAR(1000))").execute();

        auto insert2BrandId =mysql.mysess->getSchema("Ecommerce")
            .getTable("BrandId")
            .insert("cid","pid","brand_id","industry");
        auto res2 = mysql.mysess->sql("select customer_id, product_id "
                                    "from hop_pid ").execute();

        buff = 0;

        for (auto prod_id : res2){
            auto brand_id = Products.select("brand_id").where("product_id=\""+prod_id[1].get<std::string>()+"\"").execute();
            for (auto bid : brand_id){
                auto industry = Brands.select("industry").where("brand_id= "+to_string(bid[0].get<int>())).execute();
                for (auto ind : industry){
                    insert2BrandId.values(prod_id[0].get<std::string>(),prod_id[1].get<std::string>(),bid[0].get<int>(),ind[0].get<std::string>());
                    buff++;
                    if( buff>=BUFFER){
                        insert2BrandId.execute();
                        insert2BrandId =mysql.mysess->getSchema("Ecommerce").getTable("BrandId").insert("cid","pid","brand_id","industry");
                        buff=0;
                    }
                }
            }
        }
        if( buff !=0 ) insert2BrandId.execute();

        std::string query = "select industry, count(cid) as customer_count "
                            "from BrandId "
                         "group by industry ";

        auto res = mysql.mysess->sql(query).execute();

        int nrows = 0;
        for(auto row : res) {
          cout << row[0].get<std::string>()+": "+ to_string(row[1].get<int>())<<endl;
            nrows++;
        }
            cout << "TASK3: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
}


/**
 *  [Task4] Customer Interests ([R, D, G]=>R).
 *  Find the interests of top-N famous customers who made more than a certain amount of orders in a given product category.
 *
 *      A: SELECT DISTINCT Order.cid AS cid , SUM(Order.Order_line.price) as total_spent
 *         FROM Product, Order, Brand UNNEST Order.order_line
 *         WHERE Product.pid=Order.order_line.pid AND Brand.brand_id=Product.brand_id AND Brand.industry = @param
 *         GROUP BY cid
 *         HAVING total_spent > @param// Document
 *
 *      B: SELECT SNS.influencer.person_id AS person_id, COUNT(SNS.n) AS followers FROM A, SNS
 *         WHERE (n:Person)  - [r:FOLLOWS] - > (influencer:Person) AND SNS.influencer.person_id=A.cid ORDER BY followers DESC LIMIT N = @param // Relational
 *
 *      C: SELECT SNS.t.tid FROM B, SNS  WHERE (p:Person)  - [r:HAS_INTEREST] - > (t:Tag) AND SNS.p.person_id=B.person_id // Relational
 *
 *      @param industry
 *      @param N
 *      @param min_spent
 */


void T4(int min_spent, int N) {
    auto mongodb = mongodb_connector("Ecommerce");
    auto orders = mongodb.db["Order"];

    auto mysql = mysql_connector();
    auto Products = mysql.mysess->getSchema("Ecommerce").getTable("Product");
    auto Brands = mysql.mysess->getSchema("Ecommerce").getTable("Brand");
    auto Customers = mysql.mysess->getSchema("Ecommerce").getTable("Customer");
    auto neo4j = new neo4j_connector();

    mysql.mysess->sql("use Ecommerce").execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE customerId("
                      "customer_id VARCHAR(1000),"
                      "product_id VARCHAR(1000),"
                      "price FLOAT)").execute();


    std::string query1 = "select Product.product_id "
                         "from Product, Brand "
                         "where Brand.industry = \"Leisure\" "
                         "and Product.brand_id = Brand.brand_id";

    auto res1 = mysql.mysess->sql(query1).execute();

    auto insert2customerId =mysql.mysess->getSchema("Ecommerce")
            .getTable("customerId")
            .insert("customer_id","product_id","price");
    int iterator=0;
    for (auto opid: res1){
        mongocxx::pipeline p{};
        p.match(make_document(kvp("order_line.product_id",opid[0].get<std::string>())));
        p.unwind("$order_line");
        p.match(make_document(kvp("order_line.product_id",opid[0].get<std::string>())));
        p.project(make_document(kvp("customer_id",1),kvp("order_line.product_id",1),kvp("order_line.price",1)));

        auto cursor = orders.aggregate(p, mongocxx::options::aggregate{});

        std::string cid ;
        std::string pid ;

        for (auto order : cursor) {
            auto json = Json::parse(bsoncxx::to_json(order));
            cid = json["customer_id"];
            pid = json["order_line"]["product_id"];
            float pid_price = json["order_line"]["price"];
            insert2customerId.values(cid,pid,pid_price);
            iterator++;
        }
        if( iterator>=BUFFER){
            insert2customerId.execute();
            insert2customerId = mysql.mysess->getSchema("Ecommerce")
                    .getTable("customerId")
                    .insert("customer_id","product_id","price");
            iterator=0;
        }
    }

    if( iterator !=0 ) insert2customerId.execute();

    auto query2 = "select customer_id, "
                  "Sum(price) as total_spent "
                  "from customerId "
                  "group by customer_id "
                  "having total_spent> " + to_string(min_spent);

    auto res2 = mysql.mysess->sql(query2).execute();

    std::string pmatch = "(p:Person)-[r]->(m:Person)";
    std::string preturn = "count(p) as follower";
    std::string pwhere = "m.person_id=\"";

    mysql.mysess->sql("use Ecommerce").execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE followers("
                      "personid VARCHAR(1000),"
                      "follower INT )").execute();

    auto insert2followers =mysql.mysess->getSchema("Ecommerce")
            .getTable("followers")
            .insert("personid","follower");

    int buff = 0;
    for (auto tcid: res2){
        auto person_id = Customers.select("person_id").where("customer_id=\""+tcid[0].get<std::string>()+"\"").execute();
        for (auto personId: person_id){
            std::string query_str = ("MATCH " + pmatch + " WHERE " + pwhere + to_string(personId[0].get<int>()) + "\" RETURN " + preturn);

            auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
            char buffer[5000];
            while (1) {
                neo4j_result_t *result = neo4j_fetch_next(results);
                if (result == NULL) break;
                int pos = 0;
                while (1) {
                    auto n = neo4j_result_field(result, pos);
                    if (!neo4j_is_null(n)) {
                        std::string str = neo4j_tostring(n, buffer, sizeof(buffer));
                        insert2followers.values(personId[0],std::stoi(str));
                        buff++;
                    } else {
                        break;
                    }
                    pos++;
                }
                if( buff>=BUFFER){
                    insert2followers.execute();
                    insert2followers = mysql.mysess->getSchema("Ecommerce").getTable("followers").insert("personid","follower");
                    buff=0;
                }
            }
        }
        if( buff !=0 ) insert2followers.execute();
    }
    std::string query = "select distinct personid ,follower "
                        "from followers "
                        "order by follower DESC "
                        "LIMIT "+ to_string(N);
    auto B = mysql.mysess->sql(query).execute();
    mysql.mysess->sql("use Ecommerce").execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE interest("
                      "personid VARCHAR(1000),"
                      "interest_tag VARCHAR(1000))").execute();

    auto insert2interest =mysql.mysess->getSchema("Ecommerce")
            .getTable("interest")
            .insert("personid","interest_tag");

    std::string tmatch = "(p:Person)-[:interested_in]->(t:Shopping_Hashtag)";
    std::string treturn = "t.tag_id";
    std::string twhere = "p.person_id=\"";

    auto follower = mysql.mysess->getSchema("Ecommerce").getTable("followers");
    for (auto b:B){
        //cout<< b[0].get<std::string>() << " followers: " << b[1].get<int>() <<endl;
        std::string query_str = ("MATCH " + tmatch + " WHERE " + twhere + b[0].get<std::string>()+ "\" RETURN " + treturn);
        auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
        char buffer[5000];
        while (1) {
            neo4j_result_t *result = neo4j_fetch_next(results);
            if (result == NULL) break;
            int pos = 0;
            while (1) {
                auto n = neo4j_result_field(result, pos);
                if (!neo4j_is_null(n)) {
                    std::string str = neo4j_tostring(n, buffer, sizeof(buffer));
                    insert2interest.values(b[0].get<std::string>(),str);
                    buff++;
                } else {
                    break;
                }
                pos++;
            }
            if( buff>=BUFFER){
                insert2interest.execute();
                insert2interest = mysql.mysess->getSchema("Ecommerce").getTable("interest").insert("personid","interest_tag");
                buff=0;
            }
        }
        if( buff !=0 ) insert2interest.execute();
    }

    std::string query4 = "select distinct interest_tag "
                         "from interest ";
    auto res = mysql.mysess->sql(query4).execute();

    int nrows = 0;
    for(auto row : res) {
        nrows++;
    }
    cout << "TASK4: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;

}


/**
 * [Task5]. Filtering social network (R, D, G) => Graph
 *  Extract social network whose nodes are woman customers who has bougth the product 'X' within 1 year and wrote the reviews.
 *  A: SELECT Customer.person_id AS cid
 *      FROM Order, Review, Customer
 *      WHERE Review.pid=‘X’ AND Review.order_id=Order.order_id
 *          AND Order.order_date >= current_date – 1 year
 *          AND Order.cid=Customer.cid AND Customer.gender=‘female’ // Relational
 *
 *  B: SELECT p, r, node AS subGraph FROM A, SNS
 *      WHERE (p:Person) - [r] -> (node)
 *      AND SNS.p.person_id=A.person_id // Graph
 */

void T5(string pid, string curdate) {

   int a, b, c;
    char buf[11];
    sscanf(curdate.c_str(), "%d-%d-%d", &a, &b, &c);
    sprintf(buf, "%d-%02d-%02d", a-1, b, c);
    string OneYearAgo = buf;

    cout << OneYearAgo << "~" << curdate << endl;



    auto mysql = mysql_connector();
    mysql.mysess->getSchema("Ecommerce").getTable("Customer");
    auto neo4j = new neo4j_connector();
    auto mongodb = mongodb_connector("Ecommerce");
    auto customer = mysql.mysess->getSchema("Ecommerce").getTable("Customer");

    auto cursor = mongodb.db["Review"].find(
            make_document(kvp("product_id", pid)),
            mongocxx::options::find{}.projection(
                    make_document(kvp("order_id", 1), kvp("_id", 0))));

    int nrows =0;
    char buffer[5000];

    for (auto review : cursor) {
        bsoncxx::document::element stored_element{review["order_id"]};
        stdx::string_view view = stored_element.get_utf8().value;
        std::string order_id = view.to_string();
        auto cursor2 = mongodb.db["Order"].find(
                make_document(kvp("order_id", order_id),
                              kvp("order_date", make_document(kvp("$lte",curdate),kvp("$gte", OneYearAgo)))),
                              mongocxx::options::find{}.projection(
                                      make_document(kvp("customer_id", 1),
                                                    kvp("_id", 0))));

        for (auto order : cursor2) {

            bsoncxx::document::element stored_element{order["customer_id"]};
            stdx::string_view view = stored_element.get_utf8().value;
            std::string customer_id = view.to_string();
            auto rows = customer.select("person_id").where(
                    "customer_id = '" + customer_id + "' and gender = 'F'").execute();

            for (auto customer: rows) {
                auto person_id = customer[0].get<int>();
                std::string pmatch = "p=(n:Person)-[r]->(m:Person)";
                std::string pwhere = "n.person_id=\""+ to_string(person_id)+"\"";
                std::string preturn = "p";
                std::string query_str = ("MATCH " + pmatch + " WHERE " + pwhere + " RETURN " + preturn );
                auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);

                while (1) {
                    neo4j_result_t *result = neo4j_fetch_next(results);
                    if (result == NULL) break;
                    auto n = neo4j_result_field(result, 0);
                    if (!neo4j_is_null(n)) {
                        std::string path = neo4j_tostring(n, buffer, sizeof(buffer));
                    }
                    nrows++;
                }
            }
        }

    }

    cout << "[TASK5]: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
}



