# C++23 Complete Language Reference & Cheat Sheet

> **Purpose**: Comprehensive reference for C++23 features, standard library, and best practices.  
> **Target**: GSoC/LFX candidates preparing for LLVM, KDE, Godot, CERN projects.

---

## Table of Contents

1. [Type System & Declarations](#type-system--declarations)
2. [Move Semantics & Perfect Forwarding](#move-semantics--perfect-forwarding)
3. [Templates & Metaprogramming](#templates--metaprogramming)
4. [Concepts & Constraints](#concepts--constraints)
5. [Memory Management & RAII](#memory-management--raii)
6. [STL Containers](#stl-containers)
7. [STL Algorithms](#stl-algorithms)
8. [Modern C++ Patterns](#modern-c-patterns)
9. [Coroutines](#coroutines)
10. [C++23 New Features](#c23-new-features)

---

## Type System & Declarations

### Auto Type Deduction

```cpp
// Template type deduction with auto
auto x = 42;                    // int
auto y = 3.14;                  // double
auto z = "hello";               // const char*
auto ptr = std::make_unique<int>(5);  // std::unique_ptr<int>

// Structured bindings (C++17)
auto [a, b, c] = std::tuple(1, 2.5, "three");

// Range-based structured binding
for (auto [key, value] : map) {
    // key and value deduced from pair
}
```

### Decltype & Type Traits

```cpp
int x = 5;
decltype(x) y = 10;             // int
decltype(x + 5) z = 15;         // int

// Type traits for compile-time checks
if constexpr (std::is_integral_v<T>) {
    // Compile-time branch for integral types
}

std::is_same_v<int, int>        // true (C++17 _v shorthand)
std::is_convertible_v<int, double>  // true
```

### References

```cpp
// Lvalue reference - binds to lvalue
int x = 5;
int& ref = x;           // OK
int& ref2 = 10;         // ERROR: can't bind rvalue to lvalue ref

// Rvalue reference - binds to rvalue (C++11+)
int&& rref = 10;        // OK
int&& rref2 = x;        // ERROR: x is lvalue

// Reference collapse rules
template<typename T>
void forward_ref(T&& t) {  // Universal reference
    // T&& + lvalue = T& (reference collapse)
    // T&& + rvalue = T&&
}
```

### Const Correctness

```cpp
// const object: cannot modify members
const int x = 5;        // x cannot change
x = 10;                 // ERROR

// const pointer: pointer cannot change
int* const ptr = &x;    // ptr cannot change
ptr = &y;               // ERROR
*ptr = 10;              // OK

// pointer to const: pointed-to value cannot change
const int* ptr = &x;    // int cannot change
*ptr = 10;              // ERROR
ptr = &y;               // OK

// const member function: cannot modify object
class A {
    void read() const { }   // Cannot modify *this
    void write() { }        // Can modify *this
};

const A obj;
obj.read();             // OK
obj.write();            // ERROR
```

---

## Move Semantics & Perfect Forwarding

### Lvalue vs Rvalue

```cpp
int x = 5;              // x is lvalue (has address)
5;                      // 5 is rvalue (temporary, no address)

// Function parameter binding
void func(int& arg);    // Accepts lvalues only
void func(int&& arg);   // Accepts rvalues only

int y = 10;
func(y);                // Calls func(int&) - y is lvalue
func(20);               // Calls func(int&&) - 20 is rvalue
```

### Move Constructor & Assignment

```cpp
class Resource {
    int* data;
    size_t size;
    
public:
    // Move constructor: steal resources from temporary
    Resource(Resource&& other) noexcept
        : data(other.data), size(other.size) {
        other.data = nullptr;   // Null out source
        other.size = 0;
    }
    
    // Move assignment: steal and deallocate old
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            delete[] data;          // Deallocate old
            data = other.data;
            size = other.size;
            other.data = nullptr;   // Null out source
            other.size = 0;
        }
        return *this;
    }
    
    // Destructor: safe with nulled source
    ~Resource() { delete[] data; }
};
```

### std::move() & std::move_if_noexcept()

```cpp
Resource a, b;

// std::move: explicitly cast lvalue to rvalue
b = std::move(a);       // Calls move assignment
                        // a is now in moved-from state

// std::move_if_noexcept: move only if noexcept
// Falls back to copy if move could throw
```

### Perfect Forwarding

```cpp
// Perfect forwarding pattern (C++11+)
template<typename T>
void wrapper(T&& arg) {
    // std::forward preserves lvalue/rvalue nature
    process(std::forward<T>(arg));
}

int x = 5;
wrapper(x);             // T = int&, forwards as lvalue
wrapper(10);            // T = int, forwards as rvalue

// Deduction rules for T:
// wrapper(lvalue) → T = T&, T&& collapses to T&
// wrapper(rvalue) → T = T, T&& stays T&&
```

### Copy-and-Swap Idiom

```cpp
class Resource {
    int* data;
    
public:
    // Single assignment operator handles both copy and move
    Resource& operator=(Resource temp) {  // Pass by value
        swap(*this, temp);  // Use noexcept swap
        return *this;
    }
    
    friend void swap(Resource& a, Resource& b) noexcept {
        using std::swap;
        swap(a.data, b.data);
    }
};
```

---

## Templates & Metaprogramming

### Function Templates

```cpp
// Basic function template
template<typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

max(5, 10);             // T = int
max(3.14, 2.71);        // T = double

// Multiple template parameters
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

// C++14: auto return type deduction
template<typename T, typename U>
auto multiply(T a, U b) {
    return a * b;        // Return type deduced
}

// C++20: abbreviated function template
void process(auto value) {  // Implicitly templated
    // Equivalent to: template<typename T> void process(T value)
}
```

### Class Templates

```cpp
template<typename T>
class Vector {
private:
    T* data;
    size_t capacity, size;
    
public:
    Vector() : data(nullptr), capacity(0), size(0) { }
    
    // Template member function
    template<typename U>
    Vector<T>& operator=(const Vector<U>& other) {
        // ...
    }
    
    // Specialization
    T& operator[](size_t idx) { return data[idx]; }
};

// Template specialization
template<>
class Vector<bool> {  // Specialization for bool
    // Different implementation for bitwise optimization
};
```

### Variadic Templates (Parameter Packs)

```cpp
// Variadic function template
template<typename... Args>
void print(Args... args) {
    // Unpack with fold expression (C++17)
    (std::cout << ... << args);  // Print all args
}

print(1, 2.5, "three");  // Prints: 12.5three

// Recursive unpacking (pre-C++17)
template<typename T>
void print_impl(T arg) {
    std::cout << arg;
}

template<typename T, typename... Rest>
void print_impl(T arg, Rest... rest) {
    std::cout << arg << " ";
    print_impl(rest...);  // Recursive call
}

// Fold expressions (C++17)
template<typename... Args>
auto sum(Args... args) {
    return (... + args);  // Left fold: (((0 + a) + b) + c)
    // return (args + ...);  // Right fold: (a + (b + (c + 0)))
}

sum(1, 2, 3, 4, 5);  // Returns 15
```

### SFINAE (Substitution Failure Is Not An Error)

```cpp
// SFINAE for function overload resolution
template<typename T>
typename std::enable_if<std::is_integral_v<T>, void>::type
process(T value) {
    std::cout << "Integral: " << value << "\n";
}

template<typename T>
typename std::enable_if<std::is_floating_point_v<T>, void>::type
process(T value) {
    std::cout << "Float: " << value << "\n";
}

process(42);    // Calls first (integral)
process(3.14);  // Calls second (floating point)

// C++20: Concepts (preferred over SFINAE)
template<typename T>
void process_v2(T value) requires std::integral<T> {
    std::cout << "Integral: " << value << "\n";
}
```

### Template Metaprogramming

```cpp
// Compile-time factorial
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

constexpr int fact5 = Factorial<5>::value;  // Computed at compile time

// Compile-time type selection
template<bool Cond, typename T, typename F>
struct If {
    using type = T;
};

template<typename T, typename F>
struct If<false, T, F> {
    using type = F;
};

using Result = If<true, int, double>::type;  // int
```

---

## Concepts & Constraints

### Concept Definitions (C++20)

```cpp
// Basic concept
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// Concept with requires clause
template<typename T>
concept Printable = requires(T t, std::ostream& os) {
    { os << t } -> std::convertible_to<std::ostream&>;
};

// Concept with multiple constraints
template<typename T>
concept Container = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<std::size_t>;
};

// Using concepts
template<Printable T>
void print(T value) {
    std::cout << value << "\n";
}

// Concept subsumption
template<typename T>
concept SignedArithmetic = Arithmetic<T> && std::is_signed_v<T>;
```

### Constraints

```cpp
// Function template with requires clause
template<typename T>
requires Arithmetic<T>
T add(T a, T b) {
    return a + b;
}

// Shorthand with auto
void process(auto value) { }  // Implicitly requires std::regular<T>

// Compound requirements
template<typename T>
concept RandomAccessContainer = Container<T> && requires(T t, size_t i) {
    { t[i] } -> std::convertible_to<typename T::value_type>;
};

// Nested constraints
template<typename T>
requires RandomAccessContainer<T>
void sort_and_print(T& container) {
    std::sort(container.begin(), container.end());
    for (const auto& elem : container) {
        std::cout << elem << " ";
    }
}
```

---

## Memory Management & RAII

### Smart Pointers

```cpp
// unique_ptr: exclusive ownership
std::unique_ptr<int> ptr1(new int(42));
std::unique_ptr<int> ptr2 = std::make_unique<int>(42);  // Preferred

// Transfer ownership
std::unique_ptr<int> ptr3 = std::move(ptr2);  // ptr2 now null
ptr2 = nullptr;                               // Safe

// shared_ptr: shared ownership (reference counting)
std::shared_ptr<int> sptr1 = std::make_shared<int>(42);
std::shared_ptr<int> sptr2 = sptr1;  // Increment ref count
// Both destroyed when ref count reaches 0

// weak_ptr: non-owning reference to shared_ptr
std::weak_ptr<int> wptr = sptr1;
if (auto sp = wptr.lock()) {  // Convert to shared_ptr if valid
    std::cout << *sp << "\n";
}
```

### RAII (Resource Acquisition Is Initialization)

```cpp
class FileManager {
    FILE* handle;
    
public:
    FileManager(const char* filename) {
        handle = fopen(filename, "r");  // Acquire resource
        if (!handle) throw std::runtime_error("Failed to open file");
    }
    
    ~FileManager() {
        if (handle) fclose(handle);     // Release resource
    }
    
    // Prevent copies (resource cleanup issue)
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;
    
    // Allow moves
    FileManager(FileManager&& other) noexcept
        : handle(other.handle) {
        other.handle = nullptr;
    }
};

// Usage: automatic cleanup
{
    FileManager file("data.txt");
    // Process file...
}  // Destructor called automatically, file closed
```

### std::lock_guard & std::unique_lock

```cpp
std::mutex mtx;

// Automatic lock/unlock
{
    std::lock_guard<std::mutex> lock(mtx);  // RAII
    // Critical section
}  // Lock released automatically

// Scoped lock (multiple mutexes, C++17)
std::mutex m1, m2;
std::scoped_lock lock(m1, m2);  // Deadlock-free
```

---

## STL Containers

### Sequence Containers

```cpp
// vector: Dynamic array, random access, fast insertion at end
std::vector<int> v = {1, 2, 3, 4, 5};
v.push_back(6);              // O(1) amortized
v.insert(v.begin(), 0);      // O(n)
v.erase(v.begin());          // O(n)
v[2];                        // O(1)

// deque: Double-ended queue, fast insertion at both ends
std::deque<int> d;
d.push_front(0);             // O(1)
d.push_back(5);              // O(1)

// list: Doubly-linked list, fast insertion anywhere (if you have iterator)
std::list<int> l = {1, 2, 3};
auto it = l.begin();
++it;
l.insert(it, 15);            // O(1)
l.erase(it);                 // O(1)

// forward_list: Singly-linked list, memory efficient
std::forward_list<int> fl;
```

### Associative Containers

```cpp
// map: Ordered key-value pairs, O(log n) operations
std::map<std::string, int> ages;
ages["Alice"] = 30;          // Insert/update
ages.at("Bob") = 25;         // at() throws if not found
ages.contains("Alice");      // C++20: true

// unordered_map: Hash table, average O(1) operations
std::unordered_map<std::string, int> scores;
scores["Alice"] = 95;

// set: Ordered unique elements
std::set<int> unique_values = {1, 2, 3, 2, 1};  // {1, 2, 3}

// unordered_set: Hash-based unique elements
std::unordered_set<int> uset;
```

### Container Adapters

```cpp
// stack: LIFO (Last In, First Out)
std::stack<int> s;
s.push(1);
s.push(2);
int top = s.top();           // 2
s.pop();

// queue: FIFO (First In, First Out)
std::queue<int> q;
q.push(1);
q.push(2);
int front = q.front();       // 1
q.pop();

// priority_queue: Heap-based, highest element first
std::priority_queue<int> pq;
pq.push(3);
pq.push(1);
pq.push(2);
int max_val = pq.top();      // 3
```

---

## STL Algorithms

### Searching & Sorting

```cpp
std::vector<int> v = {3, 1, 4, 1, 5, 9};

// Find
auto it = std::find(v.begin(), v.end(), 4);
if (it != v.end()) { /* found */ }

// Binary search (requires sorted range)
std::sort(v.begin(), v.end());
bool found = std::binary_search(v.begin(), v.end(), 4);

// Lower/upper bound
auto lower = std::lower_bound(v.begin(), v.end(), 4);  // First >= 4
auto upper = std::upper_bound(v.begin(), v.end(), 4);  // First > 4
```

### Transformations

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};
std::vector<int> result;

// Transform: apply function to each element
std::transform(v.begin(), v.end(), 
               std::back_inserter(result),
               [](int x) { return x * 2; });  // {2, 4, 6, 8, 10}

// Copy if: conditional copy
std::vector<int> even;
std::copy_if(v.begin(), v.end(),
             std::back_inserter(even),
             [](int x) { return x % 2 == 0; });  // {2, 4}

// Generate: fill with generated values
std::vector<int> seq(5);
std::generate(seq.begin(), seq.end(),
              [n = 0]() mutable { return n++; });  // {0, 1, 2, 3, 4}
```

### Numeric Algorithms

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

// Accumulate: sum/fold
int sum = std::accumulate(v.begin(), v.end(), 0);  // 15

// Inner product
std::vector<int> v2 = {2, 3, 4, 5, 6};
int dot = std::inner_product(v.begin(), v.end(), v2.begin(), 0);

// Partial sum
std::vector<int> partial(5);
std::partial_sum(v.begin(), v.end(), partial.begin());  // {1, 3, 6, 10, 15}

// Adjacent difference
std::vector<int> diff(5);
std::adjacent_difference(v.begin(), v.end(), diff.begin());  // {1, 1, 1, 1, 1}
```

### Permutations

```cpp
std::vector<int> v = {1, 2, 3};

// All permutations
while (std::next_permutation(v.begin(), v.end())) {
    // Process each permutation
}

// Previous permutation
std::prev_permutation(v.begin(), v.end());

// Merge sorted ranges
std::vector<int> a = {1, 3, 5};
std::vector<int> b = {2, 4, 6};
std::vector<int> merged;
std::merge(a.begin(), a.end(),
           b.begin(), b.end(),
           std::back_inserter(merged));  // {1, 2, 3, 4, 5, 6}
```

---

## Modern C++ Patterns

### CRTP (Curiously Recurring Template Pattern)

```cpp
// Base class template
template<typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

// Derived class
class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived implementation\n";
    }
};

// Usage: compile-time polymorphism (no virtual overhead)
Derived d;
d.interface();  // Calls Derived::implementation
```

### PIMPL (Pointer to Implementation)

```cpp
// Header: public interface
class Widget {
public:
    Widget();
    ~Widget();
    void operation();
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

// Implementation file
class Widget::Impl {
public:
    void operation() { /* ... */ }
};

Widget::Widget() : pimpl(std::make_unique<Impl>()) { }
Widget::~Widget() = default;
void Widget::operation() { pimpl->operation(); }
```

### Factory Pattern

```cpp
class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& msg) = 0;
};

class ConsoleLogger : public Logger {
public:
    void log(const std::string& msg) override {
        std::cout << msg << "\n";
    }
};

// Factory function (preferred over Factory class)
std::unique_ptr<Logger> create_logger(const std::string& type) {
    if (type == "console") {
        return std::make_unique<ConsoleLogger>();
    }
    throw std::invalid_argument("Unknown logger type");
}
```

### RAII Resource Management

```cpp
// Timer: automatic timing with RAII
class Timer {
    std::chrono::high_resolution_clock::time_point start;
    
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) { }
    
    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start);
        std::cout << "Elapsed: " << duration.count() << "ms\n";
    }
};

{
    Timer t;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}  // Destructor prints elapsed time
```

---

## Coroutines (C++20)

### Basic Coroutine

```cpp
#include <coroutine>

class SimpleAwaitable {
public:
    bool await_ready() const { return false; }
    void await_suspend(std::coroutine_handle<>) { }
    void await_resume() { }
};

// Generator-like coroutine
template<typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        
        Generator get_return_object() {
            return Generator(std::coroutine_handle<promise_type>::from_promise(*this));
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void unhandled_exception() { }
        
        std::suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
        
        void return_void() { }
    };
};

// Coroutine function
Generator<int> counter(int n) {
    for (int i = 0; i < n; ++i) {
        co_yield i;  // Suspend and yield value
    }
}

// Usage
for (int i : counter(5)) {
    std::cout << i << " ";  // Prints: 0 1 2 3 4
}
```

---

## C++23 New Features

### `if consteval` (C++23)

```cpp
constexpr int consteval_example(int x) {
    if consteval {
        // This branch only compiled if in constant context
        return x * 2;
    } else {
        // This branch only compiled for runtime
        return x + 1;
    }
}
```

### Attributes

```cpp
// [[nodiscard]]: Compiler warning if return value ignored
[[nodiscard]] int important_function() {
    return 42;
}

// [[deprecated]]: Mark as deprecated
[[deprecated("Use new_function instead")]]
void old_function() { }

// [[no_unique_address]]: Optimize empty member space
struct Empty { };

struct Wrapper {
    [[no_unique_address]] Empty e;
    int value;
};

// Guaranteed optimization: sizeof(Wrapper) == sizeof(int)
```

### Structured Concurrency (C++23 Preview)

```cpp
// Ranges library (C++20+)
std::vector<int> v = {1, 2, 3, 4, 5};

// Pipeline view transformations
for (int x : v | std::views::filter([](int n) { return n % 2 == 0; })
               | std::views::transform([](int n) { return n * 2; })) {
    std::cout << x << " ";  // 4 8
}
```

---

## Best Practices Checklist

### Design Principles
- ✅ Use const correctness everywhere
- ✅ Prefer `auto` for type deduction
- ✅ Use RAII for resource management
- ✅ Prefer move semantics over copying
- ✅ Use `std::unique_ptr` by default
- ✅ Concepts > SFINAE (C++20+)

### Error Handling
- ✅ Exceptions for exceptional conditions
- ✅ `noexcept` for non-throwing functions
- ✅ RAII ensures cleanup despite exceptions
- ✅ std::optional for optional values
- ✅ std::expected for error codes (C++23)

### Performance
- ✅ Use `const` and `constexpr` liberally
- ✅ Avoid unnecessary copies (use move)
- ✅ Prefer STL algorithms over raw loops
- ✅ Use `std::string_view` for string parameters
- ✅ Profile before optimizing

### Testing
- ✅ Unit test with GoogleTest
- ✅ Test edge cases and error conditions
- ✅ Enable all compiler warnings
- ✅ Use sanitizers (AddressSanitizer, MemorySanitizer)
- ✅ Run under `-std=c++23 -Wall -Wextra -Werror`

---

**Last Updated**: 2026-09-12  
**Reference**: Microsoft Learn C++ (msvc-170), cppreference.com, CppCon talks
