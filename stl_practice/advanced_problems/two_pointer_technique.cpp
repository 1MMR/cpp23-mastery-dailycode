#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

// ==============================================================================
// STL PRACTICE: Two-Pointer Technique
// ==============================================================================
// Focus: Solve problems using two-pointer approach with STL
// Compile: g++-13 -std=c++23 -Wall -Wextra -Werror two_pointer_technique.cpp

int main() {
    std::cout << "\n=== Two-Pointer Technique ===\n";
    
    // ========================================================================
    // Problem 1: Container With Most Water
    // ========================================================================
    std::cout << "\nProblem 1: Container With Most Water\n";
    
    auto max_area = [](const std::vector<int>& heights) {
        int left = 0, right = heights.size() - 1;
        int max_area = 0;
        
        while (left < right) {
            int width = right - left;
            int height = std::min(heights[left], heights[right]);
            int area = width * height;
            max_area = std::max(max_area, area);
            
            // Move pointer at smaller height
            if (heights[left] < heights[right]) {
                ++left;
            } else {
                --right;
            }
        }
        
        return max_area;
    };
    
    std::vector<int> heights = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    int result = max_area(heights);
    std::cout << "Maximum area: " << result << "\n";
    assert(result == 49);  // Between indices 1 and 8
    
    // ========================================================================
    // Problem 2: Two Sum (sorted array)
    // ========================================================================
    std::cout << "\nProblem 2: Two Sum (Sorted Array)\n";
    
    auto two_sum = [](const std::vector<int>& nums, int target) {
        std::vector<int> sorted_nums = nums;
        std::sort(sorted_nums.begin(), sorted_nums.end());
        
        int left = 0, right = sorted_nums.size() - 1;
        std::vector<std::pair<int, int>> pairs;
        
        while (left < right) {
            int sum = sorted_nums[left] + sorted_nums[right];
            if (sum == target) {
                pairs.push_back({sorted_nums[left], sorted_nums[right]});
                ++left;
                --right;
            } else if (sum < target) {
                ++left;
            } else {
                --right;
            }
        }
        
        return pairs;
    };
    
    std::vector<int> nums = {2, 7, 11, 15, 3, 6};
    auto pairs = two_sum(nums, 9);
    std::cout << "Pairs summing to 9: ";
    std::for_each(pairs.begin(), pairs.end(), [](const auto& p) {
        std::cout << "(" << p.first << "," << p.second << ") ";
    });
    std::cout << "\n";
    assert(pairs.size() == 2);  // (2,7) and (3,6)
    
    // ========================================================================
    // Problem 3: Merge Sorted Arrays
    // ========================================================================
    std::cout << "\nProblem 3: Merge Sorted Arrays\n";
    
    std::vector<int> arr1 = {1, 3, 5};
    std::vector<int> arr2 = {2, 4, 6};
    
    std::vector<int> merged;
    std::merge(arr1.begin(), arr1.end(),
              arr2.begin(), arr2.end(),
              std::back_inserter(merged));
    
    std::cout << "Merged: ";
    std::for_each(merged.begin(), merged.end(), [](int x) { std::cout << x << " "; });
    std::cout << "\n";
    assert(merged == std::vector<int>{1, 2, 3, 4, 5, 6});
    
    // ========================================================================
    // Problem 4: Remove Duplicates (sorted array)
    // ========================================================================
    std::cout << "\nProblem 4: Remove Duplicates (In-Place)\n";
    
    std::vector<int> arr = {1, 1, 2, 2, 2, 3, 4, 4, 5};
    auto new_end = std::unique(arr.begin(), arr.end());
    arr.erase(new_end, arr.end());
    
    std::cout << "After removing duplicates: ";
    std::for_each(arr.begin(), arr.end(), [](int x) { std::cout << x << " "; });
    std::cout << "\n";
    assert(arr == std::vector<int>{1, 2, 3, 4, 5});
    
    // ========================================================================
    // Problem 5: Partition (move negatives to front)
    // ========================================================================
    std::cout << "\nProblem 5: Partition Negatives to Front\n";
    
    std::vector<int> mixed = {3, -1, 4, -2, 5, -3};
    auto partition_point = std::partition(mixed.begin(), mixed.end(),
                                         [](int x) { return x < 0; });
    
    std::cout << "After partition: ";
    std::for_each(mixed.begin(), mixed.end(), [](int x) { std::cout << x << " "; });
    std::cout << "\n";
    
    // All negatives should be before partition_point
    bool all_negative = std::all_of(mixed.begin(), partition_point,
                                    [](int x) { return x < 0; });
    assert(all_negative);
    
    std::cout << "\n=== All Tests Passed ===\n";
    return 0;
}
