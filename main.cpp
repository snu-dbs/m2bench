#include <iostream>
#include <cpr/cpr.h>
#include "src/Example/neo4j_example.h"
#include "src/Example/mysql_example.h"
#include "src/Example/mongodb_example.h"
#include "src/Scenarios/PolyglotDB/Healthcare_Tasks.h"
#include "src/Scenarios/PolyglotDB/Ecommerce_Tasks.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

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
    timer(T1);
    timer(T2);
    timer(T6, 9);

   return 0;
}

