#include "AppState.h"

#include "CommandIds.h"
#include "KeyboardGuard.h"
#include "LaunchAtLoginService.h"
#include "Localization.h"
#include "Resource.h"
#include "SettingsWindow.h"
#include "TrayIcon.h"

namespace {
constexpr const wchar_t* kLockedAlertWindowClass = L"ExpectoPurronumLockedAlertWindow";

struct LockedAlertData {
    const LocalizedText* text = nullptr;
    int result = IDYES;
};

HWND MakeAlertControl(HWND parent, const wchar_t* className, const wchar_t* text, DWORD style, int x, int y, int width, int height, int id) {
    return CreateWindowExW(
        0,
        className,
        text,
        WS_CHILD | WS_VISIBLE | style,
        x,
        y,
        width,
        height,
        parent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(nullptr),
        nullptr);
}

LRESULT CALLBACK LockedAlertProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* data = reinterpret_cast<LockedAlertData*>(GetWindowLongPtrW(window, GWLP_USERDATA));

    if (message == WM_NCCREATE) {
        const auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        data = reinterpret_cast<LockedAlertData*>(create->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
    }

    switch (message) {
    case WM_CREATE:
        if (data != nullptr && data->text != nullptr) {
            MakeAlertControl(window, L"STATIC", data->text->lockedAlertMessage.c_str(), SS_LEFT, 24, 24, 600, 112, IDC_LOCK_ALERT_MESSAGE);
            MakeAlertControl(window, L"BUTTON", data->text->lockedAlertUnlockButton.c_str(), BS_DEFPUSHBUTTON | BS_MULTILINE | WS_TABSTOP, 24, 158, 292, 54, IDC_LOCK_ALERT_UNLOCK);
            MakeAlertControl(window, L"BUTTON", data->text->lockedAlertPauseButton.c_str(), BS_PUSHBUTTON | BS_MULTILINE | WS_TABSTOP, 332, 158, 292, 54, IDC_LOCK_ALERT_PAUSE);
        }
        return 0;
    case WM_COMMAND:
        if (data != nullptr) {
            if (LOWORD(wParam) == IDC_LOCK_ALERT_UNLOCK) {
                data->result = IDYES;
                DestroyWindow(window);
                return 0;
            }
            if (LOWORD(wParam) == IDC_LOCK_ALERT_PAUSE) {
                data->result = IDNO;
                DestroyWindow(window);
                return 0;
            }
        }
        break;
    case WM_CLOSE:
        if (data != nullptr) {
            data->result = IDYES;
        }
        DestroyWindow(window);
        return 0;
    default:
        break;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void RegisterLockedAlertClass(HINSTANCE instance) {
    static bool registered = false;
    if (registered) {
        return;
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = LockedAlertProc;
    wc.lpszClassName = kLockedAlertWindowClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_TRAY_LOCKED));
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    registered = RegisterClassExW(&wc) != 0;
}
}

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
        if (trayIcon_) {
            const auto& text = TextFor(settings_.language);
            trayIcon_->ShowNotification(text.lockedAlertTitle, text.lockedStatus, NIIF_WARNING);
        }
        ShowLockedAlert();
    }
}

void AppState::HandleDetectionTimer() {
    if (keyboardGuard_) {
        keyboardGuard_->HandleDetectionTimer();
    }
}

void AppState::HandlePowerBroadcast(WPARAM event) {
    switch (event) {
    case PBT_APMSUSPEND:
    case PBT_APMRESUMESUSPEND:
    case PBT_APMRESUMEAUTOMATIC:
        ClearLockAndDetection();
        break;
    default:
        break;
    }
}

void AppState::ShowSettings() {
    if (settingsWindow_) {
        settingsWindow_->Show(settings_);
    }
}

bool AppState::ApplySettings(const AppSettings& settings) {
    const bool launchAtLoginChanged = settings_.launchAtLogin != settings.launchAtLogin;
    settings_ = settings;
    const bool launchAtLoginUpdated = LaunchAtLoginService::SetEnabled(settings_.launchAtLogin);
    settings_.launchAtLogin = LaunchAtLoginService::IsEnabled();
    const bool launchAtLoginMatchesRequest = settings_.launchAtLogin == settings.launchAtLogin;

    if (keyboardGuard_) {
        keyboardGuard_->UpdateSettings(settings_.detection);
    }

    SaveSettings();
    RefreshUi();

    if (trayIcon_ && launchAtLoginChanged) {
        const auto& text = TextFor(settings_.language);
        if (!launchAtLoginUpdated || !launchAtLoginMatchesRequest) {
            trayIcon_->ShowNotification(text.appName, text.launchAtLoginFailed, NIIF_WARNING);
        } else {
            trayIcon_->ShowNotification(
                text.appName,
                settings_.launchAtLogin ? text.launchAtLoginEnabled : text.launchAtLoginDisabled,
                NIIF_INFO);
        }
    }

    return launchAtLoginUpdated && launchAtLoginMatchesRequest;
}

void AppState::StartMonitoring() {
    if (isMonitoring_) {
        return;
    }

    if (keyboardGuard_ && keyboardGuard_->Start()) {
        isMonitoring_ = true;
        if (trayIcon_) {
            const auto& text = TextFor(settings_.language);
            trayIcon_->ShowNotification(text.appName, text.monitoringOn, NIIF_INFO);
        }
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
    if (trayIcon_) {
        const auto& text = TextFor(settings_.language);
        trayIcon_->ShowNotification(text.appName, text.monitoringOff, NIIF_INFO);
    }
}

void AppState::SetLocked(bool locked) {
    if (keyboardGuard_) {
        keyboardGuard_->SetLocked(locked);
    }
}

void AppState::Unlock() {
    SetLocked(false);
}

void AppState::ClearLockAndDetection() {
    if (keyboardGuard_) {
        keyboardGuard_->ResetDetection();
    }

    lockState_ = KeyboardLockState::Unlocked;
    RefreshUi();
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

    RegisterLockedAlertClass(instance_);
    LockedAlertData alertData{ &text, IDYES };
    const int width = 660;
    const int height = 280;
    const int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    const int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    HWND alertWindow = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        kLockedAlertWindowClass,
        text.lockedAlertTitle.c_str(),
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        x,
        y,
        width,
        height,
        mainWindow_,
        nullptr,
        instance_,
        &alertData);

    if (alertWindow == nullptr) {
        isShowingLockAlert_ = false;
        Unlock();
        return;
    }

    EnableWindow(mainWindow_, FALSE);
    ShowWindow(alertWindow, SW_SHOW);
    SetForegroundWindow(alertWindow);

    MSG message{};
    while (IsWindow(alertWindow) && GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(alertWindow, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    EnableWindow(mainWindow_, TRUE);
    SetForegroundWindow(mainWindow_);

    isShowingLockAlert_ = false;

    if (alertData.result == IDYES) {
        Unlock();
    } else if (alertData.result == IDNO) {
        StopMonitoring();
    }
}

void AppState::SaveSettings() {
    settingsStore_.Save(settings_);
}
