#include "DialogManager.h"
#include "ApplicationManager.h"
#include "ActivityMonitor.h"
#include "SettingsManager.h"
#include "HotkeyManager.h"
#include "SystemTray.h"
#include "resource.h"
#include <commctrl.h>

// Static member definition
DialogManager* DialogManager::s_instance = nullptr;

DialogManager::DialogManager()
{
    s_instance = this;
}

HWND DialogManager::CreateMainDialog(HINSTANCE hInstance)
{
    m_mainDialog = CreateDialogW(hInstance, MAKEINTRESOURCEW(IDD_MMA_DIALOG), nullptr, MainDlgProc);
    return m_mainDialog;
}

void DialogManager::DestroyMainDialog()
{
    if (m_mainDialog)
    {
        DestroyWindow(m_mainDialog);
        m_mainDialog = nullptr;
    }
}

INT_PTR CALLBACK DialogManager::MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!s_instance)
        return FALSE;
        
    switch (message)
    {
    case WM_INITDIALOG:
        return s_instance->HandleMainDialogInit(hDlg);
        
    case WM_COMMAND:
        return s_instance->HandleMainDialogCommand(hDlg, wParam, lParam);
        
    case WM_TIMER:
        return s_instance->HandleMainDialogTimer(hDlg, wParam);
        
    case WM_CLOSE:
        return s_instance->HandleMainDialogClose(hDlg);
        
    case WM_TRAYICON:
        return s_instance->HandleMainDialogTrayIcon(hDlg, wParam, lParam);
        
    case WM_HOTKEY:
        return s_instance->HandleMainDialogHotkey(hDlg, wParam);
    }
    
    return FALSE;
}

INT_PTR CALLBACK DialogManager::AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;
        
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    
    return FALSE;
}

INT_PTR CALLBACK DialogManager::HotkeyDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!s_instance)
        return FALSE;
        
    switch (message)
    {
    case WM_INITDIALOG:
        return s_instance->HandleHotkeyDialogInit(hDlg);
        
    case WM_COMMAND:
        return s_instance->HandleHotkeyDialogCommand(hDlg, wParam, lParam);
    }
    
    return FALSE;
}

void DialogManager::UpdateUI()
{
    if (!m_mainDialog)
        return;
        
    auto& app = ApplicationManager::GetInstance();
    bool monitoring = app.GetActivityMonitor()->IsMonitoring();
    
    // Update start/stop button
    SetWindowTextW(GetDlgItem(m_mainDialog, IDC_START_STOP), 
                   monitoring ? L"Stop Monitoring" : L"Start Monitoring");
    
    // Update timeout display
    DWORD timeout = app.GetActivityMonitor()->GetTimeout();
    SetDlgItemInt(m_mainDialog, IDC_TIMEOUT_EDIT, timeout, FALSE);
    
    // Update hotkey display
    UpdateHotkeyDisplay();
    
    // Update checkboxes based on settings
    auto* settings = app.GetSettingsManager();
    CheckDlgButton(m_mainDialog, IDC_MINIMIZE_TO_TRAY, 
                   settings->GetMinimizeToTray() ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_mainDialog, IDC_START_HIDDEN, 
                   settings->GetStartHidden() ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_mainDialog, IDC_START_MONITORING, 
                   settings->GetStartMonitoring() ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_mainDialog, IDC_START_WITH_WINDOWS, 
                   settings->GetStartWithWindows() ? BST_CHECKED : BST_UNCHECKED);
    
    // Update tray icon state
    app.GetSystemTray()->SetMonitoringState(monitoring);
}

void DialogManager::UpdateHotkeyDisplay()
{
    if (!m_mainDialog)
        return;
        
    auto& app = ApplicationManager::GetInstance();
    auto* hotkeyMgr = app.GetHotkeyManager();
    
    WCHAR* hotkeyStr = HotkeyManager::GetHotkeyDisplayString(
        hotkeyMgr->GetModifiers(), hotkeyMgr->GetVirtualKey());
    
    SetWindowTextW(GetDlgItem(m_mainDialog, IDC_HOTKEY_EDIT), hotkeyStr);
}

bool DialogManager::HandleMainDialogInit(HWND hDlg)
{
    auto& app = ApplicationManager::GetInstance();
    app.SetMainDialog(hDlg);
    m_mainDialog = hDlg;
    
    // Center the dialog on the screen
    CenterDialogOnScreen(hDlg);
    
    // Load settings into UI
    LoadSettingsToUI(hDlg);
    
    // Add tray icon
    app.GetSystemTray()->AddTrayIcon(hDlg);
    
    // Register hotkey
    auto* settings = app.GetSettingsManager();
    app.GetHotkeyManager()->RegisterGlobalHotkey(hDlg, 
        settings->GetHotkeyModifiers(), settings->GetHotkeyVK());
    
    return TRUE;
}

bool DialogManager::HandleMainDialogCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    auto& app = ApplicationManager::GetInstance();
    
    switch (LOWORD(wParam))
    {
    case IDC_START_STOP:
        {
            auto* monitor = app.GetActivityMonitor();
            if (monitor->IsMonitoring())
            {
                monitor->StopMonitoring();
            }
            else
            {
                monitor->StartMonitoring();
            }
            UpdateUI();
        }
        return TRUE;
        
    case IDC_APPLY_TIMEOUT:
        app.GetActivityMonitor()->ApplyTimeoutSetting();
        return TRUE;
        
    case IDC_CONFIGURE_HOTKEY:
        DialogBox(app.GetAppInstance(), MAKEINTRESOURCEW(IDD_HOTKEY_DIALOG), 
                  hDlg, HotkeyDlgProc);
        UpdateHotkeyDisplay();
        return TRUE;
        
    case IDM_ABOUT:
        DialogBox(app.GetAppInstance(), MAKEINTRESOURCEW(IDD_ABOUTBOX), 
                  hDlg, AboutDlgProc);
        return TRUE;
        
    case IDM_EXIT:
    case IDCANCEL:
        PostQuitMessage(0);
        return TRUE;
        
    // Handle tray menu commands
    case IDM_TRAY_SHOW:
        app.GetSystemTray()->ShowMainDialog();
        return TRUE;
        
    case IDM_TRAY_START_STOP:
        {
            auto* monitor = app.GetActivityMonitor();
            if (monitor->IsMonitoring())
            {
                monitor->StopMonitoring();
            }
            else
            {
                monitor->StartMonitoring();
            }
            UpdateUI();
        }
        return TRUE;
        
    case IDM_TRAY_ABOUT:
        DialogBox(app.GetAppInstance(), MAKEINTRESOURCEW(IDD_ABOUTBOX), 
                  hDlg, AboutDlgProc);
        return TRUE;
        
    case IDM_TRAY_EXIT:
        PostQuitMessage(0);
        return TRUE;
        
    // Handle checkboxes
    case IDC_MINIMIZE_TO_TRAY:
    case IDC_START_HIDDEN:
    case IDC_START_MONITORING:
    case IDC_START_WITH_WINDOWS:
        SaveUIToSettings(hDlg);
        return TRUE;
    }
    
    return FALSE;
}

bool DialogManager::HandleMainDialogTimer(HWND hDlg, WPARAM wParam)
{
    if (wParam == 1) // Activity check timer
    {
        auto& app = ApplicationManager::GetInstance();
        app.GetActivityMonitor()->CheckActivity();
        return TRUE;
    }
    
    return FALSE;
}

bool DialogManager::HandleMainDialogClose(HWND hDlg)
{
    auto& app = ApplicationManager::GetInstance();
    auto* settings = app.GetSettingsManager();
    
    if (settings->GetMinimizeToTray())
    {
        // Hide to tray instead of closing
        app.GetSystemTray()->HideMainDialog();
    }
    else
    {
        PostQuitMessage(0);
    }
    
    return TRUE;
}

bool DialogManager::HandleMainDialogTrayIcon(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    auto& app = ApplicationManager::GetInstance();
    return app.GetSystemTray()->HandleTrayMessage(wParam, lParam);
}

bool DialogManager::HandleMainDialogHotkey(HWND hDlg, WPARAM wParam)
{
    auto& app = ApplicationManager::GetInstance();
    return app.GetHotkeyManager()->HandleHotkeyMessage(wParam);
}

bool DialogManager::HandleHotkeyDialogInit(HWND hDlg)
{
    // Populate key combo box
    PopulateKeyCombo(GetDlgItem(hDlg, IDC_KEY_COMBO));
    
    // Set current selections
    auto& app = ApplicationManager::GetInstance();
    auto* hotkeyMgr = app.GetHotkeyManager();
    
    UINT modifiers = hotkeyMgr->GetModifiers();
    UINT vk = hotkeyMgr->GetVirtualKey();
    
    // Set modifier checkboxes
    CheckDlgButton(hDlg, IDC_CTRL_CHECK, (modifiers & MOD_CONTROL) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_ALT_CHECK, (modifiers & MOD_ALT) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_SHIFT_CHECK, (modifiers & MOD_SHIFT) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_WIN_CHECK, (modifiers & MOD_WIN) ? BST_CHECKED : BST_UNCHECKED);
    
    // Set key selection by finding the item with matching virtual key data
    HWND hKeyCombo = GetDlgItem(hDlg, IDC_KEY_COMBO);
    int itemCount = SendMessage(hKeyCombo, CB_GETCOUNT, 0, 0);
    for (int i = 0; i < itemCount; ++i)
    {
        UINT itemVK = (UINT)SendMessage(hKeyCombo, CB_GETITEMDATA, i, 0);
        if (itemVK == vk)
        {
            SendMessage(hKeyCombo, CB_SETCURSEL, i, 0);
            break;
        }
    }
    
    // Update current hotkey display
    WCHAR* hotkeyStr = HotkeyManager::GetHotkeyDisplayString(modifiers, vk);
    SetWindowTextW(GetDlgItem(hDlg, IDC_HOTKEY_CAPTURE), hotkeyStr);
    
    return TRUE;
}

bool DialogManager::HandleHotkeyDialogCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    int wmId = LOWORD(wParam);
    int wmEvent = HIWORD(wParam);
    
    switch (wmId)
    {
    case IDC_CTRL_CHECK:
    case IDC_ALT_CHECK:
    case IDC_SHIFT_CHECK:
    case IDC_WIN_CHECK:
    case IDC_KEY_COMBO:
        if (wmEvent == BN_CLICKED || wmEvent == CBN_SELCHANGE)
        {
            // Update the preview
            UINT modifiers = 0;
            if (IsDlgButtonChecked(hDlg, IDC_CTRL_CHECK) == BST_CHECKED)
                modifiers |= MOD_CONTROL;
            if (IsDlgButtonChecked(hDlg, IDC_ALT_CHECK) == BST_CHECKED)
                modifiers |= MOD_ALT;
            if (IsDlgButtonChecked(hDlg, IDC_SHIFT_CHECK) == BST_CHECKED)
                modifiers |= MOD_SHIFT;
            if (IsDlgButtonChecked(hDlg, IDC_WIN_CHECK) == BST_CHECKED)
                modifiers |= MOD_WIN;
            
            HWND hKeyCombo = GetDlgItem(hDlg, IDC_KEY_COMBO);
            int sel = SendMessage(hKeyCombo, CB_GETCURSEL, 0, 0);
            UINT vk = 0;
            if (sel != CB_ERR)
            {
                vk = (UINT)SendMessage(hKeyCombo, CB_GETITEMDATA, sel, 0);
            }
            
            if (vk != 0)
            {
                WCHAR* hotkeyStr = HotkeyManager::GetHotkeyDisplayString(modifiers, vk);
                SetWindowTextW(GetDlgItem(hDlg, IDC_HOTKEY_CAPTURE), hotkeyStr);
            }
        }
        break;
        
    case IDC_RESET_DEFAULT:
        {
            // Reset to default (Ctrl+Shift+M)
            CheckDlgButton(hDlg, IDC_CTRL_CHECK, BST_CHECKED);
            CheckDlgButton(hDlg, IDC_ALT_CHECK, BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_SHIFT_CHECK, BST_CHECKED);
            CheckDlgButton(hDlg, IDC_WIN_CHECK, BST_UNCHECKED);
            
            HWND hKeyCombo = GetDlgItem(hDlg, IDC_KEY_COMBO);
            int itemCount = SendMessage(hKeyCombo, CB_GETCOUNT, 0, 0);
            for (int i = 0; i < itemCount; i++)
            {
                UINT itemVK = (UINT)SendMessage(hKeyCombo, CB_GETITEMDATA, i, 0);
                if (itemVK == 'M')
                {
                    SendMessage(hKeyCombo, CB_SETCURSEL, i, 0);
                    break;
                }
            }
            
            SetWindowTextW(GetDlgItem(hDlg, IDC_HOTKEY_CAPTURE), L"Ctrl+Shift+M");
        }
        break;
        
    case IDOK:
        {
            // Validate and save the new hotkey
            UINT modifiers = 0;
            if (IsDlgButtonChecked(hDlg, IDC_CTRL_CHECK) == BST_CHECKED)
                modifiers |= MOD_CONTROL;
            if (IsDlgButtonChecked(hDlg, IDC_ALT_CHECK) == BST_CHECKED)
                modifiers |= MOD_ALT;
            if (IsDlgButtonChecked(hDlg, IDC_SHIFT_CHECK) == BST_CHECKED)
                modifiers |= MOD_SHIFT;
            if (IsDlgButtonChecked(hDlg, IDC_WIN_CHECK) == BST_CHECKED)
                modifiers |= MOD_WIN;
            
            HWND hKeyCombo = GetDlgItem(hDlg, IDC_KEY_COMBO);
            int sel = SendMessage(hKeyCombo, CB_GETCURSEL, 0, 0);
            UINT vk = 0;
            if (sel != CB_ERR)
            {
                vk = (UINT)SendMessage(hKeyCombo, CB_GETITEMDATA, sel, 0);
            }
            
            // Validate that at least one modifier is selected
            if (modifiers == 0)
            {
                MessageBoxW(hDlg, L"Please select at least one modifier key (Ctrl, Alt, Shift, or Win).", 
                           L"Invalid Hotkey", MB_OK | MB_ICONWARNING);
                return TRUE;
            }
            
            if (vk == 0)
            {
                MessageBoxW(hDlg, L"Please select a key.", 
                           L"Invalid Hotkey", MB_OK | MB_ICONWARNING);
                return TRUE;
            }
            
            // Update hotkey manager and settings
            auto& app = ApplicationManager::GetInstance();
            app.GetHotkeyManager()->SetHotkey(modifiers, vk);
            app.GetSettingsManager()->SetHotkeyModifiers(modifiers);
            app.GetSettingsManager()->SetHotkeyVK(vk);
            app.GetSettingsManager()->SaveSettings();
            
            // Re-register with new hotkey
            app.GetHotkeyManager()->RegisterGlobalHotkey(
                app.GetMainDialog(), modifiers, vk);
        }
        EndDialog(hDlg, IDOK);
        return TRUE;
        
    case IDCANCEL:
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
    }
    
    return FALSE;
}

void DialogManager::PopulateKeyCombo(HWND hCombo)
{
    for (int i = 0; i < g_keyMappingsCount; ++i)
    {
        int index = SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)g_keyMappings[i].name);
        SendMessage(hCombo, CB_SETITEMDATA, index, g_keyMappings[i].vk);
    }
}

void DialogManager::LoadSettingsToUI(HWND hDlg)
{
    auto& app = ApplicationManager::GetInstance();
    auto* settings = app.GetSettingsManager();
    
    // Load timeout
    SetDlgItemInt(hDlg, IDC_TIMEOUT_EDIT, settings->GetTimeout(), FALSE);
    
    // Load checkboxes
    CheckDlgButton(hDlg, IDC_MINIMIZE_TO_TRAY, 
                   settings->GetMinimizeToTray() ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_START_HIDDEN, 
                   settings->GetStartHidden() ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_START_MONITORING, 
                   settings->GetStartMonitoring() ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_START_WITH_WINDOWS, 
                   settings->GetStartWithWindows() ? BST_CHECKED : BST_UNCHECKED);
    
    // Update hotkey display
    UpdateHotkeyDisplay();
}

void DialogManager::SaveUIToSettings(HWND hDlg)
{
    auto& app = ApplicationManager::GetInstance();
    auto* settings = app.GetSettingsManager();
    
    // Save checkboxes
    settings->SetMinimizeToTray(IsDlgButtonChecked(hDlg, IDC_MINIMIZE_TO_TRAY) == BST_CHECKED);
    settings->SetStartHidden(IsDlgButtonChecked(hDlg, IDC_START_HIDDEN) == BST_CHECKED);
    settings->SetStartMonitoring(IsDlgButtonChecked(hDlg, IDC_START_MONITORING) == BST_CHECKED);
    settings->SetStartWithWindows(IsDlgButtonChecked(hDlg, IDC_START_WITH_WINDOWS) == BST_CHECKED);
    
    // Save settings
    settings->SaveSettings();
}

void DialogManager::CenterDialogOnScreen(HWND hDlg)
{
    RECT dialogRect, screenRect;
    
    // Get dialog dimensions
    GetWindowRect(hDlg, &dialogRect);
    int dialogWidth = dialogRect.right - dialogRect.left;
    int dialogHeight = dialogRect.bottom - dialogRect.top;
    
    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    // Calculate centered position
    int x = (screenWidth - dialogWidth) / 2;
    int y = (screenHeight - dialogHeight) / 2;
    
    // Ensure the dialog doesn't go off-screen
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + dialogWidth > screenWidth) x = screenWidth - dialogWidth;
    if (y + dialogHeight > screenHeight) y = screenHeight - dialogHeight;
    
    // Move the dialog to the centered position
    SetWindowPos(hDlg, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}