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
    if (format == DENSE) {
        return ScidbArr(schema, conversionTsvToDenseScidbData(raw, schema));
    } else if (format == COO) {
        return ScidbArr(schema, conversionTsvToCooScidbData(raw, schema));
    }
}

void ScidbSession::upload(const string& arrayName, ScidbData data, ScidbDataFormat format) {
    string body;
    ScidbSchema schema = this->schema(arrayName);
    if (format == DENSE) {
        body = conversionDenseScidbDataToTsv(data, schema);
    } else if (format == COO) {
        body = conversionCooScidbDataToTsv(data, schema);
    }

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
        ScidbAttr attr1(nat.at(0), nat.at(1));
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

string ScidbSession::conversionDenseScidbDataToTsv(ScidbData pMap, const ScidbSchema& schema) {
    string ret = "";

    vector<int> dimLength;
    for (auto &dim : schema.dims) {
        if (dim.end == INT32_MAX || dim.start == INT32_MIN) {
            throw runtime_error("Not supported to handle * with dense format");
        }
        dimLength.push_back(dim.end - dim.start + 1);
    }

    int totalSize = accumulate(dimLength.begin(), dimLength.end(), 1, multiplies<>()) ;
    for (int i = 0; i < totalSize; i++) {
        string line;
        for (size_t j = 0; j < schema.attrs.size(); j++) {
            auto attr = schema.attrs.at(j);
            if (attr.type.find("float") != string::npos) line += to_string(any_cast<float*>(pMap[attr.name])[i]);
            else if (attr.type.find("double") != string::npos) line += to_string(any_cast<double*>(pMap[attr.name])[i]);
            else if (attr.type.find("int") != string::npos) line += to_string(any_cast<int*>(pMap[attr.name])[i]);
            else if (attr.type.find("string") != string::npos) line += any_cast<string*>(pMap[attr.name])[i];

            if (j < schema.attrs.size() - 1) line += "\t";
        }
        ret += line + "\n";
    }

    return ret;
}

string ScidbSession::conversionCooScidbDataToTsv(ScidbData pMap, const ScidbSchema& schema) {
    if (pMap.empty()) throw runtime_error("ScidbData is empty");

    // get any data to get length
    size_t totalSize = 0;
    auto firstAttr = schema.attrs.begin();
    if (firstAttr->type.find("float") != string::npos) totalSize = any_cast<vector<float>>(pMap[firstAttr->name]).size();
    else if (firstAttr->type.find("double") != string::npos) totalSize = any_cast<vector<double>>(pMap[firstAttr->name]).size();
    else if (firstAttr->type.find("int") != string::npos) totalSize = any_cast<vector<int>>(pMap[firstAttr->name]).size();
    else if (firstAttr->type.find("string") != string::npos) totalSize = any_cast<vector<string>>(pMap[firstAttr->name]).size();

    // line (row) iteration
    string ret;
    for (size_t i = 0; i < totalSize; i++) {
        string line;

        // attribute (column) iteration
        for (size_t j = 0; j < schema.attrs.size(); j++) {
            auto attr = schema.attrs.at(j);
            if (attr.type.find("float") != string::npos) line += to_string(any_cast<vector<float>>(pMap[attr.name])[i]);
            else if (attr.type.find("double") != string::npos) line += to_string(any_cast<vector<double>>(pMap[attr.name])[i]);
            else if (attr.type.find("int") != string::npos) line += to_string(any_cast<vector<int>>(pMap[attr.name])[i]);
            else if (attr.type.find("double") != string::npos) line += any_cast<vector<string>>(pMap[attr.name])[i];

            if (j < schema.attrs.size() - 1) line += "\t";
        }
        ret += line + "\n";
    }

    return ret;
}

ScidbData ScidbSession::conversionTsvToDenseScidbData(const string& basicString,
                                                      const ScidbSchema& schema) {
    ScidbData ret;

    vector<int> dimLength;
    for (auto &dim : schema.dims) {
        if (dim.end == INT32_MAX || dim.start == INT32_MIN) throw runtime_error("Not supported to handle *");
        dimLength.push_back(dim.end - dim.start + 1);
    }

   int totalSize = accumulate(dimLength.begin(), dimLength.end(), 1, multiplies<>()) ;
    for (auto &attr : schema.attrs) {
        // TODO: Simple typing
        if (attr.type.find("float") != string::npos) ret[attr.name] = new float[totalSize];
        else if (attr.type.find("double") != string::npos) ret[attr.name] = new double[totalSize];
        else if (attr.type.find("int") != string::npos) ret[attr.name] = new int[totalSize];
        else if (attr.type.find("string") != string::npos) ret[attr.name] = new string[totalSize];
    }

    // create dataframe-like table
    vector<string> lines = split(basicString, "\n");
    for (auto& line : lines) {
        if (line.empty()) continue;

        auto items = split(line, "\t");
        // dim
        int acc = totalSize, index = 0;
        for (int i = 0; i < dimLength.size(); i++) {
            acc /= dimLength.at(i);
            index += (stoi(items[i]) - schema.dims.at(i).start)  * acc;      // align
        }
        // attr
        for (int i = dimLength.size(); i < items.size(); i++) {
            auto ta = schema.attrs.at(i - dimLength.size());
            if (ta.type.find("float") != string::npos) any_cast<float*>(ret[ta.name])[index] = stof(items[i]);
            else if (ta.type.find("double") != string::npos) any_cast<double*>(ret[ta.name])[index] = stod(items[i]);
            else if (ta.type.find("int") != string::npos) any_cast<int*>(ret[ta.name])[index] = stoi(items[i]);
            else if (ta.type.find("string") != string::npos) any_cast<string*>(ret[ta.name])[index] = items[i];
        }
    }

    return ret;
}

ScidbData ScidbSession::conversionTsvToCooScidbData(const string& basicString, const ScidbSchema& schema) {
    ScidbData ret;

    // empty vector initialization per column
    for (auto& dim : schema.dims) ret[dim.name] = vector<int>();
    for (auto& attr : schema.attrs) {
        if (attr.type.find("float") != string::npos) ret[attr.name] = vector<float>();
        else if (attr.type.find("double") != string::npos) ret[attr.name] = vector<double>();
        else if (attr.type.find("int") != string::npos) ret[attr.name] = vector<int>();
        else if (attr.type.find("string") != string::npos) ret[attr.name] = vector<string>();
    }

    // filling the table (vectors) by iterating tsv lines
    vector<string> lines = split(basicString, "\n");
    for (auto& line : lines) {
        if (line.empty()) continue;     // exception (specifically last line)

        auto items = split(line, "\t");     // split
        // attr
        size_t idx = 0;
        for (auto& dim : schema.dims) {
            auto temp = any_cast<vector<int>>(move(ret[dim.name]));
            temp.push_back(stoi(items[idx++]));
            ret[dim.name] = move(temp);
        }

        for (auto& attr : schema.attrs) {
            // TODO: Too slow?
            if (attr.type.find("float") != string::npos) {
                auto temp = any_cast<vector<float>>(move(ret[attr.name]));
                temp.push_back(stof(items[idx++]));
                ret[attr.name] = move(temp);
            } else if (attr.type.find("double") != string::npos) {
                auto temp = any_cast<vector<double>>(move(ret[attr.name]));
                temp.push_back(stod(items[idx++]));
                ret[attr.name] = move(temp);
            } else if (attr.type.find("int") != string::npos) {
                auto temp = any_cast<vector<int>>(move(ret[attr.name]));
                temp.push_back(stoi(items[idx++]));
                ret[attr.name] = move(temp);
            }  else if (attr.type.find("string") != string::npos) {
                auto temp = any_cast<vector<string>>(move(ret[attr.name]));
                temp.push_back(items[idx++]);
                ret[attr.name] = move(temp);
            }
        }
    }

    return ret;
}

