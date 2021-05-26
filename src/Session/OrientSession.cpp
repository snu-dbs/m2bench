//
// Created by Kyoseung Koo on 2021/05/05.
//

#include <iostream>

#include "Session/OrientSession.h"
#include "CurlHelper.h"

OrientSession::OrientSession(string url, string username, string password, string query)
        : url(move(url)), username(move(username)), password(move(password)), query(move(query)) {}

vector<json> OrientSession::fetch() {
    json res;

    // request
    cpr::Response r2 = cpr::Get(cpr::Url{CurlHelper::spaceReplace("http://" + url + "/query/unibench/sql/" + query + "/999999999/*:-1/")},
                                cpr::Authentication{string(username), string(password)});
    if (r2.status_code != 200) {        // error
        cerr << r2.text << endl;
        return {};
    }

    res = json::parse(r2.text);
    done = true;
    return res["result"].get<vector<json>>();
}

bool OrientSession::isDone() {
    return done;
}

