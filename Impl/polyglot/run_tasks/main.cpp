#include <iostream>
#include <string>
#include <string.h>

#include <chrono>
#include <nlohmann/json.hpp>

#include "Polyglot/Tasks.h"
#include "Polyglot/Latency_Check.h"

using json = nlohmann::json; // for convenience
using namespace std;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

template <typename T, typename O>
void timer(void (*f)(T, T, O, O), T x, T y, O a, O b)
{
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(x, y, a, b);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n"
         << endl;
}

template <typename T, typename O>
void timer(void (*f)(T, O), T x, O y)
{
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(x, y);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n"
         << endl;
}

template <typename T>
void timer(void (*f)(T), T x)
{
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(x);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n"
         << endl;
}

void timer(void (*f)())
{
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)();
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n"
         << endl;
}

int main(int argc, char *argv[])
{
    cout << "POLYGLOT TEST" << endl;
    timer(T0, 50);

    string curdate = "2021-06-01";
    timer(T1, curdate);

    timer(T2);

    string curdate3 = "2018-07-07";
    timer(T3, curdate3);

    timer(T4, 10000, 10);

    string pid = "B007SYGLZO";
    timer(T5, pid, curdate);

    int patient_id = 9;
    timer(T6, patient_id);

    timer(T7, patient_id);

    timer(T8, patient_id);

    timer(T9, patient_id);

    string T10_ts1 = "2020-06-01 00:00:00";
    string T10_ts2 = "2020-06-01 02:00:00";
    timer(T10, T10_ts1, T10_ts2);

    int earthquake_X = 43744;
    timer(T11, earthquake_X);

    string T12_ts1 = "2020-09-17 00:00:00";
    string T12_ts2 = "2020-09-17 01:00:00";
    timer(T12, T12_ts1, T12_ts2);

    timer(T13);

    int Z1 = 5, Z2 = 10;
    timer(T14, Z1, Z2);

    double CLON = -118.0614431, CLAT = 34.068509;
    timer(T15, Z1, Z2, CLON, CLAT);

    long ts = 1600182000 + 10800 * 3.5;
    timer(T16, ts);

    return 0;
}
