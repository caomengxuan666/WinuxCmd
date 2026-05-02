# Best Practices

```markdown
# Best Practices for smol

## 1. Structure Your Application

Organize async code into clear layers:

```rust
use smol::{io, net, prelude::*};

// Service layer - business logic
async fn fetch_website(url: &str) -> io::Result<String> {
    let mut stream = net::TcpStream::connect(url).await?;
    let request = format!("GET / HTTP/1.1\r\nHost: {}\r\nConnection: close\r\n\r\n", url);
    stream.write_all(request.as_bytes()).await?;
    
    let mut response = String::new();
    stream.read_to_string(&mut response).await?;
    Ok(response)
}

// Application layer - orchestration
async fn run_app() -> io::Result<()> {
    let content = fetch_website("example.com:80").await?;
    println!("{}", &content[..200]);
    Ok(())
}

fn main() -> io::Result<()> {
    smol::block_on(run_app())
}
```

## 2. Use Structured Concurrency

Group related tasks and handle their results together:

```rust
use smol::{io, prelude::*};

async fn process_data(id: u32) -> io::Result<String> {
    smol::Timer::after(std::time::Duration::from_millis(100 * id as u64)).await;
    Ok(format!("Processed task {}", id))
}

fn main() -> io::Result<()> {
    smol::block_on(async {
        // Spawn multiple tasks and collect results
        let tasks: Vec<_> = (0..5).map(|i| {
            smol::spawn(process_data(i))
        }).collect();

        // Await all tasks and handle errors
        for task in tasks {
            match task.await {
                Ok(result) => println!("{}", result),
                Err(e) => eprintln!("Task failed: {}", e),
            }
        }
        Ok(())
    })
}
```

## 3. Implement Proper Error Handling

Use custom error types for better error management:

```rust
use smol::{io, net, prelude::*};
use std::fmt;

#[derive(Debug)]
enum AppError {
    Io(io::Error),
    Network(String),
    Timeout,
}

impl fmt::Display for AppError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            AppError::Io(e) => write!(f, "I/O error: {}", e),
            AppError::Network(msg) => write!(f, "Network error: {}", msg),
            AppError::Timeout => write!(f, "Operation timed out"),
        }
    }
}

impl From<io::Error> for AppError {
    fn from(e: io::Error) -> Self {
        AppError::Io(e)
    }
}

async fn fetch_with_timeout(url: &str) -> Result<String, AppError> {
    let result = io::timeout(std::time::Duration::from_secs(5), async {
        let mut stream = net::TcpStream::connect(url).await?;
        let request = format!("GET / HTTP/1.1\r\nHost: {}\r\nConnection: close\r\n\r\n", url);
        stream.write_all(request.as_bytes()).await?;
        
        let mut response = String::new();
        stream.read_to_string(&mut response).await?;
        Ok(response)
    }).await;

    match result {
        Ok(content) => Ok(content),
        Err(e) if e.kind() == io::ErrorKind::TimedOut => Err(AppError::Timeout),
        Err(e) => Err(AppError::Io(e)),
    }
}

fn main() -> Result<(), AppError> {
    smol::block_on(async {
        match fetch_with_timeout("example.com:80").await {
            Ok(content) => println!("Success: {} bytes", content.len()),
            Err(e) => eprintln!("Error: {}", e),
        }
        Ok(())
    })
}
```

## 4. Use Channels for Task Communication

Prefer channels over shared state for communication between tasks:

```rust
use smol::{channel, prelude::*};

#[derive(Debug)]
enum Command {
    Increment,
    Decrement,
    GetValue,
}

async fn counter_actor(receiver: channel::Receiver<Command>, sender: channel::Sender<i32>) {
    let mut count = 0;
    while let Ok(cmd) = receiver.recv().await {
        match cmd {
            Command::Increment => count += 1,
            Command::Decrement => count -= 1,
            Command::GetValue => {
                let _ = sender.send(count).await;
            }
        }
    }
}

fn main() {
    smol::block_on(async {
        let (cmd_sender, cmd_receiver) = channel::unbounded::<Command>();
        let (value_sender, value_receiver) = channel::bounded::<i32>(1);
        
        smol::spawn(counter_actor(cmd_receiver, value_sender)).detach();
        
        cmd_sender.send(Command::Increment).await.unwrap();
        cmd_sender.send(Command::Increment).await.unwrap();
        cmd_sender.send(Command::GetValue).await.unwrap();
        
        let value = value_receiver.recv().await.unwrap();
        println!("Counter value: {}", value);
    })
}
```

## 5. Use Buffered I/O for Performance

Wrap streams in buffered readers/writers for better performance:

```rust
use smol::{io, net, prelude::*};
use std::io::BufReader;

fn main() -> io::Result<()> {
    smol::block_on(async {
        let stream = net::TcpStream::connect("example.com:80").await?;
        let mut reader = io::BufReader::new(stream);
        
        let request = b"GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
        reader.get_mut().write_all(request).await?;
        
        let mut response = String::new();
        reader.read_to_string(&mut response).await?;
        println!("Response: {} bytes", response.len());
        Ok(())
    })
}
```

## 6. Implement Graceful Shutdown

Handle shutdown signals properly:

```rust
use smol::{io, prelude::*, signal};
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;

fn main() -> io::Result<()> {
    smol::block_on(async {
        let running = Arc::new(AtomicBool::new(true));
        let r = running.clone();
        
        // Handle Ctrl+C
        let shutdown_task = smol::spawn(async move {
            signal::ctrl_c().await.unwrap();
            println!("Shutdown signal received");
            r.store(false, Ordering::SeqCst);
        });
        
        // Main work loop
        let work_task = smol::spawn(async move {
            while running.load(Ordering::SeqCst) {
                println!("Working...");
                smol::Timer::after(std::time::Duration::from_secs(1)).await;
            }
            println!("Shutting down gracefully");
        });
        
        shutdown_task.await;
        work_task.await;
        Ok(())
    })
}
```
```