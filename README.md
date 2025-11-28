# scrcpy-gui

Cross-platform desktop GUI for [scrcpy](https://github.com/Genymobile/scrcpy) with native look and feel.

## Features
- 📱 Automatically list installed Android apps via ADB
- 🚀 Launch scrcpy for specific apps with one click
- ➕ Manually add custom apps
- 💾 Persist user preferences
- 🎨 Native UI on Windows, macOS, and Linux

## Requirements
- Qt 6.x
- CMake 3.16+
- C++17 compatible compiler
- ADB (Android Debug Bridge) in PATH
- scrcpy in PATH

## Building

### Linux
```bash
mkdir build && cd build
cmake ..
make
```

### Windows
```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

### macOS
```bash
mkdir build && cd build
cmake ..
make
```

## Usage
1. Connect Android device via USB (with USB debugging enabled)
2. Launch the application
3. View list of installed apps
4. Click an app to launch scrcpy in new window
5. Optionally add custom apps via manual entry

## Project Structure
See [ARCHITECTURE.md](./docs/ARCHITECTURE.md) for detailed architecture.

## Development
This project uses Cursor IDE for AI-assisted development. See `.cursorrules` for coding standards.

## License
MIT License

## Contributing
Contributions are welcome! Please read the development guide in `docs/DEVELOPMENT.md`.
