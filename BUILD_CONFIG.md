# Build Configuration & Compiler Flags

## Strict Compilation Standard

All code in this project compiles cleanly with:

```bash
-std=c++23 -Wall -Wextra -Werror -pedantic \
-Wconversion -Wshadow -Wold-style-cast \
-Woverloaded-virtual -Wunused -Wuninitialized \
-Wnull-dereference -Wdouble-promotion
```

### Flag Breakdown

| Flag | Purpose | Common Issues Caught |
|------|---------|----------------------|
| `-std=c++23` | Enable C++23 standard | Deprecated features, new syntax |
| `-Wall` | Enable common warnings | Uninitialized variables |
| `-Wextra` | Additional warnings | Unused parameters, type issues |
| `-Werror` | Treat warnings as errors | Forces fix before compilation |
| `-pedantic` | Strict standard compliance | Non-standard extensions |
| `-Wconversion` | Implicit type conversions | Signed/unsigned mismatches |
| `-Wshadow` | Variable shadowing | Hidden variables in nested scopes |
| `-Wold-style-cast` | Deprecated C-style casts | Use static_cast, dynamic_cast, etc. |
| `-Woverloaded-virtual` | Virtual function hiding | Overload resolution issues |
| `-Wuninitialized` | Uninitialized variables | Undefined behavior |
| `-Wnull-dereference` | Null pointer dereference | Crash detection |
| `-Wdouble-promotion` | float→double promotion | Precision loss |

---

## Build Instructions

### Linux/macOS (GCC/Clang)

```bash
# Configure
mkdir build && cd build
cmake -DCMAKE_CXX_FLAGS="-std=c++23 -Wall -Wextra -Werror" ..

# Build
cmake --build . --parallel $(nproc)

# Test
ctest --output-on-failure

# Run specific executable
./dailycode/02_move_semantics/drill_05_move_constructors
```

### Windows (MSVC)

```bash
# Configure
mkdir build && cd build
cmake -G "Visual Studio 17 2022" ..

# Build (with strict flags via CMakeLists.txt)
cmake --build . --config Release -- /W4 /WX

# Test
ctest -C Release --output-on-failure
```

### Docker (Guaranteed Environment)

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    cmake \
    g++-13 \
    clang-16 \
    git

WORKDIR /workspace
COPY . .

RUN mkdir build && cd build && \
    cmake -DCMAKE_CXX_COMPILER=g++-13 \
          -DCMAKE_CXX_FLAGS="-std=c++23 -Wall -Wextra -Werror" .. && \
    cmake --build . --parallel 4
```

---

## Compiler Support Matrix

| Compiler | Version | C++23 Support | Notes |
|----------|---------|---------------|-------|
| GCC | 13.x+ | ✅ Full | Use `-std=c++23` |
| Clang | 16.x+ | ✅ Full | Use `-std=c++23` |
| MSVC | 17.x+ | ✅ Partial | Use `/std:c++latest` |
| Apple Clang | 15.x+ | ✅ Partial | macOS 13.5+ required |

### Verified Configurations

```bash
# GCC 13
g++-13 -std=c++23 -Wall -Wextra -Werror drill_05.cpp -o drill_05

# Clang 16
clang++-16 -std=c++23 -Wall -Wextra -Werror drill_05.cpp -o drill_05

# MSVC (cl.exe)
cl.exe /std:c++latest /W4 /WX drill_05.cpp
```

---

## Static Analysis Integration

### clang-tidy (LLVM Project)

Automatically check code quality:

```bash
clang-tidy -checks=modernize-*,readability-*,performance-* \
           --fix \
           dailycode/**/*.cpp -- -std=c++23
```

### Common Issues Fixed Automatically

- Use auto for variable deduction
- Replace deprecated STL usage
- Remove unused variables
- Simplify boolean logic
- Use range-based for loops

### cppcheck (Lightweight)

```bash
cppcheck --enable=all --suppress=missingIncludeSystem \
         --std=c++23 dailycode/ stl_practice/
```

---

## Unit Test Coverage

All code covered by GoogleTest:

```bash
cd build
ctest --output-on-failure

# Run specific test
ctest -R "MoveSemantics" --output-on-failure

# Verbose output
ctest --output-on-failure -V
```

### Coverage Reports (Optional)

```bash
# Enable coverage
cmake -DCMAKE_CXX_FLAGS="--coverage" ..
cmake --build .
ctest

# Generate HTML report (Linux)
gcovr --html-details coverage.html
```

---

## Continuous Integration (GitHub Actions)

`.github/workflows/build.yml` automatically:

1. ✅ Compiles with GCC 13, Clang 16, MSVC 17
2. ✅ Runs all GoogleTests
3. ✅ Executes clang-tidy checks
4. ✅ Verifies zero compiler warnings
5. ✅ Generates coverage reports

**Status Badge**: ![Build Status](https://github.com/1MMR/cpp23-mastery-dailycode/actions/workflows/build.yml/badge.svg)

---

## Known Warnings & Resolutions

### Warning: "narrowing conversion"
```cpp
// ❌ Will fail (-Wconversion)
int value = 3.14;

// ✅ Explicit cast
int value = static_cast<int>(3.14);
```

### Warning: "variable shadowing"
```cpp
// ❌ Will fail (-Wshadow)
{
    int x = 5;
    {
        int x = 10;  // Shadows outer x
    }
}

// ✅ Rename inner variable
{
    int outer_x = 5;
    {
        int inner_x = 10;
    }
}
```

### Warning: "unused parameter"
```cpp
// ❌ Will fail (-Wunused)
void process(int unused_value) { }

// ✅ Use [[maybe_unused]]
void process([[maybe_unused]] int value) { }
```

### Warning: "old-style cast"
```cpp
// ❌ Will fail (-Wold-style-cast)
int* ptr = (int*)malloc(sizeof(int));

// ✅ Use modern casts
auto ptr = static_cast<int*>(malloc(sizeof(int)));

// ✅ Better: use new
auto ptr = std::make_unique<int>();
```

---

## Performance Optimization Flags (Optional)

For release builds with maximum optimization:

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native" \
      ..
cmake --build . --config Release
```

---

## Troubleshooting

### Issue: "C++23 not supported"
**Solution**: Update compiler
```bash
# Ubuntu/Debian
sudo apt-get install g++-13 clang-16

# macOS
brew install gcc@13
```

### Issue: CMake not found
**Solution**: Install CMake 3.23+
```bash
sudo apt-get install cmake  # or brew install cmake
```

### Issue: GoogleTest build fails
**Solution**: Check internet connection; FetchContent downloads googletest
```bash
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..  # See details
```

### Issue: Tests fail with "SegFault"
**Solution**: Enable debug symbols and AddressSanitizer
```bash
cmake -DCMAKE_CXX_FLAGS="-g -fsanitize=address -std=c++23" ..
cmake --build .
./build/tests/test_runner  # Run with ASAN
```

---

## Verification Checklist

Before submitting GSoC/LFX proposal:

- [ ] Code compiles with `-std=c++23 -Wall -Wextra -Werror`
- [ ] All GoogleTests pass (100% coverage)
- [ ] Zero clang-tidy warnings
- [ ] No memory leaks (valgrind clean)
- [ ] RAII patterns followed everywhere
- [ ] Move semantics implemented correctly
- [ ] Template edge cases handled
- [ ] Documentation complete

---

**Last Updated**: 2026-09-12  
**Next Review**: 2026-09-26
