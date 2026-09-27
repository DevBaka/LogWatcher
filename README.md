# LogWatcher

A modern desktop application for viewing and monitoring log files in real-time. Built with C++20 and Qt6.

## Features

- **Log File Viewing**: Open and view .log, .txt, and any text files
- **Live Log Tracking**: Automatic detection and loading of new log entries
- **Configurable Refresh Interval**: Set refresh rate from 0.01s to 3600s with 2 decimal precision
- **Multi-Tab Support**: Open multiple files simultaneously in tabs
- **Project Explorer**: IDE-like folder tree view for easy file navigation
- **Syntax Highlighting**: Color-coded highlighting for ERROR, WARNING, INFO, and DEBUG levels
- **Search Functionality**: Search within log files with case-sensitive option
- **Auto-Scroll**: Automatically scroll to new content
- **Dark Theme**: Modern dark UI design
- **UTF-8 Support**: Full UTF-8 encoding support
- **Performance**: Efficient handling of large log files with chunked loading

## Requirements

- **C++20** compatible compiler (MSVC 19.30+ for Visual Studio 2022)
- **CMake** 3.20 or higher
- **Qt6** (Core and Widgets modules) - See installation instructions below

## Qt Modules Required

- `Qt6::Core` - Core functionality
- `Qt6::Widgets` - Widget-based UI components

## Building with Visual Studio 2022

### Prerequisites

1. Install Visual Studio 2022 with C++ development tools
2. Install CMake (included with Visual Studio or from https://cmake.org/download/)
3. Install Qt6 for Windows (see below for detailed instructions)

### Installing Qt6

#### Option 1: Qt Online Installer (Recommended)
1. Download the Qt Online Installer from https://www.qt.io/download-qt-installer
2. Run the installer and create a Qt account (free)
3. During installation, select:
   - Qt 6.x (latest version)
   - MinGW or MSVC compiler (matching your Visual Studio version)
   - Qt Creator (optional, for development)
4. Choose installation location (default: `C:\Qt\6.x.x\compiler_version`)

#### Option 2: Qt Offline Installer
1. Download the offline installer from https://www.qt.io/download
2. Select the appropriate compiler version:
   - For Visual Studio 2022: `msvc2022_64`
   - For Visual Studio 2019: `msvc2019_64`
   - For MinGW: `mingw_64`
3. Install to a location like `C:\Qt\6.x.x\msvc2022_64`

#### Option 3: aQtive (Portable Qt)
1. Download aQtive from https://aqtinstall.readthedocs.io/
2. Install Qt6 via command line:
   ```powershell
   pip install aqtinstall
   aqt install-qt windows desktop 6.5.0 msvc2022_64 -O C:\Qt
   ```

### Setting Qt6 Path

After installing Qt6, you need to set the Qt6 path. The updated CMakeLists.txt will automatically search for Qt6 in common locations, but you can also manually specify it:

#### Method 1: Environment Variable (Recommended)
Set the `Qt6_DIR` environment variable to your Qt6 installation path:
```powershell
# For Visual Studio 2022
setx Qt6_DIR "C:\Qt\6.5.0\msvc2022_64"
```

#### Method 2: CMake Command Line
Specify the Qt6 path when running CMake:
```powershell
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2022_64"
```

#### Method 3: Visual Studio CMake Settings
In Visual Studio 2022:
1. Open CMakeSettings.json (automatically created)
2. Add `"variables": [{"name": "CMAKE_PREFIX_PATH", "value": "C:/Qt/6.5.0/msvc2022_64"}]`

### Build Steps

#### Option 1: Using Visual Studio 2022 directly

1. Open Visual Studio 2022
2. Select "Open a local folder" and choose the LogWatcher directory
3. Visual Studio will automatically detect the CMakeLists.txt
4. Wait for CMake configuration to complete
5. Select the build configuration (Debug or Release) from the toolbar
6. Build the solution (Ctrl+Shift+B)
7. Run the executable from the build output directory

#### Option 2: Using CMake command line

```powershell
# Navigate to the project directory
cd C:\Users\DJBaka\Documents\GitHub\LogWatcher

# Create a build directory
mkdir build
cd build

# Configure with CMake (replace Qt6 path with your installation)
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"

# Build the project
cmake --build . --config Release

# Run the executable
.\bin\Release\LogWatcher.exe
```

#### Option 3: Using Qt Creator

1. Open Qt Creator
2. File → Open File or Project
3. Select the CMakeLists.txt file
4. Configure the project (select appropriate kit)
5. Build and run (Ctrl+R)

## Project Structure

```
LogWatcher/
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
└── src/
    ├── main.cpp            # Application entry point
    ├── MainWindow.h/cpp   # Main application window
    ├── LogViewerWidget.h/cpp   # Log viewer with syntax highlighting
    ├── FileTracker.h/cpp   # File change monitoring
    └── ProjectExplorer.h/cpp   # File tree view
```

## Usage

1. **Open a Folder**: Use File → Open Folder or Ctrl+Shift+O to open a directory
2. **Open a File**: Use File → Open File or Ctrl+O to open a specific log file
3. **Navigate Files**: Double-click files in the Project Explorer (left panel)
4. **Configure Refresh**: Adjust the refresh interval in the toolbar (0.01s - 3600s)
5. **Toggle Features**: Use toolbar buttons to enable/disable Auto-Scroll and Live Tracking
6. **Search**: Press Ctrl+F to search within the current tab
7. **Close Tabs**: Use File → Close Tab or Ctrl+W to close the current tab

## Architecture

The application follows a clean separation of concerns:

- **MainWindow**: Main application window managing UI layout, menus, and toolbars
- **LogViewerWidget**: Individual log file viewer with syntax highlighting and search
- **FileTracker**: Monitors file changes and triggers refreshes
- **ProjectExplorer**: File tree view for folder navigation

## License

This project is provided as-is for educational and commercial use.

## Technical Details

- **Language**: C++20
- **Framework**: Qt6 Widgets
- **Build System**: CMake
- **Platform**: Windows (Visual Studio 2022)
- **Encoding**: UTF-8
- **UI Theme**: Dark theme with custom styling
