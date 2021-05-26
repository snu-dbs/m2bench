#include <iostream>
#include <nlohmann/json.hpp>
#include <any>

#include "include/Connection/ArangoConnection.h"
#include "include/Connection/OrientConnection.h"
#include "include/Connection/ScidbConnection.h"

using json = nlohmann::json;         // for convenience

int scidbTest() {
    std::cout << "SciDB Test" << std::endl;

    // sample query
    auto *conn = new ScidbConnection("192.168.0.4:8080");

    // execute array
    conn->exec("remove(siheung3)");
    conn->exec("create array siheung3<pm10:float,pm2_5:float, lat:int64, lon:int64> [i=0:272483:0:1000000]");

    // download array
    auto arr = conn->download("siheung");             // download array "siheung".
    auto data = arr->data;                                      // get actual data (type is any)
    auto *x = std::any_cast<float*>((*data)["pm10"]);        // get dense array from data
    auto lat = new int[522*522], lon = new int[522*522];        // you can create array

    // manipulate data
    int i_length = arr->schema.dims[0].end - arr->schema.dims[0].start;
    int j_length = arr->schema.dims[1].end - arr->schema.dims[1].start;
    for (int i = 0; i < i_length; i++) {
        for (int j = 0; j < j_length; j++) {
            lat[i*j_length+j] = i, lon[i*j_length+j] = j;
        }
    }
    x[261 + 261 * j_length] = 100000;        // arbitrary manipulation

    // save it to "data'
    (*data)["pm10"] = x, (*data)["lat"] = lat, (*data)["lon"] = lon;

    // upload array. Note that SciDB only accepts 1-D array.
    conn->upload("siheung3", data);         // upload data to "siheung3" array with dense format

    // you should redimension uploaded array
    conn->exec("redimension(siheung3, <pm10:float,pm2_5:float> [lat=0:521:0:1000; lon=0:521:0:1000])");

    return 0;
}


int arangoTest() {
    std::cout << "ArangoDB Test" << std::endl;

    // sample query
    auto conn = new ArangoConnection("147.46.125.23:8529", "", "root", "dbs402418!");
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


int orientTest() {
    std::cout << "OrientDB Test" << std::endl;

    // sample query
    auto conn = new OrientConnection("147.46.125.23:2480", "unibench", "root", "dbs402418!");
    auto cursor = conn->exec("SELECT * FROM customer");

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
    scidbTest();

    return 0;
}