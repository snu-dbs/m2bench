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
#include <Generator/DataScaler.h>
#include <string.h>
using json = nlohmann::json;         // for convenience
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


template <typename T, typename O>
void timer( void (*f)(T,T,O,O), T x, T y, O a, O b)
{
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(x, y, a, b);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n" << endl;

}

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

void scalingTable(){
    //    auto ds = DataScaler();
    //    ds.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/review_schema.json");
    //    ds.scaleJson("/home/mxmdb/m2bench/data/ecommerce/json/review.json", 2);
}

int main() {
//    cout << "TEST" << endl;
//    string curdate = "2021-06-01";
//    timer(T2);



    //string curdate = "2021-06-
    /*
    long ts = 1600182000+10800*3.5;
*/
//    string T10_ts1 = "2020-06-01 00:00:00";
//    string T10_ts2 = "2020-06-01 02:00:00";
//    int earthquake_X = 41862;
//    string T12_ts1 = "2020-09-17 00:00:00";
//    string T12_ts2 = "2020-09-17 01:00:00";

//    if(argc == 2 && strcmp( argv[1],"T10") == 0){
//        cout<<"T10 start"<<endl;
//        timer(T10, T10_ts1, T10_ts2);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T11") == 0){
//        cout<<"T11 start"<<endl;
//        timer(T11, earthquake_X);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T12") == 0){
//        cout<<"T12 start"<<endl;
//        timer(T12,T12_ts1,T12_ts2);
//    }
//    else timer(T11, earthquake_X);

    string T12_ts1 = "2020-09-17 00:00:00";
    string T12_ts2 = "2020-09-17 01:00:00";
            timer(T12,T12_ts1,T12_ts2);
//    timer(T2);

//    if(argc == 2 && strcmp( argv[1],"T1") == 0){
//        timer(T1, curdate);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T2") == 0){
//        timer(T2);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T6") == 0){
//        timer(T6, 9);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T16") == 0){
//        timer(T16, ts);
//    }
    //timer(T6, 9);
    //
     // if(argc == 2 && strcmp( argv[1],"T7") == 0){
     //     timer(T7, 9);
     // }
     // else if (argc == 2 && strcmp( argv[1],"T8") == 0){
     //     timer(T8, 9);
     // }
     // else if (argc == 2 && strcmp( argv[1],"T9") == 0){
     //     timer(T9, 9);
     // }
//         timer(T8,9);

//    timer(T16, ts);

    //string curdate3 = "2018-07-07";//"2021-12-25";
    //string pid = "B007SYGLZO";
    //string curdate = "2021-06-01";
    //if(argc == 2 && strcmp( argv[1],"T3") == 0){
    //    timer(T3, curdate3);
    //}
    //else if (argc == 2 && strcmp( argv[1],"T4") == 0){
//    timer(T4,10000,10);
    //}
    //else if (argc == 2 && strcmp( argv[1],"T5") == 0){
    //    timer(T5,pid, curdate);
    //}
    //timer(T3,curdate3);

      return 0;
}

