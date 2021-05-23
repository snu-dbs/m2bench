#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include "include/Connection/ArangoConnection.h"
#include "src/Example/neo4j_example.h"
#include "src/Example/mysql_example.h"
#include "src/Example/mongodb_example.h"
#include "src/Scenarios/PolyglotDB/Healthcare_Tasks.h"

using json = nlohmann::json;         // for convenience


int main() {
    // sample query
    auto *conn = new ArangoConnection("147.46.125.23:8529", "", "root", "dbs402418!");
//    auto *cursor = conn->exec("LET x = SLEEP(1) RETURN 1");
    auto *cursor = conn->exec("FOR doc IN Customer RETURN doc");
    int a = 0;
    while (cursor->hasNext()) {
        a++;
        json row = cursor->next();
        std::cout << row << std::endl;
        // TODO:
    }

    std::cout << a << std::endl;
    delete conn;


    mysql_example();
    neo4j_example();
    mongodb_example();

    /* M2Bench */

    T5(9);
   return 0;
}

//int main(int argc, char** argv) {
//    // arangodb
//    std::cout << "arango auth" << std::endl;
//
//
//    std::cout << "arango send query" << std::endl;
//    json test = R"({"query" : "LET x = SLEEP(1) RETURN 1", "count" : true, "batchSize" : 2})"_json;
//    cpr::Response r2 = cpr::Post(cpr::Url{"http://147.46.125.23:8529/_api/cursor"},
//                                cpr::Header{{"accept", "application/json"}, {"Authorization", "bearer " + j["jwt"].get<std::string>()}},
//                                cpr::Body{test.dump()});
//    std::cout << r2.status_code << std::endl;                  // 200
//    std::cout << r2.header["content-type"] << std::endl;       // application/json; charset=utf-8
//    std::cout << r2.text << std::endl;                         // JSON text string
//
//    // orientdb
//    std::cout << "orientdb connect" << std::endl;
//    cpr::Response r3 = cpr::Get(cpr::Url{"http://147.46.125.23:2480/connect/unibench/"},
//                                 cpr::Authentication{"root", "dbs402418!"});
//    std::cout << r3.status_code << std::endl;                  // 200
//    std::cout << r3.header["content-type"] << std::endl;       // application/json; charset=utf-8
//    std::cout << r3.text << std::endl;                         // JSON text string
//
//    std::cout << "orientdb query" << std::endl;
//    cpr::Response r4 = cpr::Get(cpr::Url{spaceReplace("http://147.46.125.23:2480/query/unibench/sql/select 1/999999999/*:-1/")},
//                                cpr::Authentication{"root", "dbs402418!"});
//    std::cout << r4.status_code << std::endl;                  // 200
//    std::cout << r4.header["content-type"] << std::endl;       // application/json; charset=utf-8
//    std::cout << r4.text << std::endl;                         // JSON text string
//}
