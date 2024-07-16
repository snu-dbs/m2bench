//
// Created by Kyoseung Koo on 2021/05/05.
//

#include <regex>
#include <iostream>
#include <utility>

#include "Session/ScidbSession.h"
#include "CurlHelper.h"

/*
 * Help functions.
 */
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


/*
 * Making new session.
 */
ScidbSession::ScidbSession(const string& url, bool isFileBased): url(url), isFileBased(isFileBased) {
    cpr::Response r = cpr::Get(cpr::Url{"http://" + url + "/new_session"});
    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
    } else {
        this->sessionId = r.text;
    }
}

/*
 * Releasing the session.
 */
ScidbSession::~ScidbSession() {
    // release session.
    cpr::Response r = cpr::Get(cpr::Url{"http://" + url + "/release_session?id=" + this->sessionId});
    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
    }

    // remove tempfiles
    for (auto& filename: tmpfiles) {
        if (std::remove(filename.c_str())) cerr << filename << " is not deleted!" << endl;
    }
}

bool ScidbSession::isDone() {
    return true;
}

/*
 * The ScidbSession is not using the parnet method structure.
 */
vector<json> ScidbSession::fetch() {}

/*
 * Executing a SciDB query.
 */
void ScidbSession::exec(const string& query, bool save) {
    string reqUrl = "http://" + url + "/execute_query?id=" + this->sessionId + "&query=" + query;
    if (save) reqUrl += "&save=tsv%2B";

    cpr::Response r = cpr::Get(cpr::Url{CurlHelper::spaceReplace(reqUrl)});
    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
    }
}

/*
 * Getting the array's schema and downloading the array with the schema.
 */
unique_ptr<ScidbArr> ScidbSession::download(const string& arrayName) {
    ScidbSchema schema = this->schema(arrayName);
    this->exec("scan(" + arrayName + ")", true);

    if (!isFileBased) {
        // memory
        stringstream ss(this->pull());
        return unique_ptr<ScidbArrMem>(new ScidbArrMem(schema, move(ss)));
    }

    // file
    return unique_ptr<ScidbArrFile>(new ScidbArrFile(schema, this->pullToFile()));
}

/*
 * Downloading the array with the schema.
 */
unique_ptr<ScidbArr> ScidbSession::download(const string& query, const ScidbSchema& schema) {
    this->exec(query, true);

    if (!isFileBased) { 
        // memory
        stringstream ss(this->pull());
        return unique_ptr<ScidbArrMem>(new ScidbArrMem(schema, move(ss)));
    }

    // file
    return unique_ptr<ScidbArrFile>(new ScidbArrFile(schema, this->pullToFile()));
}

/*
 * Parsing the schema, uploading the data, and loading it to the SciDB.
 */
void ScidbSession::upload(const string& arrayName, shared_ptr<ScidbArr> arr) {
    string body;
    ScidbSchema schema = this->schema(arrayName);

    string path;
    if (!isFileBased) { 
        // memory
        path = this->push(arr->toTsv(schema));
    } else {
        // file
        path = this->pushFromFile(arr->toTsv(schema));
    }

    this->exec("load(" + arrayName + ", '" + path + "', -2, 'tsv')");
}

/*
 * Parsing the array's schema.
 */
ScidbSchema ScidbSession::schema(const string& arrayName) {
    this->exec("show(" + arrayName + ")", true);
    string text = split(this->pull(), "\t").at(1);
    text = std::regex_replace(text, std::regex("not empty "), ""); // replace 'def' -> 'klm'
    return parsingSchema(text);
}

/*
 * Getting the latest saved array that is the latest one.
 */
string ScidbSession::pull() {       // get latest
    string reqUrl = "http://" + url + "/read_lines?id=" + this->sessionId;
    cpr::Response r = cpr::Get(cpr::Url{reqUrl});
    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
    }

    return r.text;
}

/*
 * Getting the latest saved array that is the latest one.
 * It save the array into a file.
 */
string ScidbSession::pullToFile() {
    // gen tmpfile name
    char *tmpname = strdup("/tmp/dbs-scidbconnector-tempfile-XXXXXX");
    mkstemp(tmpname);
    ofstream ofs(tmpname);

    // to delete 
    tmpfiles.push_back(tmpname);

    // data fetching
    string reqUrl = "http://" + url + "/read_lines?id=" + this->sessionId;
    cpr::Response r = cpr::Get(cpr::Url{reqUrl}, cpr::WriteCallback{[&ofs](string data) -> bool {
        ofs << data;
        // std::cout << "writecallback!: " << data << std::endl;
        return true;
    }});

    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
    }

    ofs.close();

    return tmpname;
}


/*
 * Push the string (TSV array) to the SciDB.
 */
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


/*
 * Push the string of the file to the SciDB.
 */

string ScidbSession::pushFromFile(string filename) {
    ifstream ifs(filename);

    // get file size
    struct stat fs;
    stat(filename.c_str(), &fs);
    
    int bufSize = 1024;

    // data uploading
    string reqUrl = "http://" + url + "/upload?id=" + this->sessionId;
    cpr::Response r = cpr::Post(cpr::Url{reqUrl}, cpr::ReadCallback{fs.st_size, [&](char* buffer, size_t& size) -> bool {
        if (ifs.eof()) {
            return false;
        }

        size = ifs.read(buffer, bufSize).gcount();
        return true;
    }});

    if (r.status_code != 200) {        // error
        cerr << r.text << endl;
        throw runtime_error("ScidbSession uploads failed: " + r.text);
    }

    ifs.close();

    return r.text;      // path in the remote
}

/*
 * Parsing the schema.
 */
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

