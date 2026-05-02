# Lifecycle

```markdown
# Lifecycle Management in smol

## Construction

Create async resources and tasks properly:

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // Construct a TCP stream
        let stream = net::TcpStream::connect("example.com:80").await?;
        println!("Connected to: {}", stream.peer_addr()?);
        
        // Construct a file
        let file = smol::fs::File::create("test.txt").await?;
        println!("File created");
        
        // Construct a channel
        let (tx, rx) = smol::channel::bounded::<i32>(10);
        println!("Channel created with capacity 10");
        
        Ok(())
    })
}
```

## Destruction and Cleanup

Resources are automatically cleaned up when dropped:

```rust
use smol::{io, net, prelude::*};

async fn handle_connection() -> io::Result<()> {
    let mut stream = net::TcpStream::connect("example.com:80").await?;
    stream.write_all(b"GET / HTTP/1.1\r\n\r\n").await?;
    
    let mut response = String::new();
    stream.read_to_string(&mut response).await?;
    println!("{}", &response[..100]);
    // stream is automatically closed when dropped
    Ok(())
}

fn main() -> io::Result<()> {
    smol::block_on(handle_connection())
}
```

## Move Semantics

Async closures and tasks often require owned data:

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let urls = vec![
            "example.com:80",
            "google.com:80",
            "rust-lang.org:80",
        ];
        
        // Move the vector into the async block
        let tasks: Vec<_> = urls.into_iter().map(|url| {
            smol::spawn(async move {
                match net::TcpStream::connect(url).await {
                    Ok(_) => println!("Connected to {}", url),
                    Err(e) => eprintln!("Failed to connect to {}: {}", url, e),
                }
            })
        }).collect();
        
        // Wait for all tasks
        for task in tasks {
            task.await;
        }
        Ok(())
    })
}
```

## Resource Management with Scopes

Use scopes to manage resource lifetimes:

```rust
use smol::{io, net, prelude::*};

async fn process_connection(stream: net::TcpStream) -> io::Result<()> {
    let mut buf = vec![0u8; 1024];
    let n = stream.read(&mut buf).await?;
    println!("Received {} bytes", n);
    Ok(())
}

fn main() -> io::Result<()> {
    smol::block_on(async {
        let listener = net::TcpListener::bind("127.0.0.1:0").await?;
        let addr = listener.local_addr()?;
        println!("Listening on {}", addr);
        
        // Scope for connection handling
        {
            let (stream, _) = listener.accept().await?;
            process_connection(stream).await?;
        } // stream is dropped here
        
        // Scope for file operations
        {
            let mut file = smol::fs::File::create("output.txt").await?;
            file.write_all(b"Hello, world!").await?;
        } // file is flushed and closed here
        
        Ok(())
    })
}
```

## Task Lifecycle Management

Handle task lifecycle properly:

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // Spawn a task and get a JoinHandle
        let handle = smol::spawn(async {
            smol::Timer::after(std::time::Duration::from_secs(1)).await;
            42
        });
        
        // Check if task is completed
        if handle.is_finished() {
            println!("Task already completed");
        }
        
        // Await the result
        let result = handle.await;
        println!("Task returned: {}", result);
        
        // Detach a task (fire and forget)
        smol::spawn(async {
            println!("Background task running");
            smol::Timer::after(std::time::Duration::from_secs(5)).await;
            println!("Background task done");
        }).detach();
        
        Ok(())
    })
}
```

## Resource Pooling

Create and manage resource pools:

```rust
use smol::{io, net, prelude::*};
use std::collections::VecDeque;

struct ConnectionPool {
    connections: VecDeque<net::TcpStream>,
    max_size: usize,
}

impl ConnectionPool {
    fn new(max_size: usize) -> Self {
        ConnectionPool {
            connections: VecDeque::new(),
            max_size,
        }
    }
    
    async fn acquire(&mut self, addr: &str) -> io::Result<net::TcpStream> {
        if let Some(conn) = self.connections.pop_front() {
            return Ok(conn);
        }
        net::TcpStream::connect(addr).await
    }
    
    fn release(&mut self, conn: net::TcpStream) {
        if self.connections.len() < self.max_size {
            self.connections.push_back(conn);
        }
        // Otherwise, conn is dropped and closed
    }
}

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mut pool = ConnectionPool::new(5);
        
        let mut conn = pool.acquire("example.com:80").await?;
        conn.write_all(b"GET / HTTP/1.1\r\n\r\n").await?;
        
        // Return to pool
        pool.release(conn);
        Ok(())
    })
}
```
```