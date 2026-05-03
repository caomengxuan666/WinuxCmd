# Best Practices

**Singleton database instance:** Open the database connection once during application startup and reuse the `*sql.DB` instance throughout the application lifecycle. The `database/sql` package manages the connection pool internally, so creating multiple `*sql.DB` instances wastes resources.

```go
var db *sql.DB

func InitDB(dsn string) error {
    var err error
    db, err = sql.Open("mysql", dsn)
    if err != nil {
        return err
    }
    
    db.SetMaxOpenConns(25)
    db.SetMaxIdleConns(25)
    db.SetConnMaxLifetime(3 * time.Minute)
    db.SetConnMaxIdleTime(1 * time.Minute)
    
    return db.Ping()
}
```

**Context propagation:** Always pass context through your application layers. Use `context.Background()` only at the top level, and derive timeouts for each database operation.

```go
func GetUser(ctx context.Context, id int) (*User, error) {
    ctx, cancel := context.WithTimeout(ctx, 5*time.Second)
    defer cancel()
    
    user := &User{}
    err := db.QueryRowContext(ctx, 
        "SELECT id, name, email FROM users WHERE id = ?", id).
        Scan(&user.ID, &user.Name, &user.Email)
    if err != nil {
        return nil, err
    }
    return user, nil
}
```

**Transaction patterns:** Use explicit transactions for operations that must be atomic. Always defer a rollback to handle errors gracefully.

```go
func TransferFunds(ctx context.Context, from, to int, amount float64) error {
    tx, err := db.BeginTx(ctx, nil)
    if err != nil {
        return err
    }
    defer tx.Rollback() // No-op if committed
    
    _, err = tx.ExecContext(ctx, 
        "UPDATE accounts SET balance = balance - ? WHERE id = ?", amount, from)
    if err != nil {
        return err
    }
    
    _, err = tx.ExecContext(ctx,
        "UPDATE accounts SET balance = balance + ? WHERE id = ?", amount, to)
    if err != nil {
        return err
    }
    
    return tx.Commit()
}
```

**Health checks:** Implement database health checks for monitoring and graceful shutdown.

```go
func HealthCheck(ctx context.Context) error {
    ctx, cancel := context.WithTimeout(ctx, 2*time.Second)
    defer cancel()
    
    return db.PingContext(ctx)
}

func GracefulShutdown() {
    db.Close()
}
```