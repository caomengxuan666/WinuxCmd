# Quickstart

```cpp
// Quickstart examples for diesel-rs/diesel

// 1. Basic Query: Select all users
use diesel::prelude::*;
use diesel::sqlite::SqliteConnection;

let connection = SqliteConnection::establish("test.db")?;
let users = users::table.load::<User>(&connection)?;

// 2. Filtered Query with conditions
let active_users = users::table
    .filter(users::active.eq(true))
    .filter(users::age.gt(18))
    .load::<User>(&connection)?;

// 3. Insert a new record
let new_user = NewUser { name: "Alice", age: 30 };
diesel::insert_into(users::table)
    .values(&new_user)
    .execute(&connection)?;

// 4. Update existing records
diesel::update(users::table.filter(users::name.eq("Bob")))
    .set(users::age.eq(35))
    .execute(&connection)?;

// 5. Delete records
diesel::delete(users::table.filter(users::age.lt(18)))
    .execute(&connection)?;

// 6. Join tables
let posts_with_users = posts::table
    .inner_join(users::table)
    .select((posts::title, users::name))
    .load::<(String, String)>(&connection)?;

// 7. Transaction with rollback on error
connection.transaction::<_, diesel::result::Error, _>(|| {
    diesel::insert_into(users::table).values(&new_user).execute(&connection)?;
    diesel::insert_into(posts::table).values(&new_post).execute(&connection)?;
    Ok(())
})?;

// 8. Pagination with limit and offset
let page = 2;
let per_page = 10;
let paginated_users = users::table
    .limit(per_page)
    .offset((page - 1) * per_page)
    .load::<User>(&connection)?;
```