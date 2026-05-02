# Best Practices

```markdown
# Best Practices

## Application Factory Pattern

Use the application factory pattern for better testability and configuration management.

```python
# app/__init__.py
from flask import Flask
from config import Config

def create_app(config_class=Config):
    app = Flask(__name__)
    app.config.from_object(config_class)
    
    # Initialize extensions
    from app.extensions import db, migrate
    db.init_app(app)
    migrate.init_app(app, db)
    
    # Register blueprints
    from app.routes import main_bp
    app.register_blueprint(main_bp)
    
    return app
```

## Configuration Management

Separate configuration from code using environment variables and config classes.

```python
# config.py
import os
from dotenv import load_dotenv

load_dotenv()

class Config:
    SECRET_KEY = os.environ.get("SECRET_KEY") or "dev-secret-key"
    SQLALCHEMY_DATABASE_URI = os.environ.get("DATABASE_URL")
    SQLALCHEMY_TRACK_MODIFICATIONS = False

class ProductionConfig(Config):
    DEBUG = False
    # Production-specific settings

class DevelopmentConfig(Config):
    DEBUG = True
    SQLALCHEMY_ECHO = True
```

## Blueprint Organization

Organize your application into logical modules using blueprints.

```python
# app/routes/auth.py
from flask import Blueprint, render_template, redirect, url_for

auth_bp = Blueprint("auth", __name__, url_prefix="/auth")

@auth_bp.route("/login", methods=["GET", "POST"])
def login():
    return render_template("auth/login.html")

@auth_bp.route("/logout")
def logout():
    return redirect(url_for("auth.login"))
```

## Error Handling

Implement comprehensive error handling for production readiness.

```python
# app/errors.py
from flask import Blueprint, jsonify
from werkzeug.exceptions import HTTPException

errors_bp = Blueprint("errors", __name__)

@errors_bp.app_errorhandler(400)
def bad_request(error):
    return jsonify({"error": "Bad request", "message": str(error)}), 400

@errors_bp.app_errorhandler(404)
def not_found(error):
    return jsonify({"error": "Not found"}), 404

@errors_bp.app_errorhandler(500)
def internal_error(error):
    return jsonify({"error": "Internal server error"}), 500
```

## Logging Configuration

Set up proper logging for debugging and monitoring.

```python
# app/logging_config.py
import logging
import sys
from flask import Flask

def configure_logging(app: Flask):
    handler = logging.StreamHandler(sys.stdout)
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter(
        "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
    )
    handler.setFormatter(formatter)
    app.logger.addHandler(handler)
    app.logger.setLevel(logging.INFO)
```

## Database Session Management

Properly manage database sessions to avoid connection leaks.

```python
# app/models.py
from flask_sqlalchemy import SQLAlchemy
from flask import g

db = SQLAlchemy()

def get_db():
    if "db" not in g:
        g.db = db.session
    return g.db

@app.teardown_appcontext
def close_db(error):
    db_session = g.pop("db", None)
    if db_session is not None:
        db_session.close()
```

## Testing Setup

Write tests using Flask's test client.

```python
# tests/test_app.py
import pytest
from app import create_app
from app.config import TestingConfig

@pytest.fixture
def app():
    app = create_app(TestingConfig)
    yield app

@pytest.fixture
def client(app):
    return app.test_client()

def test_home_page(client):
    response = client.get("/")
    assert response.status_code == 200
    assert b"Welcome" in response.data
```
```