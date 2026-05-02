# Quickstart

```python
# Basic GET request
import aiohttp
import asyncio

async def get_example():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://python.org') as response:
            print("Status:", response.status)
            html = await response.text()
            print("Body:", html[:50])

asyncio.run(get_example())
```

```python
# POST with JSON data
import aiohttp
import asyncio

async def post_example():
    async with aiohttp.ClientSession() as session:
        payload = {'key': 'value'}
        async with session.post('https://httpbin.org/post', json=payload) as resp:
            print(await resp.json())

asyncio.run(post_example())
```

```python
# WebSocket client
import aiohttp
import asyncio

async def websocket_client():
    async with aiohttp.ClientSession() as session:
        async with session.ws_connect('ws://echo.websocket.org') as ws:
            await ws.send_str('Hello!')
            msg = await ws.receive()
            print(f"Received: {msg.data}")

asyncio.run(websocket_client())
```

```python
# Simple HTTP server
from aiohttp import web

async def handle(request):
    name = request.match_info.get('name', "Anonymous")
    return web.Response(text=f"Hello, {name}")

app = web.Application()
app.add_routes([web.get('/', handle), web.get('/{name}', handle)])

if __name__ == '__main__':
    web.run_app(app)
```

```python
# WebSocket server
from aiohttp import web

async def websocket_handler(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)
    async for msg in ws:
        if msg.type == web.WSMsgType.text:
            await ws.send_str(f"Echo: {msg.data}")
        elif msg.type == web.WSMsgType.close:
            break
    return ws

app = web.Application()
app.add_routes([web.get('/ws', websocket_handler)])
web.run_app(app)
```

```python
# Streaming response
import aiohttp
import asyncio

async def stream_response():
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com/large-file', chunked=True) as resp:
            async for chunk in resp.content.iter_chunked(1024):
                print(f"Got chunk: {len(chunk)} bytes")

asyncio.run(stream_response())
```