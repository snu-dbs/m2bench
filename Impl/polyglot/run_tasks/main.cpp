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
    int sf = atoi(argv[2]);

    switch (task_num) {
        case 0:
            timer(T0, 50);
            break;
        case 2:
            timer(T2);
            break;
        case 9: {
            int x = 9 * sf;
            timer(T9, x);
            break;
        }
        case 14: {
            int z1 = 5 * sf, z2 = 10 * sf;
            timer(T14, z1, z2);
            break;
        }
        case 15: {
            int z1 = 5 * sf, z2 = 10 * sf;
	        double CLON = -118.0614431, CLAT = 34.068509;
            timer(T15, z1, z2, CLON, CLAT);
            break;
	    }
        case 16: {
            int z1 = 3 * sf, z2 = 4 * sf;
            timer(T16, z1, z2);
            break;
	    }
        default:
            return 1;
    }

    return 0;
}
