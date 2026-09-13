# RAII Patterns: Resource Acquisition Is Initialization

> **Critical Concept**: RAII is the cornerstone of modern C++ resource management.
> Master it for GSoC/LFX interviews and production code.

---

## Core RAII Principle

```cpp
// RAII: Constructor acquires, destructor releases
class ManagedResource {
public:
    ManagedResource() {
        // Acquire resource (file, memory, lock, etc.)
    }
    
    ~ManagedResource() {
        // Release resource (guaranteed by language)
    }
};

// Automatic resource cleanup
{
    ManagedResource res;  // Constructor: acquire
    // Use resource
}  // Destructor: release (even if exception thrown)
```

## Common RAII Examples

### File Handle RAII

```cpp
class FileHandle {
    FILE* handle;
    
public:
    FileHandle(const char* filename) {
        handle = fopen(filename, "r");
        if (!handle) throw std::runtime_error("Failed to open file");
    }
    
    ~FileHandle() {
        if (handle) fclose(handle);
    }
    
    FILE* get() { return handle; }
    
    // Prevent copying
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    
    // Allow moving
    FileHandle(FileHandle&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }
};

// Usage
{
    FileHandle file("data.txt");
    // Read file
}  // File automatically closed
```

### Memory RAII (Smart Pointers)

```cpp
// unique_ptr: exclusive ownership
std::unique_ptr<int> ptr = std::make_unique<int>(42);
// Automatically deleted when ptr goes out of scope

// shared_ptr: shared ownership
std::shared_ptr<int> sptr1 = std::make_shared<int>(42);
std::shared_ptr<int> sptr2 = sptr1;  // Shared ownership
// Deleted when last shared_ptr is destroyed
```

### Lock RAII

```cpp
std::mutex mtx;

{
    std::lock_guard<std::mutex> lock(mtx);  // Acquire lock
    // Critical section
}  // Lock released automatically
```

---

## Best Practices

✅ DO:
- Use RAII for all resource management
- Implement move constructors/assignments
- Mark move operations `noexcept`
- Use smart pointers instead of raw pointers
- Acquire resources in constructor, release in destructor

❌ DON'T:
- Manage resources manually with new/delete
- Forget to null out moved-from pointers
- Use raw pointers for ownership
- Throw in destructors
- Ignore move semantics

---

**Last Updated**: 2026-09-13
