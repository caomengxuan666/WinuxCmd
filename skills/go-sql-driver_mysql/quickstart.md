# Quickstart

```go
package main

import (
	"database/sql"
	"fmt"
	"log"
	"time"

	_ "github.com/go-sql-driver/mysql"
)

// Pattern 1: Basic connection and query
func basicConnection() {
	db, err := sql.Open("mysql", "user:password@tcp(127.0.0.1:3306)/dbname")
	if err != nil {
		log.Fatal(err)
	}
	defer db.Close()

	var version string
	err = db.QueryRow("SELECT VERSION()").Scan(&version)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("MySQL version:", version)
}

// Pattern 2: Using DSN with parameters
func dsnWithParams() {
	dsn := "user:password@tcp(127.0.0.1:3306)/dbname?charset=utf8mb4&parseTime=True&loc=Local"
	db, err := sql.Open("mysql", dsn)
	if err != nil {
		log.Fatal(err)
	}
	defer db.Close()
}

// Pattern 3: Connection pool configuration
func connectionPool() {
	db, err := sql.Open("mysql", "user:password@/dbname")
	if err != nil {
		log.Fatal(err)
	}
	defer db.Close()

	db.SetConnMaxLifetime(3 * time.Minute)
	db.SetMaxOpenConns(25)
	db.SetMaxIdleConns(25)
}

// Pattern 4: Prepared statements
func preparedStatements(db *sql.DB) {
	stmt, err := db.Prepare("INSERT INTO users(name, email) VALUES(?, ?)")
	if err != nil {
		log.Fatal(err)
	}
	defer stmt.Close()

	_, err = stmt.Exec("John Doe", "john@example.com")
	if err != nil {
		log.Fatal(err)
	}
}

// Pattern 5: Transaction handling
func transactionExample(db *sql.DB) {
	tx, err := db.Begin()
	if err != nil {
		log.Fatal(err)
	}
	defer tx.Rollback()

	_, err = tx.Exec("UPDATE accounts SET balance = balance - ? WHERE id = ?", 100, 1)
	if err != nil {
		return
	}

	_, err = tx.Exec("UPDATE accounts SET balance = balance + ? WHERE id = ?", 100, 2)
	if err != nil {
		return
	}

	err = tx.Commit()
	if err != nil {
		log.Fatal(err)
	}
}

// Pattern 6: Context-based operations
func contextExample(db *sql.DB) {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	var count int
	err := db.QueryRowContext(ctx, "SELECT COUNT(*) FROM users").Scan(&count)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("User count:", count)
}

// Pattern 7: Scanning into struct
type User struct {
	ID    int
	Name  string
	Email string
}

func scanToStruct(db *sql.DB) {
	rows, err := db.Query("SELECT id, name, email FROM users LIMIT 10")
	if err != nil {
		log.Fatal(err)
	}
	defer rows.Close()

	var users []User
	for rows.Next() {
		var u User
		err := rows.Scan(&u.ID, &u.Name, &u.Email)
		if err != nil {
			log.Fatal(err)
		}
		users = append(users, u)
	}
}

// Pattern 8: Bulk insert with prepared statement
func bulkInsert(db *sql.DB) {
	tx, err := db.Begin()
	if err != nil {
		log.Fatal(err)
	}
	defer tx.Rollback()

	stmt, err := tx.Prepare("INSERT INTO logs(level, message) VALUES(?, ?)")
	if err != nil {
		log.Fatal(err)
	}
	defer stmt.Close()

	logs := []struct {
		Level   string
		Message string
	}{
		{"INFO", "Application started"},
		{"WARN", "Memory usage high"},
		{"ERROR", "Connection failed"},
	}

	for _, l := range logs {
		_, err = stmt.Exec(l.Level, l.Message)
		if err != nil {
			log.Fatal(err)
		}
	}

	err = tx.Commit()
	if err != nil {
		log.Fatal(err)
	}
}
```