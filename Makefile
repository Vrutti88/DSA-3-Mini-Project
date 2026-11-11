CXX = clang++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Icpp/include
BIN_DIR = bin
BUILD_DIR = build
SRC = cpp/src/Trie.cpp cpp/src/Autocomplete.cpp cpp/src/EditDistance.cpp cpp/src/DictionaryScanner.cpp cpp/src/Benchmark.cpp
MAIN_SRC = cpp/src/main.cpp
TARGET = $(BIN_DIR)/engine

.PHONY: all clean tests benchmark run_backend run_frontend

all: $(TARGET)

$(TARGET): $(SRC) $(MAIN_SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) $(MAIN_SRC) -o $(TARGET)
	@echo "Built successfully: $(TARGET)"

tests: $(TARGET)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) tests/cpp_tests/test_trie.cpp -o $(BIN_DIR)/test_trie
	$(CXX) $(CXXFLAGS) $(SRC) tests/cpp_tests/test_autocomplete.cpp -o $(BIN_DIR)/test_autocomplete
	$(CXX) $(CXXFLAGS) $(SRC) tests/cpp_tests/test_edit_distance.cpp -o $(BIN_DIR)/test_edit_distance
	$(CXX) $(CXXFLAGS) $(SRC) tests/cpp_tests/test_typo_correction.cpp -o $(BIN_DIR)/test_typo_correction
	@echo "Running all unit tests..."
	./$(BIN_DIR)/test_trie
	./$(BIN_DIR)/test_autocomplete
	./$(BIN_DIR)/test_edit_distance
	./$(BIN_DIR)/test_typo_correction
	@echo "All C++ Unit Tests Passed!"

benchmark: $(TARGET)
	./$(TARGET) --benchmark

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

