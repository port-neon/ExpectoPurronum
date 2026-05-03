#include "SettingsWindow.h"

#include "AppState.h"
#include "CommandIds.h"
#include "Localization.h"
#include "Resource.h"

#include <cstdio>
#include <iterator>
#include <string>

namespace {
constexpr const wchar_t* kSettingsWindowClass = L"ExpectoPurronumSettingsWindow";

HWND MakeControl(HWND parent, const wchar_t* className, const wchar_t* text, DWORD style, int x, int y, int width, int height, int id) {
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
}

SettingsWindow::SettingsWindow(HINSTANCE instance, HWND ownerWindow, AppState& appState)
    : instance_(instance), ownerWindow_(ownerWindow), appState_(appState) {
    RegisterClass();
}

SettingsWindow::~SettingsWindow() {
    if (window_ != nullptr) {
        DestroyWindow(window_);
    }
}

void SettingsWindow::Show(const AppSettings& settings) {
    CreateIfNeeded();
    PopulateControls(settings);
    ShowWindow(window_, SW_SHOW);
    SetForegroundWindow(window_);
}

void SettingsWindow::Refresh(const AppSettings& settings) {
    if (window_ != nullptr && IsWindowVisible(window_)) {
        RefreshText(settings.language);
        if (!HasFocusedDurationInput()) {
            SetDoubleControl(IDC_ADJACENT_HOLD, settings.detection.adjacentHoldDuration);
            SetDoubleControl(IDC_SINGLE_KEY_HOLD, settings.detection.singleRegularKeyHoldDuration);
            SetDoubleControl(IDC_MODIFIER_HOLD, settings.detection.singleModifierKeyHoldDuration);
        }
        SendDlgItemMessageW(window_, IDC_LAUNCH_AT_LOGIN, BM_SETCHECK, settings.launchAtLogin ? BST_CHECKED : BST_UNCHECKED, 0);
    }
}

LRESULT CALLBACK SettingsWindow::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* self = reinterpret_cast<SettingsWindow*>(GetWindowLongPtrW(window, GWLP_USERDATA));

    if (message == WM_NCCREATE) {
        const auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = reinterpret_cast<SettingsWindow*>(create->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->window_ = window;
    }

    if (self == nullptr) {
        return DefWindowProcW(window, message, wParam, lParam);
    }

    switch (message) {
    case WM_CREATE:
        self->CreateControls();
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_SAVE_SETTINGS:
            if (self->SaveFromControls()) {
                ShowWindow(window, SW_HIDE);
            }
            return 0;
        case IDC_CANCEL_SETTINGS:
            ShowWindow(window, SW_HIDE);
            return 0;
        case IDC_LANGUAGE:
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                self->PreviewLanguageFromControls();
                return 0;
            }
            break;
        default:
            break;
        }
        break;
    case WM_CLOSE:
        ShowWindow(window, SW_HIDE);
        return 0;
    default:
        break;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void SettingsWindow::RegisterClass() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance_;
    wc.lpfnWndProc = SettingsWindow::WindowProc;
    wc.lpszClassName = kSettingsWindowClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon = LoadIconW(instance_, MAKEINTRESOURCEW(IDI_APP));
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    RegisterClassExW(&wc);
}

void SettingsWindow::CreateIfNeeded() {
    if (window_ != nullptr) {
        return;
    }

    window_ = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        kSettingsWindowClass,
        TextFor(appState_.Language()).settings.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        500,
        390,
        ownerWindow_,
        nullptr,
        instance_,
        this);
}

void SettingsWindow::CreateControls() {
    const auto& text = TextFor(appState_.Language());

    MakeControl(window_, L"STATIC", text.language.c_str(), 0, 24, 24, 170, 22, IDC_LANGUAGE_LABEL);
    HWND language = MakeControl(window_, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP, 220, 20, 180, 120, IDC_LANGUAGE);
    SendMessageW(language, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"English"));
    SendMessageW(language, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"\u4e2d\u6587"));

    MakeControl(window_, L"STATIC", text.adjacentHold.c_str(), 0, 24, 68, 190, 22, IDC_ADJACENT_HOLD_LABEL);
    MakeControl(window_, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 220, 64, 100, 24, IDC_ADJACENT_HOLD);
    MakeControl(window_, L"STATIC", text.seconds.c_str(), 0, 330, 68, 120, 22, IDC_ADJACENT_HOLD_UNIT);

    MakeControl(window_, L"STATIC", text.singleKeyHold.c_str(), 0, 24, 108, 190, 22, IDC_SINGLE_KEY_HOLD_LABEL);
    MakeControl(window_, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 220, 104, 100, 24, IDC_SINGLE_KEY_HOLD);
    MakeControl(window_, L"STATIC", text.seconds.c_str(), 0, 330, 108, 120, 22, IDC_SINGLE_KEY_HOLD_UNIT);

    MakeControl(window_, L"STATIC", text.modifierHold.c_str(), 0, 24, 148, 190, 22, IDC_MODIFIER_HOLD_LABEL);
    MakeControl(window_, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 220, 144, 100, 24, IDC_MODIFIER_HOLD);
    MakeControl(window_, L"STATIC", text.seconds.c_str(), 0, 330, 148, 120, 22, IDC_MODIFIER_HOLD_UNIT);

    MakeControl(window_, L"BUTTON", text.launchAtLogin.c_str(), BS_AUTOCHECKBOX | WS_TABSTOP, 24, 192, 320, 24, IDC_LAUNCH_AT_LOGIN);
    MakeControl(window_, L"STATIC", text.launchAtLoginDescription.c_str(), 0, 44, 222, 420, 38, IDC_LAUNCH_AT_LOGIN_DESCRIPTION);

    MakeControl(window_, L"STATIC", L"", 0, 24, 282, 260, 24, IDC_SETTINGS_STATUS);
    MakeControl(window_, L"BUTTON", text.save.c_str(), BS_DEFPUSHBUTTON | WS_TABSTOP, 280, 310, 88, 30, IDC_SAVE_SETTINGS);
    MakeControl(window_, L"BUTTON", text.cancel.c_str(), BS_PUSHBUTTON | WS_TABSTOP, 380, 310, 88, 30, IDC_CANCEL_SETTINGS);
}

void SettingsWindow::PopulateControls(const AppSettings& settings) {
    RefreshText(settings.language);
    SendDlgItemMessageW(window_, IDC_LANGUAGE, CB_SETCURSEL, settings.language == AppLanguage::Chinese ? 1 : 0, 0);
    SetDoubleControl(IDC_ADJACENT_HOLD, settings.detection.adjacentHoldDuration);
    SetDoubleControl(IDC_SINGLE_KEY_HOLD, settings.detection.singleRegularKeyHoldDuration);
    SetDoubleControl(IDC_MODIFIER_HOLD, settings.detection.singleModifierKeyHoldDuration);
    SendDlgItemMessageW(window_, IDC_LAUNCH_AT_LOGIN, BM_SETCHECK, settings.launchAtLogin ? BST_CHECKED : BST_UNCHECKED, 0);
    SetDlgItemTextW(window_, IDC_SETTINGS_STATUS, L"");
}

void SettingsWindow::RefreshText(AppLanguage language) {
    const auto& text = TextFor(language);
    SetWindowTextW(window_, text.settings.c_str());
    SetDlgItemTextW(window_, IDC_LANGUAGE_LABEL, text.language.c_str());
    SetDlgItemTextW(window_, IDC_ADJACENT_HOLD_LABEL, text.adjacentHold.c_str());
    SetDlgItemTextW(window_, IDC_ADJACENT_HOLD_UNIT, text.seconds.c_str());
    SetDlgItemTextW(window_, IDC_SINGLE_KEY_HOLD_LABEL, text.singleKeyHold.c_str());
    SetDlgItemTextW(window_, IDC_SINGLE_KEY_HOLD_UNIT, text.seconds.c_str());
    SetDlgItemTextW(window_, IDC_MODIFIER_HOLD_LABEL, text.modifierHold.c_str());
    SetDlgItemTextW(window_, IDC_MODIFIER_HOLD_UNIT, text.seconds.c_str());
    SetDlgItemTextW(window_, IDC_LAUNCH_AT_LOGIN, text.launchAtLogin.c_str());
    SetDlgItemTextW(window_, IDC_LAUNCH_AT_LOGIN_DESCRIPTION, text.launchAtLoginDescription.c_str());
    SetDlgItemTextW(window_, IDC_SAVE_SETTINGS, text.save.c_str());
    SetDlgItemTextW(window_, IDC_CANCEL_SETTINGS, text.cancel.c_str());
}

void SettingsWindow::PreviewLanguageFromControls() {
    RefreshText(SelectedLanguage());
    SetDlgItemTextW(window_, IDC_SETTINGS_STATUS, L"");
}

bool SettingsWindow::SaveFromControls() {
    AppSettings settings = appState_.Settings();

    settings.language = SelectedLanguage();
    settings.detection.adjacentHoldDuration = ReadDoubleControl(IDC_ADJACENT_HOLD, settings.detection.adjacentHoldDuration);
    settings.detection.singleRegularKeyHoldDuration = ReadDoubleControl(IDC_SINGLE_KEY_HOLD, settings.detection.singleRegularKeyHoldDuration);
    settings.detection.singleModifierKeyHoldDuration = ReadDoubleControl(IDC_MODIFIER_HOLD, settings.detection.singleModifierKeyHoldDuration);
    settings.launchAtLogin = SendDlgItemMessageW(window_, IDC_LAUNCH_AT_LOGIN, BM_GETCHECK, 0, 0) == BST_CHECKED;

    const bool saved = appState_.ApplySettings(settings);
    const auto& text = TextFor(appState_.Language());
    SetDlgItemTextW(window_, IDC_SETTINGS_STATUS, saved ? text.saved.c_str() : text.launchAtLoginFailed.c_str());
    SendDlgItemMessageW(window_, IDC_LAUNCH_AT_LOGIN, BM_SETCHECK, appState_.Settings().launchAtLogin ? BST_CHECKED : BST_UNCHECKED, 0);
    return saved;
}

AppLanguage SettingsWindow::SelectedLanguage() const {
    const auto languageIndex = SendDlgItemMessageW(window_, IDC_LANGUAGE, CB_GETCURSEL, 0, 0);
    return languageIndex == 1 ? AppLanguage::Chinese : AppLanguage::English;
}

bool SettingsWindow::HasFocusedDurationInput() const {
    const int focusedId = GetDlgCtrlID(GetFocus());
    return focusedId == IDC_ADJACENT_HOLD ||
        focusedId == IDC_SINGLE_KEY_HOLD ||
        focusedId == IDC_MODIFIER_HOLD;
}

double SettingsWindow::ReadDoubleControl(int controlId, double fallback) const {
    wchar_t buffer[64]{};
    GetDlgItemTextW(window_, controlId, buffer, static_cast<int>(std::size(buffer)));

    try {
        return std::stod(buffer);
    } catch (...) {
        return fallback;
    }
}

void SettingsWindow::SetDoubleControl(int controlId, double value) {
    wchar_t buffer[32]{};
    swprintf_s(buffer, L"%.1f", value);
    SetDlgItemTextW(window_, controlId, buffer);
}
