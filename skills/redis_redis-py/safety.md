# Safety

**Condition 1: NEVER hardcode connection credentials in production code**
```python
# BAD - credentials in source code
r = redis.Redis(host='localhost', port=6379, password='mysecretpassword')

# GOOD - use environment variables
import os
r = redis.Redis(
    host=os.getenv('REDIS_HOST', 'localhost'),
    port=int(os.getenv('REDIS_PORT', 6379)),
    password=os.getenv('REDIS_PASSWORD'),
    ssl=True
)
```

**Condition 2: NEVER ignore SSL/TLS warnings in production**
```python
# BAD - disabling SSL verification
r = redis.Redis(ssl=True, ssl_cert_reqs=None)  # Insecure!

# GOOD - proper SSL configuration
r = redis.Redis(
    ssl=True,
    ssl_cert_reqs='required',
    ssl_ca_certs='/path/to/ca-certificates.crt'
)
```

**Condition 3: NEVER use eval/execute with user input**
```python
# BAD - vulnerable to injection
user_input = "'; FLUSHALL; '"
r.eval(f"return redis.call('SET', 'key', '{user_input}')", 0)

# GOOD - use parameterized commands
r.set('key', user_input)  # Safe, Redis handles escaping
```

**Condition 4: NEVER ignore connection timeouts in production**
```python
# BAD - no timeout, can hang forever
r = redis.Redis()

# GOOD - set reasonable timeouts
r = redis.Redis(
    socket_connect_timeout=5,
    socket_timeout=10,
    retry_on_timeout=True
)
```

**Condition 5: NEVER share Redis instances across threads without synchronization**
```python
# BAD - race conditions possible
r = redis.Redis()
# Thread 1: r.set('key', 'value1')
# Thread 2: r.set('key', 'value2')
# Result unpredictable

# GOOD - use locks or separate connections
import threading
lock = threading.Lock()
r = redis.Redis()
with lock:
    r.set('key', 'value1')
```