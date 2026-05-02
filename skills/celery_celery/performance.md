# Performance

**Optimize Worker Configuration**

```python
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')

# Performance

app.conf.update(
    # Reduce memory usage
    worker_max_tasks_per_child=1000,  # Restart worker after 1000 tasks
    worker_prefetch_multiplier=1,     # Process one task at a time
    
    # Optimize serialization
    task_serializer='msgpack',        # Faster than JSON
    result_serializer='msgpack',
    accept_content=['msgpack', 'json'],
    
    # Connection pooling
    broker_pool_limit=10,
    broker_connection_retry_on_startup=True,
    
    # Result backend optimization
    result_expires=3600,              # Auto-cleanup old results
    result_cache_max=1000,            # Limit result cache size
)

@app.task
def optimized_task(data):
    return data
```

**Batch Processing for High Throughput**

```python
from celery import Celery
from celery import group

app = Celery('tasks', broker='redis://localhost:6379/0')

# Inefficient: Individual tasks
@app.task
def process_item(item):
    return item * 2

# Efficient: Batch processing
@app.task
def process_batch(items):
    return [item * 2 for item in items]

# Usage comparison
def process_large_dataset(dataset):
    # Slow: Creates many tasks
    slow_results = [process_item.delay(item) for item in dataset]
    
    # Fast: Batch into chunks
    chunk_size = 100
    batches = [dataset[i:i+chunk_size] for i in range(0, len(dataset), chunk_size)]
    fast_results = group(process_batch.s(batch) for batch in batches).apply_async()
    
    return fast_results
```

**Use Connection Pooling and Reuse**

```python
from celery import Celery
import redis

app = Celery('tasks', broker='redis://localhost:6379/0')

# Bad: Create new connection per task
@app.task
def bad_connection_task():
    r = redis.Redis(host='localhost', port=6379)  # New connection each time
    return r.get('key')

# Good: Reuse connection pool
redis_pool = redis.ConnectionPool(host='localhost', port=6379, max_connections=10)

@app.task
def good_connection_task():
    r = redis.Redis(connection_pool=redis_pool)  # Reuse from pool
    return r.get('key')
```

**Optimize Task Serialization**

```python
from celery import Celery
import msgpack
import json

app = Celery('tasks', broker='redis://localhost:6379/0')

# Compare serialization performance
import time

@app.task
def json_task(data):
    return data

@app.task
def msgpack_task(data):
    return data

# Benchmark
large_data = {'key': 'value' * 1000}

# JSON serialization
start = time.time()
json_result = json_task.delay(large_data)
json_time = time.time() - start

# MessagePack serialization
app.conf.task_serializer = 'msgpack'
start = time.time()
msgpack_result = msgpack_task.delay(large_data)
msgpack_time = time.time() - start

print(f"JSON: {json_time:.4f}s, MessagePack: {msgpack_time:.4f}s")
```

**Minimize Result Backend Overhead**

```python
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')
app.conf.result_backend = 'redis://localhost:6379/0'

# Bad: Store large results
@app.task
def large_result_task():
    return {'data': 'x' * 1000000}  # 1MB result

# Good: Store minimal results
@app.task
def efficient_result_task():
    result = {'data': 'x' * 1000000}
    # Store only reference or summary
    return {'status': 'completed', 'size': len(result['data'])}

# Use task_always_eager=False for production
app.conf.task_always_eager = False  # Default, but explicit is better
```