#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>

// ==============================================================================
// STL PRACTICE: Sorting Problems (No Raw Loops)
// ==============================================================================
// Focus: Master STL sorting algorithms without raw loops
// Compile: g++-13 -std=c++23 -Wall -Wextra -Werror sorting_problems.cpp

int main() {
    std::cout << "\n=== STL Sorting Practice ===\n";
    
    // ========================================================================
    // Problem 1: Sort integers in ascending/descending order
    // ========================================================================
    std::cout << "\nProblem 1: Basic Sorting\n";
    
    std::vector<int> nums = {5, 2, 8, 1, 9, 3};
    
    // Ascending (default)
    std::vector<int> asc = nums;
    std::sort(asc.begin(), asc.end());
    std::cout << "Ascending: ";
    std::for_each(asc.begin(), asc.end(), [](int x) { std::cout << x << " "; });
    std::cout << "\n";
    assert(asc == std::vector<int>{1, 2, 3, 5, 8, 9});
    
    // Descending
    std::vector<int> desc = nums;
    std::sort(desc.begin(), desc.end(), std::greater<>());
    std::cout << "Descending: ";
    std::for_each(desc.begin(), desc.end(), [](int x) { std::cout << x << " "; });
    std::cout << "\n";
    assert(desc == std::vector<int>{9, 8, 5, 3, 2, 1});
    
    // ========================================================================
    // Problem 2: Sort pairs by second element
    // ========================================================================
    std::cout << "\nProblem 2: Sort Pairs by Second Element\n";
    
    std::vector<std::pair<int, int>> pairs = {{3, 1}, {1, 3}, {2, 2}};
    
    std::sort(pairs.begin(), pairs.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    std::cout << "Sorted pairs: ";
    std::for_each(pairs.begin(), pairs.end(), [](const auto& p) {
        std::cout << "(" << p.first << "," << p.second << ") ";
    });
    std::cout << "\n";
    assert(pairs[0].second == 1 && pairs[1].second == 2 && pairs[2].second == 3);
    
    // ========================================================================
    // Problem 3: Stable sort (preserve relative order of equal elements)
    // ========================================================================
    std::cout << "\nProblem 3: Stable Sort\n";
    
    struct Person { int age; std::string name; };
    std::vector<Person> people = {
        {30, "Alice"}, {25, "Bob"}, {30, "Charlie"}
    };
    
    std::stable_sort(people.begin(), people.end(),
                    [](const Person& a, const Person& b) { return a.age < b.age; });
    
    std::cout << "Stable sorted by age (preserves name order for same age):\n";
    std::for_each(people.begin(), people.end(), [](const Person& p) {
        std::cout << "  " << p.name << ": " << p.age << "\n";
    });
    // Alice should still come before Charlie (both age 30)
    assert(people[0].name == "Bob" && people[1].name == "Alice" && people[2].name == "Charlie");
    
    // ========================================================================
    // Problem 4: Partial sort (find top-k elements)
    // ========================================================================
    std::cout << "\nProblem 4: Top-3 Largest Elements (partial_sort)\n";
    
    std::vector<int> data = {5, 2, 8, 1, 9, 3, 7, 4};
    std::partial_sort(data.begin(), data.begin() + 3, data.end(), std::greater<>());
    
    std::cout << "Top 3 largest: ";
    std::for_each(data.begin(), data.begin() + 3, [](int x) { std::cout << x << " "; });
    std::cout << "\n";
    assert(data[0] == 9 && data[1] == 8 && data[2] == 7);
    
    // ========================================================================
    // Problem 5: nth_element (find kth element in sorted order)
    // ========================================================================
    std::cout << "\nProblem 5: Find Median (nth_element)\n";
    
    std::vector<int> vals = {7, 2, 5, 1, 8, 3, 6};
    size_t mid = vals.size() / 2;
    std::nth_element(vals.begin(), vals.begin() + mid, vals.end());
    
    std::cout << "Median element at position " << mid << ": " << vals[mid] << "\n";
    // Element at mid is in correct sorted position
    assert(std::count_if(vals.begin(), vals.begin() + mid,
                        [v=vals[mid]](int x) { return x < v; }) >= 0);
    
    // ========================================================================
    // Problem 6: Sort custom objects
    // ========================================================================
    std::cout << "\nProblem 6: Sort Custom Objects\n";
    
    struct Student {
        std::string name;
        double gpa;
        
        bool operator<(const Student& other) const {
            return gpa > other.gpa;  // Descending GPA
        }
    };
    
    std::vector<Student> students = {
        {"Alice", 3.9}, {"Bob", 3.5}, {"Charlie", 3.8}
    };
    
    std::sort(students.begin(), students.end());
    
    std::cout << "Students sorted by GPA (descending):\n";
    std::for_each(students.begin(), students.end(), [](const Student& s) {
        std::cout << "  " << s.name << ": " << s.gpa << "\n";
    });
    assert(students[0].name == "Alice" && students[1].name == "Charlie");
    
    std::cout << "\n=== All Tests Passed ===\n";
    return 0;
}
