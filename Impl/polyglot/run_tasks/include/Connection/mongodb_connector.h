/*
 * Created: May 3, 2021
 * Updated: February 2025
 */

#ifndef M2BENCH_MONGODB_CONNECTOR_H
#define M2BENCH_MONGODB_CONNECTOR_H

#include <iostream>
#include <string>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#define MONGODB_HOST "127.0.0.1"
#define MONGODB_ID ""
#define MONGODB_PW ""

class mongodb_connector {
public:
    static mongocxx::instance instance;

    mongocxx::database db;
    std::unique_ptr<mongocxx::client> client;

    explicit mongodb_connector(const std::string& dbname = "mxmdb")
    {
        std::string uri_string = MONGODB_ID[0] == '\0' && MONGODB_PW[0] == '\0'
            ? "mongodb://" + std::string(MONGODB_HOST) + ":27017/" + dbname
            : "mongodb://" + std::string(MONGODB_ID) + ":" + std::string(MONGODB_PW) + "@" + MONGODB_HOST + ":27017/?authSource=" + dbname;

        client = std::make_unique<mongocxx::client>(mongocxx::uri(uri_string));
        db = (*client)[dbname];
    }
};

#endif // M2BENCH_MONGODB_CONNECTOR_H
