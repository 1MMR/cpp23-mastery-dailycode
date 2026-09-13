# Move Semantics: Deep Dive & Mastery

> **Critical for GSoC/LFX interviews**: Move semantics is THE most important C++11+ feature.  
> Understand it completely to stand out in senior-level interviews.

---

## Table of Contents

1. [Lvalue vs Rvalue Fundamentals](#lvalue-vs-rvalue-fundamentals)
2. [Move Constructor & Assignment](#move-constructor--assignment)
3. [std::move() & Move Semantics Pitfalls](#stdmove--move-semantics-pitfalls)
4. [Perfect Forwarding](#perfect-forwarding)
5. [Move Semantics in Practice](#move-semantics-in-practice)
6. [Common Mistakes & Solutions](#common-mistakes--solutions)

---

## Lvalue vs Rvalue Fundamentals

### Core Concept

```cpp
int x = 5;              // x is lvalue: persistent object with an address
5;                      // 5 is rvalue: temporary, no persistent address
&x;                     // OK: can take address of lvalue
&5;                     // ERROR: cannot take address of rvalue (temporaries)
```

### Lvalue vs Rvalue in Function Parameters

```cpp
void take_lvalue(int& arg);    // Accepts only lvalues
void take_rvalue(int&& arg);   // Accepts only rvalues
void take_const_lvalue(const int& arg);  // Accepts lvalues and rvalues

int x = 10;
take_lvalue(x);         // OK: x is lvalue
take_lvalue(20);        // ERROR: 20 is rvalue

take_rvalue(20);        // OK: 20 is rvalue
take_rvalue(x);         // ERROR: x is lvalue

take_const_lvalue(x);   // OK
take_const_lvalue(20);  // OK
```

### Rvalue Categories (C++11)

```cpp
class Widget {
public:
    Widget() { }
};

// Prvalue (pure rvalue): temporary object
Widget();               // Prvalue

// Xvalue (expiring lvalue): lvalue about to be destroyed
std::move(x);           // Xvalue (result of std::move)
std::forward<T>(t);     // Xvalue (result of std::forward)
```

---

## Move Constructor & Assignment

### Why Move Semantics Matters

```cpp
// Without move: copying is expensive
std::vector<int> v1(1000000);  // Large vector
std::vector<int> v2 = v1;      // COPIES all 1M elements (slow!)

// With move: ownership transfer
std::vector<int> v3 = std::move(v1);  // v1's data transferred (fast!)
// v1 is now empty/moved-from state
```

### Implementing Move Constructor

```cpp
class DynamicArray {
    int* data;
    size_t size;
    
public:
    // Default constructor
    DynamicArray() : data(nullptr), size(0) { }
    
    // Destructor: cleanup allocated memory
    ~DynamicArray() { delete[] data; }
    
    // Copy constructor: deep copy
    DynamicArray(const DynamicArray& other) 
        : data(new int[other.size]), size(other.size) {
        std::copy(other.data, other.data + size, data);
    }
    
    // Move constructor: steal ownership
    DynamicArray(DynamicArray&& other) noexcept
        : data(other.data), size(other.size) {
        other.data = nullptr;       // Critical: null out source
        other.size = 0;             // Prevent double-delete
    }
    
    // Copy assignment
    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) return *this;  // Self-assignment check
        
        delete[] data;              // Cleanup old
        data = new int[other.size];
        size = other.size;
        std::copy(other.data, other.data + size, data);
        return *this;
    }
    
    // Move assignment
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this == &other) return *this;  // Essential for move!
        
        delete[] data;              // Cleanup old
        data = other.data;
        size = other.size;
        other.data = nullptr;       // Null out source
        other.size = 0;
        return *this;
    }
};

// Usage
DynamicArray a(10);
DynamicArray b = std::move(a);  // Move constructor called
a = b;                          // Copy assignment (b is lvalue)
a = std::move(b);               // Move assignment (b is moved-from)
```

### Noexcept on Move Operations

```cpp
// ✅ CRITICAL: mark move operations as noexcept
class Vector {
    int* data;
    
public:
    Vector(Vector&& other) noexcept  // noexcept: safe to move in containers
        : data(other.data) {
        other.data = nullptr;
    }
    
    Vector& operator=(Vector&& other) noexcept {  // noexcept essential
        delete[] data;
        data = other.data;
        other.data = nullptr;
        return *this;
    }
};

// Why? std::vector will only use move if noexcept
std::vector<Vector> v;
v.reserve(10);          // Allocate space
Vector x;
v.push_back(std::move(x));  // Only moves if Vector's move is noexcept
// Otherwise, std::vector may copy instead (safety guarantee)
```

---

## std::move() & Move Semantics Pitfalls

### What std::move() Actually Does

```cpp
int x = 5;
int&& rref = std::move(x);  // Convert lvalue to rvalue reference

// std::move() implementation (simplified):
template<typename T>
typename std::remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(t);
}

// std::move does NOT move anything! It just casts to rvalue reference.
// The actual move happens in the move constructor/assignment.
```

### Moved-From Objects Are Valid But Unspecified

```cpp
std::string s1 = "hello";
std::string s2 = std::move(s1);  // s1 is moved-from

// s1 is still a valid string, but contents are unspecified
std::cout << s1.size();         // Undefined behavior? No, but unpredictable

// Safe practice: don't use moved-from objects
// EXCEPT for reset/reassign:
s1 = "new value";               // OK: reassign
s1.clear();                     // OK: reset
if (!s1.empty()) { }            // OK: check state

// UNSAFE: relying on moved-from value
auto x = s1[0];                 // UNSAFE: may crash if s1 is empty
```

### std::move() Does NOT Guarantee Move

```cpp
// Example 1: Object can't be moved efficiently
class NonMovable {
public:
    NonMovable(const NonMovable&) = default;
    NonMovable(NonMovable&&) = delete;  // Explicitly no move
};

NonMovable obj;
NonMovable obj2 = std::move(obj);  // ERROR: move constructor deleted

// Example 2: Implicit copying fallback
class OnlyCopy {
public:
    OnlyCopy(const OnlyCopy&) { }   // Only copy
};

OnlyCopy obj;
OnlyCopy obj2 = std::move(obj);    // Calls copy constructor (fallback)
```

---

## Perfect Forwarding

### The Problem: Information Loss

```cpp
template<typename T>
void wrapper(T arg) {
    process(arg);  // Problem: arg is always lvalue inside function
}

int x = 5;
wrapper(x);        // T = int, arg is lvalue
wrapper(10);       // T = int, arg is LVALUE (lost rvalue-ness!)
```

### The Solution: Universal References & std::forward()

```cpp
template<typename T>
void perfect_wrapper(T&& arg) {  // Universal reference (T&&)
    process(std::forward<T>(arg));  // Perfect forward
}

// Deduction rules:
// perfect_wrapper(lvalue) → T = int&, T&& collapses to int&
// perfect_wrapper(rvalue) → T = int, T&& stays int&&

int x = 5;
perfect_wrapper(x);         // Forwards as lvalue
perfect_wrapper(10);        // Forwards as rvalue

// Real use case: wrapper function for move-aware factory
template<typename... Args>
auto make_widget(Args&&... args) {
    return std::make_unique<Widget>(std::forward<Args>(args)...);
}
```

### Reference Collapsing Rules (C++11+)

```cpp
// When T&& is involved, references collapse:
// T& + & = T&        (lvalue + lvalue = lvalue)
// T& + && = T&       (lvalue + rvalue = lvalue)
// T + && = T&&       (rvalue + rvalue = rvalue)

template<typename T>
void forward_demo(T&& arg) {
    // If called with lvalue (int&):
    //   T = int&, T&& = int& && → int& (collapsed to lvalue ref)
    // If called with rvalue (int):
    //   T = int, T&& = int&& (stays rvalue ref)
}
```

### std::forward() Implementation

```cpp
// std::forward: conditional move based on T
template<typename T>
T&& forward(std::remove_reference_t<T>& arg) noexcept {
    return static_cast<T&&>(arg);
}

// In context:
template<typename T>
void wrapper(T&& arg) {
    // If T = int& (lvalue): std::forward<int&>(arg) → arg (lvalue)
    // If T = int (rvalue): std::forward<int>(arg) → static_cast<int&&>(arg)
    process(std::forward<T>(arg));
}
```

---

## Move Semantics in Practice

### Factory Functions (Perfect Forwarding)

```cpp
class Database {
    std::string connection_string;
    int pool_size;
    
public:
    Database(std::string conn, int pool) 
        : connection_string(std::move(conn)), pool_size(pool) { }
};

// Perfect forwarding factory
template<typename... Args>
std::unique_ptr<Database> create_database(Args&&... args) {
    return std::make_unique<Database>(std::forward<Args>(args)...);
}

// Usage: avoids unnecessary copies
create_database("localhost:5432", 10);
```

### Return Value Optimization (RVO) & Move

```cpp
// Even without std::move, compiler optimizes via RVO
std::string get_name() {
    std::string name = "Alice";
    return name;  // Compiler typically uses move/RVO, not copy
}

// Named Return Value Optimization (NRVO)
std::string build_string() {
    std::string result;
    result += "Hello ";
    result += "World";
    return result;  // Compiler uses NRVO (no copy/move needed)
}

// NRVO > move > copy (compiler preference order)
```

### Container Operations

```cpp
std::vector<std::string> names;

std::string temp = "Alice";
names.push_back(temp);              // Copy (temp is lvalue)
names.push_back(std::move(temp));   // Move (temp is moved-from)
names.push_back("Bob");             // Move (temporary)

// Container implements move-aware push_back:
// template<typename T>
// void push_back(T&& value) {
//     emplace_back(std::forward<T>(value));
// }
```

---

## Common Mistakes & Solutions

### Mistake 1: Forgetting noexcept on Move Operations

```cpp
// ❌ WRONG
class Resource {
public:
    Resource(Resource&& other) {  // Missing noexcept!
        // ...
    }
};

// std::vector<Resource> will NOT use this move constructor!
// It will copy instead to maintain strong exception safety.

// ✅ CORRECT
class Resource {
public:
    Resource(Resource&& other) noexcept {  // noexcept required
        // ...
    }
};
```

### Mistake 2: Using Moved-From Object

```cpp
// ❌ WRONG
std::string s1 = "data";
std::string s2 = std::move(s1);
std::cout << s1;  // UNDEFINED: s1 is in unspecified state

// ✅ CORRECT
std::string s1 = "data";
std::string s2 = std::move(s1);
s1 = "new data";  // Reset to valid state before use
std::cout << s1;  // OK
```

### Mistake 3: Returning std::move() Unnecessarily

```cpp
// ❌ WRONG (defeats RVO)
std::string get_name() {
    std::string name = "Alice";
    return std::move(name);  // Prevents RVO!
}

// ✅ CORRECT (allows RVO)
std::string get_name() {
    std::string name = "Alice";
    return name;  // Compiler uses RVO (better than explicit move)
}

// Note: RVO is mandatory in C++17+, so both are fine
// But explicit std::move can hinder optimization
```

### Mistake 4: Move-Only Types in Containers

```cpp
// ❌ WRONG: std::unique_ptr is move-only
std::vector<std::unique_ptr<int>> v;
auto ptr = std::make_unique<int>(42);
v.push_back(ptr);  // ERROR: cannot copy move-only type

// ✅ CORRECT: use std::move
std::vector<std::unique_ptr<int>> v;
auto ptr = std::make_unique<int>(42);
v.push_back(std::move(ptr));  // OK

// ✅ BETTER: direct construction
std::vector<std::unique_ptr<int>> v;
v.push_back(std::make_unique<int>(42));  // OK (temporary is rvalue)
```

### Mistake 5: Generic Code Not Using std::forward()

```cpp
// ❌ WRONG: loses lvalue/rvalue information
template<typename T>
void wrapper(T&& arg) {
    process(arg);  // arg is always lvalue here!
}

// ✅ CORRECT: perfect forwarding
template<typename T>
void wrapper(T&& arg) {
    process(std::forward<T>(arg));  // Preserves lvalue/rvalue
}
```

### Mistake 6: Move Constructor Not Nulling Resources

```cpp
// ❌ WRONG: destructor may double-delete
class Resource {
    int* data;
public:
    Resource(Resource&& other) {
        data = other.data;  // Forgot to null other.data!
    }
    ~Resource() { delete[] data; }
};

// ✅ CORRECT: null out source
class Resource {
    int* data;
public:
    Resource(Resource&& other) noexcept {
        data = other.data;
        other.data = nullptr;  // Essential!
    }
    ~Resource() { delete[] data; }
};
```

---

## Advanced: Move Semantics in Templates

### Generic Move Wrapper

```cpp
// Generic swap (leverages move semantics)
template<typename T>
void swap(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T> &&
                               std::is_nothrow_move_assignable_v<T>) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}
```

### Move-Aware Generic Code

```cpp
// Check if type is move-constructible
template<typename T>
void process(T obj) {
    if constexpr (std::is_move_constructible_v<T>) {
        // Can use move operations
        T other = std::move(obj);
    } else {
        // Fall back to copies
        T other = obj;
    }
}
```

---

## Checklist: Master Move Semantics

- [ ] Understand lvalue vs rvalue distinction
- [ ] Know when move constructor/assignment are called
- [ ] Always mark move operations as `noexcept`
- [ ] Understand reference collapsing rules
- [ ] Implement perfect forwarding with `T&&` and `std::forward<T>()`
- [ ] Know that `std::move()` is just a cast
- [ ] Handle moved-from objects correctly
- [ ] Recognize when explicit `std::move()` helps/hurts
- [ ] Understand RVO (Return Value Optimization)
- [ ] Implement move-aware container types

---

**Last Updated**: 2026-09-12  
**Prerequisite**: C++23_CHEATSHEET.md  
**Next**: Template Metaprogramming guide
