# STL Algorithms, Dynamic Programming & Advanced Optimization

> **Purpose**: Master STL algorithms, dynamic programming, graph algorithms, and advanced sorting for competitive programming and system design.  
> **Target**: GSoC/LFX candidates tackling algorithmic problems at LLVM, CERN, competitive coding platforms.

---

## Table of Contents

1. [STL Algorithms Core](#stl-algorithms-core)
2. [Dynamic Programming Patterns](#dynamic-programming-patterns)
3. [Graph Algorithms](#graph-algorithms)
4. [Pathfinding: A* and Dijkstra](#pathfinding-a-and-dijkstra)
5. [Advanced Sorting: QuickSelect & Variants](#advanced-sorting-quickselect--variants)
6. [Competitive Programming Techniques](#competitive-programming-techniques)
7. [Algorithm Templates](#algorithm-templates)
8. [Performance Optimization](#performance-optimization)

---

## STL Algorithms Core

### Essential Search Algorithms

```cpp
#include <algorithm>
#include <vector>
#include <numeric>

// ============================================================================
// LINEAR SEARCH - O(n)
// ============================================================================

std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

// Find first occurrence
auto it = std::find(v.begin(), v.end(), 4);
if (it != v.end()) {
    int index = std::distance(v.begin(), it);  // 2
}

// Find first matching predicate
auto it_even = std::find_if(v.begin(), v.end(),
                            [](int x) { return x % 2 == 0; });

// Find first NOT matching predicate
auto it_odd = std::find_if_not(v.begin(), v.end(),
                               [](int x) { return x % 2 == 1; });

// ============================================================================
// BINARY SEARCH - O(log n) [REQUIRES SORTED RANGE]
// ============================================================================

std::vector<int> sorted_v = {1, 2, 3, 4, 5, 6, 7, 8, 9};

// Check existence
bool exists = std::binary_search(sorted_v.begin(), sorted_v.end(), 5);

// Find first position >= target (lower bound)
auto lower = std::lower_bound(sorted_v.begin(), sorted_v.end(), 5);
// Points to first 5

// Find first position > target (upper bound)
auto upper = std::upper_bound(sorted_v.begin(), sorted_v.end(), 5);
// Points to first element after 5

// Get both bounds
auto [lo, hi] = std::equal_range(sorted_v.begin(), sorted_v.end(), 5);
int count = std::distance(lo, hi);  // Number of elements == 5

// ============================================================================
// COMPARISON ALGORITHMS
// ============================================================================

std::vector<int> data = {2, 4, 6, 8, 10};

// All elements satisfy condition
bool all_even = std::all_of(data.begin(), data.end(),
                            [](int x) { return x % 2 == 0; });  // true

// Any element satisfies condition
bool any_even = std::any_of(data.begin(), data.end(),
                            [](int x) { return x % 2 == 0; });  // true

// No elements satisfy condition
bool none_even = std::none_of(data.begin(), data.end(),
                              [](int x) { return x % 2 == 1; });  // true

// Count occurrences
int freq_5 = std::count(v.begin(), v.end(), 5);

// Count with predicate
int even_count = std::count_if(v.begin(), v.end(),
                               [](int x) { return x % 2 == 0; });

// Find first mismatch between two ranges
std::vector<int> v1 = {1, 2, 3, 4, 5};
std::vector<int> v2 = {1, 2, 3, 5, 6};
auto [it1, it2] = std::mismatch(v1.begin(), v1.end(), v2.begin());
// Points to 4 and 5 respectively
```

### Sorting Algorithms Comparison

```cpp
#include <algorithm>

// ============================================================================
// std::sort - General purpose, O(n log n) average, O(n²) worst
// ============================================================================

std::vector<int> v = {5, 2, 8, 1, 9};
std::sort(v.begin(), v.end());                    // Ascending: {1, 2, 5, 8, 9}
std::sort(v.begin(), v.end(), std::greater<>());  // Descending

// Custom comparator
std::sort(v.begin(), v.end(), [](int a, int b) {
    return std::abs(a) < std::abs(b);  // Sort by absolute value
});

// ============================================================================
// std::stable_sort - Preserves relative order of equal elements
// ============================================================================

struct Person {
    std::string name;
    int age;
};

std::vector<Person> people = {
    {"Alice", 30},
    {"Bob", 25},
    {"Charlie", 30}
};

// Stable sort preserves Alice before Charlie
std::stable_sort(people.begin(), people.end(),
                [](const Person& a, const Person& b) {
                    return a.age < b.age;
                });

// ============================================================================
// std::partial_sort - Sort only first N elements, O(n log k)
// ============================================================================

std::vector<int> v = {5, 2, 8, 1, 9, 3, 7};
std::partial_sort(v.begin(), v.begin() + 3, v.end());
// v = {1, 2, 3, 8, 9, 5, 7} - first 3 elements sorted

// Use case: Find top-k elements
auto top_3 = v;
std::partial_sort(top_3.begin(), top_3.begin() + 3, top_3.end(),
                 std::greater<>());
// {9, 8, 7, ...}

// ============================================================================
// std::nth_element - Partition so nth element is in sorted position, O(n)
// ============================================================================

std::vector<int> v = {5, 2, 8, 1, 9, 3};
std::nth_element(v.begin(), v.begin() + 2, v.end());
// v[2] now contains the 3rd smallest element (would be 3 if sorted)
// All smaller elements are before index 2
// All larger elements are after index 2

// Use case: Find median
std::vector<int> nums = {7, 2, 5, 1, 8};
auto mid = nums.begin() + nums.size() / 2;
std::nth_element(nums.begin(), mid, nums.end());
double median = *mid;  // For even-sized, also check *(mid-1)

// ============================================================================
// Checking Sort Status
// ============================================================================

std::vector<int> sorted_v = {1, 2, 3, 4, 5};
std::vector<int> unsorted_v = {3, 1, 4};

bool is_sorted = std::is_sorted(sorted_v.begin(), sorted_v.end());      // true
bool un_sorted = std::is_sorted(unsorted_v.begin(), unsorted_v.end()); // false

// Find first position where sorted order is violated
auto unsorted_pos = std::is_sorted_until(unsorted_v.begin(), 
                                         unsorted_v.end());
// Points to 4 (first out of order)
```

---

## Dynamic Programming Patterns

### Pattern 1: Linear DP (1D)

```cpp
// Problem: Climbing stairs
// You can climb 1 or 2 steps at a time
// How many ways to reach step n?

int climb_stairs(int n) {
    if (n <= 1) return n;
    
    std::vector<int> dp(n + 1);
    dp[0] = 1;  // 1 way to stay at 0
    dp[1] = 1;  // 1 way to reach 1
    
    for (int i = 2; i <= n; ++i) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }
    return dp[n];
}

// Space optimization: only need last 2 values
int climb_stairs_optimized(int n) {
    if (n <= 1) return n;
    
    int prev2 = 1, prev1 = 1;
    for (int i = 2; i <= n; ++i) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}
```

### Pattern 2: 2D DP (Coin Change, Knapsack)

```cpp
// Problem: Coin Change - minimum coins to make amount
// Coins: [1, 2, 5], Amount: 5

int coin_change(std::vector<int>& coins, int amount) {
    std::vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;  // 0 coins needed to make amount 0
    
    for (int i = 1; i <= amount; ++i) {
        for (int coin : coins) {
            if (coin <= i && dp[i - coin] != INT_MAX) {
                dp[i] = std::min(dp[i], dp[i - coin] + 1);
            }
        }
    }
    
    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

// Problem: 0/1 Knapsack - maximize value with weight limit
std::vector<int> knapsack_01(std::vector<int>& weights,
                              std::vector<int>& values,
                              int capacity) {
    int n = weights.size();
    std::vector<std::vector<int>> dp(n + 1, 
                                     std::vector<int>(capacity + 1, 0));
    
    for (int i = 1; i <= n; ++i) {
        for (int w = 1; w <= capacity; ++w) {
            if (weights[i - 1] <= w) {
                dp[i][w] = std::max(
                    dp[i - 1][w],  // Don't take item
                    values[i - 1] + dp[i - 1][w - weights[i - 1]]  // Take item
                );
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }
    
    return {dp[n][capacity]};
}
```

### Pattern 3: String DP (Edit Distance, LCS)

```cpp
// Problem: Edit Distance (Levenshtein Distance)
// Minimum operations (insert, delete, replace) to transform s1 to s2

int edit_distance(const std::string& s1, const std::string& s2) {
    int m = s1.length(), n = s2.length();
    std::vector<std::vector<int>> dp(m + 1, 
                                     std::vector<int>(n + 1));
    
    // Base cases
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;
    
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];  // No operation
            } else {
                dp[i][j] = 1 + std::min({
                    dp[i - 1][j],      // Delete
                    dp[i][j - 1],      // Insert
                    dp[i - 1][j - 1]   // Replace
                });
            }
        }
    }
    
    return dp[m][n];
}

// Problem: Longest Common Subsequence (LCS)
int lcs(const std::string& s1, const std::string& s2) {
    int m = s1.length(), n = s2.length();
    std::vector<std::vector<int>> dp(m + 1, 
                                     std::vector<int>(n + 1, 0));
    
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    
    return dp[m][n];
}
```

### Pattern 4: Interval DP (Matrix Chain, Burst Balloons)

```cpp
// Problem: Matrix Chain Multiplication
// Minimize scalar multiplications for A₁ × A₂ × ... × Aₙ
// p = [10, 20, 30, 40, 30] means matrices: 10×20, 20×30, 30×40, 40×30

long long matrix_chain_order(const std::vector<int>& p) {
    int n = p.size() - 1;
    std::vector<std::vector<long long>> dp(n, 
                                           std::vector<long long>(n, 0));
    
    // l is chain length
    for (int l = 2; l <= n; ++l) {
        for (int i = 0; i < n - l + 1; ++i) {
            int j = i + l - 1;
            dp[i][j] = LLONG_MAX;
            
            // Try all split points
            for (int k = i; k < j; ++k) {
                long long cost = dp[i][k] + dp[k + 1][j] + 
                                 (long long)p[i] * p[k + 1] * p[j + 1];
                dp[i][j] = std::min(dp[i][j], cost);
            }
        }
    }
    
    return dp[0][n - 1];
}
```

---

## Graph Algorithms

### Graph Representations

```cpp
#include <vector>
#include <queue>
#include <unordered_map>

// ============================================================================
// Adjacency List (Space: O(V + E), recommended)
// ============================================================================

class GraphList {
public:
    std::unordered_map<int, std::vector<int>> adj;
    
    void add_edge(int u, int v, bool directed = false) {
        adj[u].push_back(v);
        if (!directed) adj[v].push_back(u);
    }
    
    void print() {
        for (const auto& [node, neighbors] : adj) {
            std::cout << node << ": ";
            for (int neighbor : neighbors) {
                std::cout << neighbor << " ";
            }
            std::cout << "\n";
        }
    }
};

// ============================================================================
// BFS (Breadth-First Search) - O(V + E)
// ============================================================================

std::vector<int> bfs(const std::unordered_map<int, std::vector<int>>& adj,
                     int start) {
    std::unordered_map<int, bool> visited;
    std::vector<int> result;
    std::queue<int> q;
    
    q.push(start);
    visited[start] = true;
    
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        result.push_back(node);
        
        for (int neighbor : adj.at(node)) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
    
    return result;
}

// ============================================================================
// DFS (Depth-First Search) - O(V + E)
// ============================================================================

void dfs_recursive(int node,
                   const std::unordered_map<int, std::vector<int>>& adj,
                   std::unordered_map<int, bool>& visited,
                   std::vector<int>& result) {
    visited[node] = true;
    result.push_back(node);
    
    for (int neighbor : adj.at(node)) {
        if (!visited[neighbor]) {
            dfs_recursive(neighbor, adj, visited, result);
        }
    }
}

std::vector<int> dfs(const std::unordered_map<int, std::vector<int>>& adj,
                     int start) {
    std::unordered_map<int, bool> visited;
    std::vector<int> result;
    dfs_recursive(start, adj, visited, result);
    return result;
}

// ============================================================================
// Topological Sort (DAG) - O(V + E)
// ============================================================================

std::vector<int> topological_sort(const std::unordered_map<int, std::vector<int>>& adj,
                                   int num_vertices) {
    std::vector<int> in_degree(num_vertices, 0);
    std::unordered_map<int, std::vector<int>> graph = adj;
    
    // Count in-degrees
    for (const auto& [node, neighbors] : graph) {
        for (int neighbor : neighbors) {
            in_degree[neighbor]++;
        }
    }
    
    // Kahn's algorithm
    std::queue<int> q;
    for (int i = 0; i < num_vertices; ++i) {
        if (in_degree[i] == 0) q.push(i);
    }
    
    std::vector<int> result;
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        result.push_back(node);
        
        if (graph.count(node)) {
            for (int neighbor : graph[node]) {
                in_degree[neighbor]--;
                if (in_degree[neighbor] == 0) {
                    q.push(neighbor);
                }
            }
        }
    }
    
    return result;
}

// ============================================================================
// Strongly Connected Components (Kosaraju) - O(V + E)
// ============================================================================

class SCC {
private:
    std::unordered_map<int, std::vector<int>> graph;
    std::unordered_map<int, std::vector<int>> reverse_graph;
    
    void dfs1(int node, std::unordered_set<int>& visited, 
              std::stack<int>& finish_order) {
        visited.insert(node);
        for (int neighbor : graph[node]) {
            if (!visited.count(neighbor)) {
                dfs1(neighbor, visited, finish_order);
            }
        }
        finish_order.push(node);
    }
    
    void dfs2(int node, std::unordered_set<int>& visited,
              std::vector<int>& component) {
        visited.insert(node);
        component.push_back(node);
        for (int neighbor : reverse_graph[node]) {
            if (!visited.count(neighbor)) {
                dfs2(neighbor, visited, component);
            }
        }
    }
    
public:
    void add_edge(int u, int v) {
        graph[u].push_back(v);
        reverse_graph[v].push_back(u);
    }
    
    std::vector<std::vector<int>> find_scc() {
        std::unordered_set<int> visited;
        std::stack<int> finish_order;
        
        // First DFS: fill finish order
        for (const auto& [node, _] : graph) {
            if (!visited.count(node)) {
                dfs1(node, visited, finish_order);
            }
        }
        
        // Second DFS on reverse graph
        visited.clear();
        std::vector<std::vector<int>> components;
        
        while (!finish_order.empty()) {
            int node = finish_order.top();
            finish_order.pop();
            
            if (!visited.count(node)) {
                std::vector<int> component;
                dfs2(node, visited, component);
                components.push_back(component);
            }
        }
        
        return components;
    }
};
```

---

## Pathfinding: A* and Dijkstra

### Dijkstra's Algorithm (Shortest Path, Non-Negative Weights)

```cpp
#include <queue>
#include <limits>
#include <unordered_map>

struct Edge {
    int to;
    int weight;
};

std::unordered_map<int, int> dijkstra(
    const std::unordered_map<int, std::vector<Edge>>& graph,
    int start) {
    
    std::unordered_map<int, int> distances;
    std::priority_queue<std::pair<int, int>,
                       std::vector<std::pair<int, int>>,
                       std::greater<>> pq;  // Min-heap
    
    for (const auto& [node, _] : graph) {
        distances[node] = INT_MAX;
    }
    distances[start] = 0;
    pq.push({0, start});
    
    while (!pq.empty()) {
        auto [dist, node] = pq.top();
        pq.pop();
        
        if (dist > distances[node]) continue;  // Skip outdated entry
        
        for (const auto& edge : graph.at(node)) {
            int new_dist = dist + edge.weight;
            if (new_dist < distances[edge.to]) {
                distances[edge.to] = new_dist;
                pq.push({new_dist, edge.to});
            }
        }
    }
    
    return distances;
}
```

### A* Algorithm (Best-First Search with Heuristic)

```cpp
#include <cmath>

// 2D grid point
struct Point {
    int x, y;
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct PointHash {
    size_t operator()(const Point& p) const {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};

// Heuristic: Manhattan distance
int heuristic(const Point& current, const Point& goal) {
    return std::abs(current.x - goal.x) + std::abs(current.y - goal.y);
}

// A* for grid-based pathfinding
std::vector<Point> a_star(const std::vector<std::vector<int>>& grid,
                         Point start,
                         Point goal) {
    int rows = grid.size(), cols = grid[0].size();
    
    // Neighbors: up, down, left, right
    std::vector<Point> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    std::unordered_map<Point, int, PointHash> g_score;      // Cost from start
    std::unordered_map<Point, int, PointHash> f_score;      // g + h
    std::unordered_map<Point, Point, PointHash> came_from;
    
    auto cmp = [&](const Point& a, const Point& b) {
        return f_score[a] > f_score[b];  // Min-heap on f_score
    };
    std::priority_queue<Point, std::vector<Point>, decltype(cmp)> open_set(cmp);
    
    std::unordered_set<Point, PointHash> closed_set;
    
    g_score[start] = 0;
    f_score[start] = heuristic(start, goal);
    open_set.push(start);
    
    while (!open_set.empty()) {
        Point current = open_set.top();
        open_set.pop();
        
        if (current == goal) {
            // Reconstruct path
            std::vector<Point> path;
            while (came_from.count(current)) {
                path.push_back(current);
                current = came_from[current];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        closed_set.insert(current);
        
        for (const auto& dir : directions) {
            Point neighbor = {current.x + dir.x, current.y + dir.y};
            
            // Check bounds and obstacles
            if (neighbor.x < 0 || neighbor.x >= rows ||
                neighbor.y < 0 || neighbor.y >= cols ||
                grid[neighbor.x][neighbor.y] == 1 ||
                closed_set.count(neighbor)) {
                continue;
            }
            
            int tentative_g = g_score[current] + 1;  // Cost to neighbor
            
            if (!g_score.count(neighbor)) {
                g_score[neighbor] = INT_MAX;
            }
            
            if (tentative_g < g_score[neighbor]) {
                came_from[neighbor] = current;
                g_score[neighbor] = tentative_g;
                f_score[neighbor] = tentative_g + heuristic(neighbor, goal);
                open_set.push(neighbor);
            }
        }
    }
    
    return {};  // No path found
}
```

---

## Advanced Sorting: QuickSelect & Variants

### QuickSelect (Finding kth Smallest Element)

```cpp
// QuickSelect - O(n) average, O(n²) worst case
// Find kth smallest element without fully sorting

int partition(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low;
    
    for (int j = low; j < high; ++j) {
        if (arr[j] < pivot) {
            std::swap(arr[i], arr[j]);
            ++i;
        }
    }
    
    std::swap(arr[i], arr[high]);
    return i;
}

int quickselect(std::vector<int>& arr, int low, int high, int k) {
    if (low == high) return arr[low];
    
    int pivot_index = partition(arr, low, high);
    
    if (k == pivot_index) {
        return arr[k];
    } else if (k < pivot_index) {
        return quickselect(arr, low, pivot_index - 1, k);
    } else {
        return quickselect(arr, pivot_index + 1, high, k);
    }
}

// Find kth smallest
int kth_smallest(std::vector<int> arr, int k) {
    return quickselect(arr, 0, arr.size() - 1, k - 1);  // 1-indexed
}

// ============================================================================
// Median of Two Sorted Arrays - O(log(min(m, n)))
// ============================================================================

double find_median_sorted_arrays(const std::vector<int>& nums1,
                                 const std::vector<int>& nums2) {
    // Ensure nums1 is smaller
    if (nums1.size() > nums2.size()) {
        return find_median_sorted_arrays(nums2, nums1);
    }
    
    int m = nums1.size(), n = nums2.size();
    int low = 0, high = m;
    
    while (low <= high) {
        int partition1 = (low + high) / 2;
        int partition2 = (m + n + 1) / 2 - partition1;
        
        int left1 = (partition1 == 0) ? INT_MIN : nums1[partition1 - 1];
        int left2 = (partition2 == 0) ? INT_MIN : nums2[partition2 - 1];
        int right1 = (partition1 == m) ? INT_MAX : nums1[partition1];
        int right2 = (partition2 == n) ? INT_MAX : nums2[partition2];
        
        if (left1 <= right2 && left2 <= right1) {
            if ((m + n) % 2 == 0) {
                return (std::max(left1, left2) + std::min(right1, right2)) / 2.0;
            } else {
                return std::max(left1, left2);
            }
        } else if (left1 > right2) {
            high = partition1 - 1;
        } else {
            low = partition1 + 1;
        }
    }
    
    return -1.0;  // Should never reach
}

// ============================================================================
// IntroSort (std::sort implementation strategy)
// Hybrid: QuickSort + HeapSort + InsertionSort
// ============================================================================

template<typename T>
void introsort_impl(std::vector<T>& arr, int low, int high, int depth_limit) {
    if (high - low <= 16) {
        // Use insertion sort for small subarrays
        for (int i = low + 1; i <= high; ++i) {
            T key = arr[i];
            int j = i - 1;
            while (j >= low && arr[j] > key) {
                arr[j + 1] = arr[j];
                --j;
            }
            arr[j + 1] = key;
        }
    } else if (depth_limit == 0) {
        // Depth limit exceeded: use heap sort
        std::make_heap(arr.begin() + low, arr.begin() + high + 1);
        std::sort_heap(arr.begin() + low, arr.begin() + high + 1);
    } else {
        // Recursively quicksort with depth limit
        int pivot = partition(arr, low, high);
        introsort_impl(arr, low, pivot - 1, depth_limit - 1);
        introsort_impl(arr, pivot + 1, high, depth_limit - 1);
    }
}

template<typename T>
void introsort(std::vector<T>& arr) {
    int depth_limit = 2 * std::log(arr.size());
    introsort_impl(arr, 0, arr.size() - 1, depth_limit);
}
```

---

## Competitive Programming Techniques

### Two Pointers

```cpp
// Problem: Container With Most Water
// Find two lines that form a container with maximum area

int max_area(const std::vector<int>& heights) {
    int left = 0, right = heights.size() - 1;
    int max_area = 0;
    
    while (left < right) {
        int width = right - left;
        int height = std::min(heights[left], heights[right]);
        int area = width * height;
        max_area = std::max(max_area, area);
        
        // Move pointer pointing to smaller height
        if (heights[left] < heights[right]) {
            ++left;
        } else {
            --right;
        }
    }
    
    return max_area;
}

// Problem: 3Sum (Find all unique triplets that sum to 0)
std::vector<std::vector<int>> three_sum(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::sort(nums.begin(), nums.end());
    
    for (int i = 0; i < (int)nums.size() - 2; ++i) {
        if (i > 0 && nums[i] == nums[i - 1]) continue;  // Skip duplicates
        if (nums[i] > 0) break;  // Early exit
        
        int left = i + 1, right = nums.size() - 1;
        int target = -nums[i];
        
        while (left < right) {
            int sum = nums[left] + nums[right];
            if (sum == target) {
                result.push_back({nums[i], nums[left], nums[right]});
                
                // Skip duplicates
                while (left < right && nums[left] == nums[left + 1]) ++left;
                while (left < right && nums[right] == nums[right - 1]) --right;
                
                ++left;
                --right;
            } else if (sum < target) {
                ++left;
            } else {
                --right;
            }
        }
    }
    
    return result;
}
```

### Sliding Window

```cpp
// Problem: Maximum Subarray Sum of Size K
int max_subarray_sum_k(const std::vector<int>& arr, int k) {
    int window_sum = 0;
    for (int i = 0; i < k; ++i) {
        window_sum += arr[i];
    }
    
    int max_sum = window_sum;
    for (int i = k; i < (int)arr.size(); ++i) {
        window_sum = window_sum - arr[i - k] + arr[i];
        max_sum = std::max(max_sum, window_sum);
    }
    
    return max_sum;
}

// Problem: Longest Substring Without Repeating Characters
int length_of_longest_substring(const std::string& s) {
    std::unordered_map<char, int> last_seen;
    int max_length = 0;
    int left = 0;
    
    for (int right = 0; right < (int)s.length(); ++right) {
        if (last_seen.count(s[right]) && last_seen[s[right]] >= left) {
            left = last_seen[s[right]] + 1;  // Move left pointer
        }
        
        last_seen[s[right]] = right;
        max_length = std::max(max_length, right - left + 1);
    }
    
    return max_length;
}
```

---

## Algorithm Templates

### Binary Search Template

```cpp
// Template 1: Find exact element
int binary_search_exact(const std::vector<int>& arr, int target) {
    int left = 0, right = arr.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    
    return -1;  // Not found
}

// Template 2: Find first occurrence
int binary_search_first(const std::vector<int>& arr, int target) {
    int left = 0, right = arr.size() - 1;
    int result = -1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) {
            result = mid;
            right = mid - 1;  // Search left side
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return result;
}

// Template 3: Binary search on answer
bool can_allocate(const std::vector<int>& books, int m, long long time) {
    int students = 1;
    long long current_time = 0;
    
    for (int book_time : books) {
        if (current_time + book_time <= time) {
            current_time += book_time;
        } else {
            students++;
            current_time = book_time;
            if (students > m) return false;
        }
    }
    
    return true;
}

long long allocate_books(std::vector<int> books, int m) {
    long long left = *std::max_element(books.begin(), books.end());
    long long right = std::accumulate(books.begin(), books.end(), 0LL);
    long long answer = right;
    
    while (left <= right) {
        long long mid = left + (right - left) / 2;
        if (can_allocate(books, m, mid)) {
            answer = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    return answer;
}
```

---

## Performance Optimization

### Data Structure Selection Guide

| Problem | Best Data Structure | Complexity |
|---------|-------------------|-----------|
| Frequent min/max lookup | priority_queue/heap | O(log n) |
| Sorted unique elements | set | O(log n) insert/find |
| Key-value lookup | unordered_map | O(1) average |
| Ordered key-value | map | O(log n) |
| Fast push/pop both ends | deque | O(1) |
| Frequent insertion/deletion | list | O(1) with iterator |

### Optimization Checklist

✅ **DO**:
- Reserve capacity for vectors when size is known
- Use move semantics for large objects
- Prefer `const` references in function parameters
- Use range-based for loops
- Enable compiler optimizations (-O3)

❌ **DON'T**:
- Repeatedly reallocate vectors (reserve capacity)
- Copy large objects unnecessarily
- Use `std::endl` (use `\n` instead)
- Make unnecessary function calls in loops
- Ignore cache locality

---

**Last Updated**: 2026-09-13  
**Reference**: LeetCode, Codeforces, GeeksforGeeks, Competitive Programming 3  
**Next**: Daily Drill Programs & STL Practice Problems
