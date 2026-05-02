# Safety

**Red Line 1: NEVER use pickle serializer with untrusted data**

BAD:
```python
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')
# Default pickle serializer allows arbitrary code execution
@app.task
def process_message(msg):
    return msg
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
def process_message(msg):
    return msg
```

**Red Line 2: NEVER run Celery as root**

BAD:
```bash
# Running as root
sudo celery -A tasks worker --loglevel=info
```

GOOD:
```bash
# Create dedicated user
sudo useradd -r -s /bin/false celery_user
sudo -u celery_user celery -A tasks worker --loglevel=info
```

**Red Line 3: NEVER expose Celery broker to public networks**

BAD:
```python
# Broker accessible from internet
app = Celery('tasks', broker='amqp://guest:guest@public-ip:5672//')
```

GOOD:
```python
# Broker bound to localhost
app = Celery('tasks', broker='amqp://guest:guest@localhost:5672//')
# Or use authentication
app.conf.broker_url = 'amqp://user:password@localhost:5672//'
```

**Red Line 4: NEVER ignore task time limits for external resources**

BAD:
```python
@app.task
def download_file(url):
    import requests
    # No timeout - could hang forever
    response = requests.get(url)
    return response.content
```

GOOD:
```python
@app.task(time_limit=60, soft_time_limit=50)
def download_file(url):
    import requests
    response = requests.get(url, timeout=30)
    return response.content
```

**Red Line 5: NEVER store sensitive data in task results without encryption**

BAD:
```python
@app.task
def process_credit_card(card_number):
    # Stores raw credit card number in result backend
    return {'card': card_number, 'status': 'processed'}
```

GOOD:
```python
import hashlib
from celery import Celery

app = Celery('tasks', broker='redis://localhost:6379/0')

@app.task
def process_credit_card(card_number):
    # Hash sensitive data before storing
    hashed_card = hashlib.sha256(card_number.encode()).hexdigest()
    return {'card_hash': hashed_card, 'status': 'processed'}
```