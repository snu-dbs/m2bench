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

#endif //M2BENCH_AO_SCIDBPRIMITIVES_H
