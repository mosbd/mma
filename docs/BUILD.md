# Building MMA from Source

This guide will help you build the Mouse & Keyboard Activity Monitor (MMA) from source code.

## Prerequisites

### Required Software
- **Visual Studio 2019 or later** with C++ development tools
- **Windows SDK 10.0** or later
- **Git** for source control

### System Requirements
- Windows Vista or later (Windows 10/11 recommended)
- x86 or x64 architecture
- Administrator privileges (for hook installation and registry access)

## Getting the Source Code

1. Clone the repository:
   ```bash
   git clone https://github.com/mosbd/mma.git
   cd mma
   ```

2. Or download the ZIP file from GitHub and extract it.

## Building with Visual Studio

### Method 1: Using Visual Studio IDE

1. Open `mma.sln` in Visual Studio
2. Select your target configuration:
   - **Debug**: For development and debugging
   - **Release**: For production builds
3. Select your target platform:
   - **x86**: 32-bit build
   - **x64**: 64-bit build (recommended)
4. Build the solution:
   - Press `F7` or go to **Build > Build Solution**
   - Or press `Ctrl+Shift+B`

### Method 2: Using MSBuild (Command Line)

1. Open **Developer Command Prompt for Visual Studio**
2. Navigate to the project directory
3. Build the solution:
   ```bash
   # For Release x64 build
   msbuild mma.sln /p:Configuration=Release /p:Platform=x64
   
   # For Debug x86 build
   msbuild mma.sln /p:Configuration=Debug /p:Platform=Win32
   ```

## Project Structure

```
mma/
??? src/               # Source files (.cpp)
?   ??? main.cpp       # Application entry point
?   ??? ActivityMonitor.cpp
?   ??? ApplicationManager.cpp
?   ??? DialogManager.cpp
?   ??? HotkeyManager.cpp
?   ??? SettingsManager.cpp
?   ??? SystemTray.cpp
??? include/           # Header files (.h)
?   ??? ActivityMonitor.h
?   ??? ApplicationManager.h
?   ??? common.h
?   ??? DialogManager.h
?   ??? framework.h
?   ??? HotkeyManager.h
?   ??? resource_new.h
?   ??? SettingsManager.h
?   ??? SystemTray.h
?   ??? targetver.h
??? assets/           # Resource files
?   ??? mma.ico       # Application icon
?   ??? small.ico     # Small icon
?   ??? mma.rc        # Resource script
??? docs/             # Documentation
??? mma.sln           # Visual Studio solution file
??? mma.vcxproj       # Visual Studio project file
??? README.md         # Main documentation
```

## Build Configurations

### Debug Configuration
- Includes debugging symbols
- No optimizations
- Runtime checks enabled
- Larger executable size
- Use for development and debugging

### Release Configuration
- Optimized for performance
- No debugging symbols
- Smaller executable size
- Use for production deployment

## Output Locations

After building, the executable will be located at:
- **Debug x86**: `Debug/mma.exe`
- **Debug x64**: `x64/Debug/mma.exe`
- **Release x86**: `Release/mma.exe`
- **Release x64**: `x64/Release/mma.exe`

## Troubleshooting

### Common Build Issues

1. **Missing Windows SDK**
   - Install Windows SDK through Visual Studio Installer
   - Or download standalone Windows SDK

2. **C++ Compiler Not Found**
   - Install "Desktop development with C++" workload in Visual Studio

3. **Permission Denied Errors**
   - Run Visual Studio as Administrator
   - Check if antivirus is blocking the build

4. **Missing Dependencies**
   - Ensure all required Windows SDK components are installed
   - Verify C++14 compiler support is available

### Build Warnings

Some warnings may appear during build:
- **C4996**: Deprecated function warnings (safe to ignore for Windows API calls)
- **C4244**: Type conversion warnings (should be reviewed)

## Testing the Build

1. Run the executable from the output directory
2. Verify basic functionality:
   - Application starts and shows main window
   - System tray icon appears
   - Settings can be configured
   - Monitoring can be started/stopped
   - Global hotkey works

## Deployment

For distribution:
1. Use Release configuration
2. Test on clean Windows system
3. Include any required Visual C++ Redistributables
4. Consider code signing for production releases

## Advanced Build Options

### Custom Build Configurations

You can create custom build configurations:
1. Go to **Build > Configuration Manager**
2. Create new configurations based on existing ones
3. Modify compiler and linker settings as needed

### Static Linking

To avoid runtime dependencies:
1. In Project Properties > C/C++ > Code Generation
2. Set Runtime Library to "Multi-threaded (/MT)" for Release
3. Set Runtime Library to "Multi-threaded Debug (/MTd)" for Debug