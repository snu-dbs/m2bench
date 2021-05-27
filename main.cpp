#include <iostream>
#include <nlohmann/json.hpp>
#include <any>

#include "include/Connection/ArangoConnection.h"
#include "include/Connection/OrientConnection.h"
#include "include/Connection/ScidbConnection.h"

using json = nlohmann::json;         // for convenience

int dense() {
    std::cout << "Dense Test" << std::endl;

    // connection
    unique_ptr<ScidbConnection> conn(new ScidbConnection("192.168.0.4:8080"));

    // execute array
    conn->exec("remove(siheung3)");
    conn->exec("create array siheung3<pm10:float,pm2_5:float, lat:int64, lon:int64> [i=0:272483:0:1000000]");

    // Download data with Dense format
    // download array
    auto arr = conn->download("siheung");             // download array "siheung".
    auto data = arr.data;                                       // actual ScidbData (dense array)
    auto *x = std::any_cast<float*>(data["pm10"]);           // get dense array from data
    auto lat = new int[522*522], lon = new int[522*522];        // you can create your own array

    // manipulate data
    int i_length = arr.schema.dims[0].end - arr.schema.dims[0].start;
    int j_length = arr.schema.dims[1].end - arr.schema.dims[1].start;
    for (int i = 0; i < i_length; i++) {
        for (int j = 0; j < j_length; j++) {
            lat[i*j_length+j] = i, lon[i*j_length+j] = j;
        }
    }
    x[261 + 261 * j_length] = 100000;        // arbitrary manipulation

    // save it to "data'
    data["pm10"] = x, data["lat"] = lat, data["lon"] = lon;

    // upload array. Note that SciDB only accepts 1-D array.
    conn->upload("siheung3", data);         // upload data to "siheung3" array with dense format

    // you should redimension uploaded array
    conn->exec("redimension(siheung3, <pm10:float,pm2_5:float> [lat=0:521:0:1000; lon=0:521:0:1000])");

    // you must free your pointers
    delete[] lat; delete[] lon;

    return 0;
}

int coo() {
    std::cout << "COO Test" << std::endl;

    // connection
    unique_ptr<ScidbConnection> conn(new ScidbConnection("192.168.0.4:8080"));

    // execute array
    conn->exec("remove(testarray)");
    conn->exec("create array testarray<f1:float, d2:double, i3:int32, s4:string> [i=0:*:0:1000000]");

    // Upload data with COO format
    // In COO, you should pass ScidbData as map<string, vector<any>>.
    // Note that SciDB only accepts 1-D array.
    // Making data
    vector<float> f1; vector<double> d2; vector<int> i3; vector<string> s4;
    for (int i = 0; i < 1024; i++) {
        f1.push_back((float) i * 0.1); d2.push_back((double) i * 0.1); i3.push_back(i); s4.push_back(to_string(i));
    }

    // construct ScidbData
    ScidbData data;
    data["f1"] = f1, data["d2"] = d2; data["i3"] = i3, data["s4"] = s4;
    // If you consider the performance, use std::move() instead of naked copy

    // upload array.
    conn->upload("testarray", data, COO);         // upload data to "siheung3" array with dense format

    // Download data with COO format
    // In COO, you will get ScidbData as map<string, vector<any>>.
    // Download array
    auto download = conn->download("testarray", COO);
    // Conversion
    auto ff1 = any_cast<vector<float>>(download.data["f1"]);
    auto dd2 = any_cast<vector<double>>(download.data["d2"]);
    auto ii3 = any_cast<vector<int>>(download.data["i3"]);
    auto ss4 = any_cast<vector<string>>(download.data["s4"]);

    // Iterate data
    cout << "f1\tff1\td2\tdd2\ti3\tii3\ts4\tss4\n";
    for (int i = 0; i < 1024; i++) {
        cout << f1.at(i) << "\t" << ff1.at(i) << "\t";
        cout << d2.at(i) << "\t" << dd2.at(i) << "\t";
        cout << i3.at(i) << "\t" << ii3.at(i) << "\t";
        cout << s4.at(i) << "\t" << ss4.at(i) << "\t";
        cout << endl;
    }

    return 0;
}

int scidbTest(){
    std::cout << "SciDB Test" << std::endl;

    dense();
    coo();
}


int arangoTest() {
    std::cout << "ArangoDB Test" << std::endl;

    // sample query
    auto conn = new ArangoConnection("147.46.125.23:8529", "", "root", "");
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
    auto conn = new OrientConnection("147.46.125.23:2480", "unibench", "root", "");
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
//    arangoTest();
//    orientTest();
    scidbTest();

    return 0;
}