# Threading

```cpp
// THREAD SAFETY GUARANTEES

// 1. Connections are Send but not Sync
// This means connections can be moved between threads but not shared
use std::thread;

let conn = SqliteConnection::establish("test.db").unwrap();

// OK: Move connection to thread
thread::spawn(move || {
    let users = users::table.load::<User>(&conn).unwrap();
}).join().unwrap();

// ERROR: Cannot share connection across threads
// let shared = &conn;
// thread::spawn(move || {
//     let users = users::table.load::<User>(shared).unwrap(); // Compile error
// });

// 2. Use connection pooling for concurrent access
use diesel::r2d2::{ConnectionManager, Pool};

let manager = ConnectionManager::<SqliteConnection>::new("test.db");
let pool = Pool::builder()
    .max_size(10)
    .build(manager)
    .unwrap();

// Spawn multiple threads, each getting their own connection
let mut handles = vec![];
for i in 0..5 {
    let pool = pool.clone();
    handles.push(thread::spawn(move || {
        let conn = pool.get().unwrap();
        let users = users::table.load::<User>(&conn).unwrap();
        println!("Thread {} got {} users", i, users.len());
    }));
}

for handle in handles {
    handle.join().unwrap();
}

// 3. Transactions are thread-local
// Each thread gets its own transaction scope
fn process_in_thread(pool: &DbPool, thread_id: i32) -> Result<(), diesel::result::Error> {
    let conn = pool.get()?;
    conn.transaction::<_, diesel::result::Error, _>(|| {
        diesel::insert_into(users::table)
            .values(&NewUser { name: &format!("user_{}", thread_id) })
            .execute(&conn)?;
        Ok(())
    })
}

// 4. Use Arc for shared pool reference
use std::sync::Arc;

let pool = Arc::new(pool);
let mut handles = vec![];

for i in 0..10 {
    let pool = pool.clone();
    handles.push(thread::spawn(move || {
        let conn = pool.get().unwrap();
        // Perform database operations
        let count = users::table.count().get_result::<i64>(&conn).unwrap();
        println!("Thread {}: {} users", i, count);
    }));
}

// 5. Connection pooling handles backpressure
// When all connections are in use, pool.get() will block
let pool = Pool::builder()
    .max_size(3) // Only 3 concurrent connections
    .build(manager)
    .unwrap();

// This will work fine with 3 threads
for _ in 0..3 {
    let pool = pool.clone();
    thread::spawn(move || {
        let conn = pool.get().unwrap();
        thread::sleep(Duration::from_secs(1));
    });
}

// This thread will wait until a connection is available
let conn = pool.get().unwrap(); // May block
```