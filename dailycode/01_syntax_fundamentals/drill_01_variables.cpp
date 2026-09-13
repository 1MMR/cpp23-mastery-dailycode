#include <iostream>
#include <vector>
#include <string>
#include <cassert>

// ==============================================================================
// DRILL 01: Variables, Auto Type Deduction, and Declarations
// ==============================================================================
// Focus: Understand variable initialization, auto type deduction, and const
// Time: 30 minutes
// Compile: g++-13 -std=c++23 -Wall -Wextra -Werror drill_01_variables.cpp

int main() {
    std::cout << "\n=== DRILL 01: Variables & Type Deduction ===\n";
    
    // ========================================================================
    // 1. Variable Initialization
    // ========================================================================
    std::cout << "\n1. Variable Initialization:\n";
    
    int x = 5;                          // Copy initialization
    int y(10);                          // Direct initialization
    int z{15};                          // Uniform initialization (preferred)
    
    std::cout << "x=" << x << ", y=" << y << ", z=" << z << "\n";
    assert(x == 5 && y == 10 && z == 15);
    
    // Narrowing conversion check (uniform init prevents this)
    // int w{3.14};  // ERROR: narrowing conversion
    
    // ========================================================================
    // 2. Auto Type Deduction
    // ========================================================================
    std::cout << "\n2. Auto Type Deduction:\n";
    
    auto a = 42;                        // int
    auto b = 3.14;                      // double
    auto c = "hello";                   // const char*
    auto d = std::string("world");      // std::string
    
    std::cout << "a (int): " << a << ", size: " << sizeof(a) << " bytes\n";
    std::cout << "b (double): " << b << ", size: " << sizeof(b) << " bytes\n";
    std::cout << "d (string): " << d << "\n";
    
    // ========================================================================
    // 3. Structured Bindings (C++17+)
    // ========================================================================
    std::cout << "\n3. Structured Bindings:\n";
    
    std::pair<int, std::string> pair = {42, "answer"};
    auto [value, name] = pair;          // Unpack pair
    
    std::cout << "Unpacked: value=" << value << ", name=" << name << "\n";
    assert(value == 42 && name == "answer");
    
    // Array unpacking
    int arr[] = {1, 2, 3};
    auto [x1, x2, x3] = arr;            // ERROR in some compilers (non-standard)
    // Workaround: use std::array instead
    std::array<int, 3> array = {1, 2, 3};
    auto [y1, y2, y3] = array;
    std::cout << "Array unpacked: " << y1 << ", " << y2 << ", " << y3 << "\n";
    
    // ========================================================================
    // 4. Const Correctness
    // ========================================================================
    std::cout << "\n4. Const Correctness:\n";
    
    const int const_var = 100;          // Variable cannot change
    // const_var = 200;  // ERROR
    
    int var = 50;
    const int* ptr_to_const = &var;     // Pointer to const int
    // *ptr_to_const = 60;  // ERROR
    ptr_to_const = &const_var;          // OK: can change what pointer points to
    
    int* const const_ptr = &var;        // Const pointer to int
    *const_ptr = 60;                    // OK: can modify what pointer points to
    // const_ptr = &const_var;  // ERROR: cannot change pointer itself
    
    const int* const const_ptr_const = &const_var;  // Both const
    // *const_ptr_const = 100;  // ERROR
    // const_ptr_const = &var;   // ERROR
    
    std::cout << "const int* allows modifying pointer, not pointee\n";
    std::cout << "int* const allows modifying pointee, not pointer\n";
    std::cout << "const int* const allows neither\n";
    
    // ========================================================================
    // 5. Type Deduction with References
    // ========================================================================
    std::cout << "\n5. Type Deduction with References:\n";
    
    int val = 42;
    int& ref = val;                     // Lvalue reference
    auto auto_ref = ref;                // auto deduces to int, NOT int&
    auto& auto_ref_type = ref;          // auto& deduces to int&
    
    std::cout << "auto deduces ref to: int (size=" << sizeof(auto_ref) << ")\n";
    std::cout << "auto& deduces ref to: int& (size=" << sizeof(auto_ref_type) << ")\n";
    
    // ========================================================================
    // 6. Decltype
    // ========================================================================
    std::cout << "\n6. Decltype:\n";
    
    int x_val = 5;
    decltype(x_val) y_val = 10;         // decltype(x_val) = int
    
    decltype(x_val + 1) z_val = 15;     // decltype(x_val + 1) = int
    
    std::cout << "decltype(x_val) = int, y_val=" << y_val << "\n";
    std::cout << "decltype(x_val + 1) = int, z_val=" << z_val << "\n";
    
    // ========================================================================
    // 7. constexpr (Compile-Time Constant)
    // ========================================================================
    std::cout << "\n7. constexpr:\n";
    
    constexpr int compile_time_const = 42;  // Computed at compile time
    constexpr double pi = 3.14159;          // Compile-time constant
    
    // Can use in constant expressions
    int array_size[compile_time_const];     // OK: array size is constexpr
    std::cout << "Array size: " << compile_time_const << "\n";
    
    // ========================================================================
    // 8. Type Traits Check
    // ========================================================================
    std::cout << "\n8. Type Traits (C++17+):\n";
    
    static_assert(std::is_integral_v<int> == true);      // Compile-time assertion
    static_assert(std::is_integral_v<double> == false);
    static_assert(std::is_floating_point_v<double> == true);
    
    std::cout << "Type traits verified at compile time\n";
    
    // ========================================================================
    // 9. Vector and Container Type Deduction
    // ========================================================================
    std::cout << "\n9. Container Type Deduction:\n";
    
    std::vector<int> v1 = {1, 2, 3, 4, 5};
    std::vector v2{1, 2, 3, 4, 5};      // C++17: deduced type (vector<int>)
    
    auto it = v1.begin();                // auto deduces to vector<int>::iterator
    std::cout << "First element via iterator: " << *it << "\n";
    
    // ========================================================================
    // 10. Edge Cases and Best Practices
    // ========================================================================
    std::cout << "\n10. Edge Cases:\n";
    
    // Avoid: auto with initializer_list
    auto list = {1, 2, 3};              // Type: initializer_list<int>
    std::vector<int> v(list);           // Convert to vector
    
    // Best practice: explicit cast for clarity
    auto count = static_cast<size_t>(v.size());
    std::cout << "Vector size (explicit cast): " << count << "\n";
    
    std::cout << "\n=== All Tests Passed ===\n";
    return 0;
}
