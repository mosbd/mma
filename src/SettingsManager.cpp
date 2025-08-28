#include "SettingsManager.h"
#include "ApplicationManager.h"

// Static member definitions
const WCHAR* SettingsManager::REG_KEY = L"SOFTWARE\\MMA";
const WCHAR* SettingsManager::REG_TIMEOUT = L"Timeout";
const WCHAR* SettingsManager::REG_MINIMIZE_TO_TRAY = L"MinimizeToTray";
const WCHAR* SettingsManager::REG_START_HIDDEN = L"StartHidden";
const WCHAR* SettingsManager::REG_START_MONITORING = L"StartMonitoring";
const WCHAR* SettingsManager::REG_START_WITH_WINDOWS = L"StartWithWindows";
const WCHAR* SettingsManager::REG_HOTKEY_MODIFIERS = L"HotkeyModifiers";
const WCHAR* SettingsManager::REG_HOTKEY_VK = L"HotkeyVK";
const WCHAR* SettingsManager::STARTUP_REG_KEY = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
const WCHAR* SettingsManager::STARTUP_REG_VALUE = L"MMA";

SettingsManager::SettingsManager()
{
    // Initialize with default values
}

void SettingsManager::LoadSettings()
{
    LoadFromRegistry();
    
    // Verify that Windows startup setting matches registry
    m_settings.startWithWindows = IsStartWithWindowsEnabled();
}

void SettingsManager::SaveSettings()
{
    SaveToRegistry();
}

void SettingsManager::SetTimeout(DWORD timeout)
{
    if (timeout > 0 && timeout <= MAX_TIMEOUT_SECONDS)
    {
        m_settings.timeoutSeconds = timeout;
    }
}

void SettingsManager::SetStartWithWindows(bool startup)
{
    m_settings.startWithWindows = startup;
    SetStartWithWindowsRegistry(startup);
}

bool SettingsManager::SetStartWithWindowsRegistry(bool enable)
{
    HKEY hKey;
    LONG result;
    
    result = RegOpenKeyExW(HKEY_CURRENT_USER, STARTUP_REG_KEY, 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS)
    {
        return false;
    }
    
    if (enable)
    {
        // Add to startup
        WCHAR* appPath = GetApplicationPath();
        if (appPath)
        {
            // Add quotes around path to handle spaces
            WCHAR quotedPath[MAX_PATH + 4];
            swprintf_s(quotedPath, L"\"%s\"", appPath);
            
            result = RegSetValueExW(hKey, STARTUP_REG_VALUE, 0, REG_SZ, 
                                   (LPBYTE)quotedPath, (wcslen(quotedPath) + 1) * sizeof(WCHAR));
            delete[] appPath;
        }
        else
        {
            result = ERROR_FILE_NOT_FOUND;
        }
    }
    else
    {
        // Remove from startup
        result = RegDeleteValueW(hKey, STARTUP_REG_VALUE);
        // ERROR_FILE_NOT_FOUND is OK - means it wasn't there to begin with
        if (result == ERROR_FILE_NOT_FOUND)
        {
            result = ERROR_SUCCESS;
        }
    }
    
    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

bool SettingsManager::IsStartWithWindowsEnabled()
{
    HKEY hKey;
    LONG result;
    bool isEnabled = false;
    
    result = RegOpenKeyExW(HKEY_CURRENT_USER, STARTUP_REG_KEY, 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS)
    {
        DWORD dwType;
        DWORD dwSize = 0;
        
        // Check if our value exists
        result = RegQueryValueExW(hKey, STARTUP_REG_VALUE, nullptr, &dwType, nullptr, &dwSize);
        if (result == ERROR_SUCCESS && dwType == REG_SZ && dwSize > 0)
        {
            isEnabled = true;
        }
        
        RegCloseKey(hKey);
    }
    
    return isEnabled;
}

WCHAR* SettingsManager::GetApplicationPath()
{
    WCHAR* path = new WCHAR[MAX_PATH];
    if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0)
    {
        delete[] path;
        return nullptr;
    }
    return path;
}

bool SettingsManager::LoadFromRegistry()
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
        return false;
    }

    m_settings.timeoutSeconds = ReadRegistryDWORD(hKey, REG_TIMEOUT, DEFAULT_TIMEOUT_SECONDS);
    m_settings.minimizeToTray = ReadRegistryDWORD(hKey, REG_MINIMIZE_TO_TRAY, 1) != 0;
    m_settings.startHidden = ReadRegistryDWORD(hKey, REG_START_HIDDEN, 0) != 0;
    m_settings.startMonitoring = ReadRegistryDWORD(hKey, REG_START_MONITORING, 0) != 0;
    m_settings.startWithWindows = ReadRegistryDWORD(hKey, REG_START_WITH_WINDOWS, 0) != 0;
    m_settings.hotkeyModifiers = ReadRegistryDWORD(hKey, REG_HOTKEY_MODIFIERS, MOD_CONTROL | MOD_SHIFT);
    m_settings.hotkeyVK = ReadRegistryDWORD(hKey, REG_HOTKEY_VK, 'M');

    // Validate timeout range
    if (m_settings.timeoutSeconds < 1 || m_settings.timeoutSeconds > MAX_TIMEOUT_SECONDS)
    {
        m_settings.timeoutSeconds = DEFAULT_TIMEOUT_SECONDS;
    }

    RegCloseKey(hKey);
    return true;
}

bool SettingsManager::SaveToRegistry()
{
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, nullptr, REG_OPTION_NON_VOLATILE, 
                       KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS)
    {
        return false;
    }

    bool success = true;
    success &= WriteRegistryDWORD(hKey, REG_TIMEOUT, m_settings.timeoutSeconds);
    success &= WriteRegistryDWORD(hKey, REG_MINIMIZE_TO_TRAY, m_settings.minimizeToTray ? 1 : 0);
    success &= WriteRegistryDWORD(hKey, REG_START_HIDDEN, m_settings.startHidden ? 1 : 0);
    success &= WriteRegistryDWORD(hKey, REG_START_MONITORING, m_settings.startMonitoring ? 1 : 0);
    success &= WriteRegistryDWORD(hKey, REG_START_WITH_WINDOWS, m_settings.startWithWindows ? 1 : 0);
    success &= WriteRegistryDWORD(hKey, REG_HOTKEY_MODIFIERS, m_settings.hotkeyModifiers);
    success &= WriteRegistryDWORD(hKey, REG_HOTKEY_VK, m_settings.hotkeyVK);

    RegCloseKey(hKey);
    return success;
}

DWORD SettingsManager::ReadRegistryDWORD(HKEY hKey, const WCHAR* valueName, DWORD defaultValue)
{
    DWORD dwValue = defaultValue;
    DWORD dwSize = sizeof(DWORD);
    DWORD dwType;
    
    if (RegQueryValueExW(hKey, valueName, nullptr, &dwType, (LPBYTE)&dwValue, &dwSize) != ERROR_SUCCESS ||
        dwType != REG_DWORD)
    {
        dwValue = defaultValue;
    }
    
    return dwValue;
}

bool SettingsManager::WriteRegistryDWORD(HKEY hKey, const WCHAR* valueName, DWORD value)
{
    return RegSetValueExW(hKey, valueName, 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD)) == ERROR_SUCCESS;
}