COMPILER = g++ -pedantic
PYTHON = python3
MAIN_BINS = $(basename $(wildcard *Main.cpp))
TEST_BINS = $(basename $(wildcard *Test.cpp))
HEADERS = $(wildcard *.hpp)
OBJECTS = $(addsuffix .o, $(basename $(filter-out %Main.cpp %Test.cpp, $(wildcard *.cpp))))
LIBRARIES = -lgtest -lgtest_main -lpthread

.PRECIOUS: %.o
.SUFFIXES:
.PHONY: all compile valgrind test checkstyle clean

all: compile valgrind test checkstyle

compile: $(MAIN_BINS) $(TEST_BINS)

test: $(TEST_BINS)
	for T in $(TEST_BINS); do ./$$T || exit; done

checkstyle:
	$(PYTHON) cpplint.py  --repository=. *.hpp *.cpp --filter=-build/c++11

valgrind: $(TEST_BINARIES)
	for T in $(TEST_BINS); do valgrind -s --leak-check=full ./$$T; done

clean:
	rm -f *.o
	rm -f $(MAIN_BINS)
	rm -f $(TEST_BINS)

%Main: %Main.o $(OBJECTS)
	$(COMPILER) -o $@ $^ $(LIBRARIES)

%Test: %Test.o $(OBJECTS)
	$(COMPILER) -o $@ $^ $(LIBRARIES)

%.o: %.cpp $(HEADERS)
	$(COMPILER) -c $<
