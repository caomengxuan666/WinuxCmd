# Best Practices

**Use Configuration Classes for Production Settings**

```python
# config.py
class CeleryConfig:
    broker_url = 'redis://localhost:6379/0'
    result_backend = 'redis://localhost:6379/0'
    task_serializer = 'json'
    result_serializer = 'json'
    accept_content = ['json']
    timezone = 'UTC'
    enable_utc = True
    task_track_started = True
    task_time_limit = 30 * 60
    task_soft_time_limit = 25 * 60
    worker_max_tasks_per_child = 1000
    worker_prefetch_multiplier = 1

# tasks.py
from celery import Celery
from config import CeleryConfig

app = Celery('tasks')
app.config_from_object(CeleryConfig)
```

**Implement Structured Error Handling**

```python
from celery import Celery
from celery.utils.log import get_task_logger
import logging

logger = get_task_logger(__name__)

app = Celery('tasks', broker='redis://localhost:6379/0')

class TaskError(Exception):
    pass

@app.task(bind=True, max_retries=3, default_retry_delay=60)
def robust_task(self, data):
    try:
        if not isinstance(data, dict):
            raise TaskError("Invalid data format")
        result = process_data(data)
        logger.info(f"Task {self.request.id} completed successfully")
        return result
    except TaskError as exc:
        logger.error(f"Task {self.request.id} failed: {exc}")
        raise
    except Exception as exc:
        logger.warning(f"Task {self.request.id} retrying: {exc}")
        raise self.retry(exc=exc)

def process_data(data):
    return {'processed': data}
```

**Use Task Routing for Priority Management**

```python
from celery import Celery
from kombu import Queue, Exchange

app = Celery('tasks', broker='redis://localhost:6379/0')

# Define queues with priorities
app.conf.task_queues = (
    Queue('critical', Exchange('critical'), routing_key='critical'),
    Queue('default', Exchange('default'), routing_key='default'),
    Queue('batch', Exchange('batch'), routing_key='batch'),
)

# Route tasks
@app.task(queue='critical')
def critical_task(data):
    return f'Critical: {data}'

@app.task(queue='default')
def default_task(data):
    return f'Default: {data}'

@app.task(queue='batch')
def batch_task(data):
    return f'Batch: {data}'

# Start workers with specific queues
# celery -A tasks worker -Q critical -c 4
# celery -A tasks worker -Q default,batch -c 2
```

**Implement Health Checks and Monitoring**

```python
from celery import Celery
from celery.task.control import inspect

app = Celery('tasks', broker='redis://localhost:6379/0')

def check_worker_health():
    """Check if workers are alive and responsive"""
    i = inspect()
    active = i.active()
    scheduled = i.scheduled()
    reserved = i.reserved()
    
    if not active:
        return {'status': 'WARNING', 'message': 'No active workers'}
    
    total_tasks = sum(len(tasks) for tasks in active.values())
    return {
        'status': 'OK',
        'active_workers': len(active),
        'running_tasks': total_tasks,
        'scheduled': sum(len(t) for t in scheduled.values()),
        'reserved': sum(len(t) for t in reserved.values())
    }

@app.task
def health_check():
    return check_worker_health()
```

**Use Task Expiration for Time-Sensitive Operations**

```python
from celery import Celery
from datetime import datetime, timedelta

app = Celery('tasks', broker='redis://localhost:6379/0')

@app.task(expires=3600)  # Task expires after 1 hour
def time_sensitive_task(data):
    return f'Processing: {data}'

# Or set expiration dynamically
@app.task
def dynamic_expiry_task(data):
    # Task will expire in 30 minutes
    return f'Processing: {data}'

# Call with expiration
dynamic_expiry_task.apply_async(
    args=[{'id': 1}],
    expires=datetime.now() + timedelta(minutes=30)
)
```