// main.cpp : Application entry point using Object-Oriented Design
//

#include "ApplicationManager.h"

// Global application instance pointer
ApplicationManager* g_pApp = nullptr;

// Key mapping data (moved from global variables)
const KeyMapping g_keyMappings[] = {
    {VK_F1, L"F1"}, {VK_F2, L"F2"}, {VK_F3, L"F3"}, {VK_F4, L"F4"},
    {VK_F5, L"F5"}, {VK_F6, L"F6"}, {VK_F7, L"F7"}, {VK_F8, L"F8"},
    {VK_F9, L"F9"}, {VK_F10, L"F10"}, {VK_F11, L"F11"}, {VK_F12, L"F12"},
    {'A', L"A"}, {'B', L"B"}, {'C', L"C"}, {'D', L"D"}, {'E', L"E"},
    {'F', L"F"}, {'G', L"G"}, {'H', L"H"}, {'I', L"I"}, {'J', L"J"},
    {'K', L"K"}, {'L', L"L"}, {'M', L"M"}, {'N', L"N"}, {'O', L"O"},
    {'P', L"P"}, {'Q', L"Q"}, {'R', L"R"}, {'S', L"S"}, {'T', L"T"},
    {'U', L"U"}, {'V', L"V"}, {'W', L"W"}, {'X', L"X"}, {'Y', L"Y"}, {'Z', L"Z"},
    {'0', L"0"}, {'1', L"1"}, {'2', L"2"}, {'3', L"3"}, {'4', L"4"},
    {'5', L"5"}, {'6', L"6"}, {'7', L"7"}, {'8', L"8"}, {'9', L"9"},
    {VK_SPACE, L"Space"}, {VK_ESCAPE, L"Esc"}, {VK_TAB, L"Tab"},
    {VK_RETURN, L"Enter"}, {VK_BACK, L"Backspace"}, {VK_DELETE, L"Delete"},
    {VK_INSERT, L"Insert"}, {VK_HOME, L"Home"}, {VK_END, L"End"},
    {VK_PRIOR, L"Page Up"}, {VK_NEXT, L"Page Down"},
    {VK_UP, L"Up Arrow"}, {VK_DOWN, L"Down Arrow"}, 
    {VK_LEFT, L"Left Arrow"}, {VK_RIGHT, L"Right Arrow"}
};

const int g_keyMappingsCount = sizeof(g_keyMappings) / sizeof(g_keyMappings[0]);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    int result = 0;

    try
    {
        // Get application instance and initialize
        ApplicationManager& app = ApplicationManager::GetInstance();
        g_pApp = &app;

        if (!app.Initialize(hInstance, nCmdShow))
        {
            MessageBoxW(nullptr, L"Failed to initialize application", L"Error", MB_OK | MB_ICONERROR);
            result = -1;
        }
        else
        {
            // Run main message loop
            result = app.Run();
        }

        // Cleanup
        app.Shutdown();
    }
    catch (...)
    {
        MessageBoxW(nullptr, L"An unexpected error occurred", L"Fatal Error", MB_OK | MB_ICONERROR);
        result = -1;
    }

    // Destroy singleton instance
    ApplicationManager::DestroyInstance();
    g_pApp = nullptr;

    return result;
}