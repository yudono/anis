# Standard Library

Sunda comes with built-in modules for common tasks.

## Global Functions

- `print(args...)`: Prints values without a newline.
- `println(args...)`: Prints values with a newline.

## String Module
```javascript
import { length, substr, contains } from "string";

var s = "Hello Sunda";
println(s.length); // 11
```

## Array Module
```javascript
var list = [1, 2, 3];
list.push(4);
list.pop();

var doubled = list.map(n => n * 2);
var evens = list.filter(n => n % 2 == 0);
```

## Map Module
```javascript
var user = { id: 1 };
user.name = "Alice";
println(user.name);
```
### `db` Module
The `db` module provides a unified interface for database operations.

- `db_connect(url)`: Connects to a database. Supports `sqlite://`, `mysql://`, and `mariadb://`.
- `db_query(sql, params)`: Executes a query and returns result as an array of objects.
- `db_execute(sql, params)`: Executes a command (insert, update, delete).
- `db_close()`: Closes the active connection.
- `db_error()`: Returns the last error message.

Example (SQL Injection Protected):
```javascript
import { db_connect, db_execute, db_query } from "db";
db_connect("sqlite://examples/test_db/test.db");
db_execute("INSERT INTO users (name) VALUES (?)", ["Yudono"]);
const users = db_query("SELECT * FROM users");
```

### `webserver` Module
The `webserver` module provides a low-level TCP/IP server with a high-level API.

- `WebServer_create()`: Creates a new server instance.
  - `app.get(path, handler)`: Registers a GET route. Supports `:param` syntax.
  - `app.listen({ port })`: Starts the server.
- `c.text(body)`: Helper to send a plain text response.
- `c.req.param(name)`: Returns the value of a URL parameter.

Example (Hono-like):
```javascript
import { WebServer_create } from "webserver";
const app = WebServer_create();

app.get("/hello/:name", (c) => {
  const name = c.req.param("name");
  return c.text("Hello, " + name);
});

app.listen({ port: 3000 });
```

## Math Module
```javascript
import { sin, cos, random, floor } from "math";

println(floor(3.14)); // 3
```

## Date Module
```javascript
import { now, format } from "date";

println(now()); // Current timestamp
```
