# Threading

```markdown
# Thread Safety in smol

## Single-Threaded Executor

By default, smol runs on a single thread. This is safe for most use cases:

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    // Single-threaded execution
    smol::block_on(async {
        let task1 = smol::spawn(async {
            println!("Task 1 on thread: {:?}", std::thread::current().id());
        });
        
        let task2 = smol::spawn(async {
            println!("Task 2 on thread: {:?}", std::thread::current().id());
        });
        
        task1.await;
        task2.await;
        Ok(())
    })
}
```

## Multi-Threaded Execution

Use `smol_macros` or manual executor setup for multi-threaded execution:

```rust
use smol::{io, prelude::*};
use std::sync::Arc;

fn main() -> io::Result<()> {
    // Create a multi-threaded executor
    let executor = Arc::new(smol::Executor::new());
    
    // Spawn work on multiple threads
    let mut handles = Vec::new();
    for _ in 0..4 {
        let ex = executor.clone();
        handles.push(std::thread::spawn(move || {
            smol::block_on(async {
                ex.run(async {
                    println!("Running on thread: {:?}", std::thread::current().id());
                }).await;
            })
        }));
    }
    
    // Spawn tasks on the executor
    executor.spawn(async {
        println!("Task on executor thread: {:?}", std::thread::current().id());
    }).detach();
    
    for handle in handles {
        handle.join().unwrap();
    }
    Ok(())
}
```

## Thread-Safe Types

Use `Arc` for shared state across threads:

```rust
use smol::{io, lock, prelude::*};
use std::sync::Arc;

fn main() -> io::Result<()> {
    smol::block_on(async {
        // Arc for shared ownership across threads
        let counter = Arc::new(lock::Mutex::new(0));
        
        let mut tasks = Vec::new();
        for _ in 0..10 {
            let counter = counter.clone();
            tasks.push(smol::spawn(async move {
                let mut guard = counter.lock().await;
                *guard += 1;
            }));
        }
        
        for task in tasks {
            task.await;
        }
        
        println!("Final count: {}", *counter.lock().await);
        Ok(())
    })
}
```

## Send and Sync Requirements

Ensure types are `Send` when used across threads:

```rust
use smol::{io, net, prelude::*};

// BAD: Non-Send type in async context
struct NonSend {
    data: std::rc::Rc<i32>, // Rc is not Send
}

async fn bad_example() {
    let non_send = NonSend { data: std::rc::Rc::new(42) };
    // This would fail to compile if used across threads
    smol::spawn(async move {
        println!("{}", non_send.data);
    }).await;
}

// GOOD: Use Send types
struct SendType {
    data: std::sync::Arc<i32>, // Arc is Send
}

async fn good_example() {
    let send_type = SendType { data: std::sync::Arc::new(42) };
    smol::spawn(async move {
        println!("{}", send_type.data);
    }).await;
}

fn main() -> io::Result<()> {
    smol::block_on(good_example());
    Ok(())
}
```

## Concurrent Access Patterns

Use appropriate synchronization primitives:

```rust
use smol::{io, lock, prelude::*};
use std::sync::Arc;

fn main() -> io::Result<()> {
    smol::block_on(async {
        // Mutex for exclusive access
        let mutex = Arc::new(lock::Mutex::new(Vec::new()));
        
        let mut tasks = Vec::new();
        for i in 0..5 {
            let m = mutex.clone();
            tasks.push(smol::spawn(async move {
                let mut guard = m.lock().await;
                guard.push(i);
            }));
        }
        
        for task in tasks {
            task.await;
        }
        
        println!("Mutex contents: {:?}", *mutex.lock().await);
        
        // RwLock for read-heavy workloads
        let rwlock = Arc::new(lock::RwLock::new(0));
        
        let readers: Vec<_> = (0..3).map(|_| {
            let r = rwlock.clone();
            smol::spawn(async move {
                let guard = r.read().await;
                println!("Read: {}", *guard);
            })
        }).collect();
        
        let writer = {
            let w = rwlock.clone();
            smol::spawn(async move {
                let mut guard = w.write().await;
                *guard += 1;
            })
        };
        
        for reader in readers {
            reader.await;
        }
        writer.await;
        
        Ok(())
    })
}
```

## Thread Pool for Blocking Operations

Use `spawn_blocking` for CPU-intensive or blocking work:

```rust
use smol::{io, prelude::*};

fn main() -> io::Result<()> {
    smol::block_on(async {
        let mut tasks = Vec::new();
        
        // Offload CPU-intensive work to thread pool
        for i in 0..5 {
            tasks.push(smol::spawn_blocking(move || {
                // This runs on a dedicated thread pool
                let result = (0..1000000).filter(|x| x % i == 0).count();
                format!("Thread {}: {} numbers divisible by {}", 
                    std::thread::current().name().unwrap_or("unknown"),
                    result, i)
            }));
        }
        
        for task in tasks {
            println!("{}", task.await);
        }
        
        Ok(())
    })
}
```

## Avoiding Data Races

Use proper synchronization to avoid data races:

```rust
use smol::{io, lock, prelude::*};
use std::sync::atomic::{AtomicI32, Ordering};
use std::sync::Arc;

fn main() -> io::Result<()> {
    smol::block_on(async {
        // Use atomics for simple counters
        let counter = Arc::new(AtomicI32::new(0));
        
        let mut tasks = Vec::new();
        for _ in 0..10 {
            let c = counter.clone();
            tasks.push(smol::spawn(async move {
                c.fetch_add(1, Ordering::SeqCst);
            }));
        }
        
        for task in tasks {
            task.await;
        }
        
        println!("Atomic counter: {}", counter.load(Ordering::SeqCst));
        
        // Use mutex for complex state
        let state = Arc::new(lock::Mutex::new(String::new()));
        
        let mut tasks = Vec::new();
        for i in 0..5 {
            let s = state.clone();
            tasks.push(smol::spawn(async move {
                let mut guard = s.lock().await;
                guard.push_str(&format!("Task {}; ", i));
            }));
        }
        
        for task in tasks {
            task.await;
        }
        
        println!("State: {}", *state.lock().await);
        Ok(())
    })
}
```
```