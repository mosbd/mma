#include "SystemTray.h"
#include "ApplicationManager.h"
#include "ActivityMonitor.h"
#include "DialogManager.h"
#include "resource.h"

SystemTray::SystemTray()
{
    InitializeTrayData();
}

SystemTray::~SystemTray()
{
    RemoveTrayIcon();
}

bool SystemTray::AddTrayIcon(HWND hWnd)
{
    if (m_iconAdded)
        return true;

    m_mainDialog = hWnd;
    
    ZeroMemory(&m_notifyIconData, sizeof(m_notifyIconData));
    m_notifyIconData.cbSize = sizeof(m_notifyIconData);
    m_notifyIconData.hWnd = hWnd;
    m_notifyIconData.uID = 1;
    m_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_notifyIconData.uCallbackMessage = WM_TRAYICON;
    
    auto& app = ApplicationManager::GetInstance();
    m_notifyIconData.hIcon = LoadIcon(app.GetAppInstance(), MAKEINTRESOURCEW(IDI_SMALL));
    wcscpy_s(m_notifyIconData.szTip, L"MMA - Stopped");

    if (Shell_NotifyIconW(NIM_ADD, &m_notifyIconData))
    {
        m_iconAdded = true;
        return true;
    }
    
    return false;
}

void SystemTray::RemoveTrayIcon()
{
    if (m_iconAdded)
    {
        Shell_NotifyIconW(NIM_DELETE, &m_notifyIconData);
        m_iconAdded = false;
    }
}

void SystemTray::UpdateTooltip(const WCHAR* tooltip)
{
    if (m_iconAdded && tooltip)
    {
        wcscpy_s(m_notifyIconData.szTip, tooltip);
        Shell_NotifyIconW(NIM_MODIFY, &m_notifyIconData);
    }
}

void SystemTray::SetMonitoringState(bool monitoring)
{
    if (monitoring)
    {
        UpdateTooltip(L"MMA - Monitoring Active");
    }
    else
    {
        UpdateTooltip(L"MMA - Stopped");
    }
}

void SystemTray::ShowContextMenu()
{
    auto& app = ApplicationManager::GetInstance();
    HMENU hMenu = LoadMenuW(app.GetAppInstance(), MAKEINTRESOURCEW(IDM_TRAY_MENU));
    
    if (hMenu)
    {
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        if (hSubMenu)
        {
            // Update menu text based on current state
            bool monitoring = app.GetActivityMonitor()->IsMonitoring();
            UpdateMenuItems(hSubMenu, monitoring);
            
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(m_mainDialog);
            TrackPopupMenu(hSubMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_mainDialog, nullptr);
        }
        DestroyMenu(hMenu);
    }
}

void SystemTray::ShowMainDialog()
{
    if (m_mainDialog)
    {
        ShowWindow(m_mainDialog, SW_SHOW);
        SetForegroundWindow(m_mainDialog);
        SetActiveWindow(m_mainDialog);
    }
}

void SystemTray::HideMainDialog()
{
    if (m_mainDialog)
    {
        ShowWindow(m_mainDialog, SW_HIDE);
    }
}

bool SystemTray::HandleTrayMessage(WPARAM wParam, LPARAM lParam)
{
    if (wParam != 1) // Our tray icon ID
        return false;
        
    switch (lParam)
    {
    case WM_LBUTTONDBLCLK:
        ShowMainDialog();
        return true;
        
    case WM_RBUTTONUP:
        ShowContextMenu();
        return true;
    }
    
    return false;
}

void SystemTray::InitializeTrayData()
{
    ZeroMemory(&m_notifyIconData, sizeof(m_notifyIconData));
}

void SystemTray::UpdateMenuItems(HMENU hMenu, bool monitoring)
{
    if (monitoring)
    {
        ModifyMenuW(hMenu, IDM_TRAY_START_STOP, MF_BYCOMMAND | MF_STRING, 
                   IDM_TRAY_START_STOP, L"Stop Monitoring");
    }
    else
    {
        ModifyMenuW(hMenu, IDM_TRAY_START_STOP, MF_BYCOMMAND | MF_STRING, 
                   IDM_TRAY_START_STOP, L"Start Monitoring");
    }
}