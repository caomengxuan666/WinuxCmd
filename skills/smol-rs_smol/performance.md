# Performance

```markdown
# Performance Characteristics of smol

## Runtime Overhead

smol has minimal runtime overhead compared to raw async/await:

```rust
use smol::{io, prelude::*};
use std::time::Instant;

fn main() -> io::Result<()> {
    smol::block_on(async {
        let start = Instant::now();
        
        // Measure task spawning overhead
        for _ in 0..10000 {
            smol::spawn(async {}).await;
        }
        
        println!("10,000 tasks in {:?}", start.elapsed());
        Ok(())
    })
}
```

## I/O Performance

Use buffered I/O for better performance with many small reads/writes:

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let stream = net::TcpStream::connect("example.com:80").await?;
        
        // Without buffering - many small reads
        let start = std::time::Instant::now();
        let mut buf = [0u8; 1];
        for _ in 0..1000 {
            stream.read(&mut buf).await?;
        }
        println!("Unbuffered: {:?}", start.elapsed());
        
        // With buffering
        let stream = net::TcpStream::connect("example.com:80").await?;
        let mut reader = io::BufReader::new(stream);
        let start = std::time::Instant::now();
        let mut buf = [0u8; 1];
        for _ in 0..1000 {
            reader.read(&mut buf).await?;
        }
        println!("Buffered: {:?}", start.elapsed());
        
        Ok(())
    })
}
```

## Allocation Patterns

Minimize allocations in hot paths:

```rust
use smol::{io, net, prelude::*};

// BAD: Allocating in a loop
async fn bad_echo(mut stream: net::TcpStream) -> io::Result<()> {
    loop {
        let mut buf = vec![0u8; 1024]; // BAD: Allocating each iteration
        let n = stream.read(&mut buf).await?;
        if n == 0 { break; }
        stream.write_all(&buf[..n]).await?;
    }
    Ok(())
}

// GOOD: Reuse buffer
async fn good_echo(mut stream: net::TcpStream) -> io::Result<()> {
    let mut buf = vec![0u8; 1024]; // GOOD: Allocate once
    loop {
        buf.clear();
        let n = stream.read(&mut buf).await?;
        if n == 0 { break; }
        stream.write_all(&buf[..n]).await?;
    }
    Ok(())
}

fn main() -> io::Result<()> {
    smol::block_on(async {
        let listener = net::TcpListener::bind("127.0.0.1:0").await?;
        let (stream, _) = listener.accept().await?;
        good_echo(stream).await?;
        Ok(())
    })
}
```

## Task Scheduling Overhead

Batch work to reduce scheduling overhead:

```rust
use smol::{io, prelude::*};

// BAD: Many tiny tasks
async fn bad_pattern() {
    let start = std::time::Instant::now();
    let mut tasks = Vec::new();
    for i in 0..1000 {
        tasks.push(smol::spawn(async move {
            // Tiny amount of work
            i * 2
        }));
    }
    for task in tasks {
        let _ = task.await;
    }
    println!("Many tiny tasks: {:?}", start.elapsed());
}

// GOOD: Batch work into fewer tasks
async fn good_pattern() {
    let start = std::time::Instant::now();
    let mut tasks = Vec::new();
    for chunk in (0..1000).collect::<Vec<_>>().chunks(100) {
        let chunk = chunk.to_vec();
        tasks.push(smol::spawn(async move {
            chunk.iter().map(|x| x * 2).sum::<i32>()
        }));
    }
    let total: i32 = tasks.into_iter().map(|t| t.await.unwrap()).sum();
    println!("Batched tasks: {:?}", start.elapsed());
    println!("Total: {}", total);
}

fn main() -> io::Result<()> {
    smol::block_on(async {
        bad_pattern().await;
        good_pattern().await;
        Ok(())
    })
}
```

## Channel Performance

Choose the right channel type for your use case:

```rust
use smol::{channel, prelude::*};

fn main() {
    smol::block_on(async {
        // Bounded channel - backpressure
        let (tx, rx) = channel::bounded::<i32>(100);
        let start = std::time::Instant::now();
        
        let producer = smol::spawn(async move {
            for i in 0..10000 {
                tx.send(i).await.unwrap();
            }
        });
        
        let consumer = smol::spawn(async move {
            let mut count = 0;
            while let Ok(_) = rx.recv().await {
                count += 1;
            }
            count
        });
        
        producer.await;
        drop(rx); // Close channel
        let count = consumer.await;
        println!("Bounded: {} items in {:?}", count, start.elapsed());
        
        // Unbounded channel - no backpressure
        let (tx, rx) = channel::unbounded::<i32>();
        let start = std::time::Instant::now();
        
        let producer = smol::spawn(async move {
            for i in 0..10000 {
                tx.send(i).await.unwrap();
            }
        });
        
        let consumer = smol::spawn(async move {
            let mut count = 0;
            while let Ok(_) = rx.recv().await {
                count += 1;
            }
            count
        });
        
        producer.await;
        drop(rx);
        let count = consumer.await;
        println!("Unbounded: {} items in {:?}", count, start.elapsed());
    })
}
```

## Memory Usage Optimization

Monitor and optimize memory usage:

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // Use appropriate buffer sizes
        let stream = net::TcpStream::connect("example.com:80").await?;
        
        // For HTTP headers, 8KB is usually enough
        let mut header_buf = [0u8; 8192];
        let n = stream.read(&mut header_buf).await?;
        println!("Read {} bytes of headers", n);
        
        // For large transfers, use streaming
        let stream = net::TcpStream::connect("example.com:80").await?;
        let mut stdout = smol::Unblock::new(std::io::stdout());
        io::copy(stream, &mut stdout).await?;
        
        Ok(())
    })
}
```
```