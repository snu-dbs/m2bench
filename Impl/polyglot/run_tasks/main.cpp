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
    auto t1 = high_resolution_clock::now();
    (*f)(args...);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
}

int main(int argc, char* argv[]) {
    int task_num = atoi(argv[1]);

    switch (task_num) {
        case 0:
            timer(T0, 50);
            break;
        case 2:
            timer(T2);
            break;
        case 9:
            timer(T9, 9);
            break;
        case 14:
            timer(T14, 5, 10);
            break;
        case 15: {
	    double CLON = -118.0614431, CLAT = 34.068509;
            timer(T15, 5, 10, CLON, CLAT);
            break;
	}
        case 16: {
	    long ts = 1600182000 + 10800 * 3.5;
            timer(T16, ts);
            break;
	}
        default:
            return 1;
    }

    return 0;
}
