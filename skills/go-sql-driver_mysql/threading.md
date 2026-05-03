# Threading

**Thread safety guarantees:** The `*sql.DB` object is safe for concurrent use by multiple goroutines. The `database/sql` package internally manages a mutex-protected connection pool. However, individual `*sql.Rows`, `*sql.Stmt`, and `*sql.Tx` objects are NOT safe for concurrent use.

```go
// SAFE: Multiple goroutines can use *sql.DB concurrently
func worker(id int, db *sql.DB) {
    var count int
    db.QueryRow("SELECT COUNT(*) FROM users").Scan(&count)
    fmt.Printf("Worker %d: %d users\n", id, count)
}

func main() {
    db, _ := sql.Open("mysql", dsn)
    for i := 0; i < 10; i++ {
        go worker(i, db)
    }
    time.Sleep(time.Second)
}
```

**Prepared statement concurrency:** Each `*sql.Stmt` is safe for concurrent use, but creating one per goroutine is more efficient than sharing due to internal locking.

```go
// ACCEPTABLE but slower due to mutex contention
stmt, _ := db.Prepare("SELECT name FROM users WHERE id = ?")
defer stmt.Close()

var wg sync.WaitGroup
for i := 0; i < 100; i++ {
    wg.Add(1)
    go func(id int) {
        defer wg.Done()
        var name string
        stmt.QueryRow(id).Scan(&name) // Mutex contention here
    }(i)
}
wg.Wait()

// BETTER: Each goroutine prepares its own statement
var wg sync.WaitGroup
for i := 0; i < 100; i++ {
    wg.Add(1)
    go func(id int) {
        defer wg.Done()
        stmt, _ := db.Prepare("SELECT name FROM users WHERE id = ?")
        defer stmt.Close()
        var name string
        stmt.QueryRow(id).Scan(&name)
    }(i)
}
wg.Wait()
```

**Transaction isolation:** Transactions are bound to a single connection. Do not share a `*sql.Tx` across goroutines; each goroutine should create its own transaction.

```go
// BAD: Sharing transaction across goroutines
tx, _ := db.Begin()
go func() {
    tx.Exec("UPDATE accounts SET balance = 0") // Race condition
}()
go func() {
    tx.Exec("UPDATE accounts SET balance = 100") // Race condition
}()

// GOOD: Each goroutine has its own transaction
var wg sync.WaitGroup
for i := 0; i < 2; i++ {
    wg.Add(1)
    go func(id int) {
        defer wg.Done()
        tx, _ := db.Begin()
        defer tx.Rollback()
        tx.Exec("UPDATE accounts SET balance = ? WHERE id = ?", 100*id, id)
        tx.Commit()
    }(i)
}
wg.Wait()
```

**Connection pool behavior:** The pool is shared across goroutines. When all connections are in use, goroutines block waiting for a free connection. This can cause deadlocks if transactions hold connections while waiting for other goroutines.

```go
// POTENTIAL DEADLOCK: All connections held by transactions
db.SetMaxOpenConns(2) // Only 2 connections available

for i := 0; i < 3; i++ {
    go func() {
        tx, _ := db.Begin() // Goroutine 3 blocks here
        defer tx.Rollback()
        time.Sleep(time.Second) // Holds connection
        tx.Commit()
    }()
}

// SOLUTION: Use context with timeout
ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
defer cancel()
tx, err := db.BeginTx(ctx, nil) // Will timeout if no connection available
```