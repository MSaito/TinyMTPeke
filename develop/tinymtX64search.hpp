#pragma once
#ifndef TINYMTX64SEARCH_HPP
#define TINYMTX64SEARCH_HPP
/**
 * @file tinymtX64search.hpp
 *
 * @brief For TinyMTX64 Dynamic Generator
 *
 * @author Mutsuo Saito (Hiroshima University)
 *
 * Copyright (C) 2016 Mutsuo Saito
 * All rights reserved.
 *
 */

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cerrno>
#include <sstream>
#include <unistd.h>
#include <MTToolBox/EquidistributionCalculatable.hpp>
#include "AlgorithmRange.hpp"
#include <MTToolBox/util.hpp>
#include <MTToolBox/Sequential.hpp>

#if defined(SIM_MUL)
#include "sim_mul.h"
#endif
/**
 * @namespace tinymt
 * name space for tinymtX64
 */
namespace tinymt {
    using namespace MTToolBox;
    using namespace NTL;
    using namespace std;

    //typedef AlgorithmRange<uint64_t, uint32_t, 64, 1, 63> st64;

    /**
     * @class tinymt64_param
     * @brief a class keeping parameters of tinymt64
     *
     * This class keeps parameters of tinymt64, and has some
     * method for outputting parameters.
     */
    class tinymt64_param {
    public:
        uint64_t id;
        uint64_t mat1;
        int sh1;
        int sh2;
        int tsh;
        /**
         * This method is used in output.hpp.
         * @return header line of output.
         */
        const std::string getHeaderString() {
            //return "id, mat1, sh1, sh2, tsh, ";
            return "id, mat1,";
        }

        /**
         * This method is used in output.hpp.
         * @return string of parameters
         */
        const std::string getParamString() {
            stringstream ss;
            ss << dec << id << ",";
            ss << hex << setw(16) << setfill('0') << mat1 << ",";
            //ss << dec << sh1 << ",";
            //ss << dec << sh2 << ",";
            //ss << dec << tsh << ",";
            return ss.str();
        }

        /**
         * This method is used for DEBUG.
         * @return string of parameters.
         */
        const std::string getDebugString() {
            stringstream ss;
            ss << "id:" << dec << id << endl;
            ss << "mat1:" << hex << setw(16) << setfill('0') << mat1 << endl;
            ss << "sh1:" << dec << sh1 << endl;
            ss << "sh2:" << dec << sh2 << endl;
            ss << "tsh:" << dec << tsh << endl;
            return ss.str();
        }
    };

    /**
     * @class tinymt64
     * @brief 64-bit tinymt generator class used for dynamic creation
     *
     * This class is one of the main class of tinymt dynamic creator.
     * This class is designed to be called from programs in MTToolBox,
     * but is not a subclass of some abstract class.
     * Instead, this class is passed to them as template parameters.
     */
    class tinymt64 : public TemperingCalculatable<uint64_t, uint32_t> {
    public:
        /**
         * Constructor by id.
         * @param id The number specified by a user.
         */
        tinymt64(uint64_t id) {
            param.id = id;
            param.mat1 = 0;
            param.sh1 = 0;
            param.sh2 = 0;
            param.tsh = 31;
            reverse_bit_flag = false;
            status[0] = 0;
            status[1] = 0;
        }

        /**
         * The copy constructor.
         * @param src The origin of copy.
         */
        tinymt64(const tinymt64& src) :
            TemperingCalculatable<uint64_t, uint32_t>(),
            param(src.param) {
            reverse_bit_flag = src.reverse_bit_flag;
            status[0] = src.status[0];
            status[1] = src.status[1];
        }

        /**
         * Constructor by parameter.
         * @param src_param
         */
        tinymt64(const tinymt64_param& src_param) : param(src_param) {
            reverse_bit_flag = false;
            status[0] = 0;
            status[1] = 0;
        }

        tinymt64 * clone() const {
            return new tinymt64(*this);
        }

        /**
         * This method is called by the functions in simple_shortest_basis.hpp
         */
        void setZero() {
            status[0] = 0;
            status[1] = 0;
        }

        /**
         * This method always returns 127
         * @return always 127
         */
        int bitSize() const {
            return mexp;
        }

        /**
         * This method initialize internal state.
         * This initialization is simple.
         * @param seed seed for initialization
         */
        void seed(uint64_t seed) {
            status[0] = 0;
            status[1] = seed;
        }


        /**
         * get a part of internal state without tempering
         * @return
         */
        uint64_t get_uint() {
            return status[0];
        }

        /**
         * getter of status
         * @param index index of internal status
         * @return element of internal status at \b index
         */
        uint64_t get_status(int index) {
            return status[index];
        }

        /**
         * setter of status
         * @param value new status value
         */
        void set_status(uint64_t value[2]) {
            status[0] = value[0];
            status[1] = value[1];
        }

        /**
         * Important method, generate new random number
         * @return new pseudo random number
         */
        uint64_t generate() {
            next_state();
            return temper();
        }

        /**
         * This method is called by the functions in simple_shortest_basis.hpp
         * This method returns \b bit_len bits of MSB of generated numbers
         * If reverse_bit_flag is set, bits are taken from LSB
         * @param bit_len bit length from MSB or LSB
         * @return generated numbers of bit_len
         */
        uint64_t generate(int bit_len) {
            uint64_t u;
            if (reverse_bit_flag) {
                u = reverse_bit(generate());
            } else {
                u = generate();
            }
            uint64_t mask = 0;
            mask = (~mask) << (64 - bit_len);
            return u & mask;
        }

        /**
         * make parameters from given sequential number and
         * internal id
         * @param num sequential number
         */
        void setUpParam(AbstractGenerator<uint32_t>& sq32) {
            uint32_t num = sq32.generate();
            uint32_t work = num ^ (num << 15) ^ (num << 23);
            uint32_t mat1 = (work & 0xffff0000) | (param.id & 0xffff);
            uint32_t mat2 = (work & 0xffff) | (param.id & 0xffff0000);
            mat1 ^= mat1 >> 19;
            mat2 ^= mat2 << 18;
            param.mat1 = (static_cast<uint64_t>(mat1) << 32) | mat2;
            // After some trials, following parameters are fixed.
            param.sh1 = 26;
            param.sh2 = 19;
            param.tsh = 31;
        }

        /**
         * getter of parameter
         * @return parameter
         */
        const tinymt64_param& get_param() const {
            return param;
        }

        /**
         * getter of recursion parameter
         * @param mat1 parameter mat1 is set after calling this method
         */
        void get_mat(uint64_t *mat1) const {
            *mat1 = param.mat1;
        }

        /**
         * This method is called by the functions in the file
         * simple_shortest_basis.hpp
         * @return true if all elements of status is zero
         */
        bool isZero() const {
            return (status[0] == 0) && (status[1] == 0);
        }

        /**
         * This is important,
         * This function tries to improve output quality of randomness.
         * One important point of tinymt is this tempering function,
         * which is not GF(2)-linear.
         * But in calculating parameter phase, NON_LINEAR is never defined.
         * @return improved random number
         */
        uint64_t temper() {
            uint64_t t0;
            t0 = status[0] ^ status[1];
            t0 ^= t0 << param.tsh;  // left is better
            //t0 ^= t0 >> param.tsh;// right is worse
#if defined(SIM_MUL)
            t0 = sim_mul(t0, UINT64_C(0xb6db6db6db6db6db));
#endif
            return t0 ^ (t0 >> 5);
        }

        /**
         * This method is called by functions in the file search_temper.hpp
         * @param mask available bits of pattern
         * @param pattern bit pattern
         * @param src_bit only 0 is allowed
         */
        void setTemperingPattern(uint64_t mask, uint64_t pattern, int src_bit) {
            UNUSED_VARIABLE(&src_bit);
            param.tsh &= ~mask;
            param.tsh |= pattern & mask;
        }

        /**
         * This method is called by functions in the file
         * simple_shortest_basis.hpp addition of internal state as
         * GF(2) vector is possible when state transition function and
         * output function is GF(2)-linear.
         * @param that tinymt generator added to this generator
         */
        void add(EquidistributionCalculatable<uint64_t, uint32_t>& other) {
            tinymt64* that = dynamic_cast<tinymt64 *>(&other);
            if (that == 0) {
                throw std::invalid_argument(
                    "the adder should have the same type as the addee.");
            }
            if (param.mat1 != that->param.mat1 ||
                param.sh1 != that->param.sh1 ||
                param.sh2 != that->param.sh2 ||
                param.tsh != that->param.tsh) {
                cerr << "in add: the adder should have the same parameter"
                     << " as the addee." << endl;

                cerr << "mat1:" << hex << param.mat1 << endl;
                cerr << "tsh:" << hex << param.tsh << endl;
                cerr << "that->mat1:" << hex << that->param.mat1 << endl;
                cerr << "that->tsh:" << hex << that->param.tsh << endl;
                throw std::invalid_argument(
                    "the adder should have the same parameter as the addee.");
            }
            status[0] ^= that->status[0];
            status[1] ^= that->status[1];
        }

        /**
         * This method is called by functions in the file search_temper.hpp
         * Do not remove this.
         */
        void setUpTempering() {
//            if (parity(param.tsh) == 1) {
//                param.tsh ^= 1;
//            }
        }

        /**
         * setter of parameter
         * @param src new parameter
         */
        void set_param(tinymt64_param src) {
            param = src;
        }

        const std::string getHeaderString() {
            return param.getHeaderString();
        }

        /**
         * output parameters
         * @param out output stream
         */
        const std::string getParamString() {
            return param.getParamString();
        }

        /**
         * This method is called by the functions in search_temper.hpp
         * to calculate the equidistribution properties from LSB
         */
        void setReverseOutput() {
            reverse_bit_flag = true;
        }

        /**
         * This method is called by the functions in search_temper.hpp
         * to reset the reverse_bit_flag
         */
        void resetReverseOutput() {
            reverse_bit_flag = false;
        }

        bool isReverseOutput() {
            return reverse_bit_flag;
        }
    private:
        enum {sh0 = 1, status_size = 2, mexp = 128};
        uint64_t status[2];
        tinymt64_param param;
        bool reverse_bit_flag;
        /**
         * Important state transition function.
         */
        void next_state() {
            uint64_t x = status[0];
            uint64_t y = status[1];
            x ^= y;
            x ^= x << sh0;
            x ^= x >> param.sh1;
            x ^= x << param.sh2;
            status[0] = status[1];
            status[1] = x;
            if (x & UINT64_C(0x8000000000000000)) {
                status[0] ^= param.mat1;
            }
        }

    };
}

#endif //TINYMT64SEARCH_HPP
