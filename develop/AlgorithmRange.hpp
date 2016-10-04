#ifndef MTTOOLBOX_ALGORITHM_RANGE_HPP
#define MTTOOLBOX_ALGORITHM_RANGE_HPP
/**
 * @file AlgorithmRange.hpp
 *
 *\japanese
 * @brief 疑似乱数生成器の高次元均等分布性を改善するために、テンパ
 * リングパラメータを探索するアルゴリズム
 *\endjapanese
 *
 *\english
 * @brief Algorithm that search tempering parameters to improve
 * dimension of equi-distribution of output of pseudo random number
 * generator.
 *\endenglish
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2013 Mutsuo Saito, Makoto Matsumoto
 * and Hiroshima University.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
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
     *\japanese
     * @brief 疑似乱数生成器の高次元均等分布性を改善するために、テンパ
     * リングパラメータを探索するアルゴリズム
     *
     * このアルゴリズムはMTGPのテンパリングパラメータを探索するために開
     * 発され、TinyMTのテンパリングパラメータ探索にも使われている。
     *
     * \warning テンパリングパラメータの探索をしても十分良い高次元均等
     * 分布が得られない場合は、状態遷移関数の変更を考慮した方がよい。状
     * 態遷移関数で十分ビットミックスされていない場合、単純なテンパリン
     * グで均等分布次元を最大化することはできないだろう。
     *
     * @tparam U 疑似乱数生成器の出力の型, 例えば uint32_t など。
     *
     * @tparam bit_len テンパリングパラメータのビット長, 通常は出力のビッ
     * ト長と等しいと思われる。
     *
     * @tparam param_num テンパリングパラメータの数, MTGPでは4, TinyMT
     * では 1
     *
     * @tparam try_bit_len 出力のうちテンパリングされる部分の長さ。上位
     * からこのビット数だけがテンパリングされる。
     *
     * @tparam step 一度に何ビットずつビットパターンを生成するか。この
     * 数を大きくすると実行時間が延びるであろう。MTGP では 5 ビットずつ
     * 生成している。
     *
     * @tparam lsb このフラグが指定されると、上位ビットと下位ビットが反
     * 転されて均等分布次元が計算される。つまり下位ビットの均等分布次元
     * をあげたい時に指定する。TestU01のBigCrushには下位ビットの均等分
     * 布次元を改善することによってパス可能性が高まるテストがある。
     *\endjapanese
     *
     *\english
     * @brief Algorithm that search tempering parameters to improve
     * dimension of equi-distribution of output of pseudo random
     * number generator.
     *
     * This algorithm was developed when we design Mersenne Twister
     * for Graphic Processors (MTGP). This algorithm also is used in
     * Tiny Mersenne Twister (TinyMT).
     *
     * \warning If you could not get high dimension of
     * equi-distribution after tempering using parameters got by this
     * algorithm, you should consider changing the design of random
     * number generation algorithm.
     *
     * @tparam U type of output of pseudo random number generator,
     * for example, uint32_t. Only unsigned numbers are allowed.
     *
     * @tparam bit_len bit length of variable for tempering parameter
     *
     * @tparam param_num number of tempering parameters, for instance,
     * four for MTGP and one for TinyMT.
     *
     * @tparam try_bit_len length of tempering part of output.
     *
     * @tparam step specify how many bits are generated as bit pattern.
     * If large number is specified, it will consume huge CPU time.
     * For example, five is specified in MTGP parameter search.
     *
     * @tparam lsb When \b lsb is true, meaning of MSBs and LSBs are
     * changed.  This program calculate equi-distribution from LSB not
     * MSB.
     *\endenglish
     */
    template<typename U, typename V, int bit_len, int min, int max>
    class AlgorithmRange : public AlgorithmTempering<U, V> {
    public:
        /**
         *\japanese
         * テンパリングパラメータを探索する。
         *
         * この処理の呼び出しは長時間かかる可能性がある。
         * @param rand テンパリングパラメータを探索する疑似乱数生成器
         * @param verbose 冗長な出力をするフラグ
         * @return 常に0を返す
         *\endjapanese
         *
         *\english
         * Search tempering parameters. Searched parameters are
         * set to \b rand.
         * This process may consume large CPU time.
         * @param rand pseudo random number generator
         * @param verbose if true output redundant messages.
         * @return always zero.
         *\endenglish
         */
        int operator()(TemperingCalculatable<U, V>& rand,
                       bool verbose = false) {
            using namespace std;
            //verbose = true;
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
         *\japanese
         * このテンパリングパラメータ探索がLSBからの探索であるかどうか
         *@return true LSBからのテンパリングパラメータ探索
         *\endjapanese
         *
         *\english
         * Shows if searching tempering parameters is from LSBs.
         *@return true if searching is from LSBs.
         *\endenglish
         */
        bool isLSBTempering() const {
            return false;
        }
    private:
        void make_temper_bit(TemperingCalculatable<U, V>& rand,
                             U mask,
                             int param_pos,
                             U pattern) {
            rand.setTemperingPattern(mask, pattern, param_pos);
            rand.setUpTempering();
        }

        /**
         *\japanese
         * テンパリングパラメータをひとつ探索する。
         *
         * vビット精度均等分布次元をk(v)とする。vにおけるk(v)の理論的上限と
         * 実際のk(v)の差をd(v)とする。vを変化させたd(v)の和をΔで表す。
         *<ol>
         * <li>v_bit から max_v_bit までのすべてのビットパターンを生成し</li>
         * <li>すべてのビットパターンについてk(v),d(v)をそれぞれ計算し</li>
         * <li>v-bit から max_v_bitまでのd(v)の和(Δ)の最も小さかったビッ
         * トパターンを選択する。</li>
         * <li>同じΔであればハミングウェイトの大きいビットパターンを選択する</li>
         *</ol>
         * なお、このメソッドは再帰する。
         *
         * @param rand GF(2)疑似乱数生成器
         * @param v_bit テンパリングパラメータを探索するビット範囲
         * @param param_pos 何番目のテンパリングパラメータを探索しているか
         * @param max_v_bit このビットで探索をやめる
         * @param verbose true なら探索過程の情報を出力する
         * @return delta このテンパリングパラメータの設定結果によるΔ
         *\endjapanese
         *
         *\english
         * Search a tempering parameter.
         *
         * Search the best tempering parameter. Here, k(v) means dimension
         * of equi-distribution with v-bit accuracy, d(v) means the difference
         * between k(v) and the theoretical upper bound at v. &Delta; means
         * sum of d(v) for some vs.
         *<ol>
         * <li>Generates all bit patterns from v_bit to max_v_bit,</li>
         * <li>Calculates k(v) and d(v) for all bit patterns,</li>
         * <li>Select the bit pattern which gives the least &Delta;
         * for v = 1 to max_v_bit.</li>
         * <li>Select the bit pattern whose hamming weight is the largest for
         * the bit patterns whose &Delta;s are same.</li>
         *</ol>
         * This method calls itself recursively.
         *
         * @param rand GF(2)-linear pseudo random number generator
         * @param v_bit start position bit of searching tempering parameter
         * @param param_pos index of tempering parameter
         * @param max_v_bit end position bit of searching tempering parameter
         * @param verbose if true, output redundant message.
         * @return delta &Delta; for v = 1 to max_v_bit for the
         * selected bit pattern.
         *\endenglish
         */
        int search_best_temper(TemperingCalculatable<U, V>& rand,
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
         *\japanese
         * AlgorithmEquidistributionの呼び出し
         *
         * @param rand GF(2)疑似乱数生成器
         * @param bit_length MSB から \b bit_len_ 分の均等分布次元を計算する
         * @returns Δ 理論値と実現値の差の合計
         *\endjapanese
         *
         *\english
         * Wrapper of AlgorithmEquidistribution
         *
         * @param rand GF(2)-linear pseudo random number generator
         * @param bit_length calculate k(v)s for v = 1 to \b bit_length.
         * @returns &Delta; for v = 1 to \b bit_length.
         *
         *\endenglish
         */
        int get_equidist(TemperingCalculatable<U, V>& rand,
                         int bit_length) {
            AlgorithmEquidistribution<U, V> sb(rand, bit_length);
            int veq[bit_length];
            int sum = sb.get_all_equidist(veq);
            return sum;
        }
    };
}

#endif // MTTOOLBOX_ALGORITHM_PARTIAL_BITPATTERN_HPP
