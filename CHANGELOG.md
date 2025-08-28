# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- **Single Instance Protection**: Only one instance of the application can run at a time
  - Prevents multiple copies from running simultaneously
  - Automatically brings existing instance to foreground when duplicate launch is attempted
  - Uses named mutex for reliable instance detection
  - Includes window flashing to get user attention
  - Handles window restoration from minimized state
- GitHub standard project structure with src/, include/, assets/, and docs/ directories
- Contributing guidelines (CONTRIBUTING.md)
- Changelog documentation
- Comprehensive .gitignore file for Visual Studio C++ projects

### Changed
- Reorganized source files into proper directory structure following GitHub conventions
- Updated project files to reference new directory structure
- Enhanced window detection mechanism for better reliability
- Improved foreground window handling with fallback methods

### Removed
- Build artifacts and temporary files from repository

### Technical Details
- Single instance implementation uses `CreateMutexW()` with global named mutex
- Window enumeration with process ID verification to avoid false positives
- Enhanced foreground handling with `AttachThreadInput()` fallback
- Proper mutex cleanup on application exit
- Thread-safe implementation compatible with existing singleton pattern

## [Previous Versions]

### Features
- Mouse and keyboard activity monitoring
- Configurable timeout periods (1-3600 seconds)
- Random mouse movement on inactivity
- Customizable global hotkeys
- System tray integration
- Startup behavior options
- Windows startup integration
- Settings persistence in Windows registry
- Real-time timeout updates
- Comprehensive hotkey customization

### Technical
- Built with C++ and Windows SDK
- Uses Win32 API with low-level system hooks
- Registry-based settings storage
- System tray Shell_NotifyIcon integration
- Global hotkey registration via RegisterHotKey API