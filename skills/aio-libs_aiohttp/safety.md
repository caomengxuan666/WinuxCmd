# Safety

### Condition 1: NEVER use a closed session
BAD: Using session after context manager exit
```python
import aiohttp
import asyncio

async def bad_closed():
    session = aiohttp.ClientSession()
    async with session:
        pass
    # Session is now closed
    async with session.get('http://example.com') as resp:
        pass  # RuntimeError: Session is closed

asyncio.run(bad_closed())
```

GOOD: Always use session within its context
```python
async def good_closed():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            print(await resp.text())
    # Session closed after context exit

asyncio.run(good_closed())
```

### Condition 2: NEVER ignore ClientConnectorError for DNS failures
BAD: Not handling connection errors
```python
async def bad_connector():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://nonexistent.domain') as resp:
            print(await resp.text())
    # Raises ClientConnectorError

asyncio.run(bad_connector())
```

GOOD: Catch and handle connection errors
```python
from aiohttp import ClientConnectorError

async def good_connector():
    async with aiohttp.ClientSession() as session:
        try:
            async with session.get('http://nonexistent.domain') as resp:
                print(await resp.text())
        except ClientConnectorError:
            print("DNS resolution failed, retrying...")

asyncio.run(good_connector())
```

### Condition 3: NEVER share sessions across event loops
BAD: Creating session in one loop, using in another
```python
async def bad_loop():
    session = aiohttp.ClientSession()
    # Assume this runs in a different event loop
    async with session.get('http://example.com') as resp:
        pass
    # RuntimeError: Session is attached to a different loop

asyncio.run(bad_loop())
```

GOOD: Create session in the same loop where it's used
```python
async def good_loop():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            print(await resp.text())

asyncio.run(good_loop())
```

### Condition 4: NEVER forget to await WebSocket prepare
BAD: Not preparing WebSocket response
```python
from aiohttp import web

async def bad_ws(request):
    ws = web.WebSocketResponse()
    # Missing await ws.prepare(request)
    async for msg in ws:
        pass
    return ws
```

GOOD: Always await prepare before using WebSocket
```python
async def good_ws(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)
    async for msg in ws:
        if msg.type == web.WSMsgType.text:
            await ws.send_str(f"Echo: {msg.data}")
    return ws
```

### Condition 5: NEVER ignore response content length mismatch
BAD: Assuming content-length is accurate
```python
async def bad_length():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            data = await resp.read()
            # If server lies about content-length, may get incomplete data

asyncio.run(bad_length())
```

GOOD: Use chunked reading for safety
```python
async def good_length():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com') as resp:
            chunks = []
            async for chunk in resp.content.iter_chunked(8192):
                chunks.append(chunk)
            data = b''.join(chunks)
            print(f"Read {len(data)} bytes")

asyncio.run(good_length())
```