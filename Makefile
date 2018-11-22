.POSIX:
.SUFFIXES:

CXXFLAGS=--std=c++11 -W -O
LDLIBS_MAIN=-lm

INCLUDES=-Iinclude -Iavl_tree/include

all: interactive #tests
interactive: obj/main.o
	$(CXX) $(LDFLAGS) $(LDLIBS_MAIN) -o build/sparse_matrix $^

tests: sparse_matrix_tests
win32: tests
	ren tests\all test\all.exe

obj/main.o: main.cpp include/sparse_matrix.hpp
	mkdir -p build
	mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDLIBS) -c $< -o $@

clean:
	$(RM) -r build
	$(RM) -r obj
	$(RM) -r bin

.PHONY: all interactive clean #tests