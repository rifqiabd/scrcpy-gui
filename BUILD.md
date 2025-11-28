# Quick Build Instructions

## Prerequisites
1. Install Qt 6.x SDK from https://www.qt.io/download-dev
2. Install CMake 3.16+ 
3. Install ADB and scrcpy

## Build Steps

### Linux/macOS
```bash
mkdir build
cd build
cmake ..
make
./scrcpy-gui
```

### Windows (MinGW)
```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
scrcpy-gui.exe
```

### Windows (Visual Studio)
```bash
mkdir build
cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
.\Release\scrcpy-gui.exe
```

## Troubleshooting

### CMake can't find Qt
Set Qt path:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64 ..
```

### ADB not found
Add ADB to PATH or install:
- Linux: `sudo apt install adb`
- macOS: `brew install android-platform-tools`
- Windows: Download Android SDK Platform Tools

### Scrcpy not found
Install scrcpy:
- Linux: `sudo apt install scrcpy`
- macOS: `brew install scrcpy`
- Windows: Download from https://github.com/Genymobile/scrcpy/releases

## Development with Cursor IDE
1. Open the `scrcpy-gui` folder in Cursor
2. The `.cursorrules` file will automatically provide context
3. Use `@README.md`, `@ARCHITECTURE.md` to reference documentation
4. Build using terminal or configure CMake in Cursor

## Next Steps
See `docs/DEVELOPMENT.md` for detailed development guide.
