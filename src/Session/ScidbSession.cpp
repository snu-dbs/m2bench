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

ScidbArr ScidbSession::download(const string& arrayName, ScidbDataFormat format) {
    ScidbSchema schema = this->schema(arrayName);
    this->exec("scan(" + arrayName + ")", true);
    string raw = this->pull();
    return ScidbArr(schema, conversionTsvToCooScidbData(raw, schema));
}

void ScidbSession::upload(const string& arrayName, ScidbData data, ScidbDataFormat format) {
    string body;
    ScidbSchema schema = this->schema(arrayName);
    body = conversionCooScidbDataToTsv(data, schema);

    string path = this->push(body);
    this->exec("load(" + arrayName + ", '" + path + "', -2, 'tsv')");
}

ScidbSchema ScidbSession::schema(const string& arrayName) {
    this->exec("show(" + arrayName + ")", true);
    string text = split(this->pull(), "\t").at(1);
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
    regex structure(R"(([\w\d]*)<(.*)>.*\[(.*)\])");       // array name, attrs, dims
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
        else if (nat.at(1).find("int") != string::npos) type = INT32;
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

string ScidbSession::conversionCooScidbDataToTsv(ScidbData pMap, const ScidbSchema& schema) {
    if (schema.dims.size() > 1) throw runtime_error("SciDB only accept 1D array");

    string ret;
    for (auto item = pMap.begin(); item != pMap.end(); item++) {
        string line;
        for (size_t j = 0; j < schema.attrs.size(); j++) {
            auto attr = schema.attrs.at(j);
            if (attr.type == FLOAT) line += to_string(get<float>((*item).at(j)));
            else if (attr.type == DOUBLE) line += to_string(get<double>((*item).at(j)));
            else if (attr.type == INT32) line += to_string(get<int>((*item).at(j)));
            else if (attr.type == STRING) line += get<string>((*item).at(j));

            if (j < schema.attrs.size() - 1) line += "\t";
        }
        ret += line + "\n";
    }

    return ret;
}

ScidbData ScidbSession::conversionTsvToCooScidbData(const string& basicString, const ScidbSchema& schema) {
    ScidbData ret;

    // filling the table (vectors) by iterating tsv lines
    size_t pos = 0;
    string temp = basicString;
    while ((pos = temp.find("\n")) != string::npos) {
        string line = temp.substr(0, pos);
        temp.erase(0, pos + 1);

        if (line.empty()) continue;     // exception (specifically last line)

        auto items = split(line, "\t");     // split
        ScidbLineType linedata;         // one line data
        size_t idx = 0;                 // idx in linedata

        // dim
        for (auto& dim : schema.dims) linedata.emplace_back(stoi(items[idx++]));

        // attr
        for (auto& attr : schema.attrs) {
            if (attr.type == FLOAT) linedata.emplace_back(stof(items[idx++]));
            else if (attr.type == DOUBLE) linedata.emplace_back(stod(items[idx++]));
            else if (attr.type == INT32) linedata.emplace_back(stoi(items[idx++]));
            else if (attr.type == STRING) linedata.emplace_back(items[idx++]);
        }

        // append to ret
        ret.add(linedata);
    }

    return ret;
}

