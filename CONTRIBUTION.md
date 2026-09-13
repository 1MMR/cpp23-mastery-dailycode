# Contribution Guidelines for GSoC/LFX Candidates

## How to Contribute

### 1. Fork & Clone

```bash
git clone https://github.com/1MMR/cpp23-mastery-dailycode.git
cd cpp23-mastery-dailycode
```

### 2. Create Feature Branch

```bash
git checkout -b feature/your-feature-name
```

### 3. Code Standards

All code must compile cleanly:

```bash
-std=c++23 -Wall -Wextra -Werror -pedantic
```

### 4. Add Tests

Every feature requires GoogleTest coverage:

```cpp
#include <gtest/gtest.h>

TEST(YourFeature, EdgeCase) {
    // Test implementation
}
```

### 5. Submit PR

- Reference related issue
- Include test results
- Add documentation

## Target Programs

### GSoC Organizations
- **LLVM**: Compiler optimizations, toolchain improvements
- **KDE**: Desktop applications, frameworks
- **Godot Engine**: Game engine features
- **CERN ROOT**: High-energy physics data analysis

### LFX Programs
- Linux Foundation Mentorship
- Cloud Native Computing Foundation (CNCF)
- Hyperledger Blockchain

## Resources

- [Microsoft Learn C++](https://learn.microsoft.com/en-us/cpp/?view=msvc-170)
- [cppreference.com](https://cppreference.com)
- [CppCon Talks](https://www.youtube.com/user/CppCon)

---

**Welcome to the C++23 Mastery community!**
