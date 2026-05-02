# Safety

```markdown
# Safety Guidelines

These conditions must NEVER occur in production Flask applications.

## 1. NEVER Expose Debug Mode in Production

**BAD**: Debug mode enabled in production
```python
from flask import Flask

app = Flask(__name__)
app.config["DEBUG"] = True  # NEVER do this in production

@app.route("/")
def index():
    return "Hello"
```

**GOOD**: Use environment variables to control debug mode
```python
import os
from flask import Flask

app = Flask(__name__)
app.config["DEBUG"] = os.environ.get("FLASK_DEBUG", "0") == "1"

@app.route("/")
def index():
    return "Hello"
```

## 2. NEVER Use `send_file` with User-Controlled Paths Without Validation

**BAD**: Allowing directory traversal attacks
```python
from flask import Flask, send_file

app = Flask(__name__)

@app.route("/download/<path:filename>")
def download(filename):
    # Attacker can use "../../etc/passwd" to read system files
    return send_file(filename)
```

**GOOD**: Validate and restrict paths
```python
import os
from flask import Flask, send_from_directory, abort

app = Flask(__name__)
UPLOAD_DIR = os.path.abspath("uploads")

@app.route("/download/<filename>")
def download(filename):
    # Prevent directory traversal
    safe_path = os.path.normpath(os.path.join(UPLOAD_DIR, filename))
    if not safe_path.startswith(UPLOAD_DIR):
        abort(403)
    return send_from_directory(UPLOAD_DIR, filename)
```

## 3. NEVER Store Secrets in Source Code

**BAD**: Hardcoded secret key and passwords
```python
from flask import Flask

app = Flask(__name__)
app.secret_key = "my-super-secret-key-12345"  # NEVER hardcode secrets
app.config["DATABASE_URL"] = "postgresql://user:password@localhost/db"
```

**GOOD**: Use environment variables or a config file
```python
import os
from flask import Flask

app = Flask(__name__)
app.secret_key = os.environ.get("SECRET_KEY")
if not app.secret_key:
    raise RuntimeError("SECRET_KEY environment variable not set")

app.config["DATABASE_URL"] = os.environ.get("DATABASE_URL")
```

## 4. NEVER Trust User Input Without Validation

**BAD**: Directly using user input in dangerous operations
```python
from flask import Flask, request
import subprocess

app = Flask(__name__)

@app.route("/exec")
def execute():
    cmd = request.args.get("cmd")
    # Attacker can run arbitrary commands
    result = subprocess.check_output(cmd, shell=True)
    return result
```

**GOOD**: Validate and sanitize all user input
```python
from flask import Flask, request, jsonify, abort
import re

app = Flask(__name__)

@app.route("/search")
def search():
    query = request.args.get("q", "")
    # Validate input format
    if not re.match(r"^[a-zA-Z0-9\s]{1,100}$", query):
        abort(400, "Invalid search query")
    # Safe to use validated input
    return jsonify({"query": query})
```

## 5. NEVER Disable CSRF Protection in Production

**BAD**: Disabling CSRF protection for forms
```python
from flask import Flask
from flask_wtf.csrf import CSRFProtect

app = Flask(__name__)
app.config["WTF_CSRF_ENABLED"] = False  # NEVER disable in production
csrf = CSRFProtect(app)
```

**GOOD**: Always enable CSRF protection
```python
from flask import Flask
from flask_wtf.csrf import CSRFProtect

app = Flask(__name__)
app.config["SECRET_KEY"] = os.environ.get("SECRET_KEY")
csrf = CSRFProtect(app)  # CSRF is enabled by default

# For AJAX requests, include the CSRF token
@app.route("/api/data", methods=["POST"])
def api_data():
    # CSRF token is automatically validated
    return jsonify({"status": "ok"})
```
```