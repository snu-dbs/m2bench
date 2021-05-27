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
    unique_ptr<ScidbArr> download(const string& arrayName, ScidbDataFormat format=DENSE);
    void upload(const string& arrayName, shared_ptr<ScidbData> data, ScidbDataFormat format=DENSE);

    vector<json> fetch() override;
    bool isDone() override;

private:
    string url, sessionId, fileId;

    string push(string data);
    string pull();
    ScidbSchema schema(const string& arrayName);

    static string conversionDenseScidbDataToTsv(const shared_ptr<ScidbData>& pMap, const ScidbSchema& schema);
    static string conversionCooScidbDataToTsv(const shared_ptr<ScidbData>& pMap, const ScidbSchema& schema);
    static unique_ptr<ScidbData> conversionTsvToDenseScidbData(const string& basicString, const ScidbSchema& schema);
    static unique_ptr<ScidbData> conversionTsvToCooScidbData(const string& basicString, const ScidbSchema& schema);

    static ScidbSchema parsingSchema(const string& basicString);

};


#endif //M2BENCH_AO_SCIDBSESSION_H
