# Threading

### Thread Safety Guarantees
```go
// Router is NOT safe for concurrent writes (route registration)
// Router IS safe for concurrent reads (request handling)

// BAD: Concurrent route registration
r := mux.NewRouter()
go func() { r.HandleFunc("/route1", handler1) }()
go func() { r.HandleFunc("/route2", handler2) }()

// GOOD: Single-threaded route setup
r := mux.NewRouter()
r.HandleFunc("/route1", handler1)
r.HandleFunc("/route2", handler2)
```

### Concurrent Request Handling
```go
// Multiple requests can be handled concurrently
func concurrentHandler(w http.ResponseWriter, r *http.Request) {
    vars := mux.Vars(r)
    
    // Each request gets its own vars map
    go processAsync(vars["id"]) // Safe - vars are request-scoped
}

// Safe concurrent access pattern
type SafeRouter struct {
    mu     sync.RWMutex
    router *mux.Router
}

func (sr *SafeRouter) HandleFunc(path string, f func(http.ResponseWriter, *http.Request)) {
    sr.mu.Lock()
    defer sr.mu.Unlock()
    sr.router.HandleFunc(path, f)
}

func (sr *SafeRouter) ServeHTTP(w http.ResponseWriter, r *http.Request) {
    sr.mu.RLock()
    defer sr.mu.RUnlock()
    sr.router.ServeHTTP(w, r)
}
```

### Middleware Thread Safety
```go
// Middleware must be thread-safe
func safeMiddleware(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        // BAD: Using shared state without synchronization
        sharedCounter++
        
        // GOOD: Use request-scoped data
        requestID := generateRequestID()
        ctx := context.WithValue(r.Context(), "requestID", requestID)
        next.ServeHTTP(w, r.WithContext(ctx))
    })
}
```