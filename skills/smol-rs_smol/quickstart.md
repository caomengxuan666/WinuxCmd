# Quickstart

```markdown
# Quickstart Guide for smol

## Installation

Add smol to your `Cargo.toml`:

```toml
[dependencies]
smol = "2.0"
```

## Basic Async TCP Client

Connect to a website and read the response:

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mut stream = net::TcpStream::connect("example.com:80").await?;
        let req = b"GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
        stream.write_all(req).await?;

        let mut buf = vec![0u8; 1024];
        let n = stream.read(&mut buf).await?;
        println!("Response: {}", String::from_utf8_lossy(&buf[..n]));
        Ok(())
    })
}
```

## Async File I/O

Read a file asynchronously using `Unblock`:

```rust
use smol::{fs, io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mut file = fs::File::open("Cargo.toml").await?;
        let mut contents = String::new();
        file.read_to_string(&mut contents).await?;
        println!("File contents:\n{}", contents);
        Ok(())
    })
}
```

## Spawning Tasks

Run multiple tasks concurrently:

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let task1 = smol::spawn(async {
            println!("Task 1 running");
            42
        });
        
        let task2 = smol::spawn(async {
            println!("Task 2 running");
            100
        });

        let result1 = task1.await;
        let result2 = task2.await;
        println!("Results: {} and {}", result1, result2);
        Ok(())
    })
}
```

## Async TCP Server

Create a simple echo server:

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let listener = net::TcpListener::bind("127.0.0.1:8080").await?;
        println!("Listening on port 8080");

        loop {
            let (mut stream, addr) = listener.accept().await?;
            println!("New connection from: {}", addr);
            
            smol::spawn(async move {
                let mut buf = vec![0u8; 1024];
                while let Ok(n) = stream.read(&mut buf).await {
                    if n == 0 { break; }
                    stream.write_all(&buf[..n]).await.unwrap();
                }
            }).detach();
        }
    })
}
```

## Using Channels

Communicate between tasks with async channels:

```rust
use smol::{channel, prelude::*};

fn main() {
    smol::block_on(async {
        let (sender, receiver) = channel::unbounded::<i32>();
        
        let producer = smol::spawn(async move {
            for i in 0..5 {
                sender.send(i).await.unwrap();
                println!("Sent: {}", i);
            }
        });

        let consumer = smol::spawn(async move {
            while let Ok(msg) = receiver.recv().await {
                println!("Received: {}", msg);
            }
        });

        producer.await;
        consumer.await;
    })
}
```

## Async Timer

Use timers for delays and timeouts:

```rust
use smol::{io, Timer};
use std::time::Duration;

fn main() -> io::Result<()> {
    smol::block_on(async {
        println!("Starting timer...");
        Timer::after(Duration::from_secs(2)).await;
        println!("2 seconds elapsed!");
        
        // With timeout
        let result = smol::io::timeout(Duration::from_secs(1), async {
            Timer::after(Duration::from_secs(3)).await;
            Ok::<_, io::Error>("done")
        }).await;
        
        match result {
            Ok(msg) => println!("{}", msg),
            Err(e) => println!("Timeout error: {}", e),
        }
        Ok(())
    })
}
```

## Async DNS Resolution

Resolve hostnames asynchronously:

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let addrs = net::lookup_host("example.com:80").await?;
        for addr in addrs {
            println!("Resolved: {}", addr);
        }
        Ok(())
    })
}
```
```