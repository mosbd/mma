#pragma once

#include "common.h"
#include <memory>
#include <windows.h>

// Forward declarations
class ActivityMonitor;
class SystemTray;
class SettingsManager;
class HotkeyManager;
class DialogManager;

/**
 * Main application manager class that coordinates all subsystems
 * Implements Singleton pattern to ensure single instance
 */
class ApplicationManager
{
public:
    // Singleton access
    static ApplicationManager& GetInstance();
    static void DestroyInstance();

    // Application lifecycle
    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Shutdown();
    int Run();

    // Access to subsystems
    ActivityMonitor* GetActivityMonitor() const { return m_activityMonitor.get(); }
    SystemTray* GetSystemTray() const { return m_systemTray.get(); }
    SettingsManager* GetSettingsManager() const { return m_settingsManager.get(); }
    HotkeyManager* GetHotkeyManager() const { return m_hotkeyManager.get(); }
    DialogManager* GetDialogManager() const { return m_dialogManager.get(); }

    // Application state
    HINSTANCE GetAppInstance() const { return m_hInstance; }
    HWND GetMainDialog() const { return m_hMainDlg; }
    void SetMainDialog(HWND hDlg) { m_hMainDlg = hDlg; }

    // Message handling
    bool HandleMessage(MSG& msg);
    
    // Hotkey handling
    void HandleHotkeyToggle();

    ~ApplicationManager() = default;
private:
    ApplicationManager() = default;
    ApplicationManager(const ApplicationManager&) = delete;
    ApplicationManager& operator=(const ApplicationManager&) = delete;

    // Initialization helpers
    bool InitializeSubsystems();
    void LoadGlobalStrings();
    bool CreateMainDialog();

    static std::unique_ptr<ApplicationManager> s_instance;

    // Application data
    HINSTANCE m_hInstance = nullptr;
    HWND m_hMainDlg = nullptr;
    WCHAR m_szTitle[MAX_LOADSTRING];
    WCHAR m_szWindowClass[MAX_LOADSTRING];

    // Subsystem managers
    std::unique_ptr<SettingsManager> m_settingsManager;
    std::unique_ptr<ActivityMonitor> m_activityMonitor;
    std::unique_ptr<SystemTray> m_systemTray;
    std::unique_ptr<HotkeyManager> m_hotkeyManager;
    std::unique_ptr<DialogManager> m_dialogManager;
};