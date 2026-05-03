# Performance

**Connection pooling:** The `database/sql` package maintains a pool of connections. Setting appropriate pool sizes prevents connection storms and reduces latency. Too few connections cause queueing; too many exhaust database resources.

```go
// Optimized pool settings for typical web service
db.SetMaxOpenConns(25)      // Limit concurrent DB operations
db.SetMaxIdleConns(25)      // Keep connections warm (same as max open)
db.SetConnMaxLifetime(3 * time.Minute) // Prevent connection leaks
db.SetConnMaxIdleTime(1 * time.Minute) // Close idle connections faster
```

**Prepared statements:** Use prepared statements for repeated queries to avoid parsing overhead. The driver caches prepared statements on the connection, so reuse across calls improves performance.

```go
// BAD: Parses query every time
for i := 0; i < 1000; i++ {
    db.Exec("INSERT INTO logs(level, message) VALUES(?, ?)", "INFO", "msg")
}

// GOOD: Prepare once, execute many times
stmt, _ := db.Prepare("INSERT INTO logs(level, message) VALUES(?, ?)")
defer stmt.Close()
for i := 0; i < 1000; i++ {
    stmt.Exec("INFO", "msg")
}
```

**Batch operations:** Use transactions for batch inserts to reduce round-trips. Each `Exec` in a transaction uses the same connection, avoiding connection acquisition overhead.

```go
// BAD: Individual inserts, each acquires/releases connection
for _, user := range users {
    db.Exec("INSERT INTO users(name) VALUES(?)", user.Name)
}

// GOOD: Batch in transaction
tx, _ := db.Begin()
defer tx.Rollback()
stmt, _ := tx.Prepare("INSERT INTO users(name) VALUES(?)")
defer stmt.Close()
for _, user := range users {
    stmt.Exec(user.Name)
}
tx.Commit()
```

**Query optimization:** Select only needed columns and use `LIMIT` to reduce data transfer. Avoid `SELECT *` which transfers all columns even if unused.

```go
// BAD: Transfers all columns
rows, _ := db.Query("SELECT * FROM users WHERE active = 1")

// GOOD: Only transfer needed columns
rows, _ := db.Query("SELECT id, name FROM users WHERE active = 1 LIMIT 100")
```

**Connection reuse:** Minimize connection churn by keeping idle connections alive. Setting `SetMaxIdleConns` to match `SetMaxOpenConns` prevents frequent connect/disconnect cycles.

```go
// BAD: Idle connections closed immediately
db.SetMaxIdleConns(0) // Every query opens new connection

// GOOD: Keep connections warm
db.SetMaxIdleConns(25) // Reuse connections between queries
```