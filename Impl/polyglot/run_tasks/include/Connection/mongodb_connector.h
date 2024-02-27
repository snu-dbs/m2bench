//
// Created by mxmdb on 21. 5. 3..
//

#ifndef M2BENCH_AO_MONGODB_CONNECTOR_H
#define M2BENCH_AO_MONGODB_CONNECTOR_H

#include <iostream>
#include <string>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#define MONGODB_HOST "127.0.0.1"
#define MONGODB_ID ""
#define MONGODB_PW ""

using namespace mongocxx;

class mongodb_connector
{

public:
    static int init;
    static mongocxx::instance inst;
    mongocxx::database db;
    mongocxx::client *client;
    mongodb_connector(std::string dbname = "mxmdb")
    {
        std::string id = MONGODB_ID;
        std::string pwd = MONGODB_PW;

        if (init == 1004)
        {
            mongocxx::instance instance{}; // This should be done only once.
            init = 1;
        }

        std::string uri_string_string;
        if (id == "" && pwd == "")
        {
            uri_string_string = std::string("mongodb://") + MONGODB_HOST + ":27017/";
        }
        else
        {
            uri_string_string = std::string("mongodb://") + id + ":" + pwd + "@" + MONGODB_HOST + ":27017/?authSource=" + dbname;
        }
        mongocxx::uri uri(uri_string_string);
        client = new mongocxx::client(uri);
        db = (*client)[dbname];
    }
    ~mongodb_connector()
    {
        delete client;
    }
};

#endif // M2BENCH_MONGODB_CONNECTOR_H
