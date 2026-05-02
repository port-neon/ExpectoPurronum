#include "LaunchAtLoginService.h"

#include <Windows.h>

#include <iterator>

namespace {
constexpr const wchar_t* kRunKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr const wchar_t* kValueName = L"Expecto Purronum";
}

bool LaunchAtLoginService::IsEnabled() {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kRunKey, 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return false;
    }

    wchar_t value[MAX_PATH * 2]{};
    DWORD valueSize = sizeof(value);
    const auto result = RegQueryValueExW(key, kValueName, nullptr, nullptr, reinterpret_cast<LPBYTE>(value), &valueSize);
    RegCloseKey(key);

    if (result != ERROR_SUCCESS) {
        return false;
    }

    return std::wstring(value).find(ExecutablePath()) != std::wstring::npos;
}

bool LaunchAtLoginService::SetEnabled(bool enabled) {
    HKEY key = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kRunKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
        return false;
    }

    LONG result = ERROR_SUCCESS;
    if (enabled) {
        const auto command = L"\"" + ExecutablePath() + L"\"";
        result = RegSetValueExW(
            key,
            kValueName,
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(command.c_str()),
            static_cast<DWORD>((command.size() + 1) * sizeof(wchar_t)));
    } else {
        result = RegDeleteValueW(key, kValueName);
        if (result == ERROR_FILE_NOT_FOUND) {
            result = ERROR_SUCCESS;
        }
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

std::wstring LaunchAtLoginService::ExecutablePath() {
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, static_cast<DWORD>(std::size(path)));
    return path;
}
