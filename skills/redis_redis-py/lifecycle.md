# Lifecycle

**Construction and initialization**
```python
import redis
from redis import ConnectionPool

# Basic construction
r = redis.Redis(host='localhost', port=6379, db=0)

# With connection pool
pool = ConnectionPool(host='localhost', port=6379, db=0, max_connections=10)
r = redis.Redis(connection_pool=pool)

# With all options
r = redis.Redis(
    host='localhost',
    port=6379,
    db=0,
    password='secret',
    ssl=True,
    socket_connect_timeout=5,
    socket_timeout=10,
    retry_on_timeout=True,
    decode_responses=True
)
```

**Resource management and cleanup**
```python
# Connection pool lifecycle
pool = ConnectionPool(max_connections=5)
r = Redis(connection_pool=pool)

# Use the connection
r.set('key', 'value')

# Explicitly close connections when done
pool.disconnect()  # Closes all connections in pool

# Or let it be garbage collected
del r
del pool
```

**Context manager usage**
```python
# Redis client as context manager
with redis.Redis() as r:
    r.set('key', 'value')
    value = r.get('key')
# Connection automatically returned to pool

# Pipeline as context manager
with r.pipeline() as pipe:
    pipe.set('a', 1)
    pipe.set('b', 2)
    pipe.execute()
# Pipeline automatically reset
```

**Error handling during lifecycle**
```python
try:
    r = redis.Redis(host='localhost', port=6379)
    r.ping()  # Test connection
    r.set('key', 'value')
except redis.ConnectionError:
    print("Failed to connect to Redis")
except redis.TimeoutError:
    print("Redis operation timed out")
finally:
    if 'r' in locals():
        r.close()
```