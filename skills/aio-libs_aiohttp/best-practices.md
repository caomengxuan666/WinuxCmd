# Best Practices

### Use connection pooling with session reuse
```python
import aiohttp
import asyncio

async def fetch_all(urls):
    connector = aiohttp.TCPConnector(limit=10, limit_per_host=5)
    timeout = aiohttp.ClientTimeout(total=30)
    async with aiohttp.ClientSession(connector=connector, timeout=timeout) as session:
        tasks = [fetch_one(session, url) for url in urls]
        return await asyncio.gather(*tasks)

async def fetch_one(session, url):
    async with session.get(url) as resp:
        return await resp.text()

urls = ['http://example.com'] * 20
results = asyncio.run(fetch_all(urls))
```

### Implement retry logic with exponential backoff
```python
import aiohttp
import asyncio
from aiohttp import ClientError

async def fetch_with_retry(session, url, max_retries=3):
    for attempt in range(max_retries):
        try:
            async with session.get(url) as resp:
                if resp.status >= 500:
                    await asyncio.sleep(2 ** attempt)
                    continue
                return await resp.text()
        except ClientError:
            if attempt == max_retries - 1:
                raise
            await asyncio.sleep(2 ** attempt)
    return None
```

### Use middleware for logging and error handling in servers
```python
from aiohttp import web
import logging

logging.basicConfig(level=logging.INFO)

@web.middleware
async def logging_middleware(request, handler):
    logging.info(f"Request: {request.method} {request.path}")
    try:
        response = await handler(request)
        logging.info(f"Response: {response.status}")
        return response
    except web.HTTPException as ex:
        logging.error(f"Error: {ex.status}")
        raise

app = web.Application(middlewares=[logging_middleware])
```

### Properly handle large file uploads with streaming
```python
import aiohttp
import asyncio

async def upload_large_file(session, url, filepath):
    async with aiohttp.MultipartWriter() as mpwriter:
        part = mpwriter.append(open(filepath, 'rb'))
        part.set_content_disposition('form-data', name='file', filename='data.bin')
        async with session.post(url, data=mpwriter) as resp:
            return await resp.json()
```

### Use connection keep-alive for repeated requests
```python
import aiohttp
import asyncio

async def keep_alive_example():
    connector = aiohttp.TCPConnector(force_close=False)  # Keep connections alive
    async with aiohttp.ClientSession(connector=connector) as session:
        for _ in range(100):
            async with session.get('http://example.com') as resp:
                await resp.read()
            # Connection reused automatically
```