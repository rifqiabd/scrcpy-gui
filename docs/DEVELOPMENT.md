# Development Guide

## Setup Development Environment

### 1. Install Qt SDK
Download from: https://www.qt.io/download-dev

### 2. Install CMake
- Linux: `sudo apt install cmake`
- Windows: Download from cmake.org
- macOS: `brew install cmake`

### 3. Install ADB & scrcpy
```bash
# Linux
sudo apt install adb scrcpy

# macOS
brew install android-platform-tools scrcpy

# Windows
# Download from Android SDK and scrcpy GitHub releases
```

### 4. Configure Cursor IDE
1. Open project folder in Cursor
2. Install C++ extension
3. Configure CMake kit (Ctrl+Shift+P → "CMake: Select a Kit")
4. Build project (F7 or Ctrl+Shift+B)

## Common Development Tasks

### Adding New Feature
1. Update `ARCHITECTURE.md` with design
2. Create necessary class files in `src/`
3. Update `CMakeLists.txt` if adding new files
4. Implement with AI assistance in Cursor
5. Test manually with real device

### Testing ADB Commands
```bash
# List devices
adb devices

# List apps
adb shell pm list packages -3

# Get app info
adb shell dumpsys package com.example.app

# Launch app
adb shell monkey -p com.example.app 1
```

### Debugging
- Use Qt Creator debugger for visual debugging
- Add `qDebug() << "message";` for logging
- Check ADB logcat for Android-side issues

### Building Release
```bash
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Tips for Cursor AI
- Use `@README.md` to reference project overview
- Use `@ARCHITECTURE.md` for architectural questions
- Use `.cursorrules` is automatically applied
- Tag specific files with `@filename.cpp` for context

## Troubleshooting
- **Qt not found**: Set `CMAKE_PREFIX_PATH` to Qt installation
- **MOC errors**: Run `cmake ..` again to regenerate
- **ADB not working**: Check device is authorized, USB debugging enabled

## Code Style Examples

### Signal/Slot Connection
```cpp
connect(ui->appListWidget, &QListWidget::itemClicked,
        this, &MainWindow::onAppSelected);
```

### QProcess Usage
```cpp
QProcess *process = new QProcess(this);
connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &MainWindow::onProcessFinished);
process->start("adb", QStringList() << "devices");
```

### Error Handling
```cpp
if (process->exitCode() != 0) {
    QMessageBox::critical(this, "Error",
        "ADB command failed: " + process->errorString());
    return;
}
```
