CXX = g++
CXXFLAGS = -std=c++14 -fPIC -O2 -I/opt/homebrew/include
COVERAGE_CXXFLAGS = -std=c++14 -fPIC -O0 -g --coverage -I/opt/homebrew/include
TEST_CXXFLAGS = -std=c++17 -fPIC -O0 -g --coverage -I/opt/homebrew/include
LDFLAGS = -shared
COVERAGE_LDFLAGS = -shared --coverage
TEST_LDFLAGS = -L/opt/homebrew/lib -lgtest -lgmock -lgtest_main -pthread --coverage

TARGET = liblogger.dylib
COVERAGE_TARGET = liblogger_coverage.dylib
SOURCES = logger.cpp real_file_system.cpp
OBJECTS = $(SOURCES:.cpp=.o)
COVERAGE_OBJECTS = $(SOURCES:.cpp=_coverage.o)
TEST_TARGET = test_logger
TEST_SOURCES = test_logger.cpp

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

test: $(COVERAGE_TARGET) $(TEST_TARGET)
	DYLD_LIBRARY_PATH=. ./$(TEST_TARGET)

$(COVERAGE_TARGET): $(COVERAGE_OBJECTS)
	$(CXX) $(COVERAGE_LDFLAGS) -o $@ $^

coverage: test
	gcov -r logger.cpp real_file_system.cpp

$(TEST_TARGET): $(TEST_SOURCES) $(COVERAGE_TARGET)
	$(CXX) $(TEST_CXXFLAGS) -o $@ $(TEST_SOURCES) -L. -llogger_coverage $(TEST_LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%_coverage.o: %.cpp
	$(CXX) $(COVERAGE_CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(COVERAGE_OBJECTS) $(TARGET) $(COVERAGE_TARGET) $(TEST_TARGET) *.gcno *.gcda *.gcov coverage.info
	rm -rf coverage_html

.PHONY: all test coverage clean
