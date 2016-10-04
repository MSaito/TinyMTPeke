#include "speed.hpp"
#include "TinyMTX64.hpp"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace MersenneTwister;

int main()
{
    int count = 100000000;
    cout << "TinyMTX64" << endl;
    speed<uint64_t, TinyMTX64>(count);
}
