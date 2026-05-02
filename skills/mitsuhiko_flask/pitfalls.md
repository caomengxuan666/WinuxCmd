# Pitfalls

```markdown
# Common Pitfalls

## 1. Forgetting to Set FLASK_APP

**BAD**: Running `flask run` without setting the app variable
```bash
$ flask run
Error: Could not locate a Flask application. You did not provide the "FLASK_APP" environment variable.
```

**GOOD**: Set the environment variable first
```bash
$ export FLASK_APP=app.py
$ flask run
```

Or use the `--app` flag:
```bash
$ flask --app app.py run
```

## 2. Using `send_file` with Relative Paths

**BAD**: Using a relative path that resolves incorrectly
```python
from flask import Flask, send_file

app = Flask(__name__)

@app.route("/download")
def download():
    # This may fail if the working directory is not what you expect
    return send_file("../data/file.pdf")
```

**GOOD**: Use absolute paths or `send_from_directory`
```python
import os
from flask import Flask, send_from_directory

app = Flask(__name__)
BASE_DIR = os.path.abspath(os.path.dirname(__file__))

@app.route("/download/<filename>")
def download(filename):
    return send_from_directory(
        os.path.join(BASE_DIR, "data"),
        filename,
        as_attachment=True
    )
```

## 3. Template Rendering Failure Without Debug Output

**BAD**: Template errors silently return empty responses
```python
from flask import Flask, render_template

app = Flask(__name__)
app.config["DEBUG"] = True

@app.route("/")
def index():
    # If 'unknown' template doesn't exist, browser gets empty response
    return render_template("unknown")
```

**GOOD**: Handle template errors explicitly
```python
from flask import Flask, render_template, abort

app = Flask(__name__)

@app.route("/")
def index():
    try:
        return render_template("index.html")
    except Exception as e:
        app.logger.error(f"Template error: {e}")
        abort(500)
```

## 4. Wrong JSON Library Import

**BAD**: Relying on `simplejson` which may not be installed
```python
# This may fail if simplejson is not available
from flask.json import dumps
```

**GOOD**: Use Flask's built-in JSON support
```python
from flask import jsonify

@app.route("/api/data")
def get_data():
    return jsonify({"key": "value"})
```

## 5. Setting Error Handlers for Non-Standard Status Codes

**BAD**: Registering handler for a status code not in default exceptions
```python
from flask import Flask

app = Flask(__name__)

# This raises KeyError: 402 because 402 is not a standard HTTP exception
@app.errorhandler(402)
def handle_402(error):
    return "Payment Required", 402
```

**GOOD**: Use a generic handler or register for standard codes
```python
from flask import Flask
from werkzeug.exceptions import HTTPException

app = Flask(__name__)

@app.errorhandler(HTTPException)
def handle_http_error(error):
    return {"error": error.description}, error.code

# Or register for standard codes
@app.errorhandler(404)
def not_found(error):
    return "Not Found", 404
```

## 6. Configuration File Encoding Issues on Python 3

**BAD**: Config file with non-UTF-8 characters causes crash
```python
from flask import Flask

app = Flask(__name__)
# If config.cfg contains non-ASCII characters, this crashes on Python 3
app.config.from_pyfile("config.cfg")
```

**GOOD**: Specify encoding explicitly
```python
from flask import Flask

app = Flask(__name__)

# Open with explicit encoding
with open("config.cfg", "r", encoding="utf-8") as f:
    app.config.from_pyfile(f)
```

## 7. Debug Mode Causing File Not Found Errors on Windows

**BAD**: Debug mode enabled without proper setup
```bash
$ set FLASK_DEBUG=1
$ flask run
# On Windows, this may cause "[Errno 2] No such file or directory"
```

**GOOD**: Use development server properly
```python
from flask import Flask

app = Flask(__name__)

if __name__ == "__main__":
    app.run(debug=True, use_reloader=False)  # Disable reloader on Windows
```

## 8. Import Errors with Flask CLI Commands

**BAD**: Running CLI commands from wrong directory
```bash
$ flask initdb
# ImportError because Flask can't find the application module
```

**GOOD**: Ensure you're in the correct directory or set PYTHONPATH
```bash
$ cd /path/to/your/app
$ export FLASK_APP=app.py
$ export PYTHONPATH=/path/to/your/app:$PYTHONPATH
$ flask initdb
```
```