# Overview

Taskflow is a header-only C++ library for building task-parallel programs with task dependency graphs. It provides a modern, expressive API for expressing complex parallel computations through directed acyclic graphs (DAGs) of tasks.

**When to use Taskflow:**
- You need to express complex task dependencies that go beyond simple fork-join patterns
- You want to build parallel pipelines or streaming data processing systems
- You need recursive parallelism (subflows) or conditional task execution
- You're working with GPU-accelerated computing via CUDA graphs
- You need composable parallel building blocks that can be reused across projects

**When NOT to use Taskflow:**
- Simple parallel loops (use std::for_each with execution policies instead)
- Basic async operations (std::async may be simpler)
- When you need real-time guarantees (Taskflow's scheduler is work-stealing, not real-time)
- Very simple fork-join patterns (OpenMP might be simpler)

**Key Design Concepts:**
- **Task Graph**: A DAG where nodes are callable tasks and edges represent dependencies
- **Executor**: Manages thread pool and schedules tasks for execution
- **Subflow**: Dynamic task graph creation during execution
- **Conditional Tasking**: Tasks that return integers to control execution flow
- **Composition**: Modular task graphs that can be nested within larger graphs
- **Asynchronous Tasking**: Tasks launched directly from the executor without a static graph