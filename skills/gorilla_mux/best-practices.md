# Best Practices

### Pattern 1: Structured route organization
```go
package main

import (
    "github.com/gorilla/mux"
    "net/http"
)

func SetupRouter() *mux.Router {
    r := mux.NewRouter()
    
    // API routes
    api := r.PathPrefix("/api/v1").Subrouter()
    api.Use(apiMiddleware)
    api.HandleFunc("/users", GetUsers).Methods("GET")
    api.HandleFunc("/users/{id:[0-9]+}", GetUser).Methods("GET")
    
    // Static files
    r.PathPrefix("/static/").Handler(http.StripPrefix("/static/", 
        http.FileServer(http.Dir("./static"))))
    
    // Health check
    r.HandleFunc("/health", HealthHandler).Methods("GET")
    
    return r
}
```

### Pattern 2: Middleware chain with error handling
```go
func SetupMiddleware(r *mux.Router) {
    r.Use(func(next http.Handler) http.Handler {
        return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
            // Recovery middleware
            defer func() {
                if err := recover(); err != nil {
                    http.Error(w, "Internal Server Error", http.StatusInternalServerError)
                }
            }()
            next.ServeHTTP(w, r)
        })
    })
    
    r.Use(loggingMiddleware)
    r.Use(rateLimitMiddleware)
}
```

### Pattern 3: Named routes for URL building
```go
func SetupNamedRoutes(r *mux.Router) {
    r.HandleFunc("/users/{id:[0-9]+}", GetUser).Name("get-user")
    r.HandleFunc("/users/{id}/posts", GetUserPosts).Name("get-user-posts")
}

func GenerateURL(r *mux.Router, routeName string, params ...string) (string, error) {
    return r.Get(routeName).URL(params...)
}
```

### Pattern 4: Environment-specific configuration
```go
func NewRouter(config Config) *mux.Router {
    r := mux.NewRouter()
    
    if config.Debug {
        r.Use(debugMiddleware)
    }
    
    if config.CORS {
        r.Use(corsMiddleware)
    }
    
    return r
}
```