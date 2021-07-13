//
// Created by Kyoseung Koo on 2021/05/05.
//

#ifndef M2BENCH_AO_SCIDBSESSION_H
#define M2BENCH_AO_SCIDBSESSION_H

#include <string>
#include <any>
#include <cpr/cpr.h>
#include <sys/stat.h>

#include "Session.h"
#include "Connection/ScidbPrimitives.h"


class ScidbSession : public Session {
public:
    explicit ScidbSession(const string& url, bool isFileBased = true);
    ~ScidbSession();

    void exec(const string& query, bool save=false);
    unique_ptr<ScidbArr> download(const string& query);
    unique_ptr<ScidbArr> download(const string& query, const ScidbSchema& schema);
    void upload(const string& arrayName, shared_ptr<ScidbArr> data);

    vector<json> fetch() override;
    bool isDone() override;

private:
    bool isFileBased = true;        // If true, the session maintain the array in a file.

    vector<string> tmpfiles;        // Temporary files. The session maintains it so that it can be deleted.

    string url, sessionId, fileId;

    // Memory-based
    string push(string data);
    string pull();

    // File-based
    string pushFromFile(string filename);
    string pullToFile();

    // Other functions
    ScidbSchema schema(const string& arrayName);
    static ScidbSchema parsingSchema(const string& basicString);
};


#endif //M2BENCH_AO_SCIDBSESSION_H
