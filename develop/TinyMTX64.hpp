#pragma once
#ifndef MT_TINYMTX64_HPP
#define MT_TINYMTX64_HPP
#include <stdint.h>
#include <inttypes.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace MersenneTwister {
    class TinyMTX64 {
    public:
        TinyMTX64() {
            mat1 = UINT64_C(0x2cd3059afd647f59);
            seed(0);
        }

        TinyMTX64(const TinyMTX64& src) {
            mat1 = src.mat1;
            status[0] = src.status[0];
            status[1] = src.status[1];
        }

        TinyMTX64(uint64_t p_mat1) {
            mat1 = p_mat1;
            seed(0);
        };

        const std::string getIDString() {
            using namespace std;
            stringstream ss;
            ss << "TinyMTX64";
            ss << "-" << setfill('0') << setw(16) << hex << mat1;
            string s;
            ss >> s;
            return s;
        }

        void seed(uint64_t seed) {
            status[0] = seed;
            status[1] = mat1;
            for (int i = 1; i < MIN_LOOP; i++) {
                status[i & 1] ^= i + UINT64_C(6364136223846793005)
                    * (status[(i - 1) & 1]
                       ^ (status[(i - 1) & 1] >> 62));
            }
            period_certification();
        }

        template<typename T>
        void seed(T init_key[], int length) {
            const int lag = 1;
            const int mid = 1;
            const int size = 4;
            int i, j;
            int count;
            uint64_t r;
            uint64_t st[4];

            st[0] = 0;
            st[1] = mat1;
            st[2] = 0;
            st[3] = 0;
            if (length + 1 > MIN_LOOP) {
                count = length + 1;
            } else {
                count = MIN_LOOP;
            }
            r = ini_func1(st[0] ^ st[mid % size]
                          ^ st[(size - 1) % size]);
            st[mid % size] += r;
            r += length;
            st[(mid + lag) % size] += r;
            st[0] = r;
            count--;
            for (i = 1, j = 0; (j < count) && (j < length); j++) {
                r = ini_func1(st[i] ^ st[(i + mid) % size]
                              ^ st[(i + size - 1) % size]);
                st[(i + mid) % size] += r;
                r += init_key[j] + i;
                st[(i + mid + lag) % size] += r;
                st[i] = r;
                i = (i + 1) % size;
            }
            for (; j < count; j++) {
                r = ini_func1(st[i] ^ st[(i + mid) % size]
                              ^ st[(i + size - 1) % size]);
                st[(i + mid) % size] += r;
                r += i;
                st[(i + mid + lag) % size] += r;
                st[i] = r;
                i = (i + 1) % size;
            }
            for (j = 0; j < size; j++) {
                r = ini_func2(st[i] + st[(i + mid) % size]
                              + st[(i + size - 1) % size]);
                st[(i + mid) % size] ^= r;
                r -= i;
                st[(i + mid + lag) % size] ^= r;
                st[i] = r;
                i = (i + 1) % size;
            }
            status[0] = st[0] ^ st[1];
            status[1] = st[2] ^ st[3];
            period_certification();
        }

        uint64_t generate() {
            next_state();
            return temper();
        }

        uint64_t operator()() {
            return generate();
        }

        void debug_print() {
            using namespace std;

            cout << "[";
            cout << hex << setw(16) << mat1 << "][";
            for (int j = 0; j < 2; j++) {
                cout << hex << setw(16) << status[j] << " ";
            }
            cout << "]" << endl;
        }
    private:
        enum {sh0 = 1, sh1 = 26, sh2 = 19, tsh = 31, MIN_LOOP = 8};
        uint64_t status[2];
        uint64_t mat1;
        //int tsh;
        uint64_t temper() {
            uint64_t t0 = status[0];
            t0 ^= t0 << tsh;  // left is better
            t0 *= UINT64_C(0xb6db6db6db6db6db);
            return t0 ^ (t0 >> 5);
        }

        void next_state() {
            uint64_t x = status[0];
            uint64_t y = status[1];
            x ^= y;
            x ^= x << sh0;
            x ^= x >> sh1;
            x ^= x << sh2;
            status[0] = status[1];
            status[1] = x;
            uint64_t msk = static_cast<uint64_t>(static_cast<int64_t>(y) >> 31);
            status[0] ^= mat1 & msk;
        }

        void period_certification() {
            if (status[0] == 0 && status[1] == 0) {
                status[0] = 'T';
                status[1] = 'M';
            }
        }

        uint64_t ini_func1(uint64_t x) {
            return (x ^ (x >> 59)) * UINT64_C(2173292883993);
        }

        uint64_t ini_func2(uint64_t x) {
            return (x ^ (x >> 59)) * UINT64_C(58885565329898161);
        }
    };
}

#endif // MT_TINYMTX64_HPP
