# Getting Started with Sunda

This guide will help you install Sunda and write your first program.

## Installation

### From Source (Recommended)

Sunda is built using C++11 and requires GLFW and FreeType libraries.

1.  **Clone the repository**:
    ```bash
    git clone https://github.com/yudono/sunda.git
    cd sunda
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
    ./bin/sunda --help
    ```

## Your First Script

Create a file named `hello.sd`:

```javascript
println("Hello from Sunda!");

var name = "Island";
println("Welcome to Sunda " + name);
```

Run it:
```bash
./bin/sunda hello.sd
```

## Using the REPL

Sunda includes an interactive Read-Eval-Print Loop (REPL). It's great for testing small snippets.

1.  **Start REPL**:
    ```bash
    ./bin/sunda
    ```

2.  **Try some commands**:
    ```
    sunda> var x = 10
    sunda> x * 5
    => 50
    sunda> println("REPL is active")
    REPL is active
    => undefined
    ```

3.  **Exit**: Type `exit` or press `Ctrl+C`.
