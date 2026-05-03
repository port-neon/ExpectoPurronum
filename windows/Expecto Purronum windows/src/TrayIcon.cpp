#include "TrayIcon.h"

#include "AppMessages.h"
#include "AppState.h"
#include "CommandIds.h"
#include "Localization.h"
#include "Resource.h"

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
    if (added_) {
        data_.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &data_);
    }
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
    data_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    StringCchCopyW(data_.szTip, ARRAYSIZE(data_.szTip), Tooltip().c_str());
    Shell_NotifyIconW(NIM_MODIFY, &data_);
}

void TrayIcon::ShowContextMenu() {
    const auto& text = TextFor(appState_.Language());
    const UINT startState = appState_.IsMonitoring() ? MF_DISABLED | MF_CHECKED : 0;
    const UINT pauseState = appState_.IsMonitoring() ? 0 : MF_DISABLED | MF_CHECKED;
    const UINT lockState = appState_.IsLocked() ? MF_DISABLED | MF_CHECKED : 0;
    const UINT unlockState = appState_.IsLocked() ? 0 : MF_DISABLED;

    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING | MF_DISABLED, 0, Tooltip().c_str());
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | startState, IDM_START_PROTECTION, text.startMonitoring.c_str());
    AppendMenuW(menu, MF_STRING | pauseState, IDM_PAUSE_PROTECTION, text.stopMonitoring.c_str());
    AppendMenuW(menu, MF_STRING | lockState, IDM_LOCK_KEYBOARD, text.lock.c_str());
    AppendMenuW(menu, MF_STRING | unlockState, IDM_UNLOCK_KEYBOARD, text.unlock.c_str());
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_SETTINGS, text.settings.c_str());
    AppendMenuW(menu, MF_STRING, IDM_QUIT, text.quit.c_str());
    SetMenuDefaultItem(menu, IDM_SETTINGS, FALSE);

    POINT cursor{};
    GetCursorPos(&cursor);
    SetForegroundWindow(ownerWindow_);
    TrackPopupMenu(menu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, ownerWindow_, nullptr);
    PostMessageW(ownerWindow_, WM_NULL, 0, 0);
    DestroyMenu(menu);
}

void TrayIcon::ShowNotification(const std::wstring& title, const std::wstring& message, DWORD iconFlags) {
    if (!added_) {
        return;
    }

    data_.uFlags = NIF_INFO;
    data_.dwInfoFlags = iconFlags;
    data_.uTimeout = 4000;
    StringCchCopyW(data_.szInfoTitle, ARRAYSIZE(data_.szInfoTitle), title.c_str());
    StringCchCopyW(data_.szInfo, ARRAYSIZE(data_.szInfo), message.c_str());
    Shell_NotifyIconW(NIM_MODIFY, &data_);
}

HICON TrayIcon::CurrentIcon() const {
    const int size = GetSystemMetrics(SM_CXSMICON);
    int iconId = IDI_TRAY_OFF;
    if (appState_.IsLocked()) {
        iconId = IDI_TRAY_LOCKED;
    } else if (appState_.IsMonitoring()) {
        iconId = IDI_TRAY_ON;
    }

    return static_cast<HICON>(LoadImageW(
        instance_,
        MAKEINTRESOURCEW(iconId),
        IMAGE_ICON,
        size,
        size,
        LR_SHARED));
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
