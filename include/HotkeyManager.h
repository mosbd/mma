#pragma once

#include "common.h"

/**
 * Manages global hotkey registration and handling
 */
class HotkeyManager
{
public:
    HotkeyManager();
    ~HotkeyManager();

    // Hotkey management
    bool RegisterGlobalHotkey(HWND hWnd, UINT modifiers, UINT vk);
    void UnregisterGlobalHotkey();
    bool IsHotkeyRegistered() const { return m_hotkeyRegistered; }

    // Hotkey configuration
    void SetHotkey(UINT modifiers, UINT vk);
    UINT GetModifiers() const { return m_modifiers; }
    UINT GetVirtualKey() const { return m_virtualKey; }

    // Message handling
    bool HandleHotkeyMessage(WPARAM wParam);

    // Utility functions for UI
    static const WCHAR* GetKeyName(UINT vk);
    static const WCHAR* GetModifierString(UINT modifiers);
    static WCHAR* GetHotkeyDisplayString(UINT modifiers, UINT vk);

private:
    // Private helpers
    void UpdateRegistration();

    // Member variables
    bool m_hotkeyRegistered = false;
    UINT m_modifiers = MOD_CONTROL | MOD_SHIFT;
    UINT m_virtualKey = 'M';
    HWND m_targetWindow = nullptr;
    
    static const int HOTKEY_ID = 1;
};