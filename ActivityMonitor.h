#pragma once

#include "common.h"
#include <functional>

/**
 * Manages mouse and keyboard activity monitoring
 * Handles mouse movement when inactive
 */
class ActivityMonitor
{
public:
    ActivityMonitor();
    ~ActivityMonitor();

    // Monitoring control
    bool StartMonitoring();
    void StopMonitoring();
    bool IsMonitoring() const { return m_isMonitoring; }

    // Activity tracking
    void UpdateActivityTime();
    void CheckActivity();

    // Configuration
    void SetTimeout(DWORD timeoutSeconds);
    DWORD GetTimeout() const { return m_timeoutSeconds; }
    bool ApplyTimeoutSetting();

    // Mouse movement
    void MoveMouse();

private:
    // Hook procedures (static members for Windows API compatibility)
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

    // Private helpers
    bool InstallHooks();
    void UninstallHooks();
    void StartTimer();
    void StopTimer();
    
    // Member variables
    bool m_isMonitoring = false;
    DWORD m_timeoutSeconds = DEFAULT_TIMEOUT_SECONDS;
    DWORD m_lastActivityTime = 0;
    
    // Windows hooks
    HHOOK m_mouseHook = nullptr;
    HHOOK m_keyboardHook = nullptr;
    UINT_PTR m_timerId = 0;
    
    // Random number generation for mouse movement
    std::random_device m_randomDevice;
    std::mt19937 m_randomGenerator;
    
    // Static instance pointer for hook procedures
    static ActivityMonitor* s_instance;
};