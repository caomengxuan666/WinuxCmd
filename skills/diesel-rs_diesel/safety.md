# Safety

```cpp
// CONDITION 1: NEVER use raw SQL without parameterization
// BAD: SQL injection vulnerability
let name = "'; DROP TABLE users; --";
let query = format!("SELECT * FROM users WHERE name = '{}'", name);
let users = sql_query(query).load::<User>(&conn)?;

// GOOD: Use parameterized queries
let users = users::table.filter(users::name.eq(name)).load::<User>(&conn)?;

// CONDITION 2: NEVER ignore connection establishment errors
// BAD: Unwrap without handling
let conn = SqliteConnection::establish("test.db").unwrap(); // Panics on failure

// GOOD: Handle errors properly
let conn = SqliteConnection::establish("test.db")
    .expect("Failed to connect to database");

// CONDITION 3: NEVER use mutable static connections
// BAD: Global mutable state
static mut CONN: Option<SqliteConnection> = None;
unsafe { CONN = Some(SqliteConnection::establish("test.db").unwrap()); }

// GOOD: Use connection pooling
use diesel::r2d2::{ConnectionManager, Pool};
let manager = ConnectionManager::<SqliteConnection>::new("test.db");
let pool = Pool::builder().build(manager)?;
let conn = pool.get()?;

// CONDITION 4: NEVER execute queries without checking for SQL injection in raw SQL
// BAD: Direct string interpolation
let id = "1 OR 1=1";
let result = sql_query(format!("DELETE FROM users WHERE id = {}", id)).execute(&conn)?;

// GOOD: Use Diesel's query builder or bind parameters
let result = diesel::delete(users::table.find(id)).execute(&conn)?;

// CONDITION 5: NEVER use the same connection across threads without synchronization
// BAD: Shared mutable connection across threads
let conn = SqliteConnection::establish("test.db")?;
thread::spawn(move || {
    let users = users::table.load::<User>(&conn)?; // Error: Send not implemented
});

// GOOD: Use connection pool for thread safety
let pool = Pool::builder().build(manager)?;
for _ in 0..10 {
    let pool = pool.clone();
    thread::spawn(move || {
        let conn = pool.get()?;
        let users = users::table.load::<User>(&conn)?;
    });
}
```