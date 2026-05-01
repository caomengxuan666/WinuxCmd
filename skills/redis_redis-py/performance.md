# Performance

**Connection pooling performance**
```python
# BAD - creates new connection for each operation
def bad_performance():
    for i in range(1000):
        r = redis.Redis()
        r.set(f'key:{i}', f'value:{i}')

# GOOD - reuse connection pool
pool = ConnectionPool(max_connections=10)
r = Redis(connection_pool=pool)
def good_performance():
    for i in range(1000):
        r.set(f'key:{i}', f'value:{i}')
```

**Pipeline batching for bulk operations**
```python
# BAD - individual round trips
for i in range(10000):
    r.set(f'key:{i}', f'value:{i}')

# GOOD - batch with pipeline
pipe = r.pipeline()
for i in range(10000):
    pipe.set(f'key:{i}', f'value:{i}')
pipe.execute()  # Single network round trip
```

**Using hiredis for faster parsing**
```python
# Install with hiredis support
# pip install redis[hiredis]

# Automatically uses hiredis if available
r = redis.Redis()  # Will use hiredis parser

# Check if hiredis is being used
print(f"Using hiredis: {hasattr(r.connection_pool, 'parser_class')}")
```

**Optimize data serialization**
```python
import json
import pickle

# BAD - using pickle (slow, insecure)
r.set('user', pickle.dumps({'name': 'Alice', 'age': 30}))

# GOOD - use JSON for structured data
r.set('user', json.dumps({'name': 'Alice', 'age': 30}))

# For simple values, use Redis data structures
r.hset('user:1', mapping={'name': 'Alice', 'age': '30'})
```

**Memory optimization tips**
```python
# Use appropriate data types
# BAD - storing large strings
r.set('big_data', 'x' * 1000000)

# GOOD - compress or split data
import zlib
compressed = zlib.compress(b'x' * 1000000)
r.set('big_data', compressed)

# Use expiration for temporary data
r.setex('temp_data', 3600, 'value')  # Auto-expire after 1 hour
```