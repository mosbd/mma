#pragma once

#include "common.h"

/**
 * Manages application settings and registry operations
 */
class SettingsManager
{
public:
    struct Settings
    {
        DWORD timeoutSeconds = DEFAULT_TIMEOUT_SECONDS;
        bool minimizeToTray = true;
        bool startHidden = false;
        bool startMonitoring = false;
        bool startWithWindows = false;
        UINT hotkeyModifiers = MOD_CONTROL | MOD_SHIFT;
        UINT hotkeyVK = 'M';
    };

    SettingsManager();
    ~SettingsManager() = default;

    // Settings operations
    void LoadSettings();
    void SaveSettings();

    // Settings access
    const Settings& GetSettings() const { return m_settings; }
    void SetSettings(const Settings& settings) { m_settings = settings; }

    // Individual setting accessors
    DWORD GetTimeout() const { return m_settings.timeoutSeconds; }
    void SetTimeout(DWORD timeout);
    
    bool GetMinimizeToTray() const { return m_settings.minimizeToTray; }
    void SetMinimizeToTray(bool minimize) { m_settings.minimizeToTray = minimize; }
    
    bool GetStartHidden() const { return m_settings.startHidden; }
    void SetStartHidden(bool hidden) { m_settings.startHidden = hidden; }
    
    bool GetStartMonitoring() const { return m_settings.startMonitoring; }
    void SetStartMonitoring(bool monitor) { m_settings.startMonitoring = monitor; }
    
    bool GetStartWithWindows() const { return m_settings.startWithWindows; }
    void SetStartWithWindows(bool startup);
    
    UINT GetHotkeyModifiers() const { return m_settings.hotkeyModifiers; }
    void SetHotkeyModifiers(UINT modifiers) { m_settings.hotkeyModifiers = modifiers; }
    
    UINT GetHotkeyVK() const { return m_settings.hotkeyVK; }
    void SetHotkeyVK(UINT vk) { m_settings.hotkeyVK = vk; }

    // Windows startup management
    bool SetStartWithWindowsRegistry(bool enable);
    bool IsStartWithWindowsEnabled();

    // Utility
    WCHAR* GetApplicationPath();

private:
    // Registry operations
    bool LoadFromRegistry();
    bool SaveToRegistry();
    
    // Helper methods
    DWORD ReadRegistryDWORD(HKEY hKey, const WCHAR* valueName, DWORD defaultValue);
    bool WriteRegistryDWORD(HKEY hKey, const WCHAR* valueName, DWORD value);

    Settings m_settings;

    // Registry constants
    static const WCHAR* REG_KEY;
    static const WCHAR* REG_TIMEOUT;
    static const WCHAR* REG_MINIMIZE_TO_TRAY;
    static const WCHAR* REG_START_HIDDEN;
    static const WCHAR* REG_START_MONITORING;
    static const WCHAR* REG_START_WITH_WINDOWS;
    static const WCHAR* REG_HOTKEY_MODIFIERS;
    static const WCHAR* REG_HOTKEY_VK;
    static const WCHAR* STARTUP_REG_KEY;
    static const WCHAR* STARTUP_REG_VALUE;
};