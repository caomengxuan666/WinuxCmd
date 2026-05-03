# Lifecycle

```cpp
// CONSTRUCTION: Establishing database connections
use diesel::prelude::*;
use diesel::sqlite::SqliteConnection;

// Direct connection (simple cases)
let conn = SqliteConnection::establish("test.db")
    .expect("Failed to connect");

// Connection pooling (production)
use diesel::r2d2::{ConnectionManager, Pool};
let manager = ConnectionManager::<SqliteConnection>::new("test.db");
let pool = Pool::builder()
    .max_size(10)
    .build(manager)
    .expect("Failed to create pool");

// DESTRUCTION: Connections are automatically closed when dropped
{
    let conn = pool.get().unwrap();
    // Connection is used here
} // Connection returned to pool automatically

// MOVE SEMANTICS: Connections implement Send but not Sync
fn process_with_connection(conn: SqliteConnection) -> Vec<User> {
    users::table.load::<User>(&conn).unwrap()
}

// Moving connection to thread
let conn = SqliteConnection::establish("test.db").unwrap();
let handle = std::thread::spawn(move || {
    process_with_connection(conn) // conn moved here
});
let users = handle.join().unwrap();

// RESOURCE MANAGEMENT: Using transactions for atomicity
fn transfer_funds(conn: &SqliteConnection, from: i32, to: i32, amount: f64) -> Result<(), diesel::result::Error> {
    conn.transaction::<_, diesel::result::Error, _>(|| {
        diesel::update(accounts::table.find(from))
            .set(accounts::balance.eq(accounts::balance - amount))
            .execute(conn)?;
        
        diesel::update(accounts::table.find(to))
            .set(accounts::balance.eq(accounts::balance + amount))
            .execute(conn)?;
        
        Ok(())
    })
}

// Connection lifecycle with scoped usage
fn query_users() -> Vec<User> {
    let conn = pool.get().expect("Failed to get connection");
    let users = users::table.load::<User>(&conn).expect("Query failed");
    users // conn dropped here, returned to pool
}
```