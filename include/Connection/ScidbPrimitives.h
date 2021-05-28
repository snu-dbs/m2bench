//
// Created by Kyoseung Koo on 2021/05/26.
//

#ifndef M2BENCH_AO_SCIDBPRIMITIVES_H
#define M2BENCH_AO_SCIDBPRIMITIVES_H


typedef map<string, any> ScidbData;
typedef struct ScidbDim {
    std::string name{};
    int start, end;
    int overlap, interval;

    ScidbDim(string name, int start, int end, int interval, int overlap):
            name(name), start(start), end(end), overlap(overlap), interval(interval) {}
} ScidbDim;

typedef struct ScidbAttr {
    std::string name{};
    std::string type{};

    ScidbAttr(std::string name, std::string type): name(name), type(type) {}
} ScidbAttr;

typedef struct ScidbSchema {
    vector<ScidbDim> dims{};
    vector<ScidbAttr> attrs{};
} ScidbSchema;

enum ScidbDataFormat { DENSE, COO };

typedef struct ScidbArr {

    ScidbSchema schema;     // readonly
    ScidbData data;
    ScidbDataFormat format;

    ScidbArr(ScidbSchema schema1, ScidbData data1): schema(schema1), data(data1) {}
    ~ScidbArr() {
        if (format == DENSE) {      // what the dense array connector made is deleted by the connector
            for (auto &attr : schema.attrs) {
                if (attr.type.find("float") != std::string::npos) delete[] any_cast<float*>(data[attr.name]);
                else if (attr.type.find("double") != std::string::npos) delete[] any_cast<double*>(data[attr.name]);
                else if (attr.type.find("int") != std::string::npos) delete[] any_cast<int*>(data[attr.name]);
                else if (attr.type.find("string") != std::string::npos) delete[] any_cast<std::string*>(data[attr.name]);
            }
        }
    }
} ScidbArr;

#endif //M2BENCH_AO_SCIDBPRIMITIVES_H
