common_files = parse_opt.hpp \
parse_opt.cpp \
crushes.hpp \
AlgorithmRange.hpp \
speed.hpp \
TinyMTX64.hpp \
tinymtX64search.hpp

noinst_PROGRAMS = tinymtX64dc speed_tinymtX64 crushes_tinymtX64

tinymtX64dc_SOURCES = tinymtX64dc.cpp $(common_files)
speed_tinymtX64_SOURCES = speed_tinymtX64.cpp $(common_files)
crushes_tinymtX64_SOURCES = crushes_tinymtX64.cpp $(common_files)

crushes_tinymtX64_LDADD = -ltestu01
LDADD = -lMTToolBox

EXTRA_DIST = mpi_crushes_tinymtX64.cpp Makefile.mpi

AM_CXXFLAGS = -Wall -Wextra -O3 -D__STDC_CONSTANT_MACROS \
              -D__STDC_FORMAT_MACROS
