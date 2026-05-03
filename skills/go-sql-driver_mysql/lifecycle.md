# Lifecycle

**Construction:** Create a `*sql.DB` instance using `sql.Open()`. This does NOT establish a connection; it only validates the DSN format and initializes the connection pool. Actual connections are created lazily on first query or `Ping()`.

```go
// Construction - no connection yet
db, err := sql.Open("mysql", "user:pass@tcp(localhost:3306)/dbname?parseTime=True")
if err != nil {
    log.Fatal(err) // Only fails if driver not registered
}

// First actual connection attempt
if err := db.Ping(); err != nil {
    log.Fatal("Failed to connect:", err)
}
```

**Resource management:** The `*sql.DB` manages a pool of connections. Connections are created on demand and returned to the pool after use. The pool handles connection lifecycle including reconnection on failure.

```go
// Connections are recycled automatically
func QueryUsers(db *sql.DB) {
    // Gets a connection from pool (or creates new one)
    rows, err := db.Query("SELECT id FROM users")
    if err != nil {
        log.Fatal(err)
    }
    defer rows.Close() // Returns connection to pool
    
    for rows.Next() {
        var id int
        rows.Scan(&id)
    }
    // Connection returned to pool when rows.Close() is called
}
```

**Destruction:** Call `db.Close()` to shut down the connection pool. This closes all idle connections and waits for in-use connections to be returned. After closing, the `*sql.DB` cannot be reused.

```go
// Clean shutdown
func Shutdown() {
    // Close all connections gracefully
    if err := db.Close(); err != nil {
        log.Printf("Error closing database: %v", err)
    }
}

// db is now unusable - any query will return error
```

**Move semantics:** Go's `*sql.DB` is a pointer type and can be safely passed by value (the pointer is copied). There's no move semantics needed; just pass the `*sql.DB` pointer around your application.

```go
// Safe to pass *sql.DB by value (it's a pointer)
func NewRepository(db *sql.DB) *Repository {
    return &Repository{db: db}
}

// Multiple goroutines can share the same *sql.DB
func Worker(db *sql.DB, id int) {
    db.Query("SELECT ...") // Thread-safe
}
```