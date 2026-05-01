# Threading

**Thread safety basics**
```python
import threading
import redis

# Redis client instances are NOT thread-safe for sharing
r = redis.Redis()

def worker():
    # Each thread should use its own Redis instance
    local_r = redis.Redis(connection_pool=r.connection_pool)
    local_r.incr('counter')

threads = [threading.Thread(target=worker) for _ in range(10)]
for t in threads:
    t.start()
for t in threads:
    t.join()
```

**Using locks for critical sections**
```python
import threading
import redis

lock = threading.Lock()
r = redis.Redis(decode_responses=True)

def safe_increment():
    with lock:
        current = int(r.get('counter') or 0)
        r.set('counter', current + 1)

# Or use Redis SETNX for distributed locking
def distributed_lock(key, timeout=10):
    import time
    lock_key = f"lock:{key}"
    if r.setnx(lock_key, time.time() + timeout):
        r.expire(lock_key, timeout)
        return True
    return False
```

**PubSub in threaded applications**
```python
import threading
import redis

r = redis.Redis(decode_responses=True)

def subscriber():
    pubsub = r.pubsub()
    pubsub.subscribe('channel')
    for message in pubsub.listen():
        if message['type'] == 'message':
            print(f"Received: {message['data']}")

def publisher():
    for i in range(10):
        r.publish('channel', f'Message {i}')

# Start subscriber in separate thread
sub_thread = threading.Thread(target=subscriber, daemon=True)
sub_thread.start()

# Publish from main thread
publisher()
```

**Async operations with asyncio**
```python
import asyncio
import redis.asyncio as aioredis

async def async_example():
    r = aioredis.Redis()
    
    async def worker(name):
        for i in range(5):
            await r.incr('counter')
            print(f"{name}: {await r.get('counter')}")
    
    # Run multiple coroutines concurrently
    await asyncio.gather(
        worker('A'),
        worker('B'),
        worker('C')
    )
    
    await r.close()

asyncio.run(async_example())
```