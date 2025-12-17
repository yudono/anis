# C++ Mini-GUI Dashboard

A custom, lightweight UI engine built with C++ and GLFW. Features an XML-like DSL for layout, Flexbox-style positioning (`Row`, `Column`), and smooth text rendering via FreeType.

## Prerequisites

### macOS
1.  **Install Homebrew** (if not installed):
    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```
2.  **Install Library Dependencies**:
    ```bash
    brew install glfw freetype
    ```

## Build Instructions

The project uses a standard `Makefile` configuration.

1.  **Open Terminal** in the project directory.
2.  **Run Build**:
    ```bash
    make
    ```
    This will compile `main.cpp` using `clang++` and link against GLFW and FreeType.

## How to Run

After building, an executable named `dashboard` is created in the `build/` directory.

```bash
./build/dashboard
```

## Features
- **XML Layout**: Edit the XML string in `main.cpp` to change the UI without touching C++ rendering logic.
- **Flexbox Layout**: Use `<Row>` and `<Column>` with attributes like `justifyContent="center"` or `alignItems="right"`.
- **Scrolling**: Use `<Scrollview>` containers for content larger than the window.
- **Responsive**: Use `%` (e.g., `width="50%"`) or `px` (e.g., `width="200px"`) for sizing.
