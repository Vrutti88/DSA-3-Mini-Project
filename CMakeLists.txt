cmake_minimum_required(VERSION 3.12)
project(SmartAutocompleteTypoCorrection VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /O2)
else()
    add_compile_options(-Wall -Wextra -O3)
endif()

include_directories(cpp/include)

set(LIB_SOURCES
    cpp/src/Trie.cpp
    cpp/src/Autocomplete.cpp
    cpp/src/EditDistance.cpp
    cpp/src/DictionaryScanner.cpp
    cpp/src/Benchmark.cpp
)

add_library(dsa_core STATIC ${LIB_SOURCES})

add_executable(engine cpp/src/main.cpp)
target_link_libraries(engine PRIVATE dsa_core)

# Test executables
enable_testing()

add_executable(test_trie tests/cpp_tests/test_trie.cpp)
target_link_libraries(test_trie PRIVATE dsa_core)
add_test(NAME TestTrie COMMAND test_trie)

add_executable(test_autocomplete tests/cpp_tests/test_autocomplete.cpp)
target_link_libraries(test_autocomplete PRIVATE dsa_core)
add_test(NAME TestAutocomplete COMMAND test_autocomplete)

add_executable(test_edit_distance tests/cpp_tests/test_edit_distance.cpp)
target_link_libraries(test_edit_distance PRIVATE dsa_core)
add_test(NAME TestEditDistance COMMAND test_edit_distance)

add_executable(test_typo_correction tests/cpp_tests/test_typo_correction.cpp)
target_link_libraries(test_typo_correction PRIVATE dsa_core)
add_test(NAME TestTypoCorrection COMMAND test_typo_correction)

