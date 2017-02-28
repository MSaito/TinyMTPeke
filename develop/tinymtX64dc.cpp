/**
 * @file tinymtX64dc.cpp
 *
 * @author Mutsuo Saito
 *
 * Copyright (C) 2016 Mutsuo Saito
 * All rights reserved.
 *
 */
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>
//#include <MTToolBox/AlgorithmRecursionAndTempering.hpp>
#include <MTToolBox/AlgorithmRecursionSearch.hpp>
#include <MTToolBox/AlgorithmPrimitivity.hpp>
#include <MTToolBox/Sequential.hpp>
#include <MTToolBox/MersenneTwister.hpp>
#include "tinymtX64search.hpp"
#include "parse_opt.hpp"
#include <time.h>

using namespace std;
using namespace MTToolBox;
using namespace tinymt;
static const uint32_t sequence_max = 0x7fffffff;

int search(tinymt_options& opt, int count, AlgorithmPrimitivity& ap);
const std::string toString(GF2X& poly);

/**
 * parse command line option, and search parameters
 * @param argc number of arguments
 * @param argv value of arguments
 * @return 0 if this ends normally
 */
int main(int argc, char** argv) {
    tinymt_options opt;
    AlgorithmPrimitivity ap(prime_factors2_128_1);
    bool parse = parse_opt(opt, argc, argv, sequence_max);
    if (!parse) {
        return -1;
    }
    try {
        return search(opt, opt.count, ap);
    } catch (underflow_error e) {
        return 0;
    }
}

/**
 * search parameters using all_in_one function in the file search_all.hpp
 * @param opt command line options
 * @param count number of parameters user requested
 * @return 0 if this ends normally
 */
int search(tinymt_options& opt, int count, AlgorithmPrimitivity& ap) {
    Sequential<uint32_t> sq(0, opt.start);
    //uint32_t seed = static_cast<uint32_t>(clock());
    //MersenneTwister sq(seed);
    tinymt64 g(opt.uid);

    if (opt.verbose) {
        time_t t = time(NULL);
        cout << "search start at " << ctime(&t);
        cout << "id:" << dec << opt.uid << endl;
    }
    int veq[64];
    //AlgorithmRecursionAndTempering<uint64_t> all(sq, ap);
    AlgorithmRecursionSearch<uint64_t> rec(g, sq, ap);
    //st64 st;
    int i = 0;
    while (i < count || opt.all) {
        if (rec.start(0x7fffff)) {
            //if (all.search(g, st, st, opt.verbose, cout, true)) {
            AlgorithmEquidistribution<uint64_t> eq(g, 64);
            int delta = eq.get_all_equidist(veq);
            if (delta > opt.max_delta) {
                continue;
            }
            GF2X poly = rec.getMinPoly();
            int weight = NTL::weight(poly);
            g.setReverseOutput();
            AlgorithmEquidistribution<uint64_t> equi(g, 64);
            int lsbdelta = equi.get_all_equidist(veq);
            g.resetReverseOutput();
            if (i == 0) {
                cout << "# characteristic, "
                     << g.getHeaderString()
                     << " weight, delta, lsbdelta"
                     << endl;
            }
            cout << toString(poly) << ","
                 << g.getParamString()
                 << dec << weight << "," << delta
                 << "," << dec << lsbdelta
                 << endl;
            i++;
        } else {
            cout << "search failed" << endl;
            break;
        }
    }
    if (opt.verbose) {
        time_t t = time(NULL);
        cout << "search end at " << ctime(&t) << endl;
    }
    return 0;
}

const std::string toString(GF2X& poly) {
    uint64_t p1;
    uint64_t p2;
    uint64_t p = 0;
    uint64_t mask = 1;
    for(int i = 0; i < 64; i++) {
        if (IsOne(coeff(poly, i))) {
            p |= mask;
        }
        mask <<= 1;
    }
    p2 = p;
    mask = 1;
    p = 0;
    for(int i = 0; i < 64; i++) {
        if (IsOne(coeff(poly, i + 64))) {
            p |= mask;
        }
        mask <<= 1;
    }
    p1 = p;
    stringstream ss;
    ss << hex << setfill('0') << setw(16) << p1
       << hex << setfill('0') << setw(16) << p2;
    return ss.str();
}
