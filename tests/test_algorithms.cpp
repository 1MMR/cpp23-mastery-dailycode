#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <string>

// ==============================================================================
// GOOGLETEST SUITE: Move Semantics
// ==============================================================================

class DynamicArray {
public:
    int* data;
    size_t size;
    bool moved_from = false;
    
    DynamicArray(size_t sz) : data(new int[sz]), size(sz) {}
    
    DynamicArray(const DynamicArray& other) = delete;  // No copying
    
    DynamicArray(DynamicArray&& other) noexcept
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
        other.moved_from = true;
    }
    
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this == &other) return *this;
        delete[] data;
        data = other.data;
        size = other.size;
        other.data = nullptr;
        other.size = 0;
        other.moved_from = true;
        return *this;
    }
    
    ~DynamicArray() { delete[] data; }
};

TEST(MoveSemantics, MoveConstructor) {
    DynamicArray arr1(10);
    int* original_ptr = arr1.data;
    
    DynamicArray arr2 = std::move(arr1);
    
    EXPECT_EQ(arr2.data, original_ptr);
    EXPECT_EQ(arr2.size, 10);
    EXPECT_TRUE(arr1.moved_from);
    EXPECT_EQ(arr1.data, nullptr);
}

TEST(MoveSemantics, MoveAssignment) {
    DynamicArray arr1(10);
    DynamicArray arr2(5);
    int* original_ptr = arr1.data;
    
    arr2 = std::move(arr1);
    
    EXPECT_EQ(arr2.data, original_ptr);
    EXPECT_EQ(arr2.size, 10);
    EXPECT_TRUE(arr1.moved_from);
}

TEST(MoveSemantics, SelfAssignment) {
    DynamicArray arr(10);
    arr = std::move(arr);
    // Should not crash
    EXPECT_EQ(arr.size, 0);
}

// ==============================================================================
// GOOGLETEST SUITE: STL Algorithms
// ==============================================================================

TEST(STLAlgorithms, Sort) {
    std::vector<int> v = {5, 2, 8, 1, 9};
    std::sort(v.begin(), v.end());
    
    std::vector<int> expected = {1, 2, 5, 8, 9};
    EXPECT_EQ(v, expected);
}

TEST(STLAlgorithms, Find) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto it = std::find(v.begin(), v.end(), 3);
    
    EXPECT_NE(it, v.end());
    EXPECT_EQ(*it, 3);
}

TEST(STLAlgorithms, Transform) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::vector<int> result;
    
    std::transform(v.begin(), v.end(), std::back_inserter(result),
                  [](int x) { return x * 2; });
    
    std::vector<int> expected = {2, 4, 6, 8, 10};
    EXPECT_EQ(result, expected);
}

TEST(STLAlgorithms, Accumulate) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    int sum = std::accumulate(v.begin(), v.end(), 0);
    
    EXPECT_EQ(sum, 15);
}

TEST(STLAlgorithms, CopyIf) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    std::vector<int> evens;
    
    std::copy_if(v.begin(), v.end(), std::back_inserter(evens),
                [](int x) { return x % 2 == 0; });
    
    std::vector<int> expected = {2, 4, 6};
    EXPECT_EQ(evens, expected);
}

// ==============================================================================
// GOOGLETEST SUITE: Concepts (C++20+)
// ==============================================================================

TEST(TypeTraits, IsIntegral) {
    EXPECT_TRUE(std::is_integral_v<int>);
    EXPECT_TRUE(std::is_integral_v<long>);
    EXPECT_FALSE(std::is_integral_v<double>);
    EXPECT_FALSE(std::is_integral_v<std::string>);
}

TEST(TypeTraits, IsFloatingPoint) {
    EXPECT_TRUE(std::is_floating_point_v<double>);
    EXPECT_TRUE(std::is_floating_point_v<float>);
    EXPECT_FALSE(std::is_floating_point_v<int>);
}

TEST(TypeTraits, IsSame) {
    EXPECT_TRUE((std::is_same_v<int, int>));
    EXPECT_FALSE((std::is_same_v<int, double>));
    EXPECT_TRUE((std::is_same_v<decltype(5), int>));
}
