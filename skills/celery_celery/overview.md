# Overview

Celery is a distributed task queue library for Python that enables asynchronous execution of work across threads, processes, or machines. It uses a message broker (like RabbitMQ or Redis) to mediate between clients that produce tasks and workers that consume them. The library is designed for high availability, horizontal scaling, and flexible task management.

**When to use Celery:**
- Offloading long-running operations from web requests
- Scheduling periodic tasks (cron-like functionality)
- Processing background jobs in distributed systems
- Handling task retries and failure recovery
- Implementing work queues with priority routing

**When NOT to use Celery:**
- Simple in-process task execution (use threading or asyncio)
- Real-time systems requiring sub-millisecond latency
- Single-machine applications with minimal concurrency needs
- Systems where message broker overhead is unacceptable

**Key Design Principles:**
- **Broker-based architecture**: Tasks are sent as messages to a broker, which distributes them to workers
- **Task abstraction**: Functions decorated with `@app.task` become distributed units of work
- **Result backends**: Optional storage for task results and state
- **Flexible concurrency**: Supports prefork, eventlet, gevent, and single-threaded execution
- **Automatic retries**: Built-in retry mechanisms with exponential backoff
- **Task routing**: Custom queues and exchanges for priority and workload distribution