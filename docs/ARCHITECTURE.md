# Architecture Documentation

## Overview
Qt GUI Scrcpy is designed as a multi-window Qt application that interfaces with ADB and scrcpy external tools.

## Component Architecture

### 1. MainWindow (UI Layer)
**File:** `src/mainwindow.cpp/h`

Responsibilities:
- Display main application window
- Show list of available Android apps (QListWidget)
- Handle user interactions (app selection, manual add)
- Manage application lifecycle

Key Methods:
- `loadAppList()` - Trigger ADB query for app list
- `onAppSelected(QString package)` - Handle app click
- `onManualAddClicked()` - Show dialog for manual app entry
- `saveConfig()` - Persist user-added apps

### 2. ScrcpyWindow (Display Layer)
**File:** `src/scrcpywindow.cpp/h`

Responsibilities:
- New window for each scrcpy instance
- Launch scrcpy process for specific app
- Display scrcpy output (embedded or external)
- Handle window lifecycle

Key Methods:
- `launchScrcpy(QString package)` - Start scrcpy process
- `onScrcpyFinished()` - Clean up when scrcpy closes

### 3. AppManager (Business Logic Layer)
**File:** `src/appmanager.cpp/h`

Responsibilities:
- Execute ADB commands
- Parse ADB output
- Manage scrcpy processes
- Load/save config file

Key Methods:
- `getInstalledApps()` - Query ADB for app list
- `getAppLabel(QString package)` - Get human-readable app name
- `loadCustomApps()` - Load user-added apps from config
- `saveCustomApps()` - Save config to file

## Data Flow

```
[User Click App]
    ↓
[MainWindow::onAppSelected]
    ↓
[Create ScrcpyWindow]
    ↓
[ScrcpyWindow::launchScrcpy]
    ↓
[QProcess → scrcpy --app=package]
    ↓
[Scrcpy Window Opens]
```

## ADB Integration

### List Apps
```cpp
QProcess *proc = new QProcess();
proc->start("adb", {"shell", "pm", "list", "packages", "-3"});
// Output: package:com.example.app
```

### Get App Name
```cpp
// Option 1: Parse from package name (simple)
QString appName = packageToName(package);

// Option 2: Use ADB (more accurate)
QProcess *proc = new QProcess();
proc->start("adb", {"shell", "dumpsys", "package", package});
// Parse output for application label
```

## Scrcpy Integration

### Launch Command
```cpp
QStringList args;
args << "--app" << packageName;
args << "--window-title" << windowTitle;
args << "--max-fps" << "30";

QProcess *scrcpyProc = new QProcess();
scrcpyProc->start("scrcpy", args);
```

### Window Management
Each scrcpy instance runs in separate process. ScrcpyWindow manages:
- Process lifecycle
- Window title/icon
- Error handling
- Clean shutdown

## Configuration Management

### Config File Location
- Linux: `~/.config/scrcpy-gui/config.json`
- Windows: `%APPDATA%/scrcpy-gui/config.json`
- macOS: `~/Library/Application Support/scrcpy-gui/config.json`

### Config Format
```json
{
  "version": "1.0",
  "customApps": [
    {
      "name": "My Custom App",
      "package": "com.example.custom",
      "icon": ""
    }
  ],
  "scrcpyOptions": {
    "maxFps": 30,
    "bitrate": "8M"
  }
}
```

## Error Handling Strategy

1. **ADB Not Found**: Show error dialog, offer help link
2. **No Device Connected**: Show helpful message with troubleshooting
3. **Scrcpy Not Found**: Prompt user to install or specify path
4. **App Launch Failed**: Log error, show notification
5. **Config Read Error**: Use defaults, attempt to recreate config

## Threading Model
- Main thread: UI operations
- QProcess handles external commands asynchronously
- Signals/slots for communication between threads
- No need for manual thread management (Qt handles it)

## Future Enhancements
- Device selection for multiple connected devices
- App icons display
- Scrcpy options configuration UI
- Wireless ADB connection support
- Recording/screenshot features
