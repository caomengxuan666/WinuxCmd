# Pitfalls

**Pitfall 1: Forgetting to decode responses**
```python
# BAD - returns bytes, may cause unexpected behavior
r = redis.Redis()
r.set('key', 'value')
result = r.get('key')
if result == 'value':  # Always False! Comparing bytes to string
    print("Match")

# GOOD - decode responses or compare properly
r = redis.Redis(decode_responses=True)
r.set('key', 'value')
result = r.get('key')
if result == 'value':  # Now works correctly
    print("Match")
```

**Pitfall 2: Not handling connection failures**
```python
# BAD - no error handling
r = redis.Redis(host='unreachable-host')
r.set('key', 'value')  # Will raise ConnectionError

# GOOD - handle connection errors
import redis
try:
    r = redis.Redis(host='unreachable-host', socket_connect_timeout=5)
    r.set('key', 'value')
except redis.ConnectionError as e:
    print(f"Failed to connect: {e}")
```

**Pitfall 3: Creating new connections for every operation**
```python
# BAD - creates new connection pool each time
def get_value(key):
    r = redis.Redis()  # New connection pool every call
    return r.get(key)

# GOOD - reuse connection pool
pool = redis.ConnectionPool()
def get_value(key):
    r = redis.Redis(connection_pool=pool)
    return r.get(key)
```

**Pitfall 4: Ignoring pipeline transaction behavior**
```python
# BAD - assumes atomicity without transaction
pipe = r.pipeline(transaction=False)
pipe.set('a', 1)
pipe.set('b', 2)
pipe.execute()  # Not atomic!

# GOOD - use transaction=True (default)
pipe = r.pipeline(transaction=True)
pipe.set('a', 1)
pipe.set('b', 2)
pipe.execute()  # Atomic execution
```

**Pitfall 5: Not closing pubsub connections**
```python
# BAD - pubsub connection never closed
p = r.pubsub()
p.subscribe('channel')
# ... listen forever without cleanup

# GOOD - use context manager or explicit close
with r.pubsub() as p:
    p.subscribe('channel')
    for message in p.listen():
        if message['type'] == 'message':
            print(message['data'])
# Connection automatically returned to pool
```

**Pitfall 6: Using wrong data types for operations**
```python
# BAD - mixing string and bytes operations
r.set('counter', '5')
r.incr('counter')  # Works, but confusing

# GOOD - use appropriate types
r.set('counter', 5)  # Integer
r.incr('counter')    # Now 6
```