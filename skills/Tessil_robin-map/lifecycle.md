# Lifecycle

**Construction and initialization**
```cpp
#include <tsl/robin_map.h>
#include <tsl/robin_set.h>

// Default construction
tsl::robin_map<int, std::string> map1;

// Construction with initializer list
tsl::robin_map<int, std::string> map2 = {{1, "one"}, {2, "two"}};

// Construction with custom allocator
tsl::robin_map<int, std::string, std::hash<int>, std::equal_to<int>,
               std::allocator<std::pair<const int, std::string>>> map3;

// Construction with reserved capacity
tsl::robin_map<int, std::string> map4(100);  // Reserve space for 100 elements
```

**Destruction and cleanup**
```cpp
// Automatic cleanup when map goes out of scope
{
    tsl::robin_map<int, std::string> map = {{1, "one"}, {2, "two"}};
    // Resources automatically freed
}

// Explicit clearing
tsl::robin_map<int, std::string> map = {{1, "one"}, {2, "two"}};
map.clear();  // Removes all elements, keeps capacity
map.shrink_to_fit();  // Reduces capacity to fit current size
```

**Move semantics**
```cpp
// Move construction
tsl::robin_map<int, std::string> source = {{1, "one"}, {2, "two"}};
tsl::robin_map<int, std::string> dest = std::move(source);  // source is now empty

// Move assignment
tsl::robin_map<int, std::string> map1 = {{1, "one"}};
tsl::robin_map<int, std::string> map2 = {{2, "two"}};
map2 = std::move(map1);  // map2 takes map1's data, map1 is empty

// Move-only types
tsl::robin_map<int, std::unique_ptr<int>> map;
map.insert({1, std::make_unique<int>(42)});
auto extracted = map.extract(map.begin());  // Move key-value pair out
```

**Resource management**
```cpp
// Custom allocator example
template<typename T>
struct LoggingAllocator : std::allocator<T> {
    T* allocate(std::size_t n) {
        std::cout << "Allocating " << n << " elements\n";
        return std::allocator<T>::allocate(n);
    }
    void deallocate(T* p, std::size_t n) {
        std::cout << "Deallocating " << n << " elements\n";
        std::allocator<T>::deallocate(p, n);
    }
};

tsl::robin_map<int, std::string, std::hash<int>, std::equal_to<int>,
               LoggingAllocator<std::pair<const int, std::string>>> map;
map.reserve(100);  // Triggers allocation logging
```

**Swap and merge**
```cpp
// Swapping maps
tsl::robin_map<int, std::string> map1 = {{1, "one"}, {2, "two"}};
tsl::robin_map<int, std::string> map2 = {{3, "three"}};
map1.swap(map2);  // map1 now has {3, "three"}, map2 has {1, "one"}, {2, "two"}

// Merging maps (C++17)
tsl::robin_map<int, std::string> source = {{1, "one"}, {2, "two"}};
tsl::robin_map<int, std::string> dest = {{3, "three"}};
dest.merge(source);  // dest now has all elements, source may have remaining
```