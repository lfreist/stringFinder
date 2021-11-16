COMPILER = g++ -pedantic
PYTHON = python3
MAIN_BINS = $(basename $(wildcard *Main.cpp))
TEST_BINS = $(basename $(wildcard tests/*Test.cpp))
LIB_PATH = src
HEADERS = $(wildcard src/*.hpp)
OBJECTS = $(addsuffix .o, $(basename $(filter-out %Main.cpp tests/%Test.cpp, $(wildcard src/*.cpp))))
LIBRARIES = -lgtest -lgtest_main -lpthread -fopenmp

.PRECIOUS: %.o
.SUFFIXES:
.PHONY: all compile valgrind test checkstyle benchmark clean builddocker buildwharfer

all: compile valgrind test checkstyle

compile: $(MAIN_BINS) $(TEST_BINS)

test: $(TEST_BINS)
	for T in $(TEST_BINS); do ./$$T || exit; done

checkstyle:
	$(PYTHON) cpplint.py --repository=. *.cpp */*.cpp */*.hpp

valgrind: $(TEST_BINARIES)
	for T in $(TEST_BINS); do valgrind -s --leak-check=full ./$$T; done

benchmark:

builddocker:
	docker build -t leon-freist-bachelorprojekt .
	docker run -it -v $(pwd)/files:/inputfiles/input:ro --name leon-freist-bachelorprojekt leon-freist-bachelorprojekt

buildwharfer:
	wharfer build -t leon-freist-bachelorprojekt .
	wharfer run -it -v $(pwd)/files:/inputfiles/input:ro --name leon-freist-bachelorprojekt leon-freist-bachelorprojekt

clean:
	rm -f *.o
	rm -f $(MAIN_BINS)
	rm -f $(TEST_BINS)
	rm -f $(OBJECTS) tests/*.o

%Main: %Main.o $(OBJECTS)
	$(COMPILER) -o $@ $^ $(LIBRARIES)

%Test: %Test.o $(OBJECTS)
	$(COMPILER) -o $@ $^ $(LIBRARIES)

%.o: %.cpp $(HEADERS)
	$(COMPILER) -c $< -o $@ $(LIBRARIES)
