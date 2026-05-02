#include "TrayIcon.h"

#include "AppMessages.h"
#include "AppState.h"
#include "CommandIds.h"
#include "Localization.h"

#include <strsafe.h>

TrayIcon::TrayIcon(HINSTANCE instance, HWND ownerWindow, AppState& appState)
    : instance_(instance), ownerWindow_(ownerWindow), appState_(appState) {
    data_.cbSize = sizeof(data_);
    data_.hWnd = ownerWindow_;
    data_.uID = 1;
    data_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    data_.uCallbackMessage = WM_APP_TRAY;
}

TrayIcon::~TrayIcon() {
    Remove();
}

bool TrayIcon::Add() {
    if (added_) {
        return true;
    }

    data_.hIcon = CurrentIcon();
    StringCchCopyW(data_.szTip, ARRAYSIZE(data_.szTip), Tooltip().c_str());
    added_ = Shell_NotifyIconW(NIM_ADD, &data_) == TRUE;
    return added_;
}

void TrayIcon::Remove() {
    if (!added_) {
        return;
    }

    Shell_NotifyIconW(NIM_DELETE, &data_);
    added_ = false;
}

void TrayIcon::Refresh() {
    if (!added_) {
        return;
    }

    data_.hIcon = CurrentIcon();
    StringCchCopyW(data_.szTip, ARRAYSIZE(data_.szTip), Tooltip().c_str());
    Shell_NotifyIconW(NIM_MODIFY, &data_);
}

void TrayIcon::ShowContextMenu() {
    const auto& text = TextFor(appState_.Language());

    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING | MF_DISABLED, 0, Tooltip().c_str());
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (appState_.IsMonitoring() ? MF_DISABLED : 0), IDM_START_PROTECTION, text.startMonitoring.c_str());
    AppendMenuW(menu, MF_STRING | (!appState_.IsMonitoring() ? MF_DISABLED : 0), IDM_PAUSE_PROTECTION, text.stopMonitoring.c_str());
    AppendMenuW(menu, MF_STRING | (appState_.IsLocked() ? MF_DISABLED : 0), IDM_LOCK_KEYBOARD, text.lock.c_str());
    AppendMenuW(menu, MF_STRING | (!appState_.IsLocked() ? MF_DISABLED : 0), IDM_UNLOCK_KEYBOARD, text.unlock.c_str());
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_SETTINGS, text.settings.c_str());
    AppendMenuW(menu, MF_STRING, IDM_QUIT, text.quit.c_str());

    POINT cursor{};
    GetCursorPos(&cursor);
    SetForegroundWindow(ownerWindow_);
    TrackPopupMenu(menu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, ownerWindow_, nullptr);
    PostMessageW(ownerWindow_, WM_NULL, 0, 0);
    DestroyMenu(menu);
}

HICON TrayIcon::CurrentIcon() const {
    if (appState_.IsLocked()) {
        return LoadIconW(nullptr, IDI_ERROR);
    }

    if (appState_.IsMonitoring()) {
        return LoadIconW(nullptr, IDI_SHIELD);
    }

    return LoadIconW(nullptr, IDI_APPLICATION);
}

std::wstring TrayIcon::Tooltip() const {
    const auto& text = TextFor(appState_.Language());
    if (appState_.IsLocked()) {
        return text.lockedStatus;
    }

    if (appState_.LockState() == KeyboardLockState::Suspicious) {
        return text.suspicious;
    }

    return appState_.IsMonitoring() ? text.monitoringOn : text.monitoringOff;
}
