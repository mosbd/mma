#include "ApplicationManager.h"
#include "ActivityMonitor.h"
#include "SystemTray.h"
#include "SettingsManager.h"
#include "HotkeyManager.h"
#include "DialogManager.h"
#include "resource_new.h"
#include <memory>

// Static member definition
std::unique_ptr<ApplicationManager> ApplicationManager::s_instance = nullptr;

ApplicationManager& ApplicationManager::GetInstance()
{
    if (!s_instance)
    {
        s_instance = std::unique_ptr<ApplicationManager>(new ApplicationManager());
    }
    return *s_instance;
}

void ApplicationManager::DestroyInstance()
{
    s_instance.reset();
}

bool ApplicationManager::Initialize(HINSTANCE hInstance, int nCmdShow)
{
    m_hInstance = hInstance;
    
    // Load global strings
    LoadGlobalStrings();
    
    // Initialize subsystems
    if (!InitializeSubsystems())
    {
        return false;
    }
    
    // Load settings
    m_settingsManager->LoadSettings();
    
    // Initialize hotkey manager with current settings
    m_hotkeyManager->SetHotkey(
        m_settingsManager->GetHotkeyModifiers(),
        m_settingsManager->GetHotkeyVK());
    
    // Create main dialog based on settings
    if (m_settingsManager->GetStartHidden())
    {
        // Create dialog but don't show it
        m_hMainDlg = m_dialogManager->CreateMainDialog(hInstance);
        if (!m_hMainDlg)
        {
            return false;
        }
        
        // Add tray icon
        m_systemTray->AddTrayIcon(m_hMainDlg);
        
        // Register hotkey
        m_hotkeyManager->RegisterGlobalHotkey(m_hMainDlg, 
            m_settingsManager->GetHotkeyModifiers(),
            m_settingsManager->GetHotkeyVK());
        
        // Start monitoring if configured
        if (m_settingsManager->GetStartMonitoring())
        {
            m_activityMonitor->StartMonitoring();
            m_dialogManager->UpdateUI();
        }
    }
    else
    {
        // Show main dialog normally
        m_hMainDlg = m_dialogManager->CreateMainDialog(hInstance);
        if (!m_hMainDlg)
        {
            return false;
        }
        
        ShowWindow(m_hMainDlg, nCmdShow);
        UpdateWindow(m_hMainDlg);
    }
    
    return true;
}

void ApplicationManager::Shutdown()
{
    // Stop monitoring
    if (m_activityMonitor)
    {
        m_activityMonitor->StopMonitoring();
    }
    
    // Unregister hotkey
    if (m_hotkeyManager)
    {
        m_hotkeyManager->UnregisterGlobalHotkey();
    }
    
    // Remove tray icon
    if (m_systemTray)
    {
        m_systemTray->RemoveTrayIcon();
    }
    
    // Save settings
    if (m_settingsManager)
    {
        m_settingsManager->SaveSettings();
    }
    
    // Destroy dialog
    if (m_dialogManager)
    {
        m_dialogManager->DestroyMainDialog();
    }
    
    m_hMainDlg = nullptr;
}

int ApplicationManager::Run()
{
    MSG msg;
    
    // Main message loop
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!HandleMessage(msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    return static_cast<int>(msg.wParam);
}

bool ApplicationManager::HandleMessage(MSG& msg)
{
    // Handle dialog messages
    if (m_hMainDlg && IsDialogMessage(m_hMainDlg, &msg))
    {
        return true;
    }
    
    return false;
}

void ApplicationManager::HandleHotkeyToggle()
{
    // Toggle monitoring state when hotkey is pressed
    if (m_activityMonitor->IsMonitoring())
    {
        m_activityMonitor->StopMonitoring();
    }
    else
    {
        m_activityMonitor->StartMonitoring();
    }
    
    // Update UI to reflect the new state
    m_dialogManager->UpdateUI();
}

bool ApplicationManager::InitializeSubsystems()
{
    try
    {
        // Create subsystem managers
        m_settingsManager = std::make_unique<SettingsManager>();
        m_activityMonitor = std::make_unique<ActivityMonitor>();
        m_systemTray = std::make_unique<SystemTray>();
        m_hotkeyManager = std::make_unique<HotkeyManager>();
        m_dialogManager = std::make_unique<DialogManager>();
        
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void ApplicationManager::LoadGlobalStrings()
{
    LoadStringW(m_hInstance, IDS_APP_TITLE, m_szTitle, MAX_LOADSTRING);
    LoadStringW(m_hInstance, IDC_MMA, m_szWindowClass, MAX_LOADSTRING);
}