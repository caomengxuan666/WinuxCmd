# Threading

```markdown
# Thread Safety

## Thread-Local Context

Flask uses thread-local proxies for request, session, and `g` objects. This makes them safe to use in multi-threaded WSGI servers.

```python
from flask import Flask, request, g
import threading

app = Flask(__name__)

@app.route("/")
def index():
    # request is thread-local - safe to use concurrently
    user_agent = request.headers.get("User-Agent")
    
    # g is also thread-local
    g.user = get_current_user()
    
    return f"Hello, {g.user.name}!"

# Each thread gets its own request and g objects
# Thread 1: request = GET /, g.user = Alice
# Thread 2: request = GET /, g.user = Bob
```

## Global State and Race Conditions

Shared global state is NOT thread-safe and requires synchronization.

```python
from flask import Flask
import threading

app = Flask(__name__)

# BAD: Shared mutable state without synchronization
counter = 0

@app.route("/increment")
def increment():
    global counter
    counter += 1  # Race condition! Not atomic
    return str(counter)

# GOOD: Use thread-safe mechanisms
from threading import Lock

counter = 0
counter_lock = Lock()

@app.route("/increment-safe")
def increment_safe():
    global counter
    with counter_lock:
        counter += 1
    return str(counter)

# BETTER: Use database for persistent counters
from flask_sqlalchemy import SQLAlchemy

db = SQLAlchemy(app)

class Counter(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    value = db.Column(db.Integer, default=0)

@app.route("/increment-db")
def increment_db():
    counter = Counter.query.with_for_update().first()
    counter.value += 1
    db.session.commit()
    return str(counter.value)
```

## Application Factory and Thread Safety

The application factory pattern is thread-safe because each call creates a new instance.

```python
from flask import Flask

def create_app():
    app = Flask(__name__)
    # Each call creates a completely new app instance
    return app

# Thread-safe: multiple threads can call create_app() simultaneously
def worker():
    app = create_app()
    with app.app_context():
        # Work with this app instance
        pass

threads = [threading.Thread(target=worker) for _ in range(10)]
for t in threads:
    t.start()
for t in threads:
    t.join()
```

## Extension Thread Safety

Extensions must be designed to be thread-safe. Most Flask extensions handle this correctly.

```python
from flask import Flask
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)
db = SQLAlchemy(app)

# SQLAlchemy sessions are not thread-safe by default
# BAD: Sharing session across threads
def bad_worker():
    # This session object is shared - NOT thread-safe
    user = User.query.first()  # May cause issues
    db.session.add(user)

# GOOD: Create scoped sessions per thread
from sqlalchemy.orm import scoped_session, sessionmaker

Session = scoped_session(sessionmaker())
db_session = Session()

def good_worker():
    # Each thread gets its own session
    session = Session()
    user = session.query(User).first()
    session.close()
```

## Thread Pool Configuration

Configure your WSGI server's thread pool appropriately.

```python
# Gunicorn configuration (gunicorn.conf.py)
import multiprocessing

# Recommended settings for Flask
workers = multiprocessing.cpu_count() * 2 + 1
threads = 4  # Each worker handles 4 threads
worker_class = "gthread"  # Use threaded worker
timeout = 30
keepalive = 5

# Run with:
# gunicorn -c gunicorn.conf.py app:app
```

## Async Operations in Threaded Environment

Use thread pools for blocking I/O operations.

```python
from flask import Flask, jsonify
from concurrent.futures import ThreadPoolExecutor
import time

app = Flask(__name__)
executor = ThreadPoolExecutor(max_workers=4)

def blocking_io_operation(data):
    time.sleep(2)  # Simulate I/O
    return process(data)

@app.route("/process")
def process_data():
    data = request.args.get("data")
    
    # Submit to thread pool to avoid blocking the request thread
    future = executor.submit(blocking_io_operation, data)
    
    # Wait for result (still blocks this request, but frees up worker)
    result = future.result()
    return jsonify({"result": result})
```

## Thread Safety Checklist

```python
from flask import Flask, g, request
import threading

app = Flask(__name__)

# Thread-safe patterns checklist:

# 1. Use thread-local proxies (Flask handles this)
@app.route("/safe")
def safe():
    user_agent = request.headers.get("User-Agent")  # Thread-safe
    g.data = "per-request data"  # Thread-safe
    return "OK"

# 2. Protect shared resources with locks
shared_cache = {}
cache_lock = threading.Lock()

def get_cached(key):
    with cache_lock:
        return shared_cache.get(key)

def set_cached(key, value):
    with cache_lock:
        shared_cache[key] = value

# 3. Use thread-safe data structures
from queue import Queue
task_queue = Queue()  # Thread-safe by design

# 4. Avoid modifying module-level state
MODULE_CONFIG = {"setting": "value"}  # OK if read-only
# BAD: MODULE_CONFIG["setting"] = "new_value"  # Not thread-safe
```
```