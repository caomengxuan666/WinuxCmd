# Quickstart

```cpp
#include <oneapi/tbb.h>
#include <iostream>
#include <vector>

// Pattern 1: Parallel for loop
void parallel_for_example() {
    std::vector<int> data(1000, 1);
    oneapi::tbb::parallel_for(0, (int)data.size(), [&](int i) {
        data[i] *= 2;
    });
}

// Pattern 2: Parallel reduce
void parallel_reduce_example() {
    std::vector<int> data(1000, 1);
    int sum = oneapi::tbb::parallel_reduce(
        oneapi::tbb::blocked_range<int>(0, data.size()),
        0,
        [&](oneapi::tbb::blocked_range<int> r, int init) {
            for (int i = r.begin(); i < r.end(); ++i)
                init += data[i];
            return init;
        },
        std::plus<int>()
    );
    std::cout << "Sum: " << sum << std::endl;
}

// Pattern 3: Parallel sort
void parallel_sort_example() {
    std::vector<int> data = {5, 3, 1, 4, 2};
    oneapi::tbb::parallel_sort(data.begin(), data.end());
}

// Pattern 4: Task group
void task_group_example() {
    oneapi::tbb::task_group tg;
    tg.run([] { std::cout << "Task 1\n"; });
    tg.run([] { std::cout << "Task 2\n"; });
    tg.wait();
}

// Pattern 5: Concurrent queue
void concurrent_queue_example() {
    oneapi::tbb::concurrent_queue<int> queue;
    queue.push(1);
    queue.push(2);
    int value;
    while (queue.try_pop(value)) {
        std::cout << value << std::endl;
    }
}

// Pattern 6: Parallel pipeline
void parallel_pipeline_example() {
    std::vector<int> input = {1, 2, 3, 4, 5};
    std::vector<int> output;
    
    oneapi::tbb::parallel_pipeline(
        4,
        oneapi::tbb::make_filter<void, int>(
            oneapi::tbb::filter_mode::serial_in_order,
            [&](oneapi::tbb::flow_control& fc) -> int {
                static size_t idx = 0;
                if (idx < input.size())
                    return input[idx++];
                fc.stop();
                return 0;
            }
        ) &
        oneapi::tbb::make_filter<int, int>(
            oneapi::tbb::filter_mode::parallel,
            [](int x) { return x * 2; }
        ) &
        oneapi::tbb::make_filter<int, void>(
            oneapi::tbb::filter_mode::serial_in_order,
            [&](int x) { output.push_back(x); }
        )
    );
}

int main() {
    parallel_for_example();
    parallel_reduce_example();
    parallel_sort_example();
    task_group_example();
    concurrent_queue_example();
    parallel_pipeline_example();
    return 0;
}
```