#pragma once

#include "framework.h"
#include <time.h>
#include <random>
#include <shellapi.h>
#include <windows.h>

#define MAX_LOADSTRING 100

// Forward declarations
class ApplicationManager;
class ActivityMonitor;
class SystemTray;
class SettingsManager;
class HotkeyManager;
class DialogManager;

// Global application instance pointer
extern ApplicationManager* g_pApp;

// Constants
const UINT WM_TRAYICON = WM_USER + 1;
const DWORD DEFAULT_TIMEOUT_SECONDS = 5;
const DWORD MAX_TIMEOUT_SECONDS = 3600;

// Single instance constants
const LPCWSTR APP_MUTEX_NAME = L"Global\\MMAApplication_SingleInstance_Mutex";
const LPCWSTR APP_NAME = L"Mouse & Keyboard Activity Monitor";
const LPCWSTR APP_SHORT_NAME = L"MMA";

// Key mapping structure for hotkey selection
struct KeyMapping 
{
    UINT vk;
    const WCHAR* name;
};

// Declare the key mappings array as extern
extern const KeyMapping g_keyMappings[];
extern const int g_keyMappingsCount;