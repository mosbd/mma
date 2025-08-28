# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- GitHub standard project structure with src/, include/, assets/, and docs/ directories
- Contributing guidelines (CONTRIBUTING.md)
- Changelog documentation
- Comprehensive .gitignore file for Visual Studio C++ projects

### Changed
- Reorganized source files into proper directory structure following GitHub conventions
- Updated project files to reference new directory structure

### Removed
- Build artifacts and temporary files from repository

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