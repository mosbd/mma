#include "ActivityMonitor.h"
#include "ApplicationManager.h"
#include "SystemTray.h"
#include "SettingsManager.h"
#include "resource_new.h"

// Static member definition
ActivityMonitor* ActivityMonitor::s_instance = nullptr;

ActivityMonitor::ActivityMonitor() 
    : m_randomGenerator(m_randomDevice())
{
    s_instance = this;
    UpdateActivityTime();
}

ActivityMonitor::~ActivityMonitor()
{
    StopMonitoring();
    s_instance = nullptr;
}

bool ActivityMonitor::StartMonitoring()
{
    if (m_isMonitoring)
        return true;

    // Validate timeout from UI
    auto& app = ApplicationManager::GetInstance();
    HWND hDlg = app.GetMainDialog();
    
    if (hDlg)
    {
        BOOL translated;
        UINT timeout = GetDlgItemInt(hDlg, IDC_TIMEOUT_EDIT, &translated, FALSE);
        if (translated && timeout > 0 && timeout <= MAX_TIMEOUT_SECONDS)
        {
            m_timeoutSeconds = timeout;
        }
    }

    // Install hooks
    if (!InstallHooks())
    {
        MessageBoxW(app.GetMainDialog(), 
            L"Failed to install system hooks. Try running as administrator.", 
            L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_isMonitoring = true;
    UpdateActivityTime();
    
    // Start timer
    StartTimer();
    
    // Update tray icon
    app.GetSystemTray()->SetMonitoringState(true);
    
    return true;
}

void ActivityMonitor::StopMonitoring()
{
    if (!m_isMonitoring)
        return;

    m_isMonitoring = false;

    // Remove hooks
    UninstallHooks();
    
    // Stop timer
    StopTimer();
    
    // Update tray icon
    auto& app = ApplicationManager::GetInstance();
    app.GetSystemTray()->SetMonitoringState(false);
}

void ActivityMonitor::UpdateActivityTime()
{
    m_lastActivityTime = GetTickCount();
}

void ActivityMonitor::CheckActivity()
{
    if (!m_isMonitoring)
        return;

    DWORD currentTime = GetTickCount();
    DWORD elapsed = (currentTime - m_lastActivityTime) / 1000; // Convert to seconds

    if (elapsed >= m_timeoutSeconds)
    {
        MoveMouse();
        UpdateActivityTime(); // Reset timer after moving mouse
    }
}

void ActivityMonitor::SetTimeout(DWORD timeoutSeconds)
{
    if (timeoutSeconds > 0 && timeoutSeconds <= MAX_TIMEOUT_SECONDS)
    {
        m_timeoutSeconds = timeoutSeconds;
        
        // Reset activity timer if monitoring is active
        if (m_isMonitoring)
        {
            UpdateActivityTime();
        }
    }
}

bool ActivityMonitor::ApplyTimeoutSetting()
{
    auto& app = ApplicationManager::GetInstance();
    HWND hDlg = app.GetMainDialog();
    
    if (!hDlg)
        return false;
        
    BOOL translated;
    UINT newTimeout = GetDlgItemInt(hDlg, IDC_TIMEOUT_EDIT, &translated, FALSE);
    
    if (!translated)
    {
        MessageBoxW(hDlg, L"Please enter a valid number for the timeout value.", 
                   L"Invalid Input", MB_OK | MB_ICONWARNING);
        // Reset to current value
        SetDlgItemInt(hDlg, IDC_TIMEOUT_EDIT, m_timeoutSeconds, FALSE);
        return false;
    }
    
    if (newTimeout < 1 || newTimeout > MAX_TIMEOUT_SECONDS)
    {
        MessageBoxW(hDlg, L"Timeout must be between 1 and 3600 seconds (1 hour).", 
                   L"Invalid Range", MB_OK | MB_ICONWARNING);
        // Reset to current value
        SetDlgItemInt(hDlg, IDC_TIMEOUT_EDIT, m_timeoutSeconds, FALSE);
        return false;
    }
    
    // Apply the new timeout
    SetTimeout(newTimeout);
    app.GetSettingsManager()->SetTimeout(newTimeout);
    app.GetSettingsManager()->SaveSettings();
    
    // Show confirmation
    WCHAR message[256];
    swprintf_s(message, L"Timeout updated to %d seconds and applied immediately.", newTimeout);
    MessageBoxW(hDlg, message, L"Settings Applied", MB_OK | MB_ICONINFORMATION);
    
    return true;
}

void ActivityMonitor::MoveMouse()
{
    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Generate random position
    std::uniform_int_distribution<> xDist(0, screenWidth - 1);
    std::uniform_int_distribution<> yDist(0, screenHeight - 1);
    
    int newX = xDist(m_randomGenerator);
    int newY = yDist(m_randomGenerator);

    // Move mouse cursor
    SetCursorPos(newX, newY);
}

LRESULT CALLBACK ActivityMonitor::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && s_instance)
    {
        s_instance->UpdateActivityTime();
    }
    return CallNextHookEx(s_instance ? s_instance->m_mouseHook : nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK ActivityMonitor::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && s_instance)
    {
        s_instance->UpdateActivityTime();
    }
    return CallNextHookEx(s_instance ? s_instance->m_keyboardHook : nullptr, nCode, wParam, lParam);
}

bool ActivityMonitor::InstallHooks()
{
    auto& app = ApplicationManager::GetInstance();
    HINSTANCE hInstance = app.GetAppInstance();
    
    m_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hInstance, 0);
    m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, hInstance, 0);

    return (m_mouseHook != nullptr && m_keyboardHook != nullptr);
}

void ActivityMonitor::UninstallHooks()
{
    if (m_mouseHook)
    {
        UnhookWindowsHookEx(m_mouseHook);
        m_mouseHook = nullptr;
    }
    
    if (m_keyboardHook)
    {
        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = nullptr;
    }
}

void ActivityMonitor::StartTimer()
{
    auto& app = ApplicationManager::GetInstance();
    HWND hDlg = app.GetMainDialog();
    
    if (hDlg)
    {
        m_timerId = SetTimer(hDlg, 1, 1000, nullptr); // Check every second
    }
}

void ActivityMonitor::StopTimer()
{
    auto& app = ApplicationManager::GetInstance();
    HWND hDlg = app.GetMainDialog();
    
    if (hDlg && m_timerId)
    {
        KillTimer(hDlg, m_timerId);
        m_timerId = 0;
    }
}