#pragma once

#include <Windows.h>
#include <shellapi.h>

#include "AppLanguage.h"
#include "KeyboardLockState.h"

class AppState;

class TrayIcon {
public:
    TrayIcon(HINSTANCE instance, HWND ownerWindow, AppState& appState);
    ~TrayIcon();

    bool Add();
    void Remove();
    void Refresh();
    void ShowContextMenu();

private:
    HICON CurrentIcon() const;
    std::wstring Tooltip() const;

    HINSTANCE instance_ = nullptr;
    HWND ownerWindow_ = nullptr;
    AppState& appState_;
    NOTIFYICONDATAW data_{};
    bool added_ = false;
};
