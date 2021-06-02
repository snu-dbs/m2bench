//
// Created by Kyoseung Koo on 2021/05/05.
//

#include <utility>

#include "Connection/ScidbConnection.h"
#include "Session/ScidbSession.h"

ScidbConnection::ScidbConnection(const string& url): Connection(url, "", "", "") {
    cpr::Response r = cpr::Get(cpr::Url{"http://" + url});
    if (r.status_code != 200) assert("Creating ScidbConnection failed!");
}

unique_ptr<Cursor> ScidbConnection::exec(string query) {
    unique_ptr<ScidbSession> session(new ScidbSession(url));
    session->exec(query);
    return nullptr;
}

ScidbArr ScidbConnection::download(const string& arrayName) {
    unique_ptr<ScidbSession> session(new ScidbSession(url));
    return session->download(arrayName);
}

ScidbArr ScidbConnection::download(const string& query, const ScidbSchema& schema) {
    unique_ptr<ScidbSession> session(new ScidbSession(url));
    return session->download(query, schema);
}

void ScidbConnection::upload(const string& arrayName, const ScidbArr& arr) {
    unique_ptr<ScidbSession> session(new ScidbSession(url));
    session->upload(arrayName, arr);
}
