//
// Created by mxmdb on 21. 8. 11..
//

#include <string>
#include <tuple>

#include <nlohmann/json.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "Connection/mysql_connector.h"
#include "Connection/mongodb_connector.h"
#include "Connection/ScidbConnection.h"
#include "Connection/neo4j_connector.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::milliseconds;

using Json = nlohmann::json;

#define BUFFER 1000

void Measure_MYSQL_Latency(int limit)
{

    auto t1 = high_resolution_clock::now();
    auto mysql = mysql_connector();

    auto t2 = high_resolution_clock::now();
    auto Products = mysql.mysess->getSchema("Disaster").getTable("Gps");
    auto res = Products.select("*").execute();

    auto t3 = high_resolution_clock::now();

    int nrow = 0;
    for (auto val : res)
    {
        nrow++;
    }
    auto t4 = high_resolution_clock::now();
    cout << limit << endl;
    cout << duration_cast<microseconds>(t2 - t1).count() << endl;
    cout << duration_cast<microseconds>(t3 - t2).count() << endl;
    cout << duration_cast<microseconds>(t4 - t3).count() << endl;
}