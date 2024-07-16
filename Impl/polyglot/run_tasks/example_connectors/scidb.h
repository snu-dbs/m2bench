//
// Created by Kyoseung Koo on 2021/05/27.
//

#include <iostream>
#include <nlohmann/json.hpp>
#include <any>

#include "include/Connection/ScidbConnection.h"

using json = nlohmann::json;         // for convenience
using namespace std;

#define SCIDB_HOST    "192.168.0.1"

void test_download_query() {
    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST + string(":8080")));

    ScidbSchema schema;
    schema.dims.push_back(ScidbDim("i", 0, INT32_MAX, 0, 1000000));
    schema.attrs.push_back(ScidbAttr("pm10_avg_max", FLOAT));

    auto arr = conn->download("aggregate(filter(t14t1, abs(pm10_avg-12165.2) < 1), min(pm10_avg))", schema);
    auto value = arr->readcell();
    while (value.size() != 0) {
        cout << get<int>(value.at(0)) << " " << get<float>(value.at(1)) << endl;
        value = arr->readcell();
    }
}

void test_coo_upload_and_download() {
    std::cout << "COO Test" << std::endl;

    // connection
    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST + string(":8080")));

    // execute array
    conn->exec("remove(testarray)");
    conn->exec("create array testarray<f1:float, d2:double, i3:int32, s4:string> [i=0:*:0:1000000]");

    // Upload data with COO format
    // In COO, you should pass a line to ScidbData as vector<any>.
    // Note that SciDB only accepts 1-D array.
    ScidbSchema schema;
    schema.attrs.push_back(ScidbAttr("f1", FLOAT)); 
    schema.attrs.push_back(ScidbAttr("d2", DOUBLE));
    schema.attrs.push_back(ScidbAttr("i3", INT32));
    schema.attrs.push_back(ScidbAttr("s4", STRING));

    // construct ScidbData
    shared_ptr<ScidbArrFile> arr(new ScidbArrFile(schema));
    for (int i = 0; i < 1024; i++) {
        ScidbLineType line;
        line.push_back((float) (i * 0.1));
        line.push_back((double) (i * 0.1));
        line.push_back(i);
        line.push_back(std::to_string(i));
        arr->add(line);
    }

    // upload array.
    conn->upload("testarray", arr);         // upload data to "testarray" array with coo format

    // Download data with COO format
    // Download array
    auto download = conn->download("testarray");
    // Iterate data
    // Dimensions first and attributes later. All data have the order that described in schema.
    // Example of testarray:
    //      schema is <f1:float, d2:double, i3:int32, s4:string> [i=0:*:0:1000000]
    //      each line will be vector<any> and types are int, float, double, int, and string (i, f1, d2, i3, s4).
    cout << "i\tf1\td2\ti3\ts4\n";
    auto line = download->readcell();
    while (line.size()!= 0) {
        cout << get<int>(line.at(0)) << "\t";                      //
        cout << get<float>(line.at(1)) << "\t";
        cout << get<double>(line.at(2)) << "\t";
        cout << get<int>(line.at(3)) << "\t";
        cout << get<std::string>(line.at(4)) << "\t";
        cout << endl;

        line = download->readcell();
    }
}

void test_coo_siheung_download_and_upload() {
    std::cout << "COO FineDust Test" << std::endl;

    // connection
    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST + string(":8080")));

    // download siheung
    auto download = conn->download("siheung");

    ScidbSchema schema;
    schema.attrs.push_back(ScidbAttr("lat_idx", INT32)); 
    schema.attrs.push_back(ScidbAttr("lon_idx", INT32));
    schema.attrs.push_back(ScidbAttr("pm10", FLOAT));
    schema.attrs.push_back(ScidbAttr("pm2_5", FLOAT));
    shared_ptr<ScidbArrFile> doubledUploadArr(new ScidbArrFile(schema));

    cout << "lat_idx\tlon_idx\tpm10\tpm2_5\t\n";
    auto line = download->readcell();
    while( line.size()!= 0){
        cout << get<int>(line.at(0)) << "\t";
        cout << get<int>(line.at(1)) << "\t";
        cout << get<float>(line.at(2)) << "\t";                      //
        cout << get<float>(line.at(3)) << "\t";
        cout << endl;

        ScidbLineType row;
        row.push_back(line.at(0));
        row.push_back(line.at(1));
        row.push_back(get<float>(line.at(2)) * 2);
        row.push_back(get<float>(line.at(3)) * 2);
        doubledUploadArr->add(row);
        line = download->readcell();
    }

    // execute array
    conn->exec("remove(siheung2)");
    conn->exec("remove(siheung3)");
    conn->exec("create array siheung2<lat_idx:int32, lon_idx:int32, pm10:float, pm2_5:float> [i=0:*:0:1000000]");

    // upload data
    conn->upload("siheung2", doubledUploadArr);

    // redim
    conn->exec("store(redimension(siheung2, <pm10:float, pm2_5:float>[lat_idx=0:522:0:1000000; lon_idx=0:522:0:1000000]), siheung3)");
}

void test_coo_finedust_download() {
    std::cout << "COO FineDust Test" << std::endl;

    // connection
    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST + string(":8080")));

    // download siheung
    auto download = conn->download("finedust");

    cout << "lat_idx\tlon_idx\tpm10\tpm2_5\t\n";
    auto line = download->readcell();
    while( line.size()!= 0){
        cout << get<int>(line.at(0)) << "\t";                      //
        cout << get<int>(line.at(1)) << "\t";
        cout << get<int>(line.at(2)) << "\t";
        cout << get<double>(line.at(3)) << "\t";
        cout << get<double>(line.at(4)) << "\t";
        cout << endl;

        line = download->readcell();
    }
//    return 0;
}


int scidbTest(){
    std::cout << "SciDB Test" << std::endl;

    // test_download_query();
    test_coo_upload_and_download();
    test_coo_siheung_download_and_upload();
    // test_coo_finedust_download();
}

