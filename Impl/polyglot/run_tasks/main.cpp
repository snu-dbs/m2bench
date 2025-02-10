#include <iostream>
#include <chrono>

#include "Polyglot/Tasks.h"
#include "Polyglot/Latency_Check.h"

using namespace std;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

template <typename... Args>
void timer(void (*f)(Args...), Args... args)
{
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(args...);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n"
         << endl;
}

int main()
{
    cout << "POLYGLOT TEST" << endl;

    timer(T0, 50);
    timer(T2);

    int patient_id = 9;
    timer(T9, patient_id);

    int Z1 = 5, Z2 = 10;
    timer(T14, Z1, Z2);

    double CLON = -118.0614431, CLAT = 34.068509;
    timer(T15, Z1, Z2, CLON, CLAT);

    long ts = 1600182000 + 10800 * 3.5;
    timer(T16, ts);

    return 0;
}
