# Performance

### Connection pooling reduces overhead
```python
import aiohttp
import asyncio
import time

async def benchmark_pooling():
    urls = ['http://example.com'] * 50
    
    # Without pooling (bad)
    start = time.time()
    for url in urls:
        async with aiohttp.ClientSession() as session:
            async with session.get(url) as resp:
                await resp.read()
    print(f"Without pooling: {time.time() - start:.2f}s")
    
    # With pooling (good)
    start = time.time()
    async with aiohttp.ClientSession() as session:
        for url in urls:
            async with session.get(url) as resp:
                await resp.read()
    print(f"With pooling: {time.time() - start:.2f}s")

asyncio.run(benchmark_pooling())
```

### Use streaming for large responses
```python
import aiohttp
import asyncio

async def streaming_performance():
    # Bad: Loading entire response into memory
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com/large') as resp:
            data = await resp.read()  # All in memory
            process(data)
    
    # Good: Stream chunks
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com/large') as resp:
            async for chunk in resp.content.iter_chunked(8192):
                process_chunk(chunk)  # Process incrementally

asyncio.run(streaming_performance())
```

### Optimize with connection limits
```python
import aiohttp
import asyncio

async def optimized_connections():
    # Tune connector for your workload
    connector = aiohttp.TCPConnector(
        limit=100,          # Max total connections
        limit_per_host=20,  # Max per host
        ttl_dns_cache=300,  # DNS cache TTL
        use_dns_cache=True, # Enable DNS caching
        force_close=False   # Keep-alive
    )
    
    async with aiohttp.ClientSession(connector=connector) as session:
        tasks = [session.get(f'http://example.com/{i}') for i in range(100)]
        responses = await asyncio.gather(*tasks)
        for resp in responses:
            await resp.read()
            resp.close()

asyncio.run(optimized_connections())
```

### Use compression for bandwidth savings
```python
import aiohttp
import asyncio

async def compression_example():
    # Enable automatic decompression
    async with aiohttp.ClientSession(auto_decompress=True) as session:
        async with session.get('http://example.com') as resp:
            data = await resp.read()
            # Automatically decompressed if server supports it
    
    # Manual control
    headers = {'Accept-Encoding': 'gzip, deflate'}
    async with aiohttp.ClientSession() as session:
        async with session.get('http://example.com', headers=headers) as resp:
            if resp.headers.get('Content-Encoding') == 'gzip':
                import zlib
                data = zlib.decompress(await resp.read(), 16 + zlib.MAX_WBITS)
            else:
                data = await resp.read()

asyncio.run(compression_example())
```