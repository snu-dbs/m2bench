//
// Created by Kyoseung Koo on 2021/05/05.
//

#include <regex>
#include <iostream>
#include <utility>

#include "Session/ScidbSession.h"
#include "CurlHelper.h"

string& trim(string& str, const char c = ' ') {
    str.erase(0, str.find_first_not_of(c));
    str.erase(str.find_last_not_of(c) + 1);
    return str;
}

vector<string> split(string str, const string& c) {
    vector<string> ret;
    size_t pos = 0;
    while ((pos = str.find(c)) != string::npos) {
        ret.push_back(str.substr(0, pos));
        str.erase(0, pos + c.length());
    }
    ret.push_back(str);
    return ret;
}


ScidbSession::ScidbSession(const string& url): url(url) {
    cpr::Response r = cpr::Get(cpr::Url{"http://" + url + "/new_session"});
    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
    } else {
        this->sessionId = r.text;
    }
}

ScidbSession::~ScidbSession() {
    cpr::Response r = cpr::Get(cpr::Url{"http://" + url + "/release_session?id=" + this->sessionId});
    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
    }
}

bool ScidbSession::isDone() {
    return true;
}

vector<json> ScidbSession::fetch() {}

void ScidbSession::exec(const string& query, bool save) {
    string reqUrl = "http://" + url + "/execute_query?id=" + this->sessionId + "&query=" + query;
    if (save) reqUrl += "&save=tsv%2B";

    cpr::Response r = cpr::Get(cpr::Url{CurlHelper::spaceReplace(reqUrl)});
    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
    }
}

ScidbArr ScidbSession::download(const string& arrayName) {
    ScidbSchema schema = this->schema(arrayName);
    this->exec("scan(" + arrayName + ")", true);
    stringstream ss(this->pull());
    return ScidbArr(schema, move(ss));
}

ScidbArr ScidbSession::download(const string& query, const ScidbSchema& schema) {
    this->exec(query, true);
    stringstream ss(this->pull());
    return ScidbArr(schema, move(ss));
}

void ScidbSession::upload(const string& arrayName, const ScidbArr& arr) {

    string body;
    ScidbSchema schema = this->schema(arrayName);
    string path = this->push(arr.toTsv(schema));
    this->exec("load(" + arrayName + ", '" + path + "', -2, 'tsv')");
}

ScidbSchema ScidbSession::schema(const string& arrayName) {
    this->exec("show(" + arrayName + ")", true);
    string text = split(this->pull(), "\t").at(1);
    text = std::regex_replace(text, std::regex("not empty "), ""); // replace 'def' -> 'klm'
    return parsingSchema(text);
}

string ScidbSession::pull() {       // get latest
    string reqUrl = "http://" + url + "/read_lines?id=" + this->sessionId;
    cpr::Response r = cpr::Get(cpr::Url{reqUrl});
    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
    }

    return r.text;
}

string ScidbSession::push(string data) {
    // get schema
    string reqUrl = "http://" + url + "/upload?id=" + this->sessionId;
    cpr::Response r = cpr::Post(cpr::Url{reqUrl}, cpr::Body{move(data)});
    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
        throw runtime_error("ScidbSession uploads failed: " + r.text);
    }

    return r.text;      // path in the remote
}

ScidbSchema ScidbSession::parsingSchema(const string& basicString) {

    regex structure(R"(([@\w\d]*)<(.*)>.*\[(.*)\])");       // array name, attrs, dims
    smatch match;

    if (!regex_match(basicString, match, structure)) throw runtime_error("regex failed");
    if (match.size() != 4) throw runtime_error("group is not 3 (i.e. it is not a scidb schema)");

    ScidbSchema schema;
    // attrs
    vector<string> attrs = split(string(match[2]), ",");
    for (auto &attr : attrs) {
        auto nat = split(trim(attr, ' '), ":");
        ScidbDataType type = UNSUPPORTED;

        // TODO 8, 16, 32, 64 bit primitive types
        if (nat.at(1).find("float") != string::npos) type = FLOAT;
        else if (nat.at(1).find("double") != string::npos) type = DOUBLE;
        else if (nat.at(1).find("int32") != string::npos) type = INT32;
        else if (nat.at(1).find("int64") != string::npos) type = INT64;
        else if (nat.at(1).find("string") != string::npos) type = STRING;

        ScidbAttr attr1(nat.at(0), type);
        schema.attrs.push_back(attr1);
    }

    // dims
    vector<string> dims = split(string(match[3]), ";");
    for (auto &dim : dims) {
        auto nad = split(trim(dim, ' '), "=");
        auto ds = split(trim(nad.at(1), ' '), ":");
        int start = (ds.at(0) == "*") ? INT32_MIN : stoi(ds.at(0));
        int end = (ds.at(1) == "*") ? INT32_MAX : stoi(ds.at(1));
        int interval = (ds.at(2) == "*") ? 0 : stoi(ds.at(2));
        int overlap = (ds.at(3) == "*") ? 0 : stoi(ds.at(3));
        ScidbDim dim1(nad.at(0), start, end, interval, overlap);
        schema.dims.push_back(dim1);
    }

    return schema;
}

