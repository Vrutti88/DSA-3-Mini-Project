/**
 * @file test_edit_distance.cpp
 * @brief Unit tests for standalone 2D DP Levenshtein Edit Distance calculation.
 */

#include "EditDistance.h"
#include <iostream>
#include <cassert>

void runEditDistanceTests() {
    std::cout << "--- [TEST SUITE: Levenshtein DP Calculations] ---\n";

    // 1. Identical words -> distance 0
    assert(dsa::EditDistance::calculateDistance("python", "python") == 0);
    assert(dsa::EditDistance::calculateDistance("", "") == 0);
    std::cout << "[PASS] Test 1: Identical strings distance = 0\n";

    // 2. One insertion -> distance 1
    assert(dsa::EditDistance::calculateDistance("pythn", "python") == 1); // insert 'o'
    assert(dsa::EditDistance::calculateDistance("appl", "apple") == 1);   // insert 'e'
    assert(dsa::EditDistance::calculateDistance("", "a") == 1);
    assert(dsa::EditDistance::calculateDistance("", "apple") == 5);
    std::cout << "[PASS] Test 2: Single insertion distance = 1\n";

    // 3. One deletion -> distance 1
    assert(dsa::EditDistance::calculateDistance("pythonx", "python") == 1); // delete 'x'
    assert(dsa::EditDistance::calculateDistance("apple", "") == 5);
    assert(dsa::EditDistance::calculateDistance("a", "") == 1);
    std::cout << "[PASS] Test 3: Single deletion distance = 1\n";

    // 4. One substitution -> distance 1
    assert(dsa::EditDistance::calculateDistance("pythpn", "python") == 1); // 'p' -> 'o'
    assert(dsa::EditDistance::calculateDistance("cat", "cut") == 1);
    assert(dsa::EditDistance::calculateDistance("cat", "hat") == 1);
    assert(dsa::EditDistance::calculateDistance("book", "back") == 2);
    assert(dsa::EditDistance::calculateDistance("horse", "ros") == 3);
    std::cout << "[PASS] Test 4: Single/Multiple substitution distance verified\n";

    // 5. Completely different words
    assert(dsa::EditDistance::calculateDistance("cat", "dog") == 3);
    assert(dsa::EditDistance::calculateDistance("abc", "xyz") == 3);
    assert(dsa::EditDistance::calculateDistance("algorithm", "trie") == 7);
    std::cout << "[PASS] Test 5: Completely different words\n";

    // 6. DP Matrix dimensions and boundary values
    auto matrix = dsa::EditDistance::getDPMatrix("kitten", "sitting");
    // Dimensions should be (6+1) x (7+1) = 7 x 8
    assert(matrix.size() == 7);
    assert(matrix[0].size() == 8);
    assert(matrix[6][7] == 3); // "kitten" -> "sitting" = 3 edits
    std::cout << "[PASS] Test 6: DP Matrix calculation verified\n";

    std::cout << ">>> ALL EDIT DISTANCE DP TESTS PASSED! <<<\n\n";
}

int main() {
    runEditDistanceTests();
    return 0;
}
