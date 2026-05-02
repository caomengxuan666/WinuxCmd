# Pitfalls

```markdown
# Common Pitfalls with smol

## 1. Blocking the Executor Thread

**BAD**: Performing blocking operations inside async code without using `Unblock` or `spawn_blocking`.

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // BAD: This blocks the entire executor
        std::thread::sleep(std::time::Duration::from_secs(5));
        println!("Done sleeping");
        Ok(())
    })
}
```

**GOOD**: Use `Unblock` or `spawn_blocking` for blocking operations.

```rust
use smol::{io, prelude::*, Unblock};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // GOOD: Offload blocking work to a thread pool
        let result = smol::spawn_blocking(|| {
            std::thread::sleep(std::time::Duration::from_secs(5));
            "Done sleeping"
        }).await;
        println!("{}", result);
        Ok(())
    })
}
```

## 2. Forgetting to `.await` Futures

**BAD**: Creating a future without awaiting it.

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let stream = net::TcpStream::connect("example.com:80");
        // BAD: stream is a Future, not a TcpStream
        // stream.write_all(b"GET / HTTP/1.1\r\n\r\n").await?;
        Ok(())
    })
}
```

**GOOD**: Always await futures to execute them.

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // GOOD: Await the future to get the result
        let mut stream = net::TcpStream::connect("example.com:80").await?;
        stream.write_all(b"GET / HTTP/1.1\r\n\r\n").await?;
        Ok(())
    })
}
```

## 3. Ignoring Task Cancellation

**BAD**: Not handling cancellation properly when tasks are dropped.

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let task = smol::spawn(async {
            loop {
                // BAD: This task will be silently cancelled when dropped
                println!("Running...");
                smol::Timer::after(std::time::Duration::from_secs(1)).await;
            }
        });
        
        // Task is dropped here, cancelling the loop silently
        drop(task);
        Ok(())
    })
}
```

**GOOD**: Use cancellation tokens or channels to handle cleanup.

```rust
use smol::{channel, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let (cancel_tx, cancel_rx) = channel::bounded::<()>(1);
        
        let task = smol::spawn(async move {
            loop {
                // GOOD: Check for cancellation
                if cancel_rx.try_recv().is_ok() {
                    println!("Task cancelled, cleaning up...");
                    break;
                }
                println!("Running...");
                smol::Timer::after(std::time::Duration::from_secs(1)).await;
            }
        });
        
        // Cancel the task properly
        cancel_tx.send(()).await.unwrap();
        task.await;
        Ok(())
    })
}
```

## 4. Deadlocking with Async Mutex

**BAD**: Holding an async mutex across an await point.

```rust
use smol::{io, lock, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mutex = lock::Mutex::new(42);
        
        // BAD: Holding lock across await can cause deadlock
        let guard = mutex.lock().await;
        smol::Timer::after(std::time::Duration::from_secs(1)).await;
        println!("Value: {}", *guard);
        // Lock released here
        Ok(())
    })
}
```

**GOOD**: Minimize the scope of lock guards.

```rust
use smol::{io, lock, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mutex = lock::Mutex::new(42);
        
        // GOOD: Lock only for the critical section
        let value = {
            let guard = mutex.lock().await;
            *guard
        }; // Lock released here
        
        smol::Timer::after(std::time::Duration::from_secs(1)).await;
        println!("Value: {}", value);
        Ok(())
    })
}
```

## 5. Not Handling I/O Errors Properly

**BAD**: Ignoring errors from async I/O operations.

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mut stream = net::TcpStream::connect("example.com:80").await?;
        // BAD: Ignoring write errors
        let _ = stream.write_all(b"GET / HTTP/1.1\r\n\r\n").await;
        Ok(())
    })
}
```

**GOOD**: Handle I/O errors explicitly.

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mut stream = net::TcpStream::connect("example.com:80").await?;
        
        // GOOD: Handle errors properly
        match stream.write_all(b"GET / HTTP/1.1\r\n\r\n").await {
            Ok(_) => println!("Request sent successfully"),
            Err(e) => eprintln!("Failed to send request: {}", e),
        }
        Ok(())
    })
}
```

## 6. Using `block_on` Inside Async Code

**BAD**: Nesting `block_on` calls.

```rust
use smol::{io, prelude::*};

async fn inner_function() -> io::Result<()> {
    // BAD: This will panic or deadlock
    smol::block_on(async {
        println!("Inside nested block_on");
        Ok(())
    })
}

fn main() -> io::Result<()> {
    smol::block_on(async {
        inner_function().await?;
        Ok(())
    })
}
```

**GOOD**: Use async/await consistently.

```rust
use smol::{io, prelude::*};

async fn inner_function() -> io::Result<()> {
    // GOOD: Just await directly
    println!("Inside async function");
    Ok(())
}

fn main() -> io::Result<()> {
    smol::block_on(async {
        inner_function().await?;
        Ok(())
    })
}
```
```