# Best Practices

```cpp
// BEST PRACTICE 1: Use connection pooling for production
use diesel::r2d2::{ConnectionManager, Pool};
use diesel::r2d2::PooledConnection;

type DbPool = Pool<ConnectionManager<SqliteConnection>>;

fn create_pool() -> DbPool {
    let manager = ConnectionManager::<SqliteConnection>::new("production.db");
    Pool::builder()
        .max_size(15)
        .build(manager)
        .expect("Failed to create pool")
}

// BEST PRACTICE 2: Define custom types for complex queries
#[derive(Queryable)]
struct UserWithPostCount {
    user_id: i32,
    name: String,
    post_count: i64,
}

fn get_users_with_post_count(conn: &SqliteConnection) -> Vec<UserWithPostCount> {
    users::table
        .left_join(posts::table)
        .group_by(users::id)
        .select((users::id, users::name, diesel::dsl::count(posts::id)))
        .load::<UserWithPostCount>(conn)
        .expect("Error loading users")
}

// BEST PRACTICE 3: Use repository pattern for organization
struct UserRepository {
    pool: DbPool,
}

impl UserRepository {
    fn find_by_id(&self, id: i32) -> Result<User, diesel::result::Error> {
        let conn = self.pool.get()?;
        users::table.find(id).first(&conn)
    }

    fn create(&self, new_user: &NewUser) -> Result<User, diesel::result::Error> {
        let conn = self.pool.get()?;
        diesel::insert_into(users::table)
            .values(new_user)
            .get_result(&conn)
    }
}

// BEST PRACTICE 4: Use migrations for schema changes
// In migrations/2024-01-01-000000_create_users/up.sql
// CREATE TABLE users (
//     id INTEGER PRIMARY KEY AUTOINCREMENT,
//     name TEXT NOT NULL,
//     email TEXT NOT NULL UNIQUE,
//     created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
// );

// BEST PRACTICE 5: Implement custom traits for serialization
#[derive(Insertable)]
#[table_name = "users"]
struct NewUser<'a> {
    name: &'a str,
    email: &'a str,
}

impl<'a> NewUser<'a> {
    fn from_request(name: &'a str, email: &'a str) -> Self {
        NewUser { name, email }
    }
}

// BEST PRACTICE 6: Use batch operations for performance
fn insert_many_users(conn: &SqliteConnection, users: &[NewUser]) -> Result<(), diesel::result::Error> {
    diesel::insert_into(users::table)
        .values(users)
        .execute(conn)?;
    Ok(())
}
```