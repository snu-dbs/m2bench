#include <iostream>
//#include "src/Example/neo4j_example.h"
//#include "src/Example/mysql_example.h"
//#include "src/Example/mongodb_example.h"
#include "src/Scenarios/PolyglotDB/Healthcare_Tasks.h"
#include "src/Scenarios/PolyglotDB/Ecommerce_Tasks.h"
#include "src/Scenarios/PolyglotDB/Disaster_Tasks.h"

#include <chrono>
#include <nlohmann/json.hpp>
#include "src/Example/scidb.h"
#include <string>

using json = nlohmann::json;         // for convenience
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


template <typename T, typename O>
void timer( void (*f)(T,O), T x, O y)
{
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(x,y);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n" << endl;

}

template <typename T>
void timer( void (*f)(T), T x){
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(x);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n" << endl;

}

void timer( void (*f)()){
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)();
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n" << endl;

}

int main() {
    /* Sample Connection */
    // sample_connection();

    /* M2Bench */
//    string curdate = "2021-06-01";
//    T4();
    timer(T2);
//     timer(T6, 9);

//    string pid = "B007SYGLZO";
//    string curdate = "2021-06-01";
//    timer(T5,pid, curdate);

//    timer(T6, 9);

//    timer(T8,9);

//    timer(T9);

//    string ts1 = "2020-09-16 00:00:00";
//    string ts2 = "2020-09-16 01:00:00";
//    timer(T12,ts1,ts2);

//    long ts = 1600182000+10800*3.5;
//    timer(T16,ts);

//    string ts1 = "2020-05-01 00:00:00";
//    string ts2 = "2020-06-30 23:59:59";
//    timer(T10,ts1,ts2);
//
//    int earthquake_X_id = 41865;
//    timer(T11, earthquake_X_id);

//     timer(T14, 0, 60);

//    string pid = "B007SYGLZO";
//    string curdate = "2021-06-01";
//    timer(T5,pid, curdate);

//    coo_finedust();

    return 0;
}

