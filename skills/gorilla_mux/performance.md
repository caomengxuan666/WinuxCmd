# Performance

### Performance Characteristics
```go
// Route matching complexity: O(n) where n is number of routes
// For optimal performance, organize routes by specificity

// BAD: Linear search through all routes
r.HandleFunc("/{category}/{id:[0-9]+}", GenericHandler)
r.HandleFunc("/products/{id:[0-9]+}", ProductHandler)
r.HandleFunc("/products/special", SpecialProductHandler)

// GOOD: Most specific routes first
r.HandleFunc("/products/special", SpecialProductHandler)
r.HandleFunc("/products/{id:[0-9]+}", ProductHandler)
r.HandleFunc("/{category}/{id:[0-9]+}", GenericHandler)
```

### Allocation Patterns
```go
// Minimize allocations in hot paths
func efficientHandler(w http.ResponseWriter, r *http.Request) {
    // BAD: Creating new map each request
    vars := make(map[string]string)
    vars["id"] = mux.Vars(r)["id"]
    
    // GOOD: Use mux.Vars() directly
    id := mux.Vars(r)["id"]
}
```

### Optimization Tips
```go
// Use subrouters to reduce matching time
func optimizedRouter() *mux.Router {
    r := mux.NewRouter()
    
    // Group related routes under subrouters
    api := r.PathPrefix("/api").Subrouter()
    api.HandleFunc("/users", UsersHandler)
    api.HandleFunc("/products", ProductsHandler)
    
    // Static routes don't need regex matching
    r.HandleFunc("/health", HealthHandler)
    r.HandleFunc("/version", VersionHandler)
    
    return r
}

// Pre-compile regex patterns for better performance
func efficientRegexRoutes(r *mux.Router) {
    // BAD: Complex regex in route
    r.HandleFunc("/items/{id:[a-zA-Z0-9]{8}-[a-zA-Z0-9]{4}-[a-zA-Z0-9]{4}-[a-zA-Z0-9]{4}-[a-zA-Z0-9]{12}}", Handler)
    
    // GOOD: Simple regex, validate in handler
    r.HandleFunc("/items/{id}", Handler)
}
```