# Quickstart

```markdown
# Quickstart Guide

Flask makes it easy to get a web application running with minimal code. Here are the most common patterns you'll use.

## Basic Route and Response

```python
from flask import Flask

app = Flask(__name__)

@app.route("/")
def hello():
    return "Hello, World!"
```

Run with: `flask run` or `python -m flask run`

## Route with Variable

```python
from flask import Flask

app = Flask(__name__)

@app.route("/user/<username>")
def show_user(username):
    return f"User: {username}"

@app.route("/post/<int:post_id>")
def show_post(post_id):
    return f"Post ID: {post_id}"
```

## Rendering Templates

```python
from flask import Flask, render_template

app = Flask(__name__)

@app.route("/hello/<name>")
def hello(name):
    return render_template("hello.html", name=name)
```

Create `templates/hello.html`:
```html
<!DOCTYPE html>
<html>
<body>
    <h1>Hello, {{ name }}!</h1>
</body>
</html>
```

## Handling Form Data

```python
from flask import Flask, request, render_template

app = Flask(__name__)

@app.route("/login", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        username = request.form["username"]
        password = request.form["password"]
        return f"Logged in as {username}"
    return render_template("login.html")
```

## JSON API Endpoint

```python
from flask import Flask, jsonify, request

app = Flask(__name__)

@app.route("/api/data", methods=["POST"])
def api_data():
    data = request.get_json()
    return jsonify({"received": data, "status": "ok"})
```

## Static Files

Place files in `static/` folder and access via `/static/filename.css`.

```python
from flask import Flask, url_for

app = Flask(__name__)

@app.route("/")
def index():
    css_url = url_for("static", filename="style.css")
    return f'<link rel="stylesheet" href="{css_url}">'
```

## Redirects and Error Pages

```python
from flask import Flask, redirect, url_for, abort

app = Flask(__name__)

@app.route("/")
def index():
    return redirect(url_for("login"))

@app.route("/login")
def login():
    return "Login page"

@app.route("/admin")
def admin():
    abort(401)  # Unauthorized
```

## Using Blueprints for Modular Apps

```python
from flask import Blueprint, Flask

bp = Blueprint("auth", __name__, url_prefix="/auth")

@bp.route("/login")
def login():
    return "Auth login page"

app = Flask(__name__)
app.register_blueprint(bp)
```