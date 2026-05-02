# Quickstart

```python
# Basic Celery Application
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')

@app.task
def add(x, y):
    return x + y

# Call the task
result = add.delay(4, 4)
print(result.get(timeout=10))  # Output: 8
```

```python
# Task with Configuration
from celery import Celery

app = Celery('tasks')
app.config_from_object({
    'broker_url': 'redis://localhost:6379/0',
    'result_backend': 'redis://localhost:6379/0',
    'task_serializer': 'json',
    'result_serializer': 'json',
    'accept_content': ['json'],
    'timezone': 'UTC',
    'enable_utc': True,
})

@app.task(bind=True, max_retries=3)
def process_data(self, data):
    try:
        # Process data
        return {'status': 'success', 'data': data}
    except Exception as exc:
        raise self.retry(exc=exc, countdown=60)
```

```python
# Task with Callbacks and Chains
from celery import Celery, chain, group

app = Celery('tasks', broker='redis://localhost:6379/0')

@app.task
def fetch_data(url):
    return {'url': url, 'content': 'sample'}

@app.task
def process_content(data):
    return {'processed': data['content'].upper()}

@app.task
def save_result(result):
    return {'saved': result['processed']}

# Chain tasks
result = chain(fetch_data.s('http://example.com'),
               process_content.s(),
               save_result.s()).apply_async()

# Group tasks
group_result = group(
    fetch_data.s('http://site1.com'),
    fetch_data.s('http://site2.com')
).apply_async()
```

```python
# Periodic Tasks with Celery Beat
from celery import Celery
from celery.schedules import crontab

app = Celery('tasks', broker='redis://localhost:6379/0')

@app.task
def cleanup_old_data():
    # Cleanup logic
    return 'Cleanup completed'

@app.on_after_configure.connect
def setup_periodic_tasks(sender, **kwargs):
    # Execute every hour
    sender.add_periodic_task(3600.0, cleanup_old_data.s(), name='hourly-cleanup')
    
    # Execute at specific times
    sender.add_periodic_task(
        crontab(hour=7, minute=30, day_of_week=1),
        cleanup_old_data.s(),
        name='weekly-cleanup'
    )
```

```python
# Task with Custom Routing
from celery import Celery
from kombu import Queue, Exchange

app = Celery('tasks', broker='redis://localhost:6379/0')

# Define custom queues
app.conf.task_queues = (
    Queue('high_priority', Exchange('high'), routing_key='high'),
    Queue('low_priority', Exchange('low'), routing_key='low'),
)

@app.task(queue='high_priority')
def urgent_task(data):
    return f'Processing urgent: {data}'

@app.task(queue='low_priority')
def background_task(data):
    return f'Processing background: {data}'

# Route tasks
urgent_task.delay({'id': 1, 'priority': 'high'})
background_task.delay({'id': 2, 'priority': 'low'})
```

```python
# Task with Progress Reporting
from celery import Celery
from celery.result import AsyncResult

app = Celery('tasks', broker='redis://localhost:6379/0')

@app.task(bind=True)
def long_running_task(self, items):
    total = len(items)
    for i, item in enumerate(items):
        # Update progress
        self.update_state(
            state='PROGRESS',
            meta={'current': i, 'total': total, 'status': 'Processing...'}
        )
        # Process item
        process_item(item)
    return {'status': 'Completed', 'total': total}

def process_item(item):
    # Simulate processing
    import time
    time.sleep(0.1)

# Check progress
result = long_running_task.delay([1, 2, 3, 4, 5])
while not result.ready():
    meta = result.info
    if meta and 'current' in meta:
        print(f"Progress: {meta['current']}/{meta['total']}")
    time.sleep(0.5)
```