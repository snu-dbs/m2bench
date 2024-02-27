//
// Created by mxmdb on 21. 5. 3..
//

#ifndef M2BENCH_AO_NEO4J_CONNECTOR_H
#define M2BENCH_AO_NEO4J_CONNECTOR_H

#include "neo4j-client.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;

#define NEO4J_HOST "127.0.0.1"
#define NEO4J_ID ""
#define NEO4J_PW ""

class neo4j_connector
{
public:
    neo4j_connection_t *conn;

    ~neo4j_connector()
    {
        if (conn != NULL)
        {
            neo4j_close(conn);
        }
        neo4j_client_cleanup();
    }

    neo4j_connector()
    {
        string info = (string("neo4j://") + NEO4J_ID + ":" + NEO4J_PW + "@" + NEO4J_HOST + ":3308");
        conn = neo4j_connect(info.c_str(), NULL, NEO4J_INSECURE);
        if (conn == NULL)
        {
            neo4j_perror(stderr, errno, "Connection failed");
        }
    }
};

#endif // M2BENCH_NEO4J_CONNECTOR_H
