//
// Created by mxmdb on 21. 5. 3..
//

#include <time.h>

#include <chrono>

#include <nlohmann/json.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <Connection/ScidbConnection.h>

#include "Connection/mysql_connector.h"
#include "Connection/mongodb_connector.h"
#include "Connection/neo4j_connector.h"

using namespace std;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

#define BUFFER 1000
#define SCIDB_HOST_HEALTHCARE "127.0.0.1"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

using Json = nlohmann::json;

/**

 Patient
+---------------+---------+------+-----+---------+-------+
| Field         | Type    | Null | Key | Default | Extra |
+---------------+---------+------+-----+---------+-------+
| patient_id    | int     | NO   | PRI | NULL    |       |
| gender        | char(1) | YES  |     | NULL    |       |
| date_of_birth | date    | YES  |     | NULL    |       |
| date_of_death | date    | YES  |     | NULL    |       |
+---------------+---------+------+-----+---------+-------+

 Prescription
+------------+-------------+------+-----+---------+-------+
| Field      | Type        | Null | Key | Default | Extra |
+------------+-------------+------+-----+---------+-------+
| patient_id | int         | YES  | MUL | NULL    |       |
| drug_name  | varchar(20) | YES  |     | NULL    |       |
| startdate  | date        | YES  |     | NULL    |       |
| enddate    | date        | YES  |     | NULL    |       |
| drug_id    | int         | YES  |     | NULL    |       |
+------------+-------------+------+-----+---------+-------+

 Diagnosis
+-----------------------+------+------+-----+---------+-------+
| Field                 | Type | Null | Key | Default | Extra |
+-----------------------+------+------+-----+---------+-------+
| patient_id            | int  | YES  | MUL | NULL    |       |
| snomed_id             | int  | YES  |     | NULL    |       |
| diagnoses_description | text | YES  |     | NULL    |       |
+-----------------------+------+------+-----+---------+-------+
**/

/**
 *
 * [Task6] Drug Interaction (R,D=>R)
 *  Find drugs that has known interaction with the prescribed drugs for a give patient
 *
 *      A: Select distinct(drug_id) as drug_id
 *          From Prescription
 * *        Where patient_id = @param

*      B: Select drug_interaction.interaction_drug.name
 *          From Drug, A
 *          Unnest Drug.drug_interaction_list as drug_interaction
 *          Where A.drug_id == Drug.drug_id
 *          Group by drug_interaction.interaction_drug.name
 *
 *
 *      @param patient_id
 */

void T6(int patient_id)
{

    auto mysql = mysql_connector();
    auto Prescription = mysql.mysess->getSchema("Healthcare").getTable("Prescription");
    auto mongodb = mongodb_connector("Healthcare");
    auto db = mongodb.db;
    auto Drug = db["drug"];

    // OPTION 1
    {
        db["temp"].drop();
        int buffer = 0;
        int nrow = 0;
        auto prescribed_drugs = Prescription.select("distinct(drug_id) as drug_id").where("patient_id=" + to_string(patient_id)).execute();

        for (auto row : prescribed_drugs)
        {
            mongocxx::pipeline p{};

            p.match(make_document(kvp("drug_id", row[0].get<int>())));
            p.unwind("$drug_interaction_list");
            p.project(make_document(kvp("drug", "$drug_interaction_list"), kvp("_id", 0)));
            p.merge(make_document(kvp("into", "temp")));
            auto affected_drugs = Drug.aggregate(p, mongocxx::options::aggregate{});
            for (auto affected_drug : affected_drugs)
            {
                auto json = Json::parse(bsoncxx::to_json(affected_drug));
            }
        }

        mongocxx::pipeline p{};
        auto temp = db["temp"];
        int cnt = temp.count_documents({});
        p.group(make_document(kvp("_id", "$drug.interaction_drug.drug_name")));
        auto distinct_drugs = temp.aggregate(p, mongocxx::options::aggregate{});
        for (auto distinct_drug : distinct_drugs)
        {
            //            auto json = Json::parse(bsoncxx::to_json(distinct_drug));
            //            cout << json << endl;
            nrow++;
        }
        db["temp"].drop();

        cout << "[TASK6]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl;
    }

    // OPTION 2
    //    {
    //        int nrow = 0;
    //        auto prescribed_drugs = Prescription.select("drug_id").where("patient_id=" + to_string(patient_id)).execute();
    //        options::index options{};
    //        options.unique(true);
    //        db["temp"].drop();
    //        db["temp"].create_index(make_document(kvp("drugbank_id", 1)), options);
    //
    //        for (auto row :  prescribed_drugs) {
    //            mongocxx::pipeline p{};
    //            p.match(make_document(kvp("drug_id", row[0].get<int>())));
    //            p.unwind("$drug_interactions_list");
    //            p.project(make_document(kvp("drug", "$drug_interactions_list"),
    //                                    kvp("drugbank_id", "$drug_interactions_list.affected_drug.drugbank_id"),
    //                                    kvp("_id", 0)));
    //            p.merge(make_document(kvp("into", "temp"),
    //                                  kvp("on", "drugbank_id"),
    //                                  kvp("whenMatched", "merge"),
    //                                  kvp("whenNotMatched", "insert")));
    //
    //            auto affected_drugs = Drug.aggregate(p, mongocxx::options::aggregate{});
    //            for (auto affected_drug : affected_drugs) {
    //                auto json = Json::parse(bsoncxx::to_json(affected_drug));
    //            }
    //        }
    //
    //        auto cursor = db["temp"].find({},mongocxx::options::find{}.projection(make_document(kvp("_id", 0),kvp("drugbank_id", 0))));
    //        for (auto doc : cursor) {
    //            auto json = Json::parse(bsoncxx::to_json(doc));
    //            cout << json << endl;
    //            nrow++;
    //        }
    //        db["result_list"].drop();
    //
    //        cout << "[TASK6]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl;
    //    }
}
/**
 * [Task 7] Group of patients with similar disease (RxG=>R)
 * Find the number of female and male patients suffering from a similar disease with a given patient.
 *
 * A: SELECT disease_id FROM Diagnosis WHERE patient_id = @param
 * B: SELECT d3.disease_id FROM Disease Network, A
 *    WHERE (d1: Disease) - [:is_a] -> (d2: Disease) <- [:is_a] - (d3: Disease) AND d1.disease_id in A AND d3.disease_id not in A //table
 * C: SELECT patient_id FROM Diagnosis, B
 *    WHERE Diagnosis.disease_id = B.disease_id AND Diagnosis.patient_id != @param
 * D: SELECT gender, count(gender) FROM Patient, C WHERE Patient.patient_id = C.patient_id GROUP BY gender
 *
 * @param patient_id
 */

void T7(int patient_id)
{
    auto mysql = mysql_connector();
    auto Diagnosis = mysql.mysess->getSchema("Healthcare").getTable("Diagnosis");
    auto neo4j = new neo4j_connector();

    auto diagnosis_disease_ids = Diagnosis.select("disease_id").where("patient_id=" + to_string(patient_id)).execute();

    mysql.mysess->sql("use Healthcare").execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE Related_disease("
                      "disease_id VARCHAR(100))")
        .execute();

    auto insert2related_disease = mysql.mysess->getSchema("Healthcare")
                                      .getTable("Related_disease")
                                      .insert("disease_id");

    int nrow = 0;
    int buffer = 0;
    for (auto row : diagnosis_disease_ids)
    {
        std::string pmatch = "p=(n1:Disease)-[:is_a]->(n2:Disease)<-[:is_a]-(n3:Disease)";
        std::string pwhere = ("n1.disease_id=\"" + to_string(row[0].get<int>()) + "\"");
        std::string preturn = "n3.disease_id";

        std::string query_str = ("MATCH " + pmatch + " WHERE " + pwhere + " RETURN " + preturn);
        auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);

        char buff[100];
        while (1)
        {
            neo4j_result_t *result = neo4j_fetch_next(results);
            if (result == NULL)
                break;

            int pos = 0;
            while (1)
            {
                auto n = neo4j_result_field(result, pos);
                if (!neo4j_is_null(n))
                {
                    std::string str = neo4j_tostring(n, buff, sizeof(buff));
                    insert2related_disease.values(str.substr(1, str.length() - 2));

                    buffer++;
                    if (buffer >= BUFFER)
                    {
                        insert2related_disease.execute();
                        insert2related_disease = mysql.mysess->getSchema("Healthcare")
                                                     .getTable("Related_disease")
                                                     .insert("disease_id");
                        buffer = 0;
                    }
                }
                else
                {
                    break;
                }
                pos++;
            }
        }
    }
    if (buffer != 0)
        insert2related_disease.execute();

    std::string query = "select gender, count(gender)"
                        "from Patient, (select distinct Diagnosis.patient_id "
                        "from Diagnosis, (select distinct disease_id from Related_disease) as t "
                        "where t.disease_id not in (select disease_id from Diagnosis where patient_id=" +
                        to_string(patient_id) + ") "
                                                "AND Diagnosis.disease_id = t.disease_id AND Diagnosis.patient_id != " +
                        to_string(patient_id) + ") as C "
                                                "where Patient.patient_id = C.patient_id group by gender";

    auto res = mysql.mysess->sql(query).execute();

    // cout << "gender count" << endl;
    for (auto row : res)
    {
        cout << row[0].get<std::string>() << " " << row[1].get<int>() << endl;
        nrow++;
    }

    cout << "TASK7: TOTAL " << nrow++ << " ROWS ARE REPORTED" << endl;
}

/**
 * [Task 8] Potential drug interaction (RxD=>R)
 * Find all potential interaction drugs with the given patient's prescription drug
 *
 * A: SELECT drug_id, drug_name FROM Drug
 * B: SELECT d.targets.id, d.targets.name FROM Drug UNNEST targets d
 * C: SELECT d.drug_id, d.target.id FROM Drug UNNEST targets d
 * D: Drug.toGraph(Node : A,B {label: drug, target} edge : C {label: has_bond} )//graph
 * E: SELECT d1.drug_name, d2.drug_name, count(t) AS common_target FROM D, Prescription
 *      WHERE (d1:drug)-[:has_bond]-(t:target)-[:has_bond]-(d2:drug)
 *      AND Prescription.patient_id = @param AND Prescription.drug_id = d1.drug_id
 *      GROUP BY d1.drug_name, d2.drug_name ORDER BY common_target DESC //table
 *
 * @param patient_id
 */

void T8(int patient_id)
{
    auto mysql = mysql_connector();
    auto Prescription = mysql.mysess->getSchema("Healthcare").getTable("Prescription");
    auto mongodb = mongodb_connector("Healthcare");
    auto db = mongodb.db;
    auto Drug = db["drug"];
    auto neo4j = new neo4j_connector();

    std::string query_str = ("MATCH (n:drug) DETACH DELETE n");
    auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
    neo4j_result_t *result = neo4j_fetch_next(results);

    query_str = ("MATCH (n:target) DETACH DELETE n");
    results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
    result = neo4j_fetch_next(results);

    query_str = ("MATCH (n:prescribed_drug) DETACH DELETE n");
    results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
    result = neo4j_fetch_next(results);

    auto v1_time1 = high_resolution_clock::now();
    int buffer = 0;
    std::string batch_str = "";
    mongocxx::pipeline p{};
    auto cursor = Drug.aggregate(p, mongocxx::options::aggregate{});
    for (auto doc : cursor)
    {
        bsoncxx::document::element stored_element1{doc["drug_id"]};
        bsoncxx::document::element stored_element2{doc["drug_name"]};
        if (stored_element1 && stored_element2)
        {
            buffer++;
            if (buffer >= BUFFER)
            {
                batch_str += "{drug_id: \"" + to_string(stored_element1.get_int32()) + "\", "
                                                                                       "drug_name: \"" +
                             string(stored_element2.get_utf8().value) + "\"}";
                std::string query_str = ("WITH [" + batch_str + "] AS rows "
                                                                "UNWIND rows as row "
                                                                "CREATE (n:drug) SET n += row");
                auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
                neo4j_result_t *result = neo4j_fetch_next(results);
                buffer = 0;
                batch_str = "";
            }
            else
            {
                batch_str += "{drug_id: \"" + to_string(stored_element1.get_int32()) + "\", "
                                                                                       "drug_name: \"" +
                             string(stored_element2.get_utf8().value) + "\"},";
            }
        }
    }
    if (buffer != 0)
    {
        batch_str = batch_str.substr(0, batch_str.length() - 1);
        std::string query_str = ("WITH [" + batch_str + "] AS rows "
                                                        "UNWIND rows as row "
                                                        "CREATE (n:drug) SET n += row");
        auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
        neo4j_result_t *result = neo4j_fetch_next(results);
        buffer = 0;
        batch_str = "";
    }
    auto v1_time2 = high_resolution_clock::now();
    auto v1_ms = duration_cast<milliseconds>(v1_time2 - v1_time1);

    query_str = ("CREATE INDEX idx_drug_id for (n:drug) on (n.drug_id)");
    results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
    result = neo4j_fetch_next(results);

    auto v2_time1 = high_resolution_clock::now();
    p.unwind("$targets");
    p.group(make_document(kvp("_id", "$targets.id"), kvp("name", make_document(kvp("$first", "$targets.name")))));
    cursor = Drug.aggregate(p, mongocxx::options::aggregate{});
    for (auto doc : cursor)
    {
        bsoncxx::document::element stored_element1{doc["_id"]};
        bsoncxx::document::element stored_element2{doc["name"]};
        if (stored_element1 && stored_element2)
        {
            buffer++;
            if (buffer >= BUFFER)
            {
                batch_str += "{target_id: \"" + string(stored_element1.get_utf8().value) + "\", "
                                                                                           "target_name: \"" +
                             string(stored_element2.get_utf8().value) + "\"}";
                std::string query_str = ("WITH [" + batch_str + "] AS rows "
                                                                "UNWIND rows as row "
                                                                "CREATE (n:target) SET n += row");
                auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
                neo4j_result_t *result = neo4j_fetch_next(results);
                buffer = 0;
                batch_str = "";
            }
            else
            {
                batch_str += "{target_id: \"" + string(stored_element1.get_utf8().value) + "\", "
                                                                                           "target_name: \"" +
                             string(stored_element2.get_utf8().value) + "\"},";
            }
        }
    }
    if (buffer != 0)
    {
        batch_str = batch_str.substr(0, batch_str.length() - 1);
        std::string query_str = ("WITH [" + batch_str + "] AS rows "
                                                        "UNWIND rows as row "
                                                        "CREATE (n:target) SET n += row");
        auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
        neo4j_result_t *result = neo4j_fetch_next(results);
        buffer = 0;
        batch_str = "";
    }
    auto v2_time2 = high_resolution_clock::now();
    auto v2_ms = duration_cast<milliseconds>(v2_time2 - v2_time1);

    query_str = ("CREATE INDEX idx_target_id for (n:target) on (n.target_id)");
    results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
    result = neo4j_fetch_next(results);

    auto edge_time1 = high_resolution_clock::now();
    mongocxx::pipeline p2{};
    p2.unwind("$targets");
    cursor = Drug.aggregate(p2, mongocxx::options::aggregate{});
    for (auto doc : cursor)
    {
        bsoncxx::document::element stored_element1{doc["targets"]["id"]};
        bsoncxx::document::element stored_element3{doc["drug_id"]};
        if (stored_element1 && stored_element3)
        {
            buffer++;
            if (buffer >= BUFFER)
            {
                batch_str += "{drug_id: \"" + to_string(stored_element3.get_int32()) + "\", "
                                                                                       "target_id: \"" +
                             string(stored_element1.get_utf8().value) + "\"}";
                std::string query_str = ("WITH [" + batch_str + "] AS rows "
                                                                "UNWIND rows as row "
                                                                "MATCH (d:drug{drug_id: row.drug_id}), (t:target{target_id:row.target_id})"
                                                                "CREATE (d)-[:has_bond]->(t)");
                auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
                neo4j_result_t *result = neo4j_fetch_next(results);
                buffer = 0;
                batch_str = "";
            }
            else
            {
                batch_str += "{drug_id: \"" + to_string(stored_element3.get_int32()) + "\", "
                                                                                       "target_id: \"" +
                             string(stored_element1.get_utf8().value) + "\"},";
            }
        }
    }
    if (buffer != 0)
    {
        batch_str = batch_str.substr(0, batch_str.length() - 1);
        std::string query_str = ("WITH [" + batch_str + "] AS rows "
                                                        "UNWIND rows as row "
                                                        "MATCH (d:drug{drug_id: row.drug_id}), (t:target{target_id:row.target_id})"
                                                        "CREATE (d)-[:has_bond]->(t)");
        auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
        neo4j_result_t *result = neo4j_fetch_next(results);
        buffer = 0;
        batch_str = "";
    }
    auto edge_time2 = high_resolution_clock::now();
    auto edge_ms = duration_cast<milliseconds>(edge_time2 - edge_time1);

    auto output_time1 = high_resolution_clock::now();

    mysql.mysess->sql("use Healthcare").execute();

    std::string query = "select distinct drug_id "
                        "from Prescription "
                        "where Prescription.patient_id = " +
                        to_string(patient_id);

    auto prescribed_drugs = mysql.mysess->sql(query).execute();

    for (auto row : prescribed_drugs)
    {
        std::string query_str = ("CREATE (:prescribed_drug {drug_id: \"" + to_string(row[0].get<int>()) + "\"})");
        auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
        neo4j_result_t *result = neo4j_fetch_next(results);
    }

    // cout << "d1.drug_name|d2.drug_name|common_target|" << endl;

    std::string pmatch = "p=(d1: drug)-[:has_bond]-(t: target)-[:has_bond]-(d2: drug), (pd: prescribed_drug)";
    std::string pwhere = "d1.drug_id = pd.drug_id";
    std::string preturn = "d1.drug_name, d2.drug_name, count(*) AS common_target ORDER BY common_target DESC";

    auto match_time = high_resolution_clock::now();

    query_str = ("MATCH " + pmatch + " WHERE " + pwhere + " RETURN " + preturn);
    results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);

    int nrow = 0;
    char buff[100];
    while (1)
    {
        neo4j_result_t *result = neo4j_fetch_next(results);
        if (result == NULL)
            break;

        int pos = 0;
        while (1)
        {
            auto n = neo4j_result_field(result, pos);
            if (!neo4j_is_null(n))
            {
                std::string str = neo4j_tostring(n, buff, sizeof(buff));
                // cout << buff << "|";
            }
            else
            {
                break;
            }
            pos++;
        }
        // cout << endl;
        nrow++;
    }

    auto output_time2 = high_resolution_clock::now();
    auto output_ms = duration_cast<milliseconds>(output_time2 - output_time1);
    auto match_ms = duration_cast<milliseconds>(match_time - output_time1);
    cout << v1_ms.count() << endl;
    cout << v2_ms.count() << endl;
    cout << edge_ms.count() << endl;
    cout << output_ms.count() << endl;
    cout << match_ms.count() << endl;
    cout << "TASK8: TOTAL " << nrow++ << " ROWS ARE REPORTED" << endl;
}

/**
 *  [Task9] Drug similarity (R,D=>A)
 *  Find similar drugs for a given patient X's prescribed drug
 *
 *  A: SELECT drug_id as drug, adverse_effect_list.adverse_effect_name as adverse_effect, 1 as is_adverse_effect
 *          FROM Drug
 *          UNNEST adverse_effect_list //table
 *
 *  B: A.toArray  -> (<is_adverse_effect>[drug, adverse_effect]) //array
 *  C: Cosine_similarity(B) -> (<similarity coefficient>[drug1, drug2]) //array
 *  D: SELECT * FROM C WHERE drug1 in (Select drug_id from Prescription where patient_id = X)  //array
 *
 */

void T9(int patient_id)
{
    auto mysql = mysql_connector();

    auto mongodb = mongodb_connector("Healthcare");
    auto db = mongodb.db;
    auto drug = db["drug"];

    mongocxx::pipeline p{};

    p.unwind("$adverse_effect_list");
    p.project(make_document(
        kvp("drug", "$drug_id"),
        kvp("adverse_effect", "$adverse_effect_list.adverse_effect_name"),
        kvp("is_adverse_effect", make_document(kvp("$literal", 1)))));

    mysql.mysess->sql("use Healthcare").execute();
    mysql.mysess->sql("create temporary table D2A ( "
                      " drug int, "
                      " adverse_effect varchar(100) )")
        .execute();

    auto insert2D2A = mysql.mysess->getSchema("Healthcare").getTable("D2A").insert("drug", "adverse_effect");
    auto cursor = drug.aggregate(p, mongocxx::options::aggregate{});
    int buffer = 0;
    for (auto row : cursor)
    {
        int drug_id = row["drug"].get_int32();
        string adverse_effect_id = string(row["adverse_effect"].get_utf8().value);
        int is_adverse_effect = row["is_adverse_effect"].get_int32();

        insert2D2A.values(drug_id, adverse_effect_id);
        buffer++;
        if (buffer >= BUFFER)
        {
            insert2D2A.execute();
            buffer = 0;
            insert2D2A = mysql.mysess->getSchema("Healthcare").getTable("D2A").insert("drug", "adverse_effect");
        }
    }
    if (buffer != 0)
        insert2D2A.execute();
    auto D2A = mysql.mysess->getSchema("Healthcare").getTable("D2A");

    mysql.mysess->sql("CREATE TEMPORARY TABLE Rdrug as "
                      "(SELECT t.drug, ROW_NUMBER() OVER () -1 as drug_d from "
                      "(Select distinct(drug) as drug "
                      "from D2A) as t )")
        .execute();

    mysql.mysess->sql("CREATE TEMPORARY TABLE Radverse_effect as "
                      "(SELECT t.adverse_effect, ROW_NUMBER() OVER () -1 as adverse_effect_d from "
                      "(Select distinct(adverse_effect) as adverse_effect "
                      "from D2A) as t )")
        .execute();

    mysql.mysess->sql("CREATE INDEX Rdrug on Rdrug(drug)").execute();
    mysql.mysess->sql("CREATE INDEX Radverse_effect on Radverse_effect(adverse_effect)").execute();

    auto rows = mysql.mysess->sql("SELECT drug_d, adverse_effect_d "
                                  " From Rdrug, Radverse_effect, D2A "
                                  " Where D2A.drug = Rdrug.drug "
                                  " and D2A.adverse_effect = Radverse_effect.adverse_effect ")
                    .execute();

    int dim1 = mysql.mysess->getSchema("Healthcare").getTable("Rdrug").count();
    int dim2 = mysql.mysess->getSchema("Healthcare").getTable("Radverse_effect").count();
    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST_HEALTHCARE + string(":8080")));

    conn->exec("remove(temp)");
    conn->exec("remove(drug_matrix)");
    conn->exec("remove(similarity1)");
    conn->exec("remove(similarity2)");
    conn->exec("remove(inv_norm)");
    conn->exec("remove(drug_similarity)");
    conn->exec("create array temp<drug:int64 NOT NULL, adverse_effect:int64 NOT NULL, is_adverse_effect:double NOT NULL> [i=0:" +
               to_string(dim1 * dim2 - 1) + ":0:1000000]");
    int ncell = 0;

    ScidbSchema sschema;
    sschema.attrs.push_back(ScidbAttr("drug", INT64));
    sschema.attrs.push_back(ScidbAttr("adverse_effect", INT64));
    sschema.attrs.push_back(ScidbAttr("is_adverse_effect", DOUBLE));

    shared_ptr<ScidbArrFile> coo(new ScidbArrFile(sschema));

    for (auto row : rows)
    {
        long long dim_drug = row[0].get<long>();
        long long dim_adverse_effect = row[1].get<long>();
        double is_adverse_effect = 1.0;
        /***
         *
         * Pass to SCIDB
         *
         */
        ScidbLineType line;
        line.push_back(dim_drug);
        line.push_back(dim_adverse_effect);
        line.push_back(is_adverse_effect);
        coo->add(line);
        ncell++;
    }

    conn->upload("temp", coo); // upload data to "testarray" array with coo format

    conn->exec("store(redimension(temp, <is_adverse_effect:double NOT NULL>[drug=0:" + to_string(dim1 - 1) +
               ":0:1000; adverse_effect=0:" + to_string(dim2 - 1) + ":0:1000], false),  drug_matrix)");
    conn->exec("store(spgemm(drug_matrix,transpose(drug_matrix)),similarity1)");
    conn->exec("store(project(apply(filter(similarity1,drug=drug2),result,1/sqrt(multiply)),result),inv_norm)");
    conn->exec("store(spgemm(similarity1,inv_norm),similarity2)");
    conn->exec("store(spgemm(transpose(similarity2),inv_norm),drug_similarity)");

    mysql.mysess->sql("use Healthcare").execute();

    std::string query = "select distinct Rdrug.drug_d "
                        "from Prescription, Rdrug "
                        "where Rdrug.drug = Prescription.drug_id and Prescription.patient_id = " +
                        to_string(patient_id);

    auto prescribed_drugs = mysql.mysess->sql(query).execute();

    ScidbSchema schema;
    schema.dims.push_back(ScidbDim("drug22", 0, INT32_MAX, 0, 1000000));
    schema.attrs.push_back(ScidbAttr("multiply", DOUBLE));

    // cout << "drug1\tdrug2\tsimilarity" << endl;
    int nrow = 0;
    for (auto row : prescribed_drugs)
    {
        // cout << "---------drug_idx: " << row[0].get<int>() << "----------" << endl;
        auto download = conn->download("slice(drug_similarity,drug2," + to_string(row[0].get<int>()) + ")", schema);
        auto line = download->readcell();
        while (line.size() != 0)
        {
            // cout << row[0].get<int>() << "\t";
            // cout << get<int>(line.at(0)) << "\t";
            // cout << get<double>(line.at(1)) << "\t";
            // cout << endl;
            line = download->readcell();
            nrow++;
        }
    }

    cout << "TASK9: TOTAL " << nrow++ << " ROWS ARE REPORTED" << endl;
}
