# Language Syntax

Anis's syntax is heavily inspired by JavaScript (ECMAScript), making it familiar and easy to learn.

## Variables

Variables can be declared using `var` or `const`.

```javascript
var age = 25;
const name = "Anis";

age = 26; // Success
// name = "New Name"; // Error (const if implemented, currently similar to var)
```

## Data Types

- **Number**: Integers only (e.g., `10`, `-5`).
- **String**: Double, single, or backtick quotes (e.g., `"hello"`, `'world'`, `` `template` ``).
- **Boolean**: Represented by `1` (true) and `0` (false) or empty strings.
- **Array**: `[1, 2, 3]`
- **Map/Object**: `{ key: "value", age: 30 }`

## Control Flow

### If-Else
```javascript
if (age > 18) {
    println("Adult");
} else {
    println("Minor");
}
```

### While Loop
```javascript
var i = 0;
while (i < 5) {
    println(i);
    i = i + 1;
}
```

### Switch Statement
```javascript
var type = "admin";
switch (type) {
    case "admin":
        println("Authorized");
    case "user":
        println("Limited Access");
    default:
        println("Unknown");
}
```

## Functions

### Function Declaration
```javascript
function add(a, b) {
    return a + b;
}
```

### Arrow Functions
```javascript
const multiply = (a, b) => a * b;

// With block
const greet = (name) => {
    println("Hello " + name);
};
```

## Advanced Features

### Object Spread
```javascript
var user = { name: "Bob", age: 20 };
var updatedUser = { ...user, age: 21, active: 1 };
```

### Array Destructuring
```javascript
const [first, second] = [10, 20];
```

### Ternary Operator
```javascript
var state = (age >= 18) ? "Adult" : "Minor";
```
