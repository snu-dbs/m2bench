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

#define ARANGO_HOST    "192.168.0.1"
#define ARANGO_ID      ""
#define ARANGO_PW      ""

int arangoTest() {
    std::cout << "ArangoDB Test" << std::endl;

    // sample query
    auto conn = new ArangoConnection(ARANGO_HOST + ":8529", "", ARANGO_ID, ARANGO_PW);
    auto cursor = conn->exec("FOR doc IN Customer RETURN doc");

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

