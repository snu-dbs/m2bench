/**
 * Created: May 3, 2021
 * Updated: February 2025
 */

#include <nlohmann/json.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "Connection/mysql_connector.h"
#include "Connection/mongodb_connector.h"
#include "Connection/ScidbConnection.h"

using Json = nlohmann::json;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

#define SCIDB_HOST_ECOMMERCE "127.0.0.1"
#define BUFFER 1000

/**
 *  [Task 0] Building a Logistic Model ([R, D, G, A] => A)
 *  Build a logistic regression model to predict if a user prefers a given brand.
 */
void T0(int brand_id)
{
    auto time_comm = milliseconds(0);
    auto start_comm = high_resolution_clock::now();
    auto end_comm = high_resolution_clock::now();

    auto before_mysql = milliseconds(0);
    auto before_mongo = milliseconds(0);
    auto before_scidb = milliseconds(0);

    // A
    auto start_mysql = high_resolution_clock::now();
    auto mysql = mysql_connector();
    mysql.mysess->sql("USE Ecommerce").execute();
    mysql.mysess->sql("DROP TABLE IF EXISTS TASK_NEW_B2_TEMPTABLE").execute();
    mysql.mysess->sql("CREATE TEMPORARY TABLE TASK_NEW_A_TEMPTABLE AS "
                      "SELECT _from AS person_id, _to AS tag_id "
                      "FROM Interested_in")
        .execute();
    auto end_mysql = high_resolution_clock::now();
    auto time_mysql = duration_cast<milliseconds>(end_mysql - start_mysql);
    cout << "A (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;

    // B
    auto start_mongo = high_resolution_clock::now();
    mongodb_connector mongodb("Ecommerce");
    bool mmjoin_optimized = false;
    int buffer_cnt = 0;
    auto end_mongo = high_resolution_clock::now();
    auto time_mongo = duration_cast<milliseconds>(end_mongo - start_mongo);
    cout << "Connect (MongoDB): " << duration_cast<milliseconds>(end_mongo - start_mongo).count() << " ms" << endl;

    if (mmjoin_optimized)
    {
        // Get pairs of customer_id and product_id where the customer gives the highest rating score.
        start_mongo = high_resolution_clock::now();
        mongocxx::pipeline stages;
        stages.match(make_document(kvp("rating", 5)));
        stages.lookup(make_document(
            kvp("from", "Order"),
            kvp("localField", "order_id"),
            kvp("foreignField", "order_id"),
            kvp("as", "orders")));
        stages.unwind("$orders");
        stages.project(make_document(
            kvp("_id", 0),
            kvp("customer_id", "$orders.customer_id"),
            kvp("product_id", "$product_id")));
        end_mongo = high_resolution_clock::now();
        time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);
        cout << "Get pairs from Order (MongoDB): " << duration_cast<milliseconds>(end_mongo - start_mongo).count() << " ms" << endl;

        start_mysql = high_resolution_clock::now();
        mysql.mysess->sql("USE Ecommerce").execute();
        mysql.mysess->sql("CREATE TEMPORARY TABLE TASK_NEW_B1_TEMPTABLE ("
                          "customer_id CHAR(20), "
                          "product_id CHAR(10))")
            .execute();

        // Transfer MongoDB result to MySQL
        buffer_cnt = 0;
        auto insert_temptbl_b = mysql.mysess->getSchema("Ecommerce")
                                    .getTable("TASK_NEW_B1_TEMPTABLE")
                                    .insert("customer_id", "product_id");
        end_mysql = high_resolution_clock::now();
        time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
        cout << "Create TASK_NEW_B1_TEMPTABLE (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;
        before_mysql = time_mysql;

        start_mongo = high_resolution_clock::now();
        auto cursor = mongodb.db["Review"].aggregate(stages);
        end_mongo = high_resolution_clock::now();
        time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);
        cout << "Aggregate Review (MongoDB): " << duration_cast<milliseconds>(end_mongo - start_mongo).count() << " ms" << endl;
        before_mongo = time_mongo;

        start_comm = high_resolution_clock::now();
        milliseconds time_loop = milliseconds(0);
        for (const auto &doc : cursor)
        {
            start_mongo = high_resolution_clock::now();
            auto json = Json::parse(bsoncxx::to_json(doc));
            std::string customer_id = json["customer_id"].get<std::string>();
            std::string product_id = json["product_id"].get<std::string>();
            end_mongo = high_resolution_clock::now();
            time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);
            time_loop += duration_cast<milliseconds>(end_mongo - start_mongo);

            start_mysql = high_resolution_clock::now();
            insert_temptbl_b.values(customer_id, product_id);
            buffer_cnt++;

            if (buffer_cnt >= BUFFER)
            {
                insert_temptbl_b.execute();
                insert_temptbl_b = mysql.mysess->getSchema("Ecommerce")
                                       .getTable("TASK_NEW_B1_TEMPTABLE")
                                       .insert("customer_id", "product_id");
                buffer_cnt = 0;
            }
            end_mysql = high_resolution_clock::now();
            time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
            time_loop += duration_cast<milliseconds>(end_mysql - start_mysql);
        }
        end_comm = high_resolution_clock::now();
        time_comm = duration_cast<milliseconds>(end_comm - start_comm - time_loop);
        cout << "Insert to TASK_NEW_B1_TEMPTABLE (MongoDB - Parse): " << time_mongo.count() - before_mongo.count() << " ms" << endl;
        cout << "Insert to TASK_NEW_B1_TEMPTABLE (MySQL): " << time_mysql.count() - before_mysql.count() << " ms" << endl;
        cout << "Communication Time: " << time_comm.count() << " ms" << endl;

        start_mysql = high_resolution_clock::now();
        insert_temptbl_b.execute();
        buffer_cnt = 0;

        // Create a non-temporary table for later use in multiple queries
        mysql.mysess->sql("CREATE TABLE TASK_NEW_B2_TEMPTABLE ("
                          "person_id INT, "
                          "brand_id INT, "
                          "cnt INT)")
            .execute();

        mysql.mysess->sql("INSERT INTO TASK_NEW_B2_TEMPTABLE "
                          "SELECT Customer.person_id, Product.brand_id, COUNT(*) "
                          "FROM TASK_NEW_B1_TEMPTABLE AS t, Product, Customer "
                          "WHERE t.product_id = Product.product_id "
                          "AND Customer.customer_id = t.customer_id "
                          "GROUP BY person_id, Product.brand_id")
            .execute();
        end_mysql = high_resolution_clock::now();
        time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
        cout << "TASK_NEW_B2_TEMPTABLE (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;
    }
    else
    {
        // Get pairs of customer_id and product_id that the customer gives the highest rating score.
        start_mongo = high_resolution_clock::now();
        mongocxx::pipeline stages;
        stages.match(make_document(kvp("rating", 5)));
        stages.lookup(make_document(
            kvp("from", "Order"),
            kvp("localField", "order_id"),
            kvp("foreignField", "order_id"),
            kvp("as", "orders")));
        stages.unwind("$orders");
        stages.project(make_document(
            kvp("_id", 0),
            kvp("customer_id", "$orders.customer_id"),
            kvp("product_id", "$product_id")));
        end_mongo = high_resolution_clock::now();
        time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);
        cout << "Get pairs from Order (MongoDB): " << duration_cast<milliseconds>(end_mongo - start_mongo).count() << " ms" << endl;

        start_mysql = high_resolution_clock::now();
        mysql.mysess->sql("CREATE TEMPORARY TABLE TASK_NEW_B2_TEMPTABLE_2 ("
                          "person_id INT, "
                          "brand_id INT)")
            .execute();

        auto insert_temptbl_b = mysql.mysess->getSchema("Ecommerce")
                                    .getTable("TASK_NEW_B2_TEMPTABLE_2")
                                    .insert("person_id", "brand_id");
        end_mysql = high_resolution_clock::now();
        time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
        cout << "Create TASK_NEW_B2_TEMPTABLE_2 (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;
        before_mysql = time_mysql;

        start_mongo = high_resolution_clock::now();
        auto cursor = mongodb.db["Review"].aggregate(stages);
        end_mongo = high_resolution_clock::now();
        time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);
        cout << "Aggregate Review (MongoDB): " << duration_cast<milliseconds>(end_mongo - start_mongo).count() << " ms" << endl;
        before_mongo = time_mongo;

        start_comm = high_resolution_clock::now();
        milliseconds time_loop = milliseconds(0);
        for (const auto &doc : cursor)
        {
            start_mongo = high_resolution_clock::now();
            auto json = Json::parse(bsoncxx::to_json(doc));
            std::string customer_id = json["customer_id"].get<std::string>();
            std::string product_id = json["product_id"].get<std::string>();
            end_mongo = high_resolution_clock::now();
            time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);
            time_loop += duration_cast<milliseconds>(end_mongo - start_mongo);

            start_mysql = high_resolution_clock::now();
            auto c_cursor = mysql.mysess->getSchema("Ecommerce")
                                .getTable("Customer")
                                .select("person_id")
                                .where("customer_id = '" + customer_id + "'")
                                .execute();
            int person_id = c_cursor.fetchOne()[0].get<int>();

            auto p_cursor = mysql.mysess->getSchema("Ecommerce")
                                .getTable("Product")
                                .select("brand_id")
                                .where("product_id = '" + product_id + "'")
                                .execute();
            int brand_id = p_cursor.fetchOne()[0].get<int>();

            insert_temptbl_b.values(person_id, brand_id);
            buffer_cnt++;

            if (buffer_cnt >= BUFFER)
            {
                insert_temptbl_b.execute();
                insert_temptbl_b = mysql.mysess->getSchema("Ecommerce")
                                       .getTable("TASK_NEW_B2_TEMPTABLE_2")
                                       .insert("person_id", "brand_id");
                buffer_cnt = 0;
            }
            end_mysql = high_resolution_clock::now();
            time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
            time_loop += duration_cast<milliseconds>(end_mysql - start_mysql);
        }
        end_comm = high_resolution_clock::now();
        time_comm = duration_cast<milliseconds>(end_comm - start_comm - time_loop);
        cout << "Insert to TASK_NEW_B2_TEMPTABLE_2 (MongoDB - Parse): " << time_mongo.count() - before_mongo.count() << " ms" << endl;
        cout << "Insert to TASK_NEW_B2_TEMPTABLE_2 (MySQL): " << time_mysql.count() - before_mysql.count() << " ms" << endl;
        cout << "Communication Time: " << time_comm.count() << " ms" << endl;

        start_mysql = high_resolution_clock::now();
        insert_temptbl_b.execute();
        buffer_cnt = 0;

        // Create table for storing aggregated results
        mysql.mysess->sql("CREATE TABLE TASK_NEW_B2_TEMPTABLE ("
                          "person_id INT, "
                          "brand_id INT, "
                          "cnt INT)")
            .execute();

        mysql.mysess->sql("INSERT INTO TASK_NEW_B2_TEMPTABLE "
                          "SELECT person_id, brand_id, COUNT(*) "
                          "FROM TASK_NEW_B2_TEMPTABLE_2 "
                          "GROUP BY person_id, brand_id")
            .execute();
        end_mysql = high_resolution_clock::now();
        time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
        cout << "TASK_NEW_B2_TEMPTABLE (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;
    }

    // C: Find favorite brand per customer
    start_mysql = high_resolution_clock::now();
    mysql.mysess->sql("CREATE TEMPORARY TABLE TASK_NEW_C_TEMPTABLE ("
                      "person_id INT, "
                      "brand_id INT)")
        .execute();

    // Note that MIN() is used for tie-breaking
    mysql.mysess->sql("INSERT INTO TASK_NEW_C_TEMPTABLE "
                      "SELECT t1.person_id, MIN(t1.brand_id) "
                      "FROM TASK_NEW_B2_TEMPTABLE AS t1, "
                      "(SELECT person_id, MAX(cnt) AS max_cnt "
                      " FROM TASK_NEW_B2_TEMPTABLE "
                      " GROUP BY person_id) AS t2 "
                      "WHERE t1.person_id = t2.person_id "
                      "AND t1.cnt = t2.max_cnt "
                      "GROUP BY t1.person_id")
        .execute();
    end_mysql = high_resolution_clock::now();
    time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
    cout << "C (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;

    // D
    auto start_scidb = high_resolution_clock::now();
    unique_ptr<ScidbConnection> scidb(new ScidbConnection(SCIDB_HOST_ECOMMERCE + string(":8080")));
    scidb->exec("remove(tnew_d)");

    // 2984700 = 9949 * 300
    scidb->exec("store(redimension( "
                "apply(apply(build(<val: double> [i=0:2984699:0:300000], 0), person_id, i / 300), tag_id, i % 300), "
                "<val:double>[person_id=0:9948:0:1000;tag_id=0:299:0:300], false), tnew_d)");

    // Array for uploading
    scidb->exec("remove(tnew_d_temp)");
    scidb->exec("create array tnew_d_temp <person_id:int32, tag_id:int32> [i=0:2984699:0:300000]");

    ScidbSchema schema;
    schema.attrs.push_back(ScidbAttr("person_id", INT32));
    schema.attrs.push_back(ScidbAttr("tag_id", INT32));

    shared_ptr<ScidbArrFile> coo(new ScidbArrFile(schema));
    auto end_scidb = high_resolution_clock::now();
    auto time_scidb = duration_cast<milliseconds>(end_scidb - start_scidb);
    cout << "D (SciDB): " << duration_cast<milliseconds>(end_scidb - start_scidb).count() << " ms" << endl;
    before_scidb = time_scidb;

    start_mysql = high_resolution_clock::now();
    auto res_d = mysql.mysess->getSchema("Ecommerce")
                     .getTable("TASK_NEW_A_TEMPTABLE")
                     .select("person_id", "tag_id")
                     .execute();
    end_mysql = high_resolution_clock::now();
    time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
    cout << "Get A (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;
    before_mysql = time_mysql;

    start_comm = high_resolution_clock::now();
    milliseconds time_loop = milliseconds(0);
    for (const auto &row : res_d)
    {
        start_mysql = high_resolution_clock::now();
        int person_id = row[0].get<int>();
        int tag_id = row[1].get<int>();
        end_mysql = high_resolution_clock::now();
        time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
        time_loop += duration_cast<milliseconds>(end_mysql - start_mysql);

        // Append to array for upload
        start_scidb = high_resolution_clock::now();
        ScidbLineType line;
        line.push_back(person_id);
        line.push_back(tag_id);

        coo->add(line);
        end_scidb = high_resolution_clock::now();
        time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);
        time_loop += duration_cast<milliseconds>(end_scidb - start_scidb);
    }
    scidb->upload("tnew_d_temp", coo);
    end_comm = high_resolution_clock::now();
    time_comm += duration_cast<milliseconds>(end_comm - start_comm - time_loop);
    cout << "Insert to tnew_d_temp (MySQL - Get Row): " << time_mysql.count() - before_mysql.count() << " ms" << endl;
    cout << "Append to tnew_d_temp (SciDB): " << time_scidb.count() - before_scidb.count() << " ms" << endl;
    cout << "Communication Time: " << duration_cast<milliseconds>(end_comm - start_comm - time_loop).count() << " ms" << endl;

    // Densify
    start_scidb = high_resolution_clock::now();
    scidb->exec("insert(redimension( "
                "apply(tnew_d_temp, val, 1.0), "
                "<val:double>[person_id=0:9948:0:1000;tag_id=0:299:0:300], false), tnew_d)");

    // E
    scidb->exec("remove(tnew_e)");
    scidb->exec("store(redimension( "
                "apply(build(<favorite: double> [person_id=0:9948:0:1000], 0), j, 0), "
                "<favorite: double> [person_id=0:9948:0:1000, j=0:0:0:1000]), tnew_e)");

    // Array for uploading
    scidb->exec("remove(tnew_e_temp)");
    scidb->exec("create array tnew_e_temp <person_id:int32, favorite:double> [i=0:9948:0:1000]");

    ScidbSchema schema2;
    schema2.attrs.push_back(ScidbAttr("person_id", INT32));
    schema2.attrs.push_back(ScidbAttr("brand_id", DOUBLE));

    shared_ptr<ScidbArrFile> coo2(new ScidbArrFile(schema2));
    end_scidb = high_resolution_clock::now();
    time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);
    cout << "E (SciDB): " << duration_cast<milliseconds>(end_scidb - start_scidb).count() << " ms" << endl;
    before_scidb = time_scidb;

    start_mysql = high_resolution_clock::now();
    auto res_e = mysql.mysess->getSchema("Ecommerce")
                     .getTable("TASK_NEW_C_TEMPTABLE")
                     .select("person_id", "brand_id")
                     .execute();
    end_mysql = high_resolution_clock::now();
    time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
    cout << "Get C (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;
    before_mysql = time_mysql;

    start_comm = high_resolution_clock::now();
    time_loop = milliseconds(0);
    for (const auto &row : res_e)
    {
        start_mysql = high_resolution_clock::now();
        int person_id = row[0].get<int>();
        int favorite_brand_id = row[1].get<int>();
        end_mysql = high_resolution_clock::now();
        time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
        time_loop += duration_cast<milliseconds>(end_mysql - start_mysql);

        // Append to array for upload
        start_scidb = high_resolution_clock::now();
        ScidbLineType line;
        line.push_back(person_id);
        line.push_back(favorite_brand_id == brand_id ? (double)1 : (double)0);

        coo2->add(line);
        end_scidb = high_resolution_clock::now();
        time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);
        time_loop += duration_cast<milliseconds>(end_scidb - start_scidb);
    }
    scidb->upload("tnew_e_temp", coo2);
    end_comm = high_resolution_clock::now();
    time_comm += duration_cast<milliseconds>(end_comm - start_comm - time_loop);
    cout << "Insert to tnew_e_temp (MySQL - Get Row): " << time_mysql.count() - before_mysql.count() << " ms" << endl;
    cout << "Append to tnew_e_temp (SciDB): " << time_scidb.count() - before_scidb.count() << " ms" << endl;
    cout << "Communication Time: " << duration_cast<milliseconds>(end_comm - start_comm - time_loop).count() << " ms" << endl;

    // Densify
    start_scidb = high_resolution_clock::now();
    scidb->exec("insert(redimension(tnew_e_temp, "
                "<favorite: double> [person_id=0:9948:0:1000, j=0:0:0:1000], 0), tnew_e)");

    // Logistic Regression of F
    scidb->exec("remove(empty_c)");
    scidb->exec("create array empty_c <val:double> [i=0:9948:0:1000, j=0:0:0:1000]");

    // For the third argument of the outer gemm
    scidb->exec("remove(empty_c2)");
    scidb->exec("create array empty_c2 <val:double> [i=0:299:0:300, j=0:0:0:1000]");

    // Initialize w
    scidb->exec("remove(tnew_w)");
    scidb->exec("store(build(<val:double> [i=0:299:0:300, j=0:0:0:1000], 1.0), tnew_w)");

    // Update
    int nrow = 0, max_iter = 1;
    for (int iter = 0; iter < max_iter; iter++)
    {
        // "%2B" is used for "+"
        scidb->exec("store(project(apply(join(tnew_w, "
                    "apply(gemm(tnew_d, project(apply(join(apply(gemm(tnew_d, tnew_w, empty_c), "
                    "inner_gemm, 1 / (1 %2B exp(-1 * gemm))), tnew_e), diff, inner_gemm - favorite), diff), "
                    "empty_c2, transa:true), subtract_rhs, 0.0001 * gemm)), "
                    "res, val - subtract_rhs), res), tnew_w2)");
        scidb->exec("remove(tnew_w)");
        scidb->exec("store(project(apply(tnew_w2, val, res), val), tnew_w)");
        scidb->exec("remove(tnew_w2)");
    }
    end_scidb = high_resolution_clock::now();
    time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);
    cout << "Logistic Regression (SciDB): " << duration_cast<milliseconds>(end_scidb - start_scidb).count() << " ms" << endl;

    start_mysql = high_resolution_clock::now();
    mysql.mysess->sql("DROP TABLE TASK_NEW_B2_TEMPTABLE").execute();
    end_mysql = high_resolution_clock::now();
    time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
    cout << "Drop TASK_NEW_B2_TEMPTABLE (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;

    start_comm = high_resolution_clock::now();
    auto result = scidb->download("tnew_w");
    auto cell = result->readcell();
    while (cell.size() != 0)
    {
        nrow++;
        cell = result->readcell();
    }
    end_comm = high_resolution_clock::now();
    time_comm += duration_cast<milliseconds>(end_comm - start_comm);
    cout << "Communication Time (Get result from SciDB): " << duration_cast<milliseconds>(end_comm - start_comm).count() << " ms" << endl
         << endl;

    /* save result matrix to csv */
    // scidb->exec("save(tnew_w, '/tmp/t0.csv', -2, 'csv')");

    cout << "[TASK 0]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl
         << endl;

    cout << "MySQL: " << time_mysql.count() << " ms" << endl;
    cout << "MongoDB: " << time_mongo.count() << " ms" << endl;
    cout << "SciDB: " << time_scidb.count() << " ms" << endl;
    cout << "Communication: " << time_comm.count() << " ms" << endl
         << endl;
}

/**
 *  [Task2] Product Recommendation ([D, A] => A)
 *  Perform product recommendation using "Factorization" based on past customer ratings.
 *
 *      A: SELECT Order.cid AS cid, Review.pid AS pid, Review.rating AS rating
 *          FROM Review, Order // Document
 *          WHERE Review.oid = Order.oid // Relational
 *
 *      B, C: A.toArray(<val(=A.rating)>[cid, pid]).Factorization // 2 Arrays
 *
 *      D: B(<val: latent_factor>[cid, k]) * C(<val: latent_factor>[k, pid]) // Array
 *      E: SELECT pid FROM D WHERE cid = 'x' AND val > 4 // Relational
 */
void T2()
{
    auto before_mysql = milliseconds(0);
    auto before_mongo = milliseconds(0);
    auto before_scidb = milliseconds(0);
    auto before_comm = milliseconds(0);

    auto start_mysql = high_resolution_clock::now();
    auto mysql = mysql_connector();
    mysql.mysess->sql("USE Ecommerce").execute();
    mysql.mysess->sql("CREATE TEMPORARY TABLE Rating_history ("
                      "customer_id VARCHAR(20),"
                      "product_id CHAR(11),"
                      "rating INT)")
        .execute();

    auto Rating_history = mysql.mysess->getSchema("Ecommerce")
                              .getTable("Rating_history")
                              .insert("customer_id", "product_id", "rating");
    auto end_mysql = high_resolution_clock::now();
    auto time_mysql = duration_cast<milliseconds>(end_mysql - start_mysql);
    cout << "Create Rating_history (MySQL): " << time_mysql.count() << " ms" << endl;
    before_mysql = time_mysql;

    auto start_mongo = high_resolution_clock::now();
    mongodb_connector mongodb("Ecommerce");
    auto orders = mongodb.db["Order"];
    auto reviews = mongodb.db["Review"];

    mongocxx::pipeline stages;
    stages.lookup(make_document(
        kvp("from", "Order"),
        kvp("localField", "order_id"),
        kvp("foreignField", "order_id"),
        kvp("as", "res")));
    stages.group(make_document(
        kvp("_id", make_document(
                       kvp("customer_id", "$res.customer_id"),
                       kvp("product_id", "$product_id"))),
        kvp("val", make_document(kvp("$avg", "$rating")))));
    auto cursor = reviews.aggregate(stages);
    auto end_mongo = high_resolution_clock::now();
    auto time_mongo = duration_cast<milliseconds>(end_mongo - start_mongo);
    cout << "Aggregate Review (MongoDB): " << time_mongo.count() << " ms" << endl;
    before_mongo = time_mongo;

    auto start_comm = high_resolution_clock::now();
    milliseconds time_loop = milliseconds(0);
    int buffer = 0;
    for (auto history : cursor)
    {
        start_mongo = high_resolution_clock::now();
        auto customer_id = std::string(history["_id"]["customer_id"].get_array().value[0].get_string().value);
        std::string product_id = std::string(history["_id"]["product_id"].get_string().value);
        int rating = history["val"].get_double();
        end_mongo = high_resolution_clock::now();
        time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);
        time_loop += duration_cast<milliseconds>(end_mongo - start_mongo);

        start_mysql = high_resolution_clock::now();
        Rating_history.values(customer_id, product_id, rating);

        buffer++;
        if (buffer >= BUFFER)
        {
            Rating_history.execute();
            Rating_history = mysql.mysess->getSchema("Ecommerce")
                                 .getTable("Rating_history")
                                 .insert("customer_id", "product_id", "rating");
            buffer = 0;
        }
        end_mysql = high_resolution_clock::now();
        time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
        time_loop += duration_cast<milliseconds>(end_mysql - start_mysql);
    }
    auto end_comm = high_resolution_clock::now();
    auto time_comm = duration_cast<milliseconds>(end_comm - start_comm - time_loop);
    cout << "Insert to Rating_history (MongoDB - Parse): " << time_mongo.count() - before_mongo.count() << " ms" << endl;
    cout << "Insert to Rating_history (MySQL): " << time_mysql.count() - before_mysql.count() << " ms" << endl;
    cout << "Communication Time: " << time_comm.count() << " ms" << endl;

    start_mysql = high_resolution_clock::now();
    if (buffer > 0)
        Rating_history.execute();

    mysql.mysess->sql("CREATE INDEX Rating_history_idx1 ON Rating_history(customer_id)").execute();
    mysql.mysess->sql("CREATE INDEX Rating_history_idx2 ON Rating_history(product_id)").execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE Rcustomer AS ("
                      "SELECT t.customer_id, ROW_NUMBER() OVER () -1 AS customer_id_d FROM ( "
                      "SELECT DISTINCT(customer_id) AS customer_id "
                      "FROM Rating_history) AS t )")
        .execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE Rproduct AS ("
                      "SELECT t.product_id, ROW_NUMBER() OVER () -1 AS product_id_d FROM ("
                      "SELECT DISTINCT(product_id) AS product_id "
                      "FROM Rating_history) AS t )")
        .execute();

    mysql.mysess->sql("CREATE INDEX Rcustomer_idx ON Rcustomer(customer_id)").execute();
    mysql.mysess->sql("CREATE INDEX Rproduct_idx ON Rproduct(product_id)").execute();

    auto rows = mysql.mysess->sql("SELECT customer_id_d AS person, product_id_d AS product, rating "
                                  "FROM Rcustomer, Rproduct, Rating_history "
                                  "WHERE Rating_history.customer_id = Rcustomer.customer_id "
                                  "AND Rating_history.product_id = Rproduct.product_id")
                    .execute();

    int dim1 = mysql.mysess->getSchema("Ecommerce").getTable("Rcustomer").count();
    int dim2 = mysql.mysess->getSchema("Ecommerce").getTable("Rproduct").count();
    end_mysql = high_resolution_clock::now();
    time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
    cout << "Create Index and Get A (MySQL): " << duration_cast<milliseconds>(end_mysql - start_mysql).count() << " ms" << endl;
    before_mysql = time_mysql;

    auto start_scidb = high_resolution_clock::now();
    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST_ECOMMERCE + string(":8080")));

    conn->exec("remove(temp)");
    conn->exec("create array temp<val:double, x:int32, y:int32> [i=0:" + to_string(dim1 * dim2 - 1) + ":0:300000]");

    ScidbSchema schema;
    schema.attrs.push_back(ScidbAttr("val", DOUBLE));
    schema.attrs.push_back(ScidbAttr("x", INT32));
    schema.attrs.push_back(ScidbAttr("y", INT32));

    shared_ptr<ScidbArrFile> coo(new ScidbArrFile(schema));
    auto end_scidb = high_resolution_clock::now();
    auto time_scidb = duration_cast<milliseconds>(end_scidb - start_scidb);
    cout << "A to array (SciDB): " << time_scidb.count() << " ms" << endl;
    before_scidb = time_scidb;

    start_comm = high_resolution_clock::now();
    time_loop = milliseconds(0);
    for (auto row : rows)
    {
        start_mysql = high_resolution_clock::now();
        int customer_id = row[0].get<int>();
        int product_id = row[1].get<int>();
        double rating = (double)row[2].get<int>();
        end_mysql = high_resolution_clock::now();
        time_mysql += duration_cast<milliseconds>(end_mysql - start_mysql);
        time_loop += duration_cast<milliseconds>(end_mysql - start_mysql);

        /**
         * Pass to SCIDB
         */
        start_scidb = high_resolution_clock::now();
        ScidbLineType line;
        line.push_back(rating);
        line.push_back(customer_id);
        line.push_back(product_id);

        coo->add(line);
        end_scidb = high_resolution_clock::now();
        time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);
        time_loop += duration_cast<milliseconds>(end_scidb - start_scidb);
    }

    /**
     * Upload data to temp array with coo format
     * Note that SciDB only accepts 1-D array
     */
    conn->upload("temp", coo);
    end_comm = high_resolution_clock::now();
    time_comm += duration_cast<milliseconds>(end_comm - start_comm - time_loop);
    cout << "Insert to array A (MySQL - Get Row): " << time_mysql.count() - before_mysql.count() << " ms" << endl;
    cout << "Append to array A (SciDB): " << time_scidb.count() - before_scidb.count() << " ms" << endl;
    cout << "Communication Time: " << duration_cast<milliseconds>(end_comm - start_comm - time_loop).count() << " ms" << endl;
    before_comm = time_comm;

    start_scidb = high_resolution_clock::now();
    conn->exec("remove(V)");
    conn->exec("store(redimension(temp, <val:double>[x=0:" + to_string(dim1 - 1) +
               ":0:1000; y=0:" + to_string(dim2 - 1) + ":0:300], false),  V)");

    /**
     *  INIT W H
     */
    conn->exec("load_library('dense_linear_algebra')");

    conn->exec("remove(initW)");
    conn->exec("remove(W)");

    int feature_size = 50;
    string create_W = "store(build(<val:double>[i=0:" + to_string(dim1 - 1) + ":0:1000; j=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "], 1.0), initW)";
    conn->exec(create_W);

    conn->exec("remove(initH)");
    conn->exec("remove(H)");
    string create_H = "store(build(<val:double>[i=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "; j=0:" + to_string(dim2 - 1) + ":0:300], 1.0), initH)";
    conn->exec(create_H);

    string newH = "store(initH, H)";
    string newW = "store(initW, W)";
    conn->exec(newH);
    conn->exec(newW);

    /**
     *  ZERO MATRRIX
     */
    conn->exec("remove(zeroWtV)");
    string zeroWtV = "create array zeroWtV<val:double> [i=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "; j=0:" + to_string(dim2 - 1) + ":0:300]";
    conn->exec(zeroWtV);

    conn->exec("remove(zeroWtW)");
    string zeroWtW = "create array zeroWtW<val:double> [i=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "; j=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "]";
    conn->exec(zeroWtW);

    conn->exec("remove(zeroFH)");
    string zeroFH = "create array zeroFH<val:double> [i=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "; j=0:" + to_string(dim2 - 1) + ":0:300]";
    conn->exec(zeroFH);

    conn->exec("remove(zeroVHt)");
    string zeroVHt = "create array zeroVHt<val:double> [i=0:" + to_string(dim1 - 1) + ":0:1000; j=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "]";
    conn->exec(zeroVHt);

    conn->exec("remove(zeroHHt)");
    string zeroHHt = "create array zeroHHt<val:double> [i=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "; j=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "]";
    conn->exec(zeroHHt);

    conn->exec("remove(zeroWF)");
    string zeroWF = "create array zeroWF<val:double> [i=0:" + to_string(dim1 - 1) + ":0:1000; j=0:" + to_string(feature_size - 1) + ":0:" + to_string(feature_size) + "]";
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

    string WtV = "store(gemm(W, V, zeroWtV, transa: True), WtV)";
    string WtWH = "store(gemm(gemm(W, W, zeroWtW, transa: True), H, zeroFH), WtWH)";
    string Hdiv = "store(join(WtV, WtWH), Hdiv)";
    string factorH = "store(project(apply(Hdiv, div, gemm/gemm_2), div), factorH)";
    string Hmul = "store(join(H, factorH), Hmul)";
    string renewH = "store(project(apply(Hmul, newh, val*div), newh), H)";

    string VHt = "store(gemm(V, H, zeroVHt, transb: true), VHt)";
    string WHHt = "store(gemm(W, gemm(H, H, zeroHHt, transb: true), zeroWF), WHHt)";
    string Wdiv = "store(join(VHt, WHHt), Wdiv)";
    string factorW = "store(project(apply(Wdiv, div, gemm/gemm_2), div), factorW)";
    string Wmul = "store(join(W, factorW), Wmul)";
    string renewW = "store(project(apply(Wmul, neww, val*div), neww), W)";

    // string newV = "store(join(gemm(W, H, zeroV), V), newV)";
    // string agg = "store(aggregate(apply(newV, diff, abs(gemm-val)), sum(diff)), L)";

    time_loop = milliseconds(0);
    int nrow = 0;
    for (int iter = 0; iter < 1; iter++)
    {
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

        // conn->exec(newV);
        // conn->exec(agg);

        start_comm = high_resolution_clock::now();
        auto result = conn->download("W");
        auto cell = result->readcell();
        while (cell.size() != 0)
        {
            cell = result->readcell();
            nrow++;
        }
        end_comm = high_resolution_clock::now();
        time_comm += duration_cast<milliseconds>(end_comm - start_comm);
        time_loop += duration_cast<milliseconds>(end_comm - start_comm);
    }
    end_scidb = high_resolution_clock::now();
    time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb - time_loop);
    cout << "MatMul (SciDB): " << duration_cast<milliseconds>(end_scidb - start_scidb - time_loop).count() << " ms" << endl;
    cout << "Communication Time (Get result from SciDB): " << time_comm.count() - before_comm.count() << " ms" << endl
         << endl;

    /* save result matrix to csv */
    // conn->exec("save(W, '/tmp/t2.csv', -2, 'csv')");

    cout << "[TASK 2]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl;

    cout << "MySQL: " << time_mysql.count() << " ms" << endl;
    cout << "MongoDB: " << time_mongo.count() << " ms" << endl;
    cout << "SciDB: " << time_scidb.count() << " ms" << endl;
    cout << "Communication: " << time_comm.count() << " ms" << endl
         << endl;
}
