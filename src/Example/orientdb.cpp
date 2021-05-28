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

int orientTest() {
    std::cout << "OrientDB Test" << std::endl;

    // sample query
    auto conn = new OrientConnection("147.46.125.23:2480", "unibench", "root", "dbs402418!");
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
