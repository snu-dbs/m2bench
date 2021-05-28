//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_ARANGOCONNECTION_H
#define M2BENCH_AO_ARANGOCONNECTION_H

#include <string>
#include <cpr/cpr.h>
#include "Connection.h"

class ArangoConnection: Connection {
public:
    ArangoConnection(const string& url, string db, const string& username, const string& password);
    unique_ptr<Cursor> exec(string query) override;
    static unique_ptr<ArangoConnection> getConnection(const std::string& url,
                                                      const std::string& db,
                                                      const std::string& username,
                                                      const std::string& password);

private:
    std::string jwt;
};


#endif //M2BENCH_AO_ARANGOCONNECTION_H
