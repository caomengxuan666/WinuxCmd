# Pitfalls

### Pitfall 1: Not setting connection pool limits
```go
// BAD: Default pool has no limits, can exhaust database connections
db, _ := sql.Open("mysql", dsn)
// No pool configuration - defaults to unlimited connections
```

```go
// GOOD: Always configure pool limits
db, _ := sql.Open("mysql", dsn)
db.SetMaxOpenConns(25)      // Limit concurrent connections
db.SetMaxIdleConns(25)      // Keep connections warm
db.SetConnMaxLifetime(3 * time.Minute) // Prevent stale connections
```

### Pitfall 2: Ignoring context deadlines
```go
// BAD: Query can hang indefinitely
rows, err := db.Query("SELECT * FROM large_table")
```

```go
// GOOD: Always use context for timeouts
ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
defer cancel()
rows, err := db.QueryContext(ctx, "SELECT * FROM large_table")
```

### Pitfall 3: Not closing rows after iteration
```go
// BAD: Rows leak connections
rows, _ := db.Query("SELECT id FROM users")
for rows.Next() {
    var id int
    rows.Scan(&id)
}
// rows.Close() never called - connection stays busy
```

```go
// GOOD: Always defer Close
rows, _ := db.Query("SELECT id FROM users")
defer rows.Close()
for rows.Next() {
    var id int
    rows.Scan(&id)
}
```

### Pitfall 4: Forgetting to rollback failed transactions
```go
// BAD: Transaction left open on error
tx, _ := db.Begin()
tx.Exec("UPDATE accounts SET balance = 0")
// If error occurs, tx is never rolled back
```

```go
// GOOD: Always defer rollback
tx, _ := db.Begin()
defer tx.Rollback() // No-op if committed

tx.Exec("UPDATE accounts SET balance = 0")
tx.Commit() // Rollback becomes no-op
```

### Pitfall 5: SQL injection with string concatenation
```go
// BAD: Vulnerable to SQL injection
username := "'; DROP TABLE users; --"
db.Exec("SELECT * FROM users WHERE name = '" + username + "'")
```

```go
// GOOD: Always use parameterized queries
db.Exec("SELECT * FROM users WHERE name = ?", username)
```

### Pitfall 6: Not handling `parseTime` for datetime columns
```go
// BAD: Dates returned as strings
db, _ := sql.Open("mysql", "user:pass@/dbname")
// time.Time columns return as []byte/string
```

```go
// GOOD: Enable time parsing in DSN
db, _ := sql.Open("mysql", "user:pass@/dbname?parseTime=True")
var createdAt time.Time
db.QueryRow("SELECT created_at FROM users LIMIT 1").Scan(&createdAt)
```

### Pitfall 7: Reusing prepared statements across goroutines without synchronization
```go
// BAD: Concurrent Exec on same statement
stmt, _ := db.Prepare("INSERT INTO logs(message) VALUES(?)")
go func() { stmt.Exec("msg1") }()
go func() { stmt.Exec("msg2") }() // Race condition
```

```go
// GOOD: Each goroutine prepares its own statement
go func() {
    stmt, _ := db.Prepare("INSERT INTO logs(message) VALUES(?)")
    defer stmt.Close()
    stmt.Exec("msg1")
}()
go func() {
    stmt, _ := db.Prepare("INSERT INTO logs(message) VALUES(?)")
    defer stmt.Close()
    stmt.Exec("msg2")
}()
```

### Pitfall 8: Ignoring `sql.ErrNoRows` for single-row queries
```go
// BAD: Assumes row always exists
var name string
db.QueryRow("SELECT name FROM users WHERE id = ?", 999).Scan(&name)
fmt.Println(name) // name is zero value, no error check
```

```go
// GOOD: Check for no rows
var name string
err := db.QueryRow("SELECT name FROM users WHERE id = ?", 999).Scan(&name)
if err == sql.ErrNoRows {
    fmt.Println("User not found")
} else if err != nil {
    log.Fatal(err)
}
```