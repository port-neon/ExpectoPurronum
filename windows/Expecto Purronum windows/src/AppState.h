#pragma once

#include <Windows.h>

#include <memory>

#include "AppLanguage.h"
#include "DetectionSettings.h"
#include "KeyboardLockState.h"
#include "SettingsStore.h"

class KeyboardGuard;
class SettingsWindow;
class TrayIcon;

class AppState {
public:
    AppState(HINSTANCE instance, HWND mainWindow);
    ~AppState();

    bool Initialize();
    void Shutdown();
    void HandleCommand(int commandId);
    void HandleTrayEvent(UINT event);
    void HandleGuardStateChanged(KeyboardLockState state);
    void HandleDetectionTimer();
    void ShowSettings();
    void ApplySettings(const AppSettings& settings);

    bool IsMonitoring() const { return isMonitoring_; }
    bool IsLocked() const { return lockState_ == KeyboardLockState::Locked; }
    KeyboardLockState LockState() const { return lockState_; }
    AppLanguage Language() const { return settings_.language; }
    const AppSettings& Settings() const { return settings_; }

private:
    void StartMonitoring();
    void StopMonitoring();
    void SetLocked(bool locked);
    void Unlock();
    void RefreshUi();
    void ShowLockedAlert();
    void SaveSettings();

    HINSTANCE instance_ = nullptr;
    HWND mainWindow_ = nullptr;
    AppSettings settings_{};
    SettingsStore settingsStore_;
    std::unique_ptr<KeyboardGuard> keyboardGuard_;
    std::unique_ptr<TrayIcon> trayIcon_;
    std::unique_ptr<SettingsWindow> settingsWindow_;
    bool isMonitoring_ = false;
    KeyboardLockState lockState_ = KeyboardLockState::Unlocked;
    bool isShowingLockAlert_ = false;
};
