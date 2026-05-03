#pragma once

#include <Windows.h>

#include "SettingsStore.h"

class AppState;

class SettingsWindow {
public:
    SettingsWindow(HINSTANCE instance, HWND ownerWindow, AppState& appState);
    ~SettingsWindow();

    void Show(const AppSettings& settings);
    void Refresh(const AppSettings& settings);

private:
    static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

    void RegisterClass();
    void CreateIfNeeded();
    void CreateControls();
    void PopulateControls(const AppSettings& settings);
    void RefreshText(AppLanguage language);
    void PreviewLanguageFromControls();
    bool SaveFromControls();
    AppLanguage SelectedLanguage() const;
    bool HasFocusedDurationInput() const;
    double ReadDoubleControl(int controlId, double fallback) const;
    void SetDoubleControl(int controlId, double value);

    HINSTANCE instance_ = nullptr;
    HWND ownerWindow_ = nullptr;
    HWND window_ = nullptr;
    AppState& appState_;
};
