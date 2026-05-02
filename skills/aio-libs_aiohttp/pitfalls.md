# Pitfalls

### Pitfall 1: Not using async context managers for sessions
BAD: Creating sessions without proper cleanup
```python
import aiohttp
import asyncio

async def bad_request():
    session = aiohttp.ClientSession()
    resp = await session.get('http://example.com')
    print(await resp.text())
    # Session never closed - resource leak!

asyncio.run(bad_request())
```

GOOD: Using async with for automatic cleanup
```python
import aiohttp
import asyncio

async def good_request():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            print(await resp.text())
    # Session automatically closed

asyncio.run(good_request())
```

### Pitfall 2: Forgetting to await response reading
BAD: Not awaiting response body
```python
async def bad_read():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            # resp.text() returns a coroutine, not the actual text
            print(resp.text())  # Prints coroutine object!

asyncio.run(bad_read())
```

GOOD: Always await response methods
```python
async def good_read():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            text = await resp.text()
            print(text)

asyncio.run(good_read())
```

### Pitfall 3: Creating sessions per request
BAD: New session for every request
```python
async def bad_multi():
    for i in range(10):
        async with aiohttp.ClientSession() as session:
            async with session.get(f'http://example.com/{i}') as resp:
                print(await resp.text())
    # 10 sessions created - inefficient

asyncio.run(bad_multi())
```

GOOD: Reuse session for multiple requests
```python
async def good_multi():
    async with aiohttp.ClientSession() as session:
        for i in range(10):
            async with session.get(f'http://example.com/{i}') as resp:
                print(await resp.text())
    # Single session reused

asyncio.run(good_multi())
```

### Pitfall 4: Ignoring timeout configuration
BAD: No timeout - can hang forever
```python
async def bad_timeout():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://slow-server.com') as resp:
            print(await resp.text())
    # Could hang indefinitely

asyncio.run(bad_timeout())
```

GOOD: Set reasonable timeouts
```python
import aiohttp
import asyncio
from aiohttp import ClientTimeout

async def good_timeout():
    timeout = ClientTimeout(total=10)
    async with aiohttp.ClientSession(timeout=timeout) as session:
        async with session.get('http://slow-server.com') as resp:
            print(await resp.text())
    # Will raise TimeoutError after 10 seconds

asyncio.run(good_timeout())
```

### Pitfall 5: Not handling response payload errors
BAD: Assuming response always completes
```python
async def bad_payload():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://unstable-api.com') as resp:
            data = await resp.read()
            # May raise ClientPayloadError if connection drops

asyncio.run(bad_payload())
```

GOOD: Handle payload errors gracefully
```python
import aiohttp
from aiohttp import ClientPayloadError

async def good_payload():
    async with aiohttp.ClientSession() as session:
        try:
            async with session.get('http://unstable-api.com') as resp:
                data = await resp.read()
        except ClientPayloadError:
            print("Response payload incomplete, retrying...")

asyncio.run(good_payload())
```

### Pitfall 6: Mixing sync and async code incorrectly
BAD: Blocking event loop with sync calls
```python
import time
import aiohttp
import asyncio

async def bad_blocking():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            time.sleep(5)  # Blocks event loop!
            print(await resp.text())

asyncio.run(bad_blocking())
```

GOOD: Use async sleep or run_in_executor
```python
import aiohttp
import asyncio

async def good_nonblocking():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            await asyncio.sleep(5)  # Non-blocking
            print(await resp.text())

asyncio.run(good_nonblocking())
```