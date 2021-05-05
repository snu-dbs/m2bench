//
// Created by Kyoseung Koo on 2021/05/05.
//

#include <iostream>
#include "../../include/Session/ArangoSession.h"

ArangoSession::ArangoSession(string url, string jwt, string query): url(url), jwt(jwt), query(query) {}

vector<json> ArangoSession::fetch() {
    json res;

    // request
    if (isFirst) {
        json test = json::parse(R"({"query" : ")" + query + R"("})");
        cpr::Response r2 = cpr::Post(cpr::Url{"http://" + url + "/_api/cursor"},
                                     cpr::Header{{"accept", "application/json"}, {"Authorization", "bearer " + jwt}},
                                     cpr::Body{test.dump()});

        cout << r2.text << endl;
        if (r2.status_code != 201) {        // error
            cerr << r2.text << endl;
            done = true;
            return {};
        }

        res = json::parse(r2.text);
        isFirst = false;
    } else {
        cpr::Response r2 = cpr::Put(cpr::Url{"http://" + url + "/_api/cursor/" + id},
                                    cpr::Header{{"accept", "application/json"}, {"Authorization", "bearer " + jwt}});
        res = json::parse(r2.text);
        cout << r2.text << endl;
    }

    // has more check
    if (res["hasMore"].get<bool>()) { // has more res
        id = res["id"].get<string>();
    } else {
        done = true;
    }

    // return
    return res["result"].get<vector<json>>();
}

bool ArangoSession::isDone() {
    return done;
}

