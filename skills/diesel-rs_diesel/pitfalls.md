# Pitfalls

```cpp
// PITFALL 1: Forgetting to run migrations before queries
// BAD: Query fails because table doesn't exist
let connection = SqliteConnection::establish("test.db")?;
let users = users::table.load::<User>(&connection)?; // Error: table not found

// GOOD: Run migrations first
let connection = SqliteConnection::establish("test.db")?;
diesel::run_pending_migrations(&connection)?;
let users = users::table.load::<User>(&connection)?;

// PITFALL 2: Using wrong connection type for database
// BAD: SQLite connection for PostgreSQL
let connection = SqliteConnection::establish("postgres://localhost/db")?; // Runtime error

// GOOD: Use correct connection type
let connection = PgConnection::establish("postgres://localhost/db")?;

// PITFALL 3: Not handling nullable columns properly
// BAD: Assuming non-null when column is nullable
let users = users::table.select(users::email).load::<String>(&connection)?; // Error

// GOOD: Use Option for nullable columns
let users = users::table.select(users::email).load::<Option<String>>(&connection)?;

// PITFALL 4: Forgetting to import table module
// BAD: Missing use statement
let users = users::table.load::<User>(&connection)?; // Error: cannot find `users`

// GOOD: Import schema module
use schema::users;
let users = users::table.load::<User>(&connection)?;

// PITFALL 5: Using mutable reference for connection incorrectly
// BAD: Trying to use connection after move
let conn = SqliteConnection::establish("test.db")?;
let users = users::table.load::<User>(&conn)?;
drop(conn);
let more_users = users::table.load::<User>(&conn)?; // Error: use after move

// GOOD: Keep connection alive
let conn = SqliteConnection::establish("test.db")?;
let users = users::table.load::<User>(&conn)?;
let more_users = users::table.load::<User>(&conn)?;

// PITFALL 6: Not using transactions for multiple related operations
// BAD: Partial insert on error
diesel::insert_into(users::table).values(&user1).execute(&conn)?;
diesel::insert_into(users::table).values(&user2).execute(&conn)?; // If this fails, user1 is inserted

// GOOD: Wrap in transaction
conn.transaction::<_, diesel::result::Error, _>(|| {
    diesel::insert_into(users::table).values(&user1).execute(&conn)?;
    diesel::insert_into(users::table).values(&user2).execute(&conn)?;
    Ok(())
})?;
```