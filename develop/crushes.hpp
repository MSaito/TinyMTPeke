#pragma once
#ifndef CRUSHES_HPP
#define CRUSHES_HPP
/**
 * This is a test progam using TestU01:
 * http://www.iro.umontreal.ca/~simardr/testu01/tu01.html
 */
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <getopt.h>
extern "C" {
#include "unif01.h"
#include "bbattery.h"
#include "scomp.h"
#include "sres.h"
#include "smarsa.h"
}

namespace MersenneTwister {
    void otherTest(unif01_Gen *gen)
    {
#if 0
        // LinearComp
        scomp_Res * res = scomp_CreateRes();
        //scomp_LinearComp(gen, res, 1, 400000, 0, 1);
        //scomp_LinearComp(gen, res, 1, 400000, 29, 1);
        //scomp_LinearComp(gen, res, 1, 400000, 30, 1);
        scomp_LinearComp(gen, res, 1, 400000, 31, 1);
        scomp_DeleteRes(res);
#endif
#if 0
        // MatrixRank
        sres_Chi2 * chi2 = sres_CreateChi2();
        // crush
        //smarsa_MatrixRank(gen, chi2, 1, 1000000, 0, 30, 60, 60);
        //smarsa_MatrixRank(gen, chi2, 1, 1000000, 20, 10, 60, 60);
        //smarsa_MatrixRank(gen, chi2, 1, 1000000, 22, 10, 60, 60);
        //smarsa_MatrixRank(gen, chi2, 1, 50000, 0, 30, 300, 300);
        //smarsa_MatrixRank(gen, chi2, 1, 50000, 20, 10, 300, 300);
        //smarsa_MatrixRank(gen, chi2, 1, 50000, 22, 10, 300, 300);
        //smarsa_MatrixRank(gen, chi2, 1, 2000, 0, 30, 1200, 1200);
        //smarsa_MatrixRank(gen, chi2, 1, 2000, 20, 10, 1200, 1200);
        //smarsa_MatrixRank(gen, chi2, 1, 2000, 22, 10, 1200, 1200);

        // big crush
        //smarsa_MatrixRank(gen, chi2, 10, 1000000, 0, 5, 30, 30);
        //smarsa_MatrixRank(gen, chi2, 10, 1000000, 25, 5, 30, 30);
        //smarsa_MatrixRank(gen, chi2, 10, 1000000, 27, 5, 30, 30);
        //smarsa_MatrixRank(gen, chi2, 1, 50000, 0, 4, 1000, 1000);
        //smarsa_MatrixRank(gen, chi2, 1, 50000, 26, 6, 1000, 1000);
        //smarsa_MatrixRank(gen, chi2, 1, 50000, 28, 4, 1000, 1000);
        //smarsa_MatrixRank(gen, chi2, 1, 80, 15, 15, 5000, 5000);
        smarsa_MatrixRank(gen, chi2, 1, 80, 0, 30, 5000, 5000);
        sres_DeleteChi2(chi2);
#endif
        // CollisionOver
#if 1
#define THOUSAND 1000
#define MILLION (THOUSAND * THOUSAND)
        smarsa_Res *resm;
        resm = smarsa_CreateRes();
        smarsa_CollisionOver(gen, resm, 30, 20 * MILLION, 0, 4, 21);
        //smarsa_CollisionOver(gen, resm, 30, 20 * MILLION, 28, 4, 21);
        smarsa_DeleteRes(resm);
#endif
    }

    inline static uint32_t reverse_bit(uint32_t x)
    {
        uint32_t y = 0x55555555;
        x = (((x >> 1) & y) | ((x & y) << 1));
        y = 0x33333333;
        x = (((x >> 2) & y) | ((x & y) << 2));
        y = 0x0f0f0f0f;
        x = (((x >> 4) & y) | ((x & y) << 4));
        y = 0x00ff00ff;
        x = (((x >> 8) & y) | ((x & y) << 8));
        return((x >> 16) | (x << 16));
    }

    typedef double (*double_gen)(void);
    typedef uint32_t (*u32_gen)(void);
    template<typename G>
    class crushes {
    public:
        static bool parse_opt(crushes& opt, int argc, char **argv,
            bool u64 = false) {
            using namespace std;
            int c;
            int error = 0;
            char * pgm = argv[0];
            struct option longopts[] = {
                {"seed", required_argument, NULL, 's'},
                {"ttype", required_argument, NULL, 't'},
                {"big", no_argument, NULL, 'B'},
                {"medium", no_argument, NULL, 'M'},
                {"small", no_argument, NULL, 'S'},
                {"other", no_argument, NULL, 'x'},
                {"reverse", no_argument, NULL, 'r'},
                {"otype", required_argument, NULL, 'o'},
                {"high", no_argument, NULL, 'H'},
                {"low", no_argument, NULL, 'L'},
                {"mixed", no_argument, NULL, 'X'},
                {"double", no_argument, NULL, 'D'},
                {NULL, 0, NULL, 0}};
            errno = 0;
            opt.seed = 1;
            opt.ttype = 'b';
            opt.otype = 'h';
            opt.reverse = false;
            for (;;) {
                c = getopt_long(argc, argv, "BMSHLXxDrs:t:o:", longopts, NULL);
                if (error) {
                    break;
                }
                if (c == -1) {
                    break;
                }
                switch (c) {
                case 's':
                    opt.seed = strtoull(optarg, NULL, 0);
                    if (errno) {
                        error = 1;
                        printf("seed must be a number\n");
                    }
                    break;
                case 't':
                    opt.ttype = optarg[0];
                    break;
                case 'B':
                    opt.ttype = 'b';
                    break;
                case 'M':
                    opt.ttype = 'm';
                    break;
                case 'S':
                    opt.ttype = 's';
                    break;
                case 'x':
                    opt.ttype = 'x';
                    break;
                case 'r':
                    opt.reverse = true;
                    break;
                case 'o':
                    opt.otype = optarg[0];
                    break;
                case 'H':
                    opt.otype = 'h';
                    break;
                case 'L':
                    opt.otype = 'l';
                    break;
                case 'X':
                    opt.otype = 'm';
                    break;
                case 'D':
                    opt.otype = 'd';
                    break;
                case '?':
                default:
                    error = 1;
                    break;
                }
            }
            argc -= optind;
            argv += optind;
            if (error) {
                output_help(pgm);
                return false;
            }
            if ((argc < 3 && !u64) || (argc < 1 && u64)) {
                error = 1;
                printf("mat1, mat2, tsh must be number\n");
            } else {
                if (u64) {
                    do {
                        opt.matu64 = strtoull(argv[0], NULL, 16);
                        if (errno) {
                            break;
                        }
                    } while (0);
                } else {
                    do {
                        opt.mat1 = strtoul(argv[0], NULL, 16);
                        if (errno) {
                            break;
                        }
                        opt.mat2 = strtoul(argv[1], NULL, 16);
                        if (errno) {
                            break;
                        }
                        opt.tsh = strtoul(argv[2], NULL, 10);
                        if (errno) {
                            break;
                        }
                    } while (0);
                }
                if (errno) {
                    error = 1;
                    printf("mat1, mat2, tsh must be number\n");
                }
            }
            if (error) {
                output_help(pgm);
                return false;
            }
            return true;
        }

        static void output_help(char * pgm) {
            printf("%s ", pgm);
            printf("[--seed s] [--ttype t] [--otype o]");
            printf("mat1, mat2, tsh\n");
            printf("\t--seed, -s         seed of generator\n");
            printf("\t--ttype, -t        test type s(mall), m(edium), b(ig)\n");
            printf("\t--otype, -o        output type h(igh), l(ow),"
                   " m(ix), d(ouble)\n");
            printf("\t--big              test type big\n");
            printf("\t--small            test type small\n");
            printf("\t--other            test type other\n");
            printf("\t--reverse -r       reverse bit\n");
            printf("\t--medium           test type medium\n");
            printf("\t--low              output type low\n");
            printf("\t--high             output type high\n");
            printf("\t--mix              output type low\n");
            printf("\t--double           output type double\n");
        }
        uint64_t matu64;
        uint32_t mat1;
        uint32_t mat2;
        uint32_t tsh;
        uint32_t seed;
        char ttype;
        char otype;
        bool reverse;
        uint32_t buff;
        int index;
        void test(char * name, G * prng, double_gen dgen, u32_gen ugen) {
            unif01_Gen *gen;
            test_init(prng);
            if (otype == 'd') {
                gen = unif01_CreateExternGen01(name, dgen);
            } else {
                gen = unif01_CreateExternGenBits(name, ugen);
            }
            switch (ttype) {
            case 's':
                bbattery_SmallCrush(gen);
                break;
            case 'm':
                bbattery_Crush(gen);
                break;
            case 'x':
                otherTest(gen);
                break;
            case 'b':
            default:
                bbattery_BigCrush(gen);
                break;
            }
            if (otype == 'd') {
                unif01_DeleteExternGen01(gen);
            } else {
                unif01_DeleteExternGenBits(gen);
            }
        }
    private:
        void test_init(G * prng) {
            using namespace std;
            cout << prng->getIDString()
                 << " seed = " << dec << seed
                 << " test_type = " << ttype
                 << " reverse = " << reverse
                 << " output_type = " << otype << endl;
            prng->seed(seed);
            buff = 0;
            index = 0;
        }
    };

}
#endif //CRUSHES_HPP
