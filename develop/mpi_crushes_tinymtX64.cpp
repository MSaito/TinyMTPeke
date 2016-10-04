#include "crushes.hpp"
#include "TinyMTX64.hpp"
#include <mpi.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

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

struct param {
    uint64_t mat1;
};
typedef struct param param_t;

param_t parray[16] = {
    {0x88ca111901b8806e},
    {0xd4f11a9ea4fce93f},
    {0xe6331cc61edc07b7},
    {0xa4a21494cf14f3c5},
    {0xdc881b91405cd017},
    {0xa66a14cdd018b406},
    {0x234104680ce40339},
    {0x87f510feb9742e5d},
    {0x5cf80b9f59449651},
    {0x62da0c5bbc5caf17},
    {0x180503000630418c},
    {0xd4431a8887dc21f7},
    {0x72490e494f2453c9},
    {0x1960032c0a34c28d},
    {0xf2561e4a711cdc47},
    {0x363d06c7c08c3023}
};

int main(int argc, char *argv[])
{
    int rank;
    int num_process;
    // MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);

    if (!crushes<TinyMTX64>::parse_opt(test, argc, argv, true)) {
        MPI_Finalize();
        return -1;
    }
    if (test.otype != 'l' && test.otype != 'm' && test.otype != 'h') {
        cout << "please set otype" << endl;
        MPI_Finalize();
        return -1;
    }

    if (rank >= 0 && rank < 16) {
        test.matu64 = parray[rank].mat1;
    } else {
        test.seed = test.seed + rank;
    }

    char * pgm = argv[0];
    char fname[500];

    sprintf(fname, "%s-%c-%c-%" PRIx64 "-%04d.txt",
            pgm, test.ttype, test.otype, test.matu64, rank);
    int fd;
    errno = 0;
    fd = open(fname, O_WRONLY | O_CREAT, 0660);
    if (fd < 0) {
        printf("%s: open file error\n", pgm);
        MPI_Finalize();
        return 1;
    }
    dup2(fd, 1);
    if (errno) {
        printf("%s: dup error.\n", argv[0]);
        close(fd);
        MPI_Finalize();
        return 1;
    }

    tinyx64 = new TinyMTX64(test.matu64);
    char name[200];
    sprintf(name, "TinyMTX64-%x-%" PRIx64, test.mat1);
    test.test(name, tinyx64, NULL, test_generator);
    delete tinyx64;


    close(fd);
    MPI_Finalize();
    return 0;
}
