.POSIX:
.SUFFIXES:

CXXFLAGS=--std=c++11 -W -O
GOOGLE_TEST_LIB = gtest

LDLIBS_MAIN=-lm
LDLIBS_TESTS=-lm -l$(GOOGLE_TEST_LIB) -lpthread

INCLUDES=-Iinclude -Iavl_tree/include

all: interactive tests
interactive: obj/main.o
	$(CXX) $(LDFLAGS) $(LDLIBS_MAIN) -o build/sparse_matrix $^

tests: sparse_matrix_tests
win32: tests
	ren tests\all test\all.exe

%_tests: obj/%_tests.o
	mkdir -p build/tests
	$(CXX) $(LDFLAGS) $(LDLIBS_TESTS) -o build/tests/$* $^

obj/%_tests.o: tests/%_tests.cpp include/%.hpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDLIBS) -c $< -o $@

obj/main.o: main.cpp include/sparse_matrix.hpp
	mkdir -p build
	mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDLIBS) -c $< -o $@

clean:
	$(RM) -r build
	$(RM) -r obj
	$(RM) -r bin

.PHONY: all interactive tests clean