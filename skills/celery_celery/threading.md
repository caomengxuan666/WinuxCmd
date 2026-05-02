# Threading

**Celery Workers and Thread Safety**

```python
from celery import Celery
from threading import Lock
import threading

app = Celery('tasks', broker='redis://localhost:6379/0')

# Thread-safe counter using locks
class ThreadSafeCounter:
    def __init__(self):
        self._lock = Lock()
        self._count = 0
    
    def increment(self):
        with self._lock:
            self._count += 1
            return self._count
    
    def get_count(self):
        with self._lock:
            return self._count

counter = ThreadSafeCounter()

@app.task
def safe_increment():
    return counter.increment()

# Thread-unsafe: Don't share mutable state without locks
unsafe_counter = 0

@app.task
def unsafe_increment():
    global unsafe_counter
    unsafe_counter += 1  # Race condition!
    return unsafe_counter
```

**Concurrent Access to Shared Resources**

```python
from celery import Celery
import threading
import redis

app = Celery('tasks', broker='redis://localhost:6379/0')

# Thread-safe: Use Redis atomic operations
redis_client = redis.Redis(host='localhost', port=6379, db=1)

@app.task
def atomic_increment():
    return redis_client.incr('global_counter')

# Thread-safe: Use database transactions
import sqlite3
from contextlib import contextmanager

@contextmanager
def get_db():
    conn = sqlite3.connect('tasks.db', check_same_thread=False)
    conn.execute('BEGIN IMMEDIATE')
    try:
        yield conn
        conn.commit()
    except:
        conn.rollback()
        raise
    finally:
        conn.close()

@app.task
def safe_db_operation(data):
    with get_db() as conn:
        conn.execute('INSERT INTO tasks (data) VALUES (?)', (data,))
        return conn.execute('SELECT last_insert_rowid()').fetchone()[0]
```

**Prefork Worker Pool and Thread Safety**

```python
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')

# Prefork pool creates separate processes
# Each process has its own memory space
# Thread safety is per-process

# Safe: Each process has its own instance
class ProcessLocalCache:
    def __init__(self):
        self.cache = {}
    
    def get(self, key):
        return self.cache.get(key)
    
    def set(self, key, value):
        self.cache[key] = value

# Create per-process cache
import os

@app.task
def process_cached_task(key, value):
    # Each process has its own cache
    cache = ProcessLocalCache()
    cache.set(key, value)
    return f"Process {os.getpid()}: Cached {key}={value}"

# Unsafe: Sharing file handles across processes
file_handle = open('shared.log', 'w')  # Not safe for concurrent writes

@app.task
def unsafe_write_task(message):
    file_handle.write(f"{message}\n")  # Race condition!
    file_handle.flush()
```

**Eventlet/Gevent Pool and Thread Safety**

```python
from celery import Celery
import eventlet

app = Celery('tasks', broker='redis://localhost:6379/0')

# Eventlet pool uses green threads (cooperative multitasking)
# Green threads share the same memory space

# Safe: Eventlet-aware operations
@app.task
def eventlet_safe_task(url):
    # eventlet monkey-patches standard library
    import urllib.request
    response = urllib.request.urlopen(url)
    return response.read()

# Unsafe: Blocking operations in eventlet
@app.task
def eventlet_unsafe_task():
    import time
    time.sleep(10)  # Blocks all green threads!
    return 'Done'

# Safe: Use eventlet sleep
@app.task
def eventlet_safe_sleep():
    eventlet.sleep(10)  # Yields control to other green threads
    return 'Done'
```

**Thread-Safe Result Handling**

```python
from celery import Celery
from celery.result import AsyncResult
import threading

app = Celery('tasks', broker='redis://localhost:6379/0')
app.conf.result_backend = 'redis://localhost:6379/0'

@app.task
def compute(data):
    return data * 2

# Thread-safe: Each thread gets its own AsyncResult
def worker_thread(task_id):
    result = AsyncResult(task_id, app=app)
    return result.get(timeout=10)

# Usage
task = compute.delay(42)
threads = []
for _ in range(5):
    t = threading.Thread(target=worker_thread, args=(task.id,))
    threads.append(t)
    t.start()

for t in threads:
    t.join()

# Thread-safe: Use result backend for shared state
@app.task
def shared_state_task(data):
    # Store intermediate results in Redis
    redis_client = redis.Redis(host='localhost', port=6379, db=1)
    redis_client.hset(f'task:{data}', 'status', 'processing')
    # Process...
    redis_client.hset(f'task:{data}', 'status', 'completed')
    return data
```