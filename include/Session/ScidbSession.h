//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_SCIDBSESSION_H
#define M2BENCH_AO_SCIDBSESSION_H

#include <string>
#include <any>
#include <cpr/cpr.h>

#include "Session.h"
#include "Connection/ScidbPrimitives.h"


class ScidbSession : public Session {
public:
    explicit ScidbSession(const string& url);
    ~ScidbSession();

    void exec(const string& query, bool save=false);
    ScidbArr download(const string& query);
    void upload(const string& arrayName, const ScidbArr& data);

    vector<json> fetch() override;
    bool isDone() override;

private:
    string url, sessionId, fileId;

    string push(string data);
    string pull();
    ScidbSchema schema(const string& arrayName);

    static ScidbSchema parsingSchema(const string& basicString);
};


#endif //M2BENCH_AO_SCIDBSESSION_H
