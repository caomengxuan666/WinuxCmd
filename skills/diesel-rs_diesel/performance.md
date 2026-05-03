# Performance

```cpp
// PERFORMANCE CHARACTERISTICS

// 1. Batch inserts are significantly faster than individual inserts
// SLOW: Individual inserts
for user in users {
    diesel::insert_into(users::table)
        .values(&user)
        .execute(&conn)?;
}

// FAST: Batch insert
diesel::insert_into(users::table)
    .values(&users) // users is a Vec<NewUser>
    .execute(&conn)?;

// 2. Use select for specific columns instead of loading entire rows
// SLOW: Loads all columns
let users = users::table.load::<User>(&conn)?;

// FAST: Only load needed columns
let names = users::table
    .select(users::name)
    .load::<String>(&conn)?;

// 3. Use eager loading to avoid N+1 queries
// SLOW: N+1 query problem
let posts = posts::table.load::<Post>(&conn)?;
for post in &posts {
    let user = users::table.find(post.user_id).first::<User>(&conn)?;
}

// FAST: Join query
let posts_with_users = posts::table
    .inner_join(users::table)
    .select((posts::all_columns, users::all_columns))
    .load::<(Post, User)>(&conn)?;

// 4. Use indexes for frequently queried columns
// Create index in migration:
// CREATE INDEX idx_users_email ON users(email);

// 5. Limit result sets for pagination
// SLOW: Loads all records
let all_users = users::table.load::<User>(&conn)?;

// FAST: Paginated query
let page = users::table
    .limit(20)
    .offset(40)
    .load::<User>(&conn)?;

// 6. Use prepared statements for repeated queries
// Diesel automatically caches prepared statements
fn find_user_by_id(conn: &SqliteConnection, id: i32) -> QueryResult<User> {
    users::table.find(id).first(conn) // Prepared statement cached
}

// 7. Avoid unnecessary allocations in hot paths
// SLOW: Creating new strings each time
for i in 0..1000 {
    let name = format!("user_{}", i);
    diesel::insert_into(users::table)
        .values(&NewUser { name: &name })
        .execute(&conn)?;
}

// FAST: Reuse buffer
let mut name = String::with_capacity(20);
for i in 0..1000 {
    name.clear();
    name.push_str("user_");
    name.push_str(&i.to_string());
    diesel::insert_into(users::table)
        .values(&NewUser { name: &name })
        .execute(&conn)?;
}
```