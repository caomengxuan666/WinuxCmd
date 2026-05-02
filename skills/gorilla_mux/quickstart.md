# Quickstart

```go
package main

import (
    "fmt"
    "log"
    "net/http"
    "time"

    "github.com/gorilla/mux"
)

// Pattern 1: Basic route with path variables
func main() {
    r := mux.NewRouter()
    r.HandleFunc("/", HomeHandler)
    r.HandleFunc("/products/{id}", ProductHandler)
    r.HandleFunc("/articles/{category}/{id:[0-9]+}", ArticleHandler)
    
    http.Handle("/", r)
    log.Fatal(http.ListenAndServe(":8080", nil))
}

func HomeHandler(w http.ResponseWriter, r *http.Request) {
    fmt.Fprintf(w, "Home Page")
}

func ProductHandler(w http.ResponseWriter, r *http.Request) {
    vars := mux.Vars(r)
    fmt.Fprintf(w, "Product ID: %s", vars["id"])
}

func ArticleHandler(w http.ResponseWriter, r *http.Request) {
    vars := mux.Vars(r)
    fmt.Fprintf(w, "Category: %s, ID: %s", vars["category"], vars["id"])
}

// Pattern 2: Route with method restrictions
func methodExample() {
    r := mux.NewRouter()
    r.HandleFunc("/api/users", GetUsers).Methods("GET")
    r.HandleFunc("/api/users", CreateUser).Methods("POST")
    r.HandleFunc("/api/users/{id}", UpdateUser).Methods("PUT")
    r.HandleFunc("/api/users/{id}", DeleteUser).Methods("DELETE")
}

// Pattern 3: Subrouter with host matching
func subrouterExample() {
    r := mux.NewRouter()
    s := r.Host("api.example.com").Subrouter()
    s.HandleFunc("/v1/users", APIUsersHandler)
    s.HandleFunc("/v1/products", APIProductsHandler)
}

// Pattern 4: Path prefix for static files
func staticFilesExample() {
    r := mux.NewRouter()
    r.PathPrefix("/static/").Handler(http.StripPrefix("/static/", 
        http.FileServer(http.Dir("./static"))))
}

// Pattern 5: Query parameters matching
func queryParamsExample() {
    r := mux.NewRouter()
    r.HandleFunc("/search", SearchHandler).Queries("q", "{query}")
}

func SearchHandler(w http.ResponseWriter, r *http.Request) {
    vars := mux.Vars(r)
    fmt.Fprintf(w, "Searching for: %s", vars["query"])
}

// Pattern 6: Custom matcher function
func customMatcherExample() {
    r := mux.NewRouter()
    r.MatcherFunc(func(r *http.Request, rm *mux.RouteMatch) bool {
        return r.Header.Get("X-API-Version") == "2"
    }).HandlerFunc(Version2Handler)
}

// Pattern 7: Middleware chain
func middlewareExample() {
    r := mux.NewRouter()
    r.Use(loggingMiddleware)
    r.Use(authMiddleware)
    r.HandleFunc("/protected", ProtectedHandler)
}

func loggingMiddleware(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        log.Printf("%s %s", r.Method, r.URL.Path)
        next.ServeHTTP(w, r)
    })
}

// Pattern 8: URL building (reverse routing)
func urlBuildingExample() {
    r := mux.NewRouter()
    r.HandleFunc("/articles/{category}/{id:[0-9]+}", ArticleHandler).Name("article")
    
    url, _ := r.Get("article").URL("category", "tech", "id", "123")
    fmt.Println(url) // /articles/tech/123
}
```