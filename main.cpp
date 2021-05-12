#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iomanip>

#include "include/Connection/ArangoConnection.h"
#include "include/Connection/OrientConnection.h"


using json = nlohmann::json;         // for convenience


int arangoTest() {
    std::cout << "ArangoDB Test" << std::endl;

    // sample query
    auto *conn = new ArangoConnection("147.46.125.23:8529", "", "root", "");
    auto *cursor = conn->exec("FOR doc IN Customer RETURN doc");

    int count = 0;
    while (cursor->hasNext()) {
        json row = cursor->next();
        std::cout << to_string(row) << std::endl;
        count ++;
        // TODO:
    }

    std::cout << count << std::endl;
    delete conn;

    return 0;
}


int orientTest() {
    std::cout << "OrientDB Test" << std::endl;

    // sample query
    auto *conn = new OrientConnection("147.46.125.23:2480", "unibench", "root", "");
    auto *cursor = conn->exec("SELECT * FROM customer");

    int count = 0;
    while (cursor->hasNext()) {
        json row = cursor->next();
        std::cout << to_string(row) << std::endl;
        count ++;
        // TODO:
    }

    std::cout << count << std::endl;
    delete conn;

    return 0;
}


int main() {
    arangoTest();
    orientTest();

    return 0;
}
//    // orientdb
//    std::cout << "orientdb connect" << std::endl;
//    cpr::Response r3 = cpr::Get(cpr::Url{"http://147.46.125.23:2480/connect/unibench/"},
//                                 cpr::Authentication{"root", ""});
//    std::cout << r3.status_code << std::endl;                  // 200
//    std::cout << r3.header["content-type"] << std::endl;       // application/json; charset=utf-8
//    std::cout << r3.text << std::endl;                         // JSON text string
//
//    std::cout << "orientdb query" << std::endl;
//    cpr::Response r4 = cpr::Get(cpr::Url{spaceReplace("http://147.46.125.23:2480/query/unibench/sql/select 1/999999999/*:-1/")},
//                                cpr::Authentication{"root", ""});
//    std::cout << r4.status_code << std::endl;                  // 200
//    std::cout << r4.header["content-type"] << std::endl;       // application/json; charset=utf-8
//    std::cout << r4.text << std::endl;                         // JSON text string