#include "HotkeyManager.h"
#include "ApplicationManager.h"
#include "SystemTray.h"

HotkeyManager::HotkeyManager()
{
}

HotkeyManager::~HotkeyManager()
{
    UnregisterGlobalHotkey();
}

bool HotkeyManager::RegisterGlobalHotkey(HWND hWnd, UINT modifiers, UINT vk)
{
    // Unregister existing hotkey first
    UnregisterGlobalHotkey();
    
    m_targetWindow = hWnd;
    m_modifiers = modifiers;
    m_virtualKey = vk;
    
    if (RegisterHotKey(hWnd, HOTKEY_ID, modifiers, vk))
    {
        m_hotkeyRegistered = true;
        return true;
    }
    
    return false;
}

void HotkeyManager::UnregisterGlobalHotkey()
{
    if (m_hotkeyRegistered && m_targetWindow)
    {
        UnregisterHotKey(m_targetWindow, HOTKEY_ID);
        m_hotkeyRegistered = false;
    }
}

void HotkeyManager::SetHotkey(UINT modifiers, UINT vk)
{
    m_modifiers = modifiers;
    m_virtualKey = vk;
    
    // Update registration if currently registered
    if (m_hotkeyRegistered && m_targetWindow)
    {
        RegisterGlobalHotkey(m_targetWindow, modifiers, vk);
    }
}

bool HotkeyManager::HandleHotkeyMessage(WPARAM wParam)
{
    if (wParam == HOTKEY_ID)
    {
        // Toggle monitoring state instead of dialog visibility
        auto& app = ApplicationManager::GetInstance();
        
        // Call a method on ApplicationManager to handle the hotkey
        app.HandleHotkeyToggle();
        
        return true;
    }
    
    return false;
}

const WCHAR* HotkeyManager::GetKeyName(UINT vk)
{
    // Find the key name in the global mapping array
    for (int i = 0; i < g_keyMappingsCount; ++i)
    {
        if (g_keyMappings[i].vk == vk)
        {
            return g_keyMappings[i].name;
        }
    }
    
    return L"Unknown";
}

const WCHAR* HotkeyManager::GetModifierString(UINT modifiers)
{
    static WCHAR buffer[256];
    buffer[0] = L'\0';
    
    if (modifiers & MOD_CONTROL)
    {
        wcscat_s(buffer, L"Ctrl");
    }
    
    if (modifiers & MOD_SHIFT)
    {
        if (wcslen(buffer) > 0) wcscat_s(buffer, L"+");
        wcscat_s(buffer, L"Shift");
    }
    
    if (modifiers & MOD_ALT)
    {
        if (wcslen(buffer) > 0) wcscat_s(buffer, L"+");
        wcscat_s(buffer, L"Alt");
    }
    
    if (modifiers & MOD_WIN)
    {
        if (wcslen(buffer) > 0) wcscat_s(buffer, L"+");
        wcscat_s(buffer, L"Win");
    }
    
    return buffer;
}

WCHAR* HotkeyManager::GetHotkeyDisplayString(UINT modifiers, UINT vk)
{
    static WCHAR buffer[512];
    const WCHAR* modifierStr = GetModifierString(modifiers);
    const WCHAR* keyName = GetKeyName(vk);
    
    if (wcslen(modifierStr) > 0)
    {
        swprintf_s(buffer, L"%s+%s", modifierStr, keyName);
    }
    else
    {
        wcscpy_s(buffer, keyName);
    }
    
    return buffer;
}