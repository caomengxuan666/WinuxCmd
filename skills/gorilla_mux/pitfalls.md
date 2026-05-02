# Pitfalls

### Pitfall 1: Forgetting to call `next.ServeHTTP()` in middleware
```go
// BAD: Middleware that doesn't call next.ServeHTTP()
func badMiddleware(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        log.Println("Request received")
        // Missing next.ServeHTTP(w, r) - handler chain stops
    })
}

// GOOD: Proper middleware implementation
func goodMiddleware(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        log.Printf("Request: %s %s", r.Method, r.URL.Path)
        next.ServeHTTP(w, r)
    })
}
```

### Pitfall 2: Incorrect path variable regex patterns
```go
// BAD: Regex pattern that doesn't match expected input
r.HandleFunc("/users/{id:[a-z]+}", UserHandler) // Won't match numeric IDs

// GOOD: Proper regex patterns
r.HandleFunc("/users/{id:[0-9]+}", UserHandler) // Numeric IDs only
r.HandleFunc("/users/{username:[a-zA-Z0-9_]+}", UserHandler) // Alphanumeric usernames
```

### Pitfall 3: Route ordering issues
```go
// BAD: Specific route after catch-all
r := mux.NewRouter()
r.PathPrefix("/").Handler(catchAllHandler)
r.HandleFunc("/specific", specificHandler) // Never reached

// GOOD: Specific routes before catch-all
r := mux.NewRouter()
r.HandleFunc("/specific", specificHandler)
r.PathPrefix("/").Handler(catchAllHandler)
```

### Pitfall 4: Missing OPTIONS method for CORS
```go
// BAD: CORS middleware without OPTIONS handler
r := mux.NewRouter()
r.Use(corsMiddleware)
r.HandleFunc("/api/data", DataHandler).Methods("GET")

// GOOD: Include OPTIONS method
r := mux.NewRouter()
r.Use(corsMiddleware)
r.HandleFunc("/api/data", DataHandler).Methods("GET", "OPTIONS")
```

### Pitfall 5: Not handling StrictSlash properly
```go
// BAD: Inconsistent trailing slash behavior
r := mux.NewRouter()
r.HandleFunc("/products", ProductsHandler) // Matches /products but not /products/

// GOOD: Use StrictSlash for consistency
r := mux.NewRouter()
r.StrictSlash(true)
r.HandleFunc("/products", ProductsHandler) // Matches both /products and /products/
```

### Pitfall 6: Subrouter path prefix confusion
```go
// BAD: Incorrect subrouter path handling
r := mux.NewRouter()
s := r.PathPrefix("/api").Subrouter()
s.HandleFunc("/users", UsersHandler) // Matches /api/users, not /users

// GOOD: Clear subrouter path structure
r := mux.NewRouter()
s := r.PathPrefix("/api/v1").Subrouter()
s.HandleFunc("/users", UsersHandler) // Matches /api/v1/users
```