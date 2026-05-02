# Lifecycle

**Task Construction and Registration**

```python
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')

# Task registration via decorator
@app.task(name='tasks.add_numbers')
def add_numbers(x, y):
    return x + y

# Task registration via bind method
def multiply(x, y):
    return x * y

app.task(name='tasks.multiply', bind=True)(multiply)

# Verify registration
print(app.tasks['tasks.add_numbers'].name)  # tasks.add_numbers
print(app.tasks['tasks.multiply'].name)     # tasks.multiply
```

**Task Execution Lifecycle**

```python
from celery import Celery
from celery.states import PENDING, STARTED, SUCCESS, FAILURE, RETRY

app = Celery('tasks', broker='redis://localhost:6379/0')
app.conf.result_backend = 'redis://localhost:6379/0'

@app.task(bind=True, track_started=True)
def lifecycle_task(self, data):
    # Task is in PENDING state
    print(f"Task ID: {self.request.id}")
    print(f"Task State: {self.AsyncResult(self.request.id).state}")
    
    try:
        # Task transitions to STARTED
        result = process_data(data)
        # Task transitions to SUCCESS
        return result
    except Exception:
        # Task transitions to RETRY
        raise self.retry(countdown=60)
    # If max_retries exceeded, transitions to FAILURE

def process_data(data):
    return {'processed': data}
```

**Resource Management and Cleanup**

```python
from celery import Celery
from contextlib import contextmanager

app = Celery('tasks', broker='redis://localhost:6379/0')

class DatabaseConnection:
    def __init__(self):
        self.connected = False
    
    def connect(self):
        self.connected = True
        print("Database connected")
    
    def close(self):
        self.connected = False
        print("Database disconnected")
    
    def query(self, sql):
        if not self.connected:
            raise RuntimeError("Not connected")
        return f"Result: {sql}"

# Global connection pool
db_pool = {}

@app.task(bind=True)
def database_task(self, query):
    # Acquire connection
    conn = DatabaseConnection()
    conn.connect()
    db_pool[self.request.id] = conn
    
    try:
        result = conn.query(query)
        return result
    finally:
        # Always cleanup
        conn.close()
        db_pool.pop(self.request.id, None)
```

**Worker Lifecycle Management**

```python
from celery import Celery
from celery.signals import (
    worker_init, worker_shutdown,
    task_prerun, task_postrun, task_failure
)

app = Celery('tasks', broker='redis://localhost:6379/0')

@worker_init.connect
def on_worker_init(sender, **kwargs):
    print(f"Worker {sender} started")
    # Initialize resources (database pools, etc.)

@worker_shutdown.connect
def on_worker_shutdown(sender, **kwargs):
    print(f"Worker {sender} shutting down")
    # Cleanup resources

@task_prerun.connect
def on_task_prerun(task_id, task, **kwargs):
    print(f"Task {task_id} starting")

@task_postrun.connect
def on_task_postrun(task_id, task, **kwargs):
    print(f"Task {task_id} completed")

@task_failure.connect
def on_task_failure(task_id, exception, **kwargs):
    print(f"Task {task_id} failed: {exception}")

@app.task
def managed_task(data):
    return f'Processing: {data}'
```