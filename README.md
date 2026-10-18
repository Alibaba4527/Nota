# Nota

## Build and run

Requires CMake 3.21 or newer, a C++20 compiler, and Qt 6.4 or newer (Widgets). The build links the system Qt and does not vendor a copy.

- Debian or Ubuntu: `sudo apt install build-essential cmake qt6-base-dev`
- Fedora: `sudo dnf install gcc-c++ cmake qt6-qtbase-devel`
- macOS: Homebrew `cmake` and `qt`, or the Qt installer
- Windows: the Qt 6 SDK, CMake, and MSVC or another C++20 compiler

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/nota
```

On macOS with Homebrew, pass `-DCMAKE_PREFIX_PATH="$(brew --prefix qt)"` if CMake does not find Qt. On Windows the executable is `build\nota.exe`, or `build\Debug\nota.exe` with a multi-config generator.
