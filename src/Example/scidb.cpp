//
// Created by Kyoseung Koo on 2021/05/27.
//

#include <iostream>
#include <nlohmann/json.hpp>
#include <any>

#include "include/Connection/ScidbConnection.h"

using json = nlohmann::json;         // for convenience


int coo() {
    std::cout << "COO Test" << std::endl;

    // connection
    unique_ptr<ScidbConnection> conn(new ScidbConnection("192.168.0.4:8080"));

    // execute array
    conn->exec("remove(testarray)");
    conn->exec("create array testarray<f1:float, d2:double, i3:int32, s4:string> [i=0:*:0:1000000]");

    // Upload data with COO format
    // In COO, you should pass a line to ScidbData as vector<any>.
    // Note that SciDB only accepts 1-D array.

    // construct ScidbData
    ScidbData data;
    for (int i = 0; i < 1024; i++) {
        vector<any> line;
        line.push_back((float) (i * 0.1));
        line.push_back((double) (i * 0.1));
        line.push_back(i);
        line.push_back(std::to_string(i));
        data.add(line);
    }

    // upload array.
    conn->upload("testarray", data);         // upload data to "testarray" array with coo format

    // Download data with COO format
    // Download array
    auto download = conn->download("testarray");
    // Iterate data
    // Dimensions first and attributes later. All data have the order that described in schema.
    // Example of testarray:
    //      schema is <f1:float, d2:double, i3:int32, s4:string> [i=0:*:0:1000000]
    //      each line will be vector<any> and types are int, float, double, int, and string (i, f1, d2, i3, s4).
    cout << "i\tf1\td2\ti3\ts4\n";
    for (auto it = download.data.begin(); it != download.data.end(); it++) {
        vector<any> line = *it;
        cout << any_cast<int>(line.at(0)) << "\t";                      //
        cout << any_cast<float>(line.at(1)) << "\t";
        cout << any_cast<double>(line.at(2)) << "\t";
        cout << any_cast<int>(line.at(3)) << "\t";
        cout << any_cast<std::string>(line.at(4)) << "\t";
        cout << endl;
    }

    return 0;
}

int scidbTest(){
    std::cout << "SciDB Test" << std::endl;

    coo();
}

