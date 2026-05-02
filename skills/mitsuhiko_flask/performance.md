# Performance

```markdown
# Performance Considerations

## Request Handling Overhead

Flask adds minimal overhead per request. The main performance considerations come from your application code and extensions.

```python
from flask import Flask, jsonify
import time

app = Flask(__name__)

# BAD: Doing expensive operations in every request
@app.route("/slow")
def slow_endpoint():
    time.sleep(2)  # Blocks the entire worker
    return jsonify({"result": "slow"})

# GOOD: Use async or background tasks for expensive operations
from celery import Celery

celery = Celery(app.name, broker=app.config["CELERY_BROKER_URL"])

@celery.task
def expensive_task(data):
    time.sleep(2)
    return process_data(data)

@app.route("/fast")
def fast_endpoint():
    task = expensive_task.delay(request.args.get("data"))
    return jsonify({"task_id": task.id}), 202
```

## Template Rendering Performance

Jinja2 template compilation is cached, but complex templates can still be slow.

```python
from flask import Flask, render_template

app = Flask(__name__)

# BAD: Complex logic in templates
# templates/slow.html:
# {% for item in items %}
#   {% if item.complex_calculation() > 100 %}
#     {{ item.expensive_method() }}
#   {% endif %}
# {% endfor %}

# GOOD: Pre-process data in the view
@app.route("/fast")
def fast_view():
    items = get_items()
    processed = [
        {
            "name": item.name,
            "value": item.complex_calculation(),
            "display": item.expensive_method()
        }
        for item in items
        if item.complex_calculation() > 100
    ]
    return render_template("fast.html", items=processed)
```

## Database Query Optimization

N+1 query problems are common in Flask applications.

```python
from flask import Flask
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)
db = SQLAlchemy(app)

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    posts = db.relationship("Post", backref="author")

class Post(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    user_id = db.Column(db.Integer, db.ForeignKey("user.id"))

# BAD: N+1 queries
@app.route("/users-slow")
def users_slow():
    users = User.query.all()
    result = []
    for user in users:  # Each iteration triggers a new query
        result.append({
            "name": user.name,
            "post_count": len(user.posts)  # Triggers query
        })
    return jsonify(result)

# GOOD: Eager loading
@app.route("/users-fast")
def users_fast():
    users = User.query.options(
        db.joinedload(User.posts)
    ).all()
    result = [
        {"name": user.name, "post_count": len(user.posts)}
        for user in users
    ]
    return jsonify(result)
```

## Caching Strategies

Implement caching for frequently accessed data.

```python
from flask import Flask
from flask_caching import Cache

app = Flask(__name__)
app.config["CACHE_TYPE"] = "redis"
cache = Cache(app)

# Cache expensive computations
@app.route("/expensive")
@cache.cached(timeout=300)  # Cache for 5 minutes
def expensive_view():
    data = perform_expensive_calculation()
    return jsonify(data)

# Cache with key based on arguments
@app.route("/user/<int:user_id>")
@cache.cached(timeout=60, key_prefix="user_")
def user_view(user_id):
    user = User.query.get(user_id)
    return jsonify(user.to_dict())
```

## Response Compression

Enable compression for large responses.

```python
from flask import Flask, jsonify
from flask_compress import Compress

app = Flask(__name__)
Compress(app)

@app.route("/large-data")
def large_data():
    # Response will be automatically compressed
    data = generate_large_dataset()
    return jsonify(data)
```

## Static File Serving

For production, serve static files through a web server like Nginx.

```python
from flask import Flask

app = Flask(__name__)

# Development: Flask serves static files
# Production: Configure Nginx to serve /static/ directly

# Nginx configuration example:
# location /static/ {
#     alias /path/to/app/static/;
#     expires 30d;
#     add_header Cache-Control "public, immutable";
# }
```

## Connection Pooling

Use connection pooling for database connections.

```python
from flask import Flask
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)
app.config["SQLALCHEMY_DATABASE_URI"] = "postgresql://user:pass@localhost/db"
app.config["SQLALCHEMY_ENGINE_OPTIONS"] = {
    "pool_size": 10,
    "pool_recycle": 3600,
    "pool_pre_ping": True,
}
db = SQLAlchemy(app)
```
```