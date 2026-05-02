# Lifecycle

### Construction
```go
// Basic router creation
r := mux.NewRouter()

// Router with options
r := &mux.Router{
    StrictSlash: true,
    SkipClean:   false,
    UseEncodedPath: false,
}

// Subrouter creation from parent
parent := mux.NewRouter()
child := parent.PathPrefix("/api").Subrouter()
```

### Resource Management
```go
// Router cleanup - routers don't hold external resources
// They can be safely garbage collected when no longer referenced
func createRouter() *mux.Router {
    r := mux.NewRouter()
    r.HandleFunc("/", handler)
    return r // Safe to return, no cleanup needed
}

// Server lifecycle with router
func StartServer(addr string) *http.Server {
    r := mux.NewRouter()
    // Setup routes...
    
    srv := &http.Server{
        Handler:      r,
        Addr:         addr,
        ReadTimeout:  15 * time.Second,
        WriteTimeout: 15 * time.Second,
    }
    
    go func() {
        if err := srv.ListenAndServe(); err != nil {
            log.Printf("Server error: %v", err)
        }
    }()
    
    return srv
}
```

### Move Semantics (Not applicable - Go uses pointers)
```go
// In Go, routers are used via pointers
// Copying a router creates a shallow copy
r1 := mux.NewRouter()
r2 := *r1 // Shallow copy - not recommended

// Always use pointers
r := mux.NewRouter()
someFunction(r) // Pass by pointer
```