# Best Practices

**Use connection pooling for production applications**
```python
import redis
from redis import ConnectionPool

# Create pool once at application startup
pool = ConnectionPool(
    host='localhost',
    port=6379,
    max_connections=10,
    decode_responses=True
)

# Reuse pool across the application
def get_redis():
    return redis.Redis(connection_pool=pool)
```

**Implement retry logic for transient failures**
```python
from redis import Redis
from redis.backoff import ExponentialBackoff
from redis.retry import Retry
from redis.exceptions import ConnectionError, TimeoutError

retry = Retry(ExponentialBackoff(), retries=3)
r = Redis(
    retry=retry,
    retry_on_error=[ConnectionError, TimeoutError]
)
```

**Use pipelines for batch operations**
```python
# Instead of multiple round trips
pipe = r.pipeline()
for i in range(1000):
    pipe.set(f'key:{i}', f'value:{i}')
pipe.execute()  # Single round trip
```

**Monitor connection pool usage**
```python
pool = ConnectionPool(max_connections=20)
r = Redis(connection_pool=pool)

# Check pool statistics
print(f"Active connections: {pool._created_connections}")
print(f"Available connections: {pool._available_connections}")
```

**Use appropriate data structures**
```python
# For counters: use INCR/DECR
r.incr('page_views')

# For session data: use HASH
r.hset('session:123', mapping={
    'user_id': '456',
    'last_access': '2024-01-01'
})

# For queues: use LIST
r.rpush('task_queue', 'task1', 'task2')
task = r.blpop('task_queue', timeout=5)
```