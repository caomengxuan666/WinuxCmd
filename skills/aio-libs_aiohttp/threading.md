# Threading

### aiohttp is single-threaded by design
```python
import aiohttp
import asyncio
import threading

async def async_work():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            return await resp.text()

# All async code runs in one thread
async def main():
    result = await async_work()
    print(result)

# Threading with asyncio requires careful handling
def thread_worker():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    result = loop.run_until_complete(async_work())
    loop.close()
    print(f"Thread result: {result}")

thread = threading.Thread(target=thread_worker)
thread.start()
thread.join()
```

### Thread safety with run_in_executor
```python
import aiohttp
import asyncio
from concurrent.futures import ThreadPoolExecutor

async def thread_safe_example():
    # Use run_in_executor for blocking operations
    loop = asyncio.get_event_loop()
    
    async with aiohttp.ClientSession() as session:
        # Blocking operation in thread pool
        def blocking_work():
            import time
            time.sleep(1)
            return "Done"
        
        result = await loop.run_in_executor(None, blocking_work)
        
        # Async HTTP request
        async with session.get('http://example.com') as resp:
            data = await resp.text()
            print(f"Async result: {data[:50]}")
            print(f"Thread result: {result}")

asyncio.run(thread_safe_example())
```

### Avoid sharing sessions across threads
```python
import aiohttp
import asyncio
import threading

# BAD: Sharing session across threads
session = aiohttp.ClientSession()

def bad_thread():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    # This will fail - session attached to different loop
    loop.run_until_complete(session.get('http://example.com'))
    loop.close()

# GOOD: Each thread gets its own session
def good_thread():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    async def work():
        async with aiohttp.ClientSession() as s:
            async with s.get('http://example.com') as resp:
                return await resp.text()
    result = loop.run_until_complete(work())
    loop.close()
    print(f"Thread result: {result[:50]}")

thread = threading.Thread(target=good_thread)
thread.start()
thread.join()
```

### Use asyncio locks for shared resources
```python
import aiohttp
import asyncio

async def concurrent_safe():
    lock = asyncio.Lock()
    shared_counter = 0
    
    async def safe_increment(session):
        nonlocal shared_counter
        async with lock:
            # Critical section
            shared_counter += 1
            async with session.get('http://example.com') as resp:
                await resp.read()
    
    async with aiohttp.ClientSession() as session:
        tasks = [safe_increment(session) for _ in range(10)]
        await asyncio.gather(*tasks)
        print(f"Final counter: {shared_counter}")

asyncio.run(concurrent_safe())
```