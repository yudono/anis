# 🏝️ Anis Programming Language

<p align="center">
  <img src="logo.png" alt="Anis Programming Language Logo" width="400"/>
</p>

Anis is a modern, lightweight, and cross-platform programming language built in C++. It features a JavaScript-like syntax, a built-in GUI library with JSX-style layout, and an interactive REPL.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## ✨ Key Features

- **Standard Syntax**: JavaScript-like syntax with support for arrow functions, object spread, and destructuring.
- **Built-in GUI**: Create desktop applications using JSX-like components (`<Row>`, `<Column>`, `<Button>`, etc.).
- **HTTP Module**: Full support for `GET` and `POST` requests via `libcurl`.
- **Remote Imports**: Import scripts directly from URLs (`http://` or `https://`).
- **REPL**: Interactive shell for quick prototyping and testing.
- **Cross-Platform**: Support for macOS, Linux, and Windows.
- **Single Binary**: Compiles to a self-contained executable with no heavy runtime dependencies.

## 🚀 Installation

### Prerequisites

| OS | Requirements |
|---|---|
| **macOS** | Clang++, GLFW, Freetype, SQLite3, MySQL, Libcurl (via Homebrew) |
| **Linux** | Clang++, `pkg-config`, `libglfw3-dev`, `libfreetype6-dev`, `libsqlite3-dev`, `libmysqlclient-dev`, `libcurl4-openssl-dev` |
| **Windows**| Clang++ (MinGW-w64), GLFW3, Freetype, SQLite3, MySQL, Libcurl |

#### macOS
```bash
brew install glfw freetype sqlite mysql-client
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install clang libglfw3-dev libfreetype6-dev libsqlite3-dev libmysqlclient-dev libcurl4-openssl-dev pkg-config
```

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yudono/anis.git
   cd anis
   ```

2. Build the project:
   ```bash
   make
   ```

3. The binary will be available at `bin/anis`.

## 📖 Usage

### Running a Script
```bash
./bin/anis examples/test_syntax/test_hello_world.anis
```

### Interactive REPL
Simply run the binary without arguments:
```bash
./bin/anis
```

### Help & Documentation
```bash
./bin/anis --help
```

For detailed documentation, explore the files in the `DOCS/` directory:
- [Getting Started](DOCS/getting_started.md): Installation and first steps.
- [Language Syntax](DOCS/syntax.md): Syntax, variables, and control flow.
- [GUI Components](DOCS/gui.md): Building UIs with JSX-style components.
- [Standard Library](DOCS/stdlib.md): Reference for built-in modules.
- [Native Library Guide](DOCS/native_library_guide.md): Guide for extending Anis with C++.

## 📸 Screenshots

### GUI Components
![GUI Screenshot](screenshot_gui.jpeg)

### Web Server
![Web Server Screenshot](screenshot_webserver.jpeg)

## 🛠️ Development

- **Build Output**: Object files are stored in `build/[os_name]/`.
- **Clean Build**: `make clean`
- **Dependencies Check**: `make check_deps`

## 📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
