#include <Windows.h>

#include <memory>

#include "AppMessages.h"
#include "AppState.h"

namespace {
constexpr const wchar_t* kMainWindowClass = L"ExpectoPurronumMainWindow";
std::unique_ptr<AppState> g_appState;

LRESULT CALLBACK MainWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        return 0;
    case WM_COMMAND:
        if (g_appState) {
            g_appState->HandleCommand(LOWORD(wParam));
        }
        return 0;
    case WM_TIMER:
        if (wParam == IDT_KEYBOARD_DETECTION && g_appState) {
            g_appState->HandleDetectionTimer();
        }
        return 0;
    case WM_APP_GUARD_STATE_CHANGED:
        if (g_appState) {
            g_appState->HandleGuardStateChanged(static_cast<KeyboardLockState>(wParam));
        }
        return 0;
    case WM_APP_TRAY:
        if (g_appState) {
            g_appState->HandleTrayEvent(LOWORD(lParam));
        }
        return 0;
    case WM_DESTROY:
        if (g_appState) {
            g_appState->Shutdown();
            g_appState.reset();
        }
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

bool RegisterMainWindowClass(HINSTANCE instance) {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = MainWindowProc;
    wc.lpszClassName = kMainWindowClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    return RegisterClassExW(&wc) != 0;
}
}

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int) {
    if (!RegisterMainWindowClass(instance)) {
        return 1;
    }

    HWND mainWindow = CreateWindowExW(
        0,
        kMainWindowClass,
        L"Expecto Purronum",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (mainWindow == nullptr) {
        return 1;
    }

    g_appState = std::make_unique<AppState>(instance, mainWindow);
    if (!g_appState->Initialize()) {
        MessageBoxW(mainWindow, L"Failed to initialize Expecto Purronum.", L"Expecto Purronum", MB_ICONERROR | MB_OK);
        return 1;
    }

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}
