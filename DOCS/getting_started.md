# Getting Started with Anis

This guide will help you install Anis and write your first program.

## Installation

### From Source (Recommended)

Anis is built using C++11 and requires GLFW and FreeType libraries.

1.  **Clone the repository**:
    ```bash
    git clone https://github.com/yudono/anis.git
    cd anis
    ```

2.  **Install dependencies**:
    - **macOS**: `brew install glfw freetype`
    - **Linux**: `sudo apt install libglfw3-dev libfreetype6-dev`

3.  **Compile**:
    ```bash
    make
    ```

4.  **Verify**:
    ```bash
    ./bin/anis --help
    ```

## Your First Script

Create a file named `hello.anis`:

```javascript
println("Hello from Anis!");

var name = "Island";
println("Welcome to Anis " + name);
```

Run it:
```bash
./bin/anis hello.anis
```

## Using the REPL

Anis includes an interactive Read-Eval-Print Loop (REPL). It's great for testing small snippets.

1.  **Start REPL**:
    ```bash
    ./bin/anis
    ```

2.  **Try some commands**:
    ```
    anis> var x = 10
    anis> x * 5
    => 50
    anis> println("REPL is active")
    REPL is active
    => undefined
    ```

3.  **Exit**: Type `exit` or press `Ctrl+C`.
