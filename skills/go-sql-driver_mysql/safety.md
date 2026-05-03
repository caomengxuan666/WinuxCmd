# Safety

### Red-line 1: NEVER use string interpolation for SQL queries
```go
// BAD: SQL injection vulnerability
userInput := "1 OR 1=1"
db.Exec("DELETE FROM users WHERE id = " + userInput)
```

```go
// GOOD: Always use parameterized queries
db.Exec("DELETE FROM users WHERE id = ?", userInput)
```

### Red-line 2: NEVER leave connections without lifetime limits
```go
// BAD: Connections never recycled, can become stale
db, _ := sql.Open("mysql", dsn)
// No SetConnMaxLifetime - connections live forever
```

```go
// GOOD: Always set connection lifetime
db, _ := sql.Open("mysql", dsn)
db.SetConnMaxLifetime(3 * time.Minute) // Must be < 5 min for most proxies
```

### Red-line 3: NEVER ignore errors from `db.Ping()` or `db.PingContext()`
```go
// BAD: Silent connection failure
db, _ := sql.Open("mysql", dsn)
// No ping - application starts even if DB is down
```

```go
// GOOD: Verify connection before use
db, err := sql.Open("mysql", dsn)
if err != nil {
    log.Fatal(err)
}
if err := db.Ping(); err != nil {
    log.Fatal("Database unreachable:", err)
}
```

### Red-line 4: NEVER use `allowAllFiles=true` in production
```go
// BAD: Allows arbitrary file reads from MySQL server
dsn := "user:pass@/dbname?allowAllFiles=true"
db, _ := sql.Open("mysql", dsn)
// Attacker can read /etc/passwd via LOAD DATA LOCAL INFILE
```

```go
// GOOD: Only use allowAllFiles in controlled test environments
// In production, never set allowAllFiles=true
dsn := "user:pass@/dbname"
db, _ := sql.Open("mysql", dsn)
```

### Red-line 5: NEVER close `*sql.DB` in a defer if you reuse it
```go
// BAD: Closing shared database connection
func handler(w http.ResponseWriter, r *http.Request) {
    db, _ := sql.Open("mysql", dsn)
    defer db.Close() // Closes connection for ALL users
    db.Query("SELECT ...")
}
```

```go
// GOOD: Open once, reuse globally
var db *sql.DB

func init() {
    var err error
    db, err = sql.Open("mysql", dsn)
    if err != nil {
        log.Fatal(err)
    }
}

func handler(w http.ResponseWriter, r *http.Request) {
    db.Query("SELECT ...")
}
```