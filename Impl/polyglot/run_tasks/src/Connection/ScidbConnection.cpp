//
// Created by Kyoseung Koo on 2021/05/05.
//

#include <utility>

#include "Connection/ScidbConnection.h"
#include "Session/ScidbSession.h"

/*
 * Making a SciDB connection.
 */
ScidbConnection::ScidbConnection(const string& url): Connection(url, "", "", "") {
    cpr::Response r = cpr::Get(cpr::Url{"http://" + url});
    if (r.status_code != 200) assert("Creating ScidbConnection failed!");
}

/*
 * Executing a SciDB query.
 */
unique_ptr<Cursor> ScidbConnection::exec(string query) {
    unique_ptr<ScidbSession> session(new ScidbSession(url));
    session->exec(query);
    return nullptr;
}

/*
 * Downloading the array without an array schema.
 */
unique_ptr<ScidbArr> ScidbConnection::download(const string& arrayName) {
    unique_ptr<ScidbSession> session(new ScidbSession(url));
    return session->download(arrayName);
}

/*
 * Downloading the array with an array schema.
 */
unique_ptr<ScidbArr> ScidbConnection::download(const string& query, const ScidbSchema& schema) {
    unique_ptr<ScidbSession> session(new ScidbSession(url));
    return session->download(query, schema);
}

/*
 * Uploading an array with an array name.
 */
void ScidbConnection::upload(const string& arrayName, const shared_ptr<ScidbArr> arr) {
    unique_ptr<ScidbSession> session(new ScidbSession(url));
    session->upload(arrayName, arr);
}
