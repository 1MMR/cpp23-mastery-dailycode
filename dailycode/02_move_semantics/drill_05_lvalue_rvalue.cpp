#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <string>

// ==============================================================================
// DRILL 02: Lvalue vs Rvalue, Rvalue References
// ==============================================================================
// Focus: Master the distinction between lvalue and rvalue
// Time: 30 minutes
// Compile: g++-13 -std=c++23 -Wall -Wextra -Werror drill_05_lvalue_rvalue.cpp

int main() {
    std::cout << "\n=== DRILL 05: Lvalue vs Rvalue ===\n";
    
    // ========================================================================
    // 1. Lvalue vs Rvalue Basics
    // ========================================================================
    std::cout << "\n1. Lvalue vs Rvalue:\n";
    
    int x = 5;                          // x is lvalue (has address)
    int y = x + 1;                      // (x + 1) is rvalue (temporary)
    
    std::cout << "Address of x (lvalue): " << &x << "\n";
    // std::cout << "Address of 5 (rvalue): " << &5 << "\n";  // ERROR
    
    std::cout << "x is lvalue, 5 is rvalue\n";
    
    // ========================================================================
    // 2. Lvalue and Rvalue References
    // ========================================================================
    std::cout << "\n2. Binding to References:\n";
    
    int& lvalue_ref = x;                // Lvalue reference binds to lvalue
    // int& lvalue_ref2 = 5;  // ERROR: cannot bind rvalue to lvalue ref
    
    const int& const_lvalue_ref = 5;    // const lvalue ref binds to rvalue
    
    int&& rvalue_ref = 10;              // Rvalue reference binds to rvalue
    // int&& rvalue_ref2 = x;  // ERROR: cannot bind lvalue to rvalue ref
    
    std::cout << "Lvalue ref binds to: lvalue\n";
    std::cout << "const Lvalue ref binds to: lvalue or rvalue\n";
    std::cout << "Rvalue ref binds to: rvalue\n";
    
    // ========================================================================
    // 3. Function Overload Resolution
    // ========================================================================
    std::cout << "\n3. Function Overload Resolution:\n";
    
    auto process_lvalue = [](int& x) { std::cout << "Called with lvalue\n"; };
    auto process_rvalue = [](int&& x) { std::cout << "Called with rvalue\n"; };
    
    struct CallResolver {
        void operator()(int& x) { std::cout << "Called with lvalue\n"; }
        void operator()(int&& x) { std::cout << "Called with rvalue\n"; }
    };
    
    CallResolver resolver;
    int val = 5;
    resolver(val);                      // Calls lvalue version
    resolver(10);                       // Calls rvalue version
    
    // ========================================================================
    // 4. Lvalue to Rvalue Conversion with std::move
    // ========================================================================
    std::cout << "\n4. std::move (convert lvalue to rvalue):\n";
    
    std::string s1 = "hello";
    std::string s2 = std::move(s1);     // s1 converted to rvalue
    
    std::cout << "s1 after move (moved-from state): '" << s1 << "'\n";
    std::cout << "s2 after move: '" << s2 << "'\n";
    assert(s2 == "hello");
    // s1 is in valid but unspecified state
    
    // ========================================================================
    // 5. Rvalue References in Function Parameters
    // ========================================================================
    std::cout << "\n5. Rvalue Reference Parameters:\n";
    
    auto take_rvalue = [](std::string&& str) {
        std::cout << "Received rvalue: " << str << "\n";
        // Can safely move from str
        std::string local = std::move(str);
    };
    
    take_rvalue("temporary string");    // Temporary rvalue
    
    std::string owned = "owned string";
    take_rvalue(std::move(owned));      // Convert lvalue to rvalue
    // owned is now in moved-from state
    
    // ========================================================================
    // 6. Temporary Objects (Prvalues)
    // ========================================================================
    std::cout << "\n6. Temporary Objects:\n";
    
    struct Widget {
        Widget() { std::cout << "  Widget constructed\n"; }
        ~Widget() { std::cout << "  Widget destroyed\n"; }
        Widget(const Widget&) = delete;   // Prevent copying
        Widget(Widget&&) { std::cout << "  Widget moved\n"; }
    };
    
    {
        std::cout << "Creating temporary Widget::\n";
        Widget&& temp = Widget();         // Temporary widget
        std::cout << "Temporary extended beyond expression\n";
    }  // Widget destroyed here
    
    // ========================================================================
    // 7. Move Constructors and Assignment
    // ========================================================================
    std::cout << "\n7. Move Semantics:\n";
    
    class DynamicArray {
    public:
        int* data;
        size_t size;
        
        DynamicArray(size_t sz) : data(new int[sz]), size(sz) {
            std::cout << "    Constructor: allocated " << sz << " ints\n";
        }
        
        DynamicArray(DynamicArray&& other) noexcept
            : data(other.data), size(other.size) {
            other.data = nullptr;         // Critical: null out source
            other.size = 0;
            std::cout << "    Move constructor: stole resources\n";
        }
        
        ~DynamicArray() {
            delete[] data;
            if (data) std::cout << "    Destructor: freed memory\n";
        }
    };
    
    {
        std::cout << "Creating and moving array:\n";
        DynamicArray arr1(5);
        std::cout << "Before move: arr1.data = " << (arr1.data != nullptr ? "valid" : "null") << "\n";
        
        DynamicArray arr2 = std::move(arr1);
        std::cout << "After move: arr1.data = " << (arr1.data != nullptr ? "valid" : "null") << "\n";
        std::cout << "After move: arr2.data = " << (arr2.data != nullptr ? "valid" : "null") << "\n";
    }
    
    // ========================================================================
    // 8. Xvalue (Expiring Value)
    // ========================================================================
    std::cout << "\n8. Xvalue (result of std::move):\n";
    
    int num = 42;
    int& lval_ref = num;                 // lvalue reference
    int&& xval_ref = std::move(num);     // xvalue reference
    
    std::cout << "num through xvalue ref: " << xval_ref << "\n";
    
    // ========================================================================
    // 9. Return Value Optimization (RVO)
    // ========================================================================
    std::cout << "\n9. Return Value Optimization:\n";
    
    auto create_vector = []() {
        std::vector<int> v = {1, 2, 3, 4, 5};
        return v;  // Compiler optimizes: no copy/move needed (C++17+)
    };
    
    std::vector<int> result = create_vector();  // RVO: no copies
    std::cout << "Vector created via RVO, size: " << result.size() << "\n";
    
    // ========================================================================
    // 10. Common Mistakes
    // ========================================================================
    std::cout << "\n10. Common Mistakes:\n";
    
    // Mistake 1: Returning std::move from function (prevents RVO)
    auto bad_return = []() {
        std::string s = "result";
        return s;  // Better: let compiler use RVO
        // return std::move(s);  // Worse: explicit move prevents RVO
    };
    
    // Mistake 2: Using moved-from object
    std::string str = "data";
    std::string str_moved = std::move(str);
    // std::cout << str;  // UNSAFE: str is in undefined state
    
    std::cout << "Compiler-optimized return (RVO) is better than explicit move\n";
    
    std::cout << "\n=== All Tests Passed ===\n";
    return 0;
}
