# Pitfalls

**Pitfall 1: Using Pickle Serializer in Production**

BAD:
```python
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')
# Default serializer is pickle - insecure!
@app.task
def process_data(data):
    return data
```

GOOD:
```python
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')
app.conf.update(
    task_serializer='json',
    accept_content=['json'],
    result_serializer='json',
)

@app.task
def process_data(data):
    return data
```

**Pitfall 2: Ignoring Task Timeouts**

BAD:
```python
@app.task
def infinite_task():
    while True:
        pass  # Never completes, blocks worker
```

GOOD:
```python
@app.task(time_limit=300, soft_time_limit=240)
def bounded_task():
    import time
    start = time.time()
    while time.time() - start < 100:
        # Do work
        pass
    return 'Completed'
```

**Pitfall 3: Not Handling Task Failures**

BAD:
```python
@app.task
def fragile_task(data):
    result = 1 / data  # Will crash if data is 0
    return result
```

GOOD:
```python
@app.task(bind=True, max_retries=3, default_retry_delay=60)
def resilient_task(self, data):
    try:
        if data == 0:
            raise ValueError("Cannot divide by zero")
        result = 1 / data
        return result
    except Exception as exc:
        raise self.retry(exc=exc)
```

**Pitfall 4: Blocking the Worker with Synchronous Calls**

BAD:
```python
@app.task
def blocking_task():
    import time
    time.sleep(30)  # Blocks entire worker process
    return 'Done'
```

GOOD:
```python
@app.task
def async_task():
    # Use non-blocking operations
    import asyncio
    async def work():
        await asyncio.sleep(30)
        return 'Done'
    return asyncio.run(work())
```

**Pitfall 5: Forgetting to Configure Result Backend**

BAD:
```python
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')
# No result backend configured

@app.task
def add(x, y):
    return x + y

result = add.delay(4, 4)
print(result.get())  # Will raise NotImplementedError
```

GOOD:
```python
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')
app.conf.result_backend = 'redis://localhost:6379/0'

@app.task
def add(x, y):
    return x + y

result = add.delay(4, 4)
print(result.get(timeout=10))  # Works correctly
```

**Pitfall 6: Using Global State in Tasks**

BAD:
```python
global_counter = 0

@app.task
def increment_counter():
    global global_counter
    global_counter += 1  # Not thread-safe across workers
    return global_counter
```

GOOD:
```python
from celery import Celery
import redis

app = Celery('tasks', broker='redis://localhost:6379/0')
cache = redis.Redis(host='localhost', port=6379, db=1)

@app.task
def increment_counter():
    return cache.incr('counter')  # Atomic operation
```

**Pitfall 7: Not Setting Task Time Limits for External Calls**

BAD:
```python
@app.task
def fetch_url(url):
    import requests
    response = requests.get(url)  # Could hang indefinitely
    return response.text
```

GOOD:
```python
@app.task(time_limit=30, soft_time_limit=25)
def fetch_url(url):
    import requests
    response = requests.get(url, timeout=20)
    return response.text
```