/**
 * Created: May 3, 2021
 * Updated: Feburary 2025
 */

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "Connection/mysql_connector.h"
#include "Connection/mongodb_connector.h"
#include "Connection/ScidbConnection.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

#define SCIDB_HOST_HEALTHCARE "127.0.0.1"
#define BUFFER 1000

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
*/

/**
 *  [Task9] Drug similarity (R, D => A)
 *  Find similar drugs for a given patient X's prescribed drug.
 *
 *  A: SELECT drug_id AS drug, adverse_effect_list.adverse_effect_name AS adverse_effect, 1 AS is_adverse_effect
 *          FROM Drug
 *          UNNEST adverse_effect_list // Relational
 *
 *  B: A.toArray(<is_adverse_effect>[drug, adverse_effect]) // Array
 *  C: Cosine_similarity(B)(<similarity coefficient>[drug1, drug2]) // Array
 *  D: SELECT * FROM C WHERE drug1 IN (SELECT drug_id FROM Prescription WHERE patient_id = X) // Array
 */
void T9(int patient_id)
{
    auto mysql = mysql_connector();
    mysql.mysess->sql("USE Healthcare").execute();
    mysql.mysess->sql("CREATE TEMPORARY TABLE D2A ("
                      "drug INT,"
                      "adverse_effect VARCHAR(100))").execute();

    auto insert2D2A = mysql.mysess->getSchema("Healthcare")
                                    .getTable("D2A")
                                    .insert("drug", "adverse_effect");

    mongodb_connector mongodb("Healthcare");
    auto drug = mongodb.db["drug"];

    mongocxx::pipeline stages;
    stages.unwind("$adverse_effect_list");
    stages.project(make_document(
        kvp("drug", "$drug_id"),
        kvp("adverse_effect", "$adverse_effect_list.adverse_effect_name"),
        kvp("is_adverse_effect", make_document(kvp("$literal", 1)))
    ));
    auto cursor = drug.aggregate(stages);

    int buffer = 0;
    for (auto row : cursor) {
        int drug_id = row["drug"].get_int32();
        string adverse_effect_id = string(row["adverse_effect"].get_string().value);
        int is_adverse_effect = row["is_adverse_effect"].get_int32();

        insert2D2A.values(drug_id, adverse_effect_id);
        
        buffer++;
        if (buffer >= BUFFER) {
            insert2D2A.execute();
            insert2D2A = mysql.mysess->getSchema("Healthcare")
                                        .getTable("D2A")
                                        .insert("drug", "adverse_effect");

            buffer = 0;
        }
    }
    if (buffer > 0)
        insert2D2A.execute();

    auto D2A = mysql.mysess->getSchema("Healthcare").getTable("D2A");

    mysql.mysess->sql("CREATE TEMPORARY TABLE Rdrug AS ("
                      "SELECT t.drug, ROW_NUMBER() OVER () -1 AS drug_d FROM ("
                      "SELECT DISTINCT(drug) AS drug "
                      "FROM D2A) AS t)").execute();
                    //   "FROM D2A ORDER BY drug) AS t)").execute(); // for answer validation

    mysql.mysess->sql("CREATE TEMPORARY TABLE Radverse_effect AS ("
                      "SELECT t.adverse_effect, ROW_NUMBER() OVER () -1 AS adverse_effect_d FROM ("
                      "SELECT DISTINCT(adverse_effect) AS adverse_effect "
                      "FROM D2A) AS t)").execute();
                    //   "FROM D2A ORDER BY adverse_effect) AS t)").execute(); // for answer validation

    mysql.mysess->sql("CREATE INDEX Rdrug ON Rdrug(drug)").execute();
    mysql.mysess->sql("CREATE INDEX Radverse_effect ON Radverse_effect(adverse_effect)").execute();

    auto rows = mysql.mysess->sql("SELECT drug_d, adverse_effect_d "
                                  "FROM Rdrug, Radverse_effect, D2A "
                                  "WHERE D2A.drug = Rdrug.drug "
                                  "AND D2A.adverse_effect = Radverse_effect.adverse_effect").execute();

    int dim1 = mysql.mysess->getSchema("Healthcare").getTable("Rdrug").count();
    int dim2 = mysql.mysess->getSchema("Healthcare").getTable("Radverse_effect").count();

    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST_HEALTHCARE + string(":8080")));

    conn->exec("remove(temp)");
    conn->exec("remove(drug_matrix)");
    conn->exec("remove(similarity1)");
    conn->exec("remove(similarity2)");
    conn->exec("remove(inv_norm)");
    conn->exec("remove(drug_similarity)");
    conn->exec("create array temp<drug:int64 NOT NULL, adverse_effect:int64 NOT NULL, is_adverse_effect:double NOT NULL> [i=0:"
                + to_string(dim1 * dim2 - 1) + ":0:1000000]");

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

        /**
         * Pass to SCIDB
         */
        ScidbLineType line;
        line.push_back(dim_drug);
        line.push_back(dim_adverse_effect);
        line.push_back(is_adverse_effect);

        coo->add(line);
    }

    /* Upload data to temp array with coo format */
    conn->upload("temp", coo);

    conn->exec("store(redimension(temp, <is_adverse_effect:double NOT NULL>[drug=0:" + to_string(dim1 - 1) +
               ":0:1000; adverse_effect=0:" + to_string(dim2 - 1) + ":0:1000], false),  drug_matrix)");
    conn->exec("store(spgemm(drug_matrix,transpose(drug_matrix)),similarity1)");
    conn->exec("store(project(apply(filter(similarity1,drug=drug2),result,1/sqrt(multiply)),result),inv_norm)");
    conn->exec("store(spgemm(similarity1,inv_norm),similarity2)");
    conn->exec("store(spgemm(transpose(similarity2),inv_norm),drug_similarity)");

    mysql.mysess->sql("use Healthcare").execute();
    std::string query = "SELECT DISTINCT Rdrug.drug_d "
                        "FROM Prescription, Rdrug "
                        "WHERE Rdrug.drug = Prescription.drug_id AND Prescription.patient_id = "
                        + to_string(patient_id);
                        // + " ORDER BY Rdrug.drug_d"; // for answer validation
    auto prescribed_drugs = mysql.mysess->sql(query).execute();

    ScidbSchema schema;
    schema.dims.push_back(ScidbDim("result", 0, INT32_MAX, 0, 1000000));
    schema.attrs.push_back(ScidbAttr("similarity", DOUBLE));

    // std::ofstream csv_file("/tmp/t9.csv");
    // csv_file << "drug_1,drug_2,val\n";

    // int nrow = 0;
    // for (auto row : prescribed_drugs) {
    //     int drug1 = row[0].get<int>();
    //     auto download = conn->download("slice(drug_similarity,drug2," + to_string(drug1) + ")", schema);
        
    //     auto line = download->readcell();
    //     while (line.size() != 0) {
            // int drug2 = std::get<int>(line.at(0));
            // double similarity = std::get<double>(line.at(1));
            // csv_file << drug1 << "," << drug2 << "," << similarity << "\n";

            // line = download->readcell();
            // nrow++;
        // }
    // }

    /* save result matrix to csv */
    // csv_file.close();

    // cout << "[TASK 9]: TOTAL " << nrow++ << " ROWS ARE REPORTED" << endl;
    cout << "[TASK 9]: TASK COMPLETED" << endl;
}
