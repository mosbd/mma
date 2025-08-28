# API Documentation

This document provides technical details about the MMA (Mouse & Keyboard Activity Monitor) application architecture and key components.

## Architecture Overview

MMA is built using a modular architecture with the following key components:

```
+--------------------+    +-------------------+    +-------------------+
| ApplicationManager |----| ActivityMonitor   |----|   Win32 Hooks     |
+--------------------+    +-------------------+    +-------------------+
         |                       |
         |              +-------------------+
         |              |  SettingsManager  |
         |              +-------------------+
         |
+-------------------+    +-------------------+    +-------------------+
|   SystemTray      |    |  HotkeyManager    |    | DialogManager     |
+-------------------+    +-------------------+    +-------------------+
```

## Single Instance Protection

### Overview
MMA implements single instance protection to ensure only one copy of the application runs at a time. This prevents conflicts, resource issues, and user confusion.

### Implementation Details

#### Named Mutex
```cpp
// Constants in common.h
const LPCWSTR APP_MUTEX_NAME = L"Global\\MMAApplication_SingleInstance_Mutex";

// Implementation in main.cpp
bool CheckSingleInstance()
{
    g_hSingleInstanceMutex = CreateMutexW(
        nullptr,        // Default security attributes
        TRUE,           // Initially owned
        APP_MUTEX_NAME  // Unique mutex name
    );
    
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        // Another instance is running
        BringExistingInstanceToFront();
        return false;
    }
    
    return true; // First instance
}
```

#### Window Detection and Activation
```cpp
struct FindWindowData
{
    HWND foundWindow;
    DWORD targetProcessId;
};

BOOL CALLBACK FindWindowCallback(HWND hwnd, LPARAM lParam)
{
    // 1. Check window class (dialog: #32770)
    // 2. Check window title contains app name
    // 3. Verify different process ID
    // 4. Store handle and stop enumeration
}

bool BringExistingInstanceToFront()
{
    // 1. Enumerate all windows
    // 2. Find matching application window
    // 3. Restore if minimized
    // 4. Bring to foreground with fallback methods
    // 5. Flash window for user attention
}
```

#### Foreground Handling
```cpp
// Primary method
SetForegroundWindow(existingWindow);

// Fallback for restricted scenarios
DWORD dwMyID = GetCurrentThreadId();
DWORD dwCurID = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);

if (AttachThreadInput(dwCurID, dwMyID, TRUE))
{
    SetForegroundWindow(existingWindow);
    AttachThreadInput(dwCurID, dwMyID, FALSE);
}
```

### Security Considerations
- Uses `Global\\` prefix for system-wide mutex visibility
- Process ID verification prevents false window matches
- Graceful handling of permission failures
- No elevation requirements for basic functionality

## Core Components

### ApplicationManager
**File**: `ApplicationManager.cpp` / `ApplicationManager.h`

Main application controller that coordinates all other components.

#### Key Methods
```cpp
class ApplicationManager {
public:
    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void Shutdown();
    
    void StartMonitoring();
    void StopMonitoring();
    void ToggleMainWindow();
    
private:
    HINSTANCE m_hInstance;
    HWND m_hWnd;
    bool m_isRunning;
};
```

#### Responsibilities
- Application lifecycle management
- Window message handling
- Component coordination
- System shutdown handling

### ActivityMonitor
**File**: `ActivityMonitor.cpp` / `ActivityMonitor.h`

Monitors system-wide mouse and keyboard activity using low-level Windows hooks.

#### Key Methods
```cpp
class ActivityMonitor {
public:
    bool StartMonitoring();
    void StopMonitoring();
    void SetTimeout(int seconds);
    void ResetTimer();
    
    bool IsMonitoring() const;
    int GetRemainingTime() const;
    
private:
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    
    void MoveMouseToRandomPosition();
    
    HHOOK m_mouseHook;
    HHOOK m_keyboardHook;
    UINT_PTR m_timerId;
    int m_timeoutSeconds;
};
```

#### Technical Details
- Uses `WH_MOUSE_LL` and `WH_KEYBOARD_LL` hooks
- Implements timer-based timeout mechanism
- Generates cryptographically secure random mouse positions
- Thread-safe operation with static callback methods

### SettingsManager
**File**: `SettingsManager.cpp` / `SettingsManager.h`

Handles persistent storage of application settings in Windows Registry.

#### Key Methods
```cpp
class SettingsManager {
public:
    bool LoadSettings();
    bool SaveSettings();
    
    // Timeout settings
    int GetTimeout() const;
    void SetTimeout(int seconds);
    
    // Hotkey settings
    UINT GetHotkeyModifiers() const;
    UINT GetHotkeyVirtualKey() const;
    void SetHotkey(UINT modifiers, UINT virtualKey);
    
    // Startup behavior
    bool GetMinimizeToTray() const;
    bool GetStartHidden() const;
    bool GetStartMonitoringAutomatically() const;
    bool GetStartWithWindows() const;
    
private:
    static const LPCWSTR REGISTRY_KEY;
    static const LPCWSTR VALUE_TIMEOUT;
    static const LPCWSTR VALUE_HOTKEY_MODIFIERS;
    // ... other constants
};
```

#### Registry Structure
```
HKEY_CURRENT_USER\SOFTWARE\MMA\
+-- Timeout (DWORD)
+-- HotkeyModifiers (DWORD)
+-- HotkeyVirtualKey (DWORD)
+-- MinimizeToTray (DWORD)
+-- StartHidden (DWORD)
+-- StartMonitoringAutomatically (DWORD)
+-- StartWithWindows (DWORD)
```

### HotkeyManager
**File**: `HotkeyManager.cpp` / `HotkeyManager.h`

Manages global hotkey registration and handling.

#### Key Methods
```cpp
class HotkeyManager {
public:
    bool RegisterHotkey(HWND hwnd, UINT modifiers, UINT virtualKey);
    void UnregisterHotkey(HWND hwnd);
    
    static bool IsHotkeyMessage(UINT message, WPARAM wParam);
    
private:
    static const int HOTKEY_ID = 1;
    bool m_isRegistered;
    HWND m_hwnd;
};
```

#### Supported Modifiers
- `MOD_CONTROL` - Ctrl key
- `MOD_ALT` - Alt key
- `MOD_SHIFT` - Shift key
- `MOD_WIN` - Windows key

### SystemTray
**File**: `SystemTray.cpp` / `SystemTray.h`

Handles system tray integration and context menu.

#### Key Methods
```cpp
class SystemTray {
public:
    bool Initialize(HWND hwnd, HINSTANCE hInstance);
    void Cleanup();
    
    void SetIcon(bool monitoring);
    void SetTooltip(const std::wstring& tooltip);
    void ShowBalloonTip(const std::wstring& title, const std::wstring& text);
    
    bool HandleTrayMessage(WPARAM wParam, LPARAM lParam);
    
private:
    NOTIFYICONDATA m_notifyIconData;
    HWND m_hwnd;
    HINSTANCE m_hInstance;
    
    void ShowContextMenu();
};
```

### DialogManager
**File**: `DialogManager.cpp` / `DialogManager.h`

Manages dialog windows and user interface interactions.

#### Key Methods
```cpp
class DialogManager {
public:
    static INT_PTR ShowHotkeyDialog(HWND parent, UINT& modifiers, UINT& virtualKey);
    static INT_PTR ShowAboutDialog(HWND parent);
    
private:
    static INT_PTR CALLBACK HotkeyDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};
```

## Message Flow

### Application Startup
1. `CheckSingleInstance()` - Verify no other instance running
2. `WinMain` entry point
3. `ApplicationManager::Initialize()`
4. Load settings via `SettingsManager`
5. Initialize system tray via `SystemTray`
6. Register global hotkey via `HotkeyManager`
7. Start monitoring if configured

### Single Instance Detection
1. Second instance attempts to start
2. `CheckSingleInstance()` detects existing mutex
3. `BringExistingInstanceToFront()` called
4. Window enumeration finds existing instance
5. Existing window brought to foreground
6. Second instance exits gracefully

### Activity Detection
1. Windows generates mouse/keyboard events
2. Low-level hooks capture events
3. `ActivityMonitor` processes events
4. Timer is reset on activity
5. Timeout triggers mouse movement

### Settings Changes
1. User modifies settings in UI
2. `SettingsManager::SaveSettings()` called
3. Changes written to registry
4. Components updated with new settings

## Threading Model

MMA uses a single-threaded apartment model:
- Main UI thread handles all Windows messages
- System hooks execute on the main thread
- Timer callbacks execute on the main thread
- No additional worker threads are used
- Single instance detection is synchronous

## Error Handling

### Return Codes
- `TRUE`/`FALSE` for Win32 API success/failure
- `S_OK`/`E_FAIL` for COM operations
- Custom error codes for application-specific errors

### Error Sources
- Registry access failures
- Hook installation failures
- Window creation failures
- Resource loading failures
- Mutex creation failures

## Performance Considerations

### Memory Usage
- Minimal static allocation
- RAII pattern for resource management
- Automatic cleanup on destruction
- Single mutex handle per instance

### CPU Usage
- Efficient hook procedures
- Minimal processing in callback functions
- Timer-based activity checking
- Fast window enumeration

## Security Considerations

### Privilege Requirements
- Standard user privileges for basic operation
- Administrator privileges for startup registry modification
- No elevation prompts during normal operation

### Single Instance Security
- Global mutex prevents user-level conflicts
- Process ID verification prevents spoofing
- No shared memory or IPC vulnerabilities
- Graceful handling of permission failures

### System Integration
- Uses only documented Windows APIs
- No direct memory manipulation
- Respects Windows security boundaries

## Testing Hooks

For testing purposes, you can use these debugging aids:

```cpp
#ifdef _DEBUG
    // Enable debug output
    #define DEBUG_PRINT(msg) OutputDebugStringA(msg)
#else
    #define DEBUG_PRINT(msg)
#endif
```

## Extension Points

The architecture supports extensions in these areas:
- Additional activity sources (network, disk I/O)
- Alternative mouse movement patterns
- Custom notification methods
- Additional hotkey combinations
- Alternative single instance mechanisms (named pipes, shared memory)