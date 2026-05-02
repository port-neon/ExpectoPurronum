#include "AppState.h"

#include "CommandIds.h"
#include "KeyboardGuard.h"
#include "LaunchAtLoginService.h"
#include "Localization.h"
#include "SettingsWindow.h"
#include "TrayIcon.h"

AppState::AppState(HINSTANCE instance, HWND mainWindow)
    : instance_(instance), mainWindow_(mainWindow) {
}

AppState::~AppState() {
    Shutdown();
}

bool AppState::Initialize() {
    settings_ = settingsStore_.Load();
    settings_.launchAtLogin = LaunchAtLoginService::IsEnabled();

    keyboardGuard_ = std::make_unique<KeyboardGuard>(mainWindow_);
    keyboardGuard_->UpdateSettings(settings_.detection);

    settingsWindow_ = std::make_unique<SettingsWindow>(instance_, mainWindow_, *this);
    trayIcon_ = std::make_unique<TrayIcon>(instance_, mainWindow_, *this);
    if (!trayIcon_->Add()) {
        return false;
    }

    StartMonitoring();
    return true;
}

void AppState::Shutdown() {
    if (keyboardGuard_) {
        keyboardGuard_->Stop();
    }
    if (trayIcon_) {
        trayIcon_->Remove();
    }
}

void AppState::HandleCommand(int commandId) {
    switch (commandId) {
    case IDM_START_PROTECTION:
        StartMonitoring();
        break;
    case IDM_PAUSE_PROTECTION:
        StopMonitoring();
        break;
    case IDM_LOCK_KEYBOARD:
        SetLocked(true);
        break;
    case IDM_UNLOCK_KEYBOARD:
        Unlock();
        break;
    case IDM_SETTINGS:
        ShowSettings();
        break;
    case IDM_QUIT:
        PostQuitMessage(0);
        break;
    default:
        break;
    }
}

void AppState::HandleTrayEvent(UINT event) {
    if (event == WM_RBUTTONUP || event == WM_CONTEXTMENU) {
        if (trayIcon_) {
            trayIcon_->ShowContextMenu();
        }
    } else if (event == WM_LBUTTONUP || event == WM_LBUTTONDBLCLK) {
        ShowSettings();
    }
}

void AppState::HandleGuardStateChanged(KeyboardLockState state) {
    const bool didBecomeLocked = lockState_ != KeyboardLockState::Locked && state == KeyboardLockState::Locked;
    lockState_ = state;
    RefreshUi();

    if (didBecomeLocked) {
        ShowLockedAlert();
    }
}

void AppState::HandleDetectionTimer() {
    if (keyboardGuard_) {
        keyboardGuard_->HandleDetectionTimer();
    }
}

void AppState::ShowSettings() {
    if (settingsWindow_) {
        settingsWindow_->Show(settings_);
    }
}

void AppState::ApplySettings(const AppSettings& settings) {
    settings_ = settings;
    LaunchAtLoginService::SetEnabled(settings_.launchAtLogin);
    settings_.launchAtLogin = LaunchAtLoginService::IsEnabled();

    if (keyboardGuard_) {
        keyboardGuard_->UpdateSettings(settings_.detection);
    }

    SaveSettings();
    RefreshUi();
}

void AppState::StartMonitoring() {
    if (isMonitoring_) {
        return;
    }

    if (keyboardGuard_ && keyboardGuard_->Start()) {
        isMonitoring_ = true;
    } else {
        MessageBoxW(mainWindow_, L"Failed to start the keyboard hook.", TextFor(settings_.language).appName.c_str(), MB_ICONERROR | MB_OK);
    }

    RefreshUi();
}

void AppState::StopMonitoring() {
    if (!isMonitoring_) {
        return;
    }

    if (keyboardGuard_) {
        keyboardGuard_->Stop();
    }

    isMonitoring_ = false;
    lockState_ = KeyboardLockState::Unlocked;
    RefreshUi();
}

void AppState::SetLocked(bool locked) {
    if (keyboardGuard_) {
        keyboardGuard_->SetLocked(locked);
    }
}

void AppState::Unlock() {
    SetLocked(false);
}

void AppState::RefreshUi() {
    if (trayIcon_) {
        trayIcon_->Refresh();
    }
    if (settingsWindow_) {
        settingsWindow_->Refresh(settings_);
    }
}

void AppState::ShowLockedAlert() {
    if (isShowingLockAlert_) {
        return;
    }

    isShowingLockAlert_ = true;
    const auto& text = TextFor(settings_.language);
    const int response = MessageBoxW(
        mainWindow_,
        text.lockedAlertMessage.c_str(),
        text.lockedAlertTitle.c_str(),
        MB_ICONWARNING | MB_YESNO | MB_SETFOREGROUND);

    isShowingLockAlert_ = false;

    if (response == IDYES) {
        Unlock();
    } else if (response == IDNO) {
        StopMonitoring();
    }
}

void AppState::SaveSettings() {
    settingsStore_.Save(settings_);
}
