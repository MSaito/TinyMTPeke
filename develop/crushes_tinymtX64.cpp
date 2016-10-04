#include "crushes.hpp"
#include "TinyMTX64.hpp"

using namespace MersenneTwister;
using namespace std;

TinyMTX64 *tinyx64;
crushes<TinyMTX64> test;
//TEST_GENERATOR64(test_generator, double, dsfmt, crushes);

uint32_t test_generator() {
    static int has_buffer = 0;
    static uint32_t buffer = 0;
    uint64_t x;
    if (test.otype == 'm') {
        if (has_buffer) {
            has_buffer = 0;
            return buffer;
        } else {
            if (test.reverse) {
                x = reverse_bit(tinyx64->generate());
            } else {
                x = tinyx64->generate();
            }
            buffer = static_cast<uint32_t>(x >> 32);
            has_buffer = true;
            return static_cast<uint32_t>(x & UINT32_C(0xffffffff));
        }
    }
    if (test.reverse) {
        x = reverse_bit(tinyx64->generate());
    } else {
        x = tinyx64->generate();
    }
    if (test.otype == 'l') {
        return static_cast<uint32_t>(x & UINT32_C(0xffffffff));
    } else if (test.otype == 'h') {
        return static_cast<uint32_t>(x >> 32);
    } else {
        return x;
    }
}

int main(int argc, char *argv[]) {
    if (!crushes<TinyMTX64>::parse_opt(test, argc, argv, true)) {
        return -1;
    }
    if (test.otype != 'l' && test.otype != 'm' && test.otype != 'h') {
        cout << "please set otype" << endl;
        return -1;
    }

    tinyx64 = new TinyMTX64(test.matu64);
    char name[200];
    sprintf(name, "TinyMTX64-%" PRIx64, test.matu64);
    test.test(name, tinyx64, NULL, test_generator);
    delete tinyx64;
    return 0;
}
