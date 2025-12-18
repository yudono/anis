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
