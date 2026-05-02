# Lifecycle

```markdown
# Lifecycle Management

## Application Construction

Flask applications are constructed by creating a `Flask` instance with the application's module name.

```python
from flask import Flask

# Basic construction
app = Flask(__name__)

# With custom configuration
app = Flask(__name__)
app.config["SECRET_KEY"] = "your-secret-key"
app.config["MAX_CONTENT_LENGTH"] = 16 * 1024 * 1024  # 16 MB max upload
```

## Application Factory Pattern

For production applications, use the factory pattern to create multiple instances with different configurations.

```python
from flask import Flask

def create_app(testing=False):
    app = Flask(__name__)
    
    if testing:
        app.config["TESTING"] = True
        app.config["DATABASE_URI"] = "sqlite:///:memory:"
    else:
        app.config.from_envvar("FLASK_CONFIG")
    
    # Initialize extensions
    from flask_sqlalchemy import SQLAlchemy
    db = SQLAlchemy()
    db.init_app(app)
    
    return app

# Create instances
dev_app = create_app()
test_app = create_app(testing=True)
```

## Resource Management with Application Context

Use the application context to manage resources that need cleanup.

```python
from flask import Flask, g
import sqlite3

app = Flask(__name__)

def get_db():
    if "db" not in g:
        g.db = sqlite3.connect("database.db")
    return g.db

@app.teardown_appcontext
def close_db(exception):
    db = g.pop("db", None)
    if db is not None:
        db.close()

@app.route("/")
def index():
    db = get_db()
    # Use database
    return "OK"
```

## Request Lifecycle

Each request goes through a well-defined lifecycle with hooks.

```python
from flask import Flask, request, g

app = Flask(__name__)

@app.before_request
def before_request():
    # Runs before each request
    g.start_time = __import__("time").time()
    g.user = get_current_user()  # Hypothetical function

@app.after_request
def after_request(response):
    # Runs after each request (even if errors occur)
    elapsed = __import__("time").time() - g.start_time
    response.headers["X-Response-Time"] = str(elapsed)
    return response

@app.teardown_request
def teardown_request(exception=None):
    # Runs at the end of request, even if unhandled exception
    cleanup_resources()
```

## Extension Lifecycle

Extensions follow a specific initialization pattern.

```python
# Custom extension
class MyExtension:
    def __init__(self, app=None):
        self.app = None
        if app is not None:
            self.init_app(app)
    
    def init_app(self, app):
        self.app = app
        # Register with app
        app.extensions["my_extension"] = self
        
        # Configure from app config
        self.setting = app.config.get("MY_EXTENSION_SETTING", "default")
        
        # Register teardown
        @app.teardown_appcontext
        def cleanup(exception):
            self.cleanup()

# Usage
ext = MyExtension()
ext.init_app(app)
```

## Blueprint Lifecycle

Blueprints are registered with the application and can have their own lifecycle hooks.

```python
from flask import Blueprint

bp = Blueprint("api", __name__, url_prefix="/api")

@bp.before_app_request
def before_api_request():
    # Runs before any request to the app, not just this blueprint
    pass

@bp.after_app_request
def after_api_request(response):
    # Runs after any request to the app
    return response

# Register with app
app.register_blueprint(bp)
```

## Cleanup and Destruction

Properly clean up resources when the application shuts down.

```python
import atexit
from flask import Flask

app = Flask(__name__)

# Register cleanup on normal exit
@atexit.register
def cleanup():
    # Close database connections
    # Release file handles
    # Stop background threads
    pass

# For graceful shutdown in production
import signal

def shutdown_handler(signum, frame):
    cleanup()
    exit(0)

signal.signal(signal.SIGTERM, shutdown_handler)
signal.signal(signal.SIGINT, shutdown_handler)
```
```