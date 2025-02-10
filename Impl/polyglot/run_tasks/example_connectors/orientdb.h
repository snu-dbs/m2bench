//
// Created by Kyoseung Koo on 2021/05/27.
//

#include <iostream>
#include <nlohmann/json.hpp>
#include <any>

#include "include/Connection/ArangoConnection.h"
#include "include/Connection/OrientConnection.h"
#include "include/Connection/ScidbConnection.h"

using json = nlohmann::json;         // for convenience

#define ORIENT_HOST    "192.168.0.1"
#define ORIENT_ID      ""
#define ORIENT_PW      ""

int orientTest() {
    std::cout << "OrientDB Test" << std::endl;

    // sample query
    auto conn = new OrientConnection(ORIENT_HOST + ":2480", "u", ORIENT_ID, ORIENT_PW);
    auto cursor = conn->exec("SELECT * FROM customer");

    int count = 0;
    while (cursor->hasNext()) {
        json row = cursor->next();
        std::cout << to_string(row) << std::endl;
        count++;
        // TODO:
    }

    std::cout << count << std::endl;
    delete conn;
}
