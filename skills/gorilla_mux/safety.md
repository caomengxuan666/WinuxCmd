# Safety

### Red Line 1: NEVER modify the request context directly
```go
// BAD: Direct context manipulation
func badHandler(w http.ResponseWriter, r *http.Request) {
    ctx := context.WithValue(r.Context(), "user", "admin")
    *r = *r.WithContext(ctx) // Dangerous - breaks mux internals
}

// GOOD: Use mux.Vars() for route variables
func goodHandler(w http.ResponseWriter, r *http.Request) {
    vars := mux.Vars(r)
    user := vars["user"]
}
```

### Red Line 2: NEVER ignore errors from URL building
```go
// BAD: Ignoring URL building errors
url, _ := r.Get("route").URL("id", "123") // Silently ignores errors

// GOOD: Handle URL building errors
url, err := r.Get("route").URL("id", "123")
if err != nil {
    http.Error(w, "Invalid route parameters", http.StatusInternalServerError)
    return
}
```

### Red Line 3: NEVER use the same router instance concurrently without synchronization
```go
// BAD: Concurrent route registration
r := mux.NewRouter()
go func() { r.HandleFunc("/route1", handler1) }()
go func() { r.HandleFunc("/route2", handler2) }() // Race condition

// GOOD: Register all routes before starting server
r := mux.NewRouter()
r.HandleFunc("/route1", handler1)
r.HandleFunc("/route2", handler2)
```

### Red Line 4: NEVER store request references for later use
```go
// BAD: Storing request reference
var storedRequest *http.Request
func badHandler(w http.ResponseWriter, r *http.Request) {
    storedRequest = r // Request becomes invalid after handler returns
}

// GOOD: Extract needed data immediately
func goodHandler(w http.ResponseWriter, r *http.Request) {
    vars := mux.Vars(r)
    userID := vars["id"]
    // Store only the extracted data
}
```

### Red Line 5: NEVER modify route matchers after server starts
```go
// BAD: Modifying routes after server start
r := mux.NewRouter()
go http.ListenAndServe(":8080", r)
time.Sleep(time.Second)
r.HandleFunc("/new-route", handler) // Unsafe - race condition

// GOOD: Complete route setup before server start
r := mux.NewRouter()
r.HandleFunc("/new-route", handler)
http.ListenAndServe(":8080", r)
```