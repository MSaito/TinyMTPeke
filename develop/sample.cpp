#include "TinyMTX64.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>

int main(int argc, char * argv[])
{
    using namespace std;
    using namespace MersenneTwister;
    TinyMTX64 mt;
    if (argc > 1) {
        mt.seed(argv[1], strlen(argv[1]));
    }
    for (int i = 0; i < 100; i++) {
        cout << setw(16) << hex << mt() << " ";
        if (i % 3 == 2) {
            cout << endl;
        }
    }
    cout << endl;
}
