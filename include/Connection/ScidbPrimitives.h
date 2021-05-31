//
// Created by Kyoseung Koo on 2021/05/26.
//

#ifndef M2BENCH_AO_SCIDBPRIMITIVES_H
#define M2BENCH_AO_SCIDBPRIMITIVES_H

#include <variant>
#include <string>

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


typedef class ScidbArr {
private:
    ScidbDataFormat format = COO;     // only COO available

    // Download schema
    ScidbSchema downloadedSchema;     // readonly
    stringstream datastream{};

    // Upload schema
    vector<ScidbLineType> uploadData;       // TODO: will be replaced

public:
    ScidbArr() = default;
    ScidbArr(ScidbSchema schema1, stringstream datastream1)
            :downloadedSchema(schema1), datastream(move(datastream1)) {}

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

    ScidbLineType readcell(){
        ScidbLineType linedata;           // one line data
        string line;
        getline(datastream,line);
        if (line.empty()) return linedata;           // one line data
        // exception (specifically last line)

        auto items = split(line, "\t");     // split
        size_t idx = 0;                 // idx in linedata

        // dim
        for (auto& dim : downloadedSchema.dims) linedata.emplace_back(stoi(items[idx++]));
        // attr
        for (auto& attr : downloadedSchema.attrs) {
            if (attr.type == FLOAT) linedata.emplace_back(stof(items[idx++]));
            else if (attr.type == DOUBLE) linedata.emplace_back(stod(items[idx++]));
            else if (attr.type == INT32) linedata.emplace_back(stoi(items[idx++]));
            else if (attr.type == STRING) linedata.emplace_back(items[idx++]);
        }

        return linedata;
    }

    string toTsv(ScidbSchema schema) const {
        stringstream ss;

        for (auto& linedata: uploadData) {
            for (size_t i = 0; i < schema.attrs.size(); i++) {
                if (schema.attrs.at(i).type == FLOAT) ss << get<float>(linedata.at(i)) << "\t";
                else if (schema.attrs.at(i).type == DOUBLE) ss << get<double>(linedata.at(i)) << "\t";
                else if (schema.attrs.at(i).type == INT32) ss << get<int>(linedata.at(i)) << "\t";
                else if (schema.attrs.at(i).type == STRING) ss << get<string>(linedata.at(i)) << "\t";
            }
            ss << "\n";
        }
        return ss.str();
    }

    void add(ScidbLineType linedata) {
        uploadData.push_back(linedata);
    }

} ScidbArr;


#endif //M2BENCH_AO_SCIDBPRIMITIVES_H
