# Safety

```markdown
# Safety Guidelines for smol

## Red-Line Condition 1: Never Block the Async Executor

**NEVER** perform blocking operations (like `std::thread::sleep`, synchronous I/O, or CPU-intensive computations) inside async code without offloading them.

**BAD**: Blocking the executor thread.

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // RED LINE: This blocks the entire executor
        std::thread::sleep(std::time::Duration::from_secs(10));
        println!("Done");
        Ok(())
    })
}
```

**GOOD**: Offload blocking work to the thread pool.

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // SAFE: Offload to thread pool
        let result = smol::spawn_blocking(|| {
            std::thread::sleep(std::time::Duration::from_secs(10));
            "Done"
        }).await;
        println!("{}", result);
        Ok(())
    })
}
```

## Red-Line Condition 2: Never Hold Async Mutex Across Await Points

**NEVER** hold an async mutex lock while awaiting another future, as this can cause deadlocks.

**BAD**: Holding lock across await.

```rust
use smol::{io, lock, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mutex = lock::Mutex::new(0);
        let mut guard = mutex.lock().await;
        // RED LINE: Holding lock while awaiting
        smol::Timer::after(std::time::Duration::from_secs(1)).await;
        *guard += 1;
        Ok(())
    })
}
```

**GOOD**: Minimize lock scope.

```rust
use smol::{io, lock, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mutex = lock::Mutex::new(0);
        let value = {
            let mut guard = mutex.lock().await;
            *guard += 1;
            *guard
        }; // Lock released here
        // SAFE: No lock held during await
        smol::Timer::after(std::time::Duration::from_secs(1)).await;
        println!("Value: {}", value);
        Ok(())
    })
}
```

## Red-Line Condition 3: Never Nest `block_on` Calls

**NEVER** call `smol::block_on` from within an async context.

**BAD**: Nested block_on.

```rust
use smol::{io, prelude::*};

async fn nested() -> io::Result<()> {
    // RED LINE: This will panic
    smol::block_on(async {
        println!("Nested");
        Ok(())
    })
}

fn main() -> io::Result<()> {
    smol::block_on(async {
        nested().await?;
        Ok(())
    })
}
```

**GOOD**: Use async/await consistently.

```rust
use smol::{io, prelude::*};

async fn nested() -> io::Result<()> {
    // SAFE: Just await directly
    println!("Nested");
    Ok(())
}

fn main() -> io::Result<()> {
    smol::block_on(async {
        nested().await?;
        Ok(())
    })
}
```

## Red-Line Condition 4: Never Ignore Task Panics

**NEVER** spawn tasks without handling potential panics.

**BAD**: Ignoring task panics.

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // RED LINE: Panic will be silently ignored
        smol::spawn(async {
            panic!("Task panicked!");
        }).detach();
        
        smol::Timer::after(std::time::Duration::from_secs(1)).await;
        Ok(())
    })
}
```

**GOOD**: Handle task panics.

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        // SAFE: Await and handle the result
        let result = smol::spawn(async {
            if true {
                panic!("Task panicked!");
            }
            42
        }).await;
        
        match result {
            Ok(val) => println!("Task completed: {}", val),
            Err(e) => eprintln!("Task panicked: {:?}", e),
        }
        Ok(())
    })
}
```

## Red-Line Condition 5: Never Use Uninitialized or Dangling Resources

**NEVER** use async resources after they have been dropped or are in an invalid state.

**BAD**: Using a dropped stream.

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let stream = net::TcpStream::connect("example.com:80").await?;
        drop(stream);
        // RED LINE: Using dropped stream
        stream.write_all(b"GET / HTTP/1.1\r\n\r\n").await?;
        Ok(())
    })
}
```

**GOOD**: Ensure resources are valid before use.

```rust
use smol::{io, net, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mut stream = net::TcpStream::connect("example.com:80").await?;
        
        // SAFE: Check stream is valid before use
        if !stream.peer_addr().is_ok() {
            return Err(io::Error::new(io::ErrorKind::NotConnected, "Stream not connected"));
        }
        
        stream.write_all(b"GET / HTTP/1.1\r\n\r\n").await?;
        Ok(())
    })
}
```
```