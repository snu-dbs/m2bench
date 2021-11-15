//
// Created by Kyoseung Koo on 2021/05/26.
//

#ifndef M2BENCH_AO_SCIDBPRIMITIVES_H
#define M2BENCH_AO_SCIDBPRIMITIVES_H

#include <variant>
#include <string>

// long long is for int64
typedef vector<variant<int, float, double, string, long long>> ScidbLineType;

typedef class ScidbData {
private:
    vector<ScidbLineType> data{};       // TODO: this will replaced as file-based implementation available

public:
    // TODO: separate declare and impl
    vector<ScidbLineType>::iterator begin() { return data.begin(); }
    vector<ScidbLineType>::iterator end() { return data.end(); }
    void add(ScidbLineType input) { data.push_back(input); }

} ScidbData;

typedef struct ScidbDim {
    std::string name{};
    int start, end;
    int overlap, interval;

    ScidbDim(string name, int start, int end, int interval, int overlap):
            name(name), start(start), end(end), overlap(overlap), interval(interval) {}
} ScidbDim;

enum ScidbDataType { UNSUPPORTED, INT32, STRING, DOUBLE, FLOAT, INT64 };

typedef struct ScidbAttr {
    std::string name{};
    ScidbDataType type = UNSUPPORTED;

    ScidbAttr(std::string name, ScidbDataType type): name(name), type(type) {}
} ScidbAttr;

typedef struct ScidbSchema {
    vector<ScidbDim> dims{};
    vector<ScidbAttr> attrs{};
} ScidbSchema;

enum ScidbDataFormat { COO };


// SciDB base class
typedef class ScidbArr {
protected:
    ScidbDataFormat format = COO;     // only COO available

    ScidbSchema schema;     // readonly

public:
    ScidbArr() = default;
    ScidbArr(ScidbSchema schema1) : schema(schema1) {};

    virtual ScidbLineType readcell() = 0;
    virtual string toTsv(ScidbSchema schema) = 0;
    virtual void add(ScidbLineType linedata) = 0;

    std::vector<string> split(string str, const string& c) {
        vector<string> ret;
        size_t pos = 0;
        while ((pos = str.find(c)) != string::npos) {
            ret.push_back(str.substr(0, pos));
            str.erase(0, pos + c.length());
        }
        ret.push_back(str);
        return ret;
    }

    ScidbLineType parseLinestring(string line) {
        ScidbLineType linedata;           // one line data

        auto items = split(line, "\t");     // split
        size_t idx = 0;                 // idx in linedata

        // dim
        for (auto& dim : schema.dims) linedata.emplace_back(stoi(items[idx++]));
        // attr
        for (auto& attr : schema.attrs) {
            if (attr.type == FLOAT) linedata.emplace_back(stof(items[idx++]));
            else if (attr.type == DOUBLE) linedata.emplace_back(stod(items[idx++]));
            else if (attr.type == INT32) linedata.emplace_back(stoi(items[idx++]));
            else if (attr.type == INT64) linedata.emplace_back(stoll(items[idx++]));
            else if (attr.type == STRING) linedata.emplace_back(items[idx++]);
        }
        return linedata;
    }

    string parsingLinedata(ScidbLineType linedata) {
        stringstream ss;

        for (size_t i = 0; i < schema.attrs.size(); i++) {
            if (schema.attrs.at(i).type == FLOAT) ss << get<float>(linedata.at(i)) << "\t";
            else if (schema.attrs.at(i).type == DOUBLE) ss << get<double>(linedata.at(i)) << "\t";
            else if (schema.attrs.at(i).type == INT32) ss << get<int>(linedata.at(i)) << "\t";
            else if (schema.attrs.at(i).type == INT64) ss << get<long long>(linedata.at(i)) << "\t";
            else if (schema.attrs.at(i).type == STRING) ss << get<string>(linedata.at(i)) << "\t";
        }
        ss << "\n";

        return ss.str();
    }

} ScidbArr;


// Memory based SciDB
typedef class ScidbArrMem : public ScidbArr {
private:
    // Download schema
    stringstream datastream{};

    // Upload schema
    vector<ScidbLineType> uploadData;       

public:
    ScidbArrMem() = default;
    ScidbArrMem(ScidbSchema schema1, stringstream datastream1)
            : ScidbArr(schema1), datastream(move(datastream1)) {}

    ScidbLineType readcell() override {
        string line;
        getline(datastream, line);
        if (line.empty()) return ScidbLineType();           // one line data
        // exception (specifically last line)

        return parseLinestring(line);
    }

    string toTsv(ScidbSchema schema) override {
        stringstream ss;

        for (auto& linedata: uploadData) {
            ss << parsingLinedata(linedata);
        }
        return ss.str();
    }

    void add(ScidbLineType linedata) override {
        uploadData.push_back(linedata);
    }

} ScidbArrMem;


// File based SciDB
typedef class ScidbArrFile : public ScidbArr {
    // for reading
    ifstream ifs;       // fp of downloaded array if made with the constructor having filename

    // for writing
    ofstream ofs;       // tmpfile (in writing)
    string filename;

public:
    ScidbArrFile(ScidbSchema schema1) : ScidbArr(schema1) {
        char *tmpname = strdup("/tmp/dbs-scidbconnector-tempfile-XXXXXX");
        mkstemp(tmpname);
        ofs.open(tmpname);
        filename = tmpname;
    }

    ScidbArrFile(ScidbSchema schema1, string filename1) : ScidbArr(schema1) {
        ifs.open(filename1);
    }

    ~ScidbArrFile() {
        if (ofs.is_open()) ofs.close();
        if (ifs.is_open()) ifs.close();

        if (std::remove(filename.c_str())) cerr << filename << " is not deleted!" << endl;
    }

    ScidbLineType readcell() override {
        string line;
        getline(ifs, line);
        if (line.empty()) return ScidbLineType();
        return parseLinestring(line);
    }

    string toTsv(ScidbSchema schema) override {
        ofs.flush();
        return filename;
    }

    void add(ScidbLineType linedata) override {
        ofs << parsingLinedata(linedata);
    }

} ScidbArrFile;


#endif //M2BENCH_AO_SCIDBPRIMITIVES_H
