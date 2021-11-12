//
// Created by mxmdb on 21. 5. 3..
//

/**
 * http://mongocxx.org/mongocxx-v3/installation/
 */

#ifndef M2BENCH_AO_MYSQL_CONNECTOR_H
#define M2BENCH_AO_MYSQL_CONNECTOR_H

#include <mysqlx/xdevapi.h>
#include <iostream>

#define MYSQL_HOST    "192.168.0.1"
#define MYSQL_ID      ""
#define MYSQL_PW      ""

using namespace std;
class mysql_connector {
public:
    mysqlx::Session* mysess;

    mysql_connector(){
        mysess =  new mysqlx::Session(MYSQL_HOST, 33060, MYSQL_ID, MYSQL_PW);
    }

    ~mysql_connector(){
        delete mysess;

    }

};


#endif //M2BENCH_MYSQL_CONNECTOR_H
