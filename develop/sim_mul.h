#pragma once
#ifndef SIM_MUL_H
#define SIM_MUL_H

#include <inttypes.h>

static inline uint32_t sim_mul(uint32_t x, uint32_t c)
{
    uint32_t w = 0;
    for (int i = 0; i < 32; i++) {
        if (c & 1) {
            w ^= x;
        }
        c = c >> 1;
        x = x << 1;
    }
    return w;
}

static inline uint64_t sim_mul(uint64_t x, uint64_t c)
{
    uint64_t w = 0;
    for (int i = 0; i < 64; i++) {
        if (c & 1) {
            w ^= x;
        }
        c = c >> 1;
        x = x << 1;
    }
    return w;
}

#endif // SIM_MUL_H
