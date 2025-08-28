# Mouse & Keyboard Activity Monitor (MMA)

A Windows application that monitors mouse and keyboard activity and automatically moves the mouse cursor to a random position when no activity is detected for a specified period.

## Features

1. **Activity Monitoring**: Monitors both mouse and keyboard events system-wide
2. **Configurable Timeout**: Set the inactivity period (default: 30 seconds, max: 3600 seconds)
3. **Instant Timeout Updates**: Apply timeout changes immediately with the "Apply" button
4. **Random Mouse Movement**: Moves mouse cursor to random screen position when timeout expires
5. **Customizable Global Hotkey**: Configure any combination of modifier keys (Ctrl, Alt, Shift, Win) with function keys, letters, numbers, or special keys
6. **System Tray Integration**: 
   - Minimize to system tray instead of taskbar
   - Tray icon shows current monitoring status
   - Right-click tray icon for context menu
   - Double-click tray icon to show main window
7. **Startup Behavior Options**:
   - Start application hidden (tray only)
   - Start monitoring automatically
   - Minimize to tray instead of taskbar
   - **Start with Windows** - Automatically launch when Windows starts
8. **Settings Persistence**: All settings are saved in Windows registry

## Usage

### Basic Operation
1. Run the application (`mma.exe`)
2. Set the desired timeout period in seconds
3. Click "Apply" to update the timeout immediately (works even while monitoring)
4. Click "Start Monitoring" or use the global hotkey to start
5. The application will monitor for mouse and keyboard activity
6. If no activity is detected within the timeout period, the mouse cursor will move to a random position

### Hotkey Configuration
- **Set Hotkey Button**: Opens the hotkey configuration dialog
- **Modifier Keys**: Choose any combination of Ctrl, Alt, Shift, and Win keys
- **Key Selection**: Choose from F1-F12, A-Z, 0-9, or special keys (Space, Enter, Arrow keys, etc.)
- **Live Preview**: See your hotkey combination as you configure it
- **Reset to Default**: Quickly restore the default Ctrl+Shift+M hotkey
- **Validation**: Ensures at least one modifier key is selected for security

### Timeout Configuration
- **Edit Field**: Enter timeout value (1-3600 seconds)
- **Apply Button**: Instantly applies the new timeout value
- **Real-time Updates**: Changes take effect immediately, even during active monitoring
- **Input Validation**: Shows error messages for invalid values and reverts to current setting
- **Confirmation**: Displays success message when timeout is updated

### Tray Icon Features
- **Double-click**: Show/hide main window
- **Right-click**: Access context menu with:
  - Show Window
  - Start/Stop Monitoring
  - About
  - Exit

### Startup Options
- **Minimize to system tray**: When checked, minimizing the window will hide it to the system tray
- **Start application hidden**: When checked, the application will start with only the tray icon visible
- **Start monitoring automatically**: When checked, monitoring will begin automatically when the application starts
- **Start with Windows**: When checked, the application will automatically launch when Windows starts

## Hotkey Examples

- **Default**: Ctrl+Shift+M
- **Function Key**: Ctrl+F12
- **Alt Combination**: Alt+Shift+H
- **Win Key**: Win+Ctrl+Space
- **Simple**: Ctrl+Q

## Windows Startup Integration

The "Start with Windows" feature adds the application to Windows startup registry. When enabled:
- The application will automatically launch when Windows starts
- Works with the current executable path (handles moved files automatically)
- Can be combined with "Start application hidden" for completely silent startup
- Registry entry is added to `HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run`

**Note**: If the feature fails to enable/disable, try running the application as administrator.

## Technical Details

- Built with C++ and Windows SDK (raw Win32 API)
- Uses low-level system hooks (WH_MOUSE_LL and WH_KEYBOARD_LL)
- System tray integration using Shell_NotifyIcon API
- Global hotkey registration using RegisterHotKey API
- Settings stored in HKEY_CURRENT_USER\SOFTWARE\MMA
- Windows startup integration via registry manipulation
- Requires Windows Vista or later
- May require administrator privileges for proper hook installation and startup registry modification

## Building

This project uses Visual Studio and requires:
- Windows SDK
- C++14 compiler support

Simply open the solution file and build in Visual Studio.

## Registry Settings

The application stores settings in the Windows registry under:
`HKEY_CURRENT_USER\SOFTWARE\MMA`

Settings include:
- Timeout value
- Hotkey modifier keys and virtual key code
- Minimize to tray preference
- Start hidden preference  
- Start monitoring automatically preference
- Start with Windows preference

Windows startup is managed via:
`HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run`

## Supported Hotkey Keys

### Function Keys
F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12

### Letters
A through Z

### Numbers
0 through 9

### Special Keys
- Space, Enter, Escape, Tab
- Backspace, Delete, Insert
- Home, End, Page Up, Page Down
- Arrow Keys (Up, Down, Left, Right)

### Modifier Keys
- Ctrl (Control)
- Alt
- Shift
- Win (Windows key)

## Notes

- The application resets the activity timer after each automatic mouse movement
- Global hotkey is automatically registered when the application starts
- The timeout value is validated (1-3600 seconds range)
- The application uses a secure random number generator for mouse positioning
- Tray icon tooltip shows current monitoring status
- Settings are automatically saved when changed
- Windows startup setting is verified on application startup
- Application path is automatically quoted in registry to handle paths with spaces
- **New**: Timeout changes can be applied instantly without restarting monitoring
- **New**: Apply button provides immediate feedback and validation
- **New**: Complete hotkey customization with intuitive dialog interface
- **New**: Hotkey validation prevents conflicts and ensures usability