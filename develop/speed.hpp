#pragma once
#ifndef SPEED_HPP
#define SPEED_HPP

#include <inttypes.h>
#include <climits>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <cerrno>

namespace MersenneTwister {
    template<typename T, typename E>
    int speed(uint64_t count)
    {
        using namespace std;
        E engine;
        T sum = 0;
        chrono::system_clock::time_point  start, end;
        start = chrono::system_clock::now();

        for (uint64_t i = 0; i < count; ++i) {
            sum += engine();
        }
        end = chrono::system_clock::now();
        double elapsed = chrono::duration_cast
            <chrono::milliseconds>(end-start).count();
        cout << elapsed << "ms" << endl;
        uint32_t s = static_cast<uint32_t>(sum);
        return s & 1;
    }
}
#endif // SPEED_HPP
