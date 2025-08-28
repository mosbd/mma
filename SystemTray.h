#pragma once

#include "common.h"

/**
 * Manages system tray icon and notifications
 */
class SystemTray
{
public:
    SystemTray();
    ~SystemTray();

    // Tray icon management
    bool AddTrayIcon(HWND hWnd);
    void RemoveTrayIcon();
    bool IsIconAdded() const { return m_iconAdded; }

    // Icon state updates
    void UpdateTooltip(const WCHAR* tooltip);
    void SetMonitoringState(bool monitoring);

    // Context menu
    void ShowContextMenu();

    // Dialog visibility
    void ShowMainDialog();
    void HideMainDialog();

    // Message handling
    bool HandleTrayMessage(WPARAM wParam, LPARAM lParam);

private:
    // Private helpers
    void InitializeTrayData();
    void UpdateMenuItems(HMENU hMenu, bool monitoring);

    // Member variables
    NOTIFYICONDATAW m_notifyIconData;
    bool m_iconAdded = false;
    HWND m_mainDialog = nullptr;
};