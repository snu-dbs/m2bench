//
// Created by Kyoseung Koo on 2021/05/26.
//

#ifndef M2BENCH_AO_SCIDBPRIMITIVES_H
#define M2BENCH_AO_SCIDBPRIMITIVES_H
#include <variant>

typedef vector<variant<int, float, double, string>> ScidbLineType;

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

enum ScidbDataType { UNSUPPORTED, INT32, STRING, DOUBLE, FLOAT,  };

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

typedef struct ScidbArr {

    ScidbSchema schema;     // readonly
    ScidbData data;
    ScidbDataFormat format;     // only COO available

    ScidbArr(ScidbSchema schema1, ScidbData data1): schema(schema1), data(data1) {}
    ~ScidbArr() {
    }
} ScidbArr;



typedef class ScidbStream {
private:
    ScidbSchema schema;     // readonly
    stringstream datastream;       // TODO: this will replaced as file-based implementation available
public:
    // TODO: separate declare and impl

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

    vector<any> readvalue(){


        vector<any> linedata;           // one line data
        string line;
        getline(datastream,line);
        if (line.empty()) return linedata;           // one line data
        // exception (specifically last line)

        auto items = split(line, "\t");     // split
        size_t idx = 0;                 // idx in linedata

        // dim
        for (auto& dim : schema.dims) linedata.emplace_back(stoi(items[idx++]));
        // attr
        for (auto& attr : schema.attrs) {
            if (attr.type.find("float") != string::npos) linedata.emplace_back(stof(items[idx++]));
            else if (attr.type.find("double") != string::npos) linedata.emplace_back(stod(items[idx++]));
            else if (attr.type.find("int") != string::npos) linedata.emplace_back(stoi(items[idx++]));
            else if (attr.type.find("string") != string::npos) linedata.emplace_back(items[idx++]);
        }

        return linedata;
    }

    void setString(const string& raw){
        cout<<raw.size()<< endl;
        datastream << raw;
    }
    void setSchema(ScidbSchema pschema){
        schema = pschema;
    }


} ScidbStream;


typedef struct ScidbArrStream {

    ScidbStream data;
    ScidbDataFormat format;     // only COO available

    ScidbArrStream(ScidbSchema schema1, const string& data1) {
        cout << "init" << endl;
        data.setString(data1);
        data.setSchema(schema1);

    }
    ~ScidbArrStream() {
    }
} ScidbArrStream;


#endif //M2BENCH_AO_SCIDBPRIMITIVES_H
