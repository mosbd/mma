# Contributing to Mouse & Keyboard Activity Monitor (MMA)

Thank you for your interest in contributing to MMA! This document provides guidelines for contributing to the project.

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/yourusername/mma.git`
3. Create a feature branch: `git checkout -b feature-name`
4. Make your changes
5. Test your changes
6. Commit with a clear message: `git commit -m "Add feature description"`
7. Push to your fork: `git push origin feature-name`
8. Create a Pull Request

## Development Environment

### Requirements
- Visual Studio 2019 or later
- Windows SDK 10.0 or later
- C++14 compiler support

### Building
1. Open `mma.sln` in Visual Studio
2. Select your target configuration (Debug/Release) and platform (x86/x64)
3. Build the solution (F7 or Build > Build Solution)

## Code Style

- Follow existing code conventions
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and concise

## Project Structure

```
mma/
??? src/           # Source files (.cpp)
??? include/       # Header files (.h)
??? assets/        # Resource files (.ico, .rc)
??? docs/          # Documentation
??? README.md      # Main project documentation
??? CONTRIBUTING.md # This file
??? LICENSE        # Project license
??? .gitignore     # Git ignore rules
```

## Testing

- Test all functionality before submitting
- Verify the application works in both Debug and Release builds
- Test on different Windows versions if possible
- Ensure no memory leaks or resource issues

## Reporting Issues

When reporting bugs, please include:
- Windows version
- Steps to reproduce
- Expected vs actual behavior
- Any error messages
- Relevant log information

## Pull Request Guidelines

- Keep changes focused on a single feature or fix
- Update documentation if needed
- Test thoroughly
- Write clear commit messages
- Squash commits if necessary

## Questions?

Feel free to open an issue for any questions about contributing!