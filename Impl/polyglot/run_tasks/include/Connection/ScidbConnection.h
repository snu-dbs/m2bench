//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_SCIDBCONNECTION_H
#define M2BENCH_AO_SCIDBCONNECTION_H

#include <string>
#include <cpr/cpr.h>
#include <any>

#include "Connection.h"
#include "ScidbPrimitives.h"

class ScidbConnection: Connection {
public:
    explicit ScidbConnection(const string& url);
    unique_ptr<Cursor> exec(string query) override;
    unique_ptr<ScidbArr> download(const string& arrayName);
    unique_ptr<ScidbArr> download(const string& query, const ScidbSchema& schema);
    void upload(const string& arrayName, const shared_ptr<ScidbArr> arr);
};

#endif //M2BENCH_AO_SCIDBCONNECTION_H
