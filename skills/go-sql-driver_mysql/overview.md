# Overview

The `go-sql-driver/mysql` package is a pure Go implementation of the MySQL wire protocol that implements Go's `database/sql/driver` interface. It provides lightweight, fast MySQL connectivity without C bindings, supporting TCP/IPv4, TCP/IPv6, Unix domain sockets, and custom protocols. The driver automatically handles broken connections and integrates with Go's connection pooling via `database/sql`.

**When to use:** You need to connect a Go application to MySQL, MariaDB, or compatible databases like TiDB or Percona Server. The driver is ideal for production systems requiring connection pooling, prepared statements, transactions, and context-based cancellation.

**When NOT to use:** You need ORM-level abstractions (use GORM or similar), require database-agnostic query building, or need features not supported by MySQL's wire protocol. For read-heavy workloads with complex queries, consider using a dedicated query builder on top of this driver.

**Key design principles:** The driver follows Go's `database/sql` conventions, meaning you interact with `*sql.DB`, `*sql.Tx`, and `*sql.Stmt` objects rather than driver-specific types. Connection management is delegated to `database/sql`, while the driver handles protocol-level details like authentication, packet encoding/decoding, and result set parsing. The DSN format `[username[:password]@][protocol[(address)]]/dbname[?param1=value1]` provides flexible configuration without code changes.