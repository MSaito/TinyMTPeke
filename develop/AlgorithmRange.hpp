#pragma once
#ifndef MTTOOLBOX_ALGORITHM_RANGE_HPP
#define MTTOOLBOX_ALGORITHM_RANGE_HPP
/**
 * @file AlgorithmRange.hpp
 *
 * @brief Algorithm that search tempering parameters to improve
 * dimension of equi-distribution of output of pseudo random number
 * generator.
 *
 * @author Mutsuo Saito
 *
 * Copyright (C) 2016 Mutsuo Saito
 * All rights reserved.
 *
 */
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>
#include <MTToolBox/AlgorithmTempering.hpp>
#include <MTToolBox/TemperingCalculatable.hpp>
#include <MTToolBox/AlgorithmEquidistribution.hpp>

namespace MTToolBox {
    /**
     * @class AlgorithmRange
     *
     */
    template<typename U, int bit_len, int min, int max>
    class AlgorithmRange : public AlgorithmTempering<U> {
    public:
        /**
         */
        int operator()(TemperingCalculatable<U>& rand,
                       bool verbose = false) {
            using namespace std;
            if (verbose) {
                cout << "searching..." << endl;
            }
            rand.resetReverseOutput();
            if (verbose) {
                cout << "searching from MSB" << endl;
            }
            int delta = search_best_temper(rand, verbose);
            if (verbose) {
                cout << "delta = " << dec << delta << endl;
            }
            rand.resetReverseOutput();
            return 0;
        }

        /**
         * Shows if searching tempering parameters is from LSBs.
         * @return true if searching is from LSBs.
         */
        bool isLSBTempering() const {
            return false;
        }
    private:
        void make_temper_bit(TemperingCalculatable<U>& rand,
                             U mask,
                             int param_pos,
                             U pattern) {
            rand.setTemperingPattern(mask, pattern, param_pos);
            rand.setUpTempering();
        }

        /**
         */
        int search_best_temper(TemperingCalculatable<U>& rand,
                               bool verbose) {
            using namespace std;
            int bitSize = rand.bitSize();
            int delta;
            int lsbdelta;
            int min_msb = 0;
            int min_delta = bitSize * bit_len;
            U mask = 0;
            int min_pattern = 0;
            int pattern;
            int param_pos = 1;
            mask = ~mask;
            //int length = bit_size<U>() / 4;
            for (int i = min; i <= max; i++) {
                pattern = i;
                make_temper_bit(rand, mask, param_pos, pattern);
                rand.resetReverseOutput();
                delta = get_equidist(rand, bit_len);
                rand.setReverseOutput();
                lsbdelta = get_equidist(rand, bit_len);
                rand.resetReverseOutput();
                if (verbose) {
                    cout << "delta " << dec << delta
                         << ":" << dec << pattern << endl;
                    cout << "lsbdelta " << dec << lsbdelta
                         << ":" << dec << pattern << endl;
                }
                if (delta * 3 + lsbdelta < min_delta) {
                    if (verbose) {
                        cout << "pattern change " << dec << min_pattern
                             << ":" << pattern << endl;
                    }
                    min_delta = delta * 3 + lsbdelta;
                    min_msb = delta;
                    min_pattern = pattern;
                }
            }
            make_temper_bit(rand, mask, param_pos, min_pattern);
            if (verbose) {
                cout << dec << min_delta << ":"
                     << dec << min_pattern << endl;
            }
            return min_msb;
        }

        /**
         */
        int get_equidist(TemperingCalculatable<U>& rand,
                         int bit_length) {
            AlgorithmEquidistribution<U> sb(rand, bit_length);
            int veq[bit_length];
            int sum = sb.get_all_equidist(veq);
            return sum;
        }
    };
}

#endif // MTTOOLBOX_ALGORITHM_PARTIAL_BITPATTERN_HPP
