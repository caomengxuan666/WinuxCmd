# Quickstart

```python
# Basic connection and key-value operations
import redis
r = redis.Redis(host='localhost', port=6379, db=0)
r.set('foo', 'bar')
value = r.get('foo')  # Returns b'bar'
print(value)

# Connection with connection pool
pool = redis.ConnectionPool(host='localhost', port=6379, db=0)
r = redis.Redis(connection_pool=pool)
r.set('user:1', 'Alice')
print(r.get('user:1'))

# Using pipelines for batch operations
pipe = r.pipeline()
pipe.set('counter', 0)
pipe.incr('counter')
pipe.incr('counter')
results = pipe.execute()  # Returns [True, 1, 2]
print(results)

# Working with hashes
r.hset('user:100', mapping={'name': 'Bob', 'age': '30'})
user = r.hgetall('user:100')
print(user)  # Returns {b'name': b'Bob', b'age': b'30'}

# Pub/Sub messaging
pubsub = r.pubsub()
pubsub.subscribe('news')
# In another process/thread:
r.publish('news', 'Hello subscribers!')
message = pubsub.get_message(timeout=1)
print(message)

# Working with lists
r.rpush('tasks', 'task1', 'task2', 'task3')
task = r.lpop('tasks')
print(task)  # Returns b'task1'

# Using sets
r.sadd('tags', 'python', 'redis', 'database')
members = r.smembers('tags')
print(members)  # Returns set of bytes

# Setting expiration
r.setex('temp_key', 10, 'This will expire in 10 seconds')
ttl = r.ttl('temp_key')
print(f'Time to live: {ttl} seconds')
```