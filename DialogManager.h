#pragma once

#include "common.h"

/**
 * Manages dialog boxes and UI updates
 */
class DialogManager
{
public:
    DialogManager();
    ~DialogManager() = default;

    // Dialog creation and management
    HWND CreateMainDialog(HINSTANCE hInstance);
    void DestroyMainDialog();

    // Dialog procedures (static for Windows API compatibility)
    static INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK HotkeyDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    // UI updates
    void UpdateUI();
    void UpdateHotkeyDisplay();

    // Dialog state
    HWND GetMainDialog() const { return m_mainDialog; }

private:
    // Message handlers for main dialog
    bool HandleMainDialogInit(HWND hDlg);
    bool HandleMainDialogCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);
    bool HandleMainDialogTimer(HWND hDlg, WPARAM wParam);
    bool HandleMainDialogClose(HWND hDlg);
    bool HandleMainDialogTrayIcon(HWND hDlg, WPARAM wParam, LPARAM lParam);
    bool HandleMainDialogHotkey(HWND hDlg, WPARAM wParam);

    // Message handlers for hotkey dialog
    bool HandleHotkeyDialogInit(HWND hDlg);
    bool HandleHotkeyDialogCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);

    // UI helper methods
    void PopulateKeyCombo(HWND hCombo);
    void LoadSettingsToUI(HWND hDlg);
    void SaveUIToSettings(HWND hDlg);
    void CenterDialogOnScreen(HWND hDlg);

    // Member variables
    HWND m_mainDialog = nullptr;
    
    // Static instance pointer for dialog procedures
    static DialogManager* s_instance;
};