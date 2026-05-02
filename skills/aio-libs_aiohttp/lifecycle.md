# Lifecycle

### Construction: Creating sessions and connectors
```python
import aiohttp
import asyncio

async def construct_example():
    # Basic session creation
    session = aiohttp.ClientSession()
    
    # With custom connector
    connector = aiohttp.TCPConnector(limit=100, verify_ssl=True)
    session_with_connector = aiohttp.ClientSession(connector=connector)
    
    # With timeout
    timeout = aiohttp.ClientTimeout(total=30, connect=10)
    session_with_timeout = aiohttp.ClientSession(timeout=timeout)
    
    # Clean up
    await session.close()
    await session_with_connector.close()
    await session_with_timeout.close()

asyncio.run(construct_example())
```

### Destruction: Proper cleanup
```python
import aiohttp
import asyncio

async def destruction_example():
    session = aiohttp.ClientSession()
    try:
        async with session.get('http://example.com') as resp:
            await resp.text()
    finally:
        await session.close()  # Explicit cleanup
    
    # Or use context manager for automatic cleanup
    async with aiohttp.ClientSession() as session2:
        async with session2.get('http://example.com') as resp:
            await resp.text()
    # session2 automatically closed

asyncio.run(destruction_example())
```

### Resource management with connectors
```python
import aiohttp
import asyncio

async def resource_management():
    # Connector manages TCP connections
    connector = aiohttp.TCPConnector(limit=10)
    async with aiohttp.ClientSession(connector=connector) as session:
        async with session.get('http://example.com') as resp:
            data = await resp.read()
    # Connector closed when session closes
    
    # Manual connector lifecycle
    conn = aiohttp.TCPConnector()
    await conn.connect()  # Initialize
    # Use connector...
    await conn.close()  # Clean up

asyncio.run(resource_management())
```

### Move semantics and copying
```python
import aiohttp
import asyncio
from copy import deepcopy

async def copy_example():
    # Sessions cannot be copied directly
    session = aiohttp.ClientSession()
    # session2 = session  # This would share the same session
    
    # But you can copy configuration
    config = {
        'timeout': aiohttp.ClientTimeout(total=30),
        'connector': aiohttp.TCPConnector(limit=10)
    }
    session1 = aiohttp.ClientSession(**config)
    session2 = aiohttp.ClientSession(**config)  # New session with same config
    
    await session1.close()
    await session2.close()

asyncio.run(copy_example())
```