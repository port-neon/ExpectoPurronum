#include "SettingsStore.h"

#include <ShlObj.h>
#include <Windows.h>

#include <fstream>
#include <sstream>
#include <string>

namespace {
std::wstring GetKnownFolderPath(REFKNOWNFOLDERID folderId) {
    PWSTR rawPath = nullptr;
    if (FAILED(SHGetKnownFolderPath(folderId, KF_FLAG_CREATE, nullptr, &rawPath))) {
        return L".";
    }

    std::wstring path(rawPath);
    CoTaskMemFree(rawPath);
    return path;
}

bool ParseBool(const std::string& value, bool fallback) {
    if (value == "true" || value == "1") {
        return true;
    }
    if (value == "false" || value == "0") {
        return false;
    }
    return fallback;
}

double ParseDouble(const std::string& value, double fallback) {
    try {
        return std::stod(value);
    } catch (...) {
        return fallback;
    }
}
}

AppSettings SettingsStore::Load() const {
    AppSettings settings;
    std::ifstream file(SettingsPath());
    if (!file) {
        return settings;
    }

    std::string line;
    while (std::getline(file, line)) {
        const auto separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        const auto key = line.substr(0, separator);
        const auto value = line.substr(separator + 1);

        if (key == "language") {
            settings.language = value == "chinese" ? AppLanguage::Chinese : AppLanguage::English;
        } else if (key == "adjacentHoldDuration") {
            settings.detection.adjacentHoldDuration = ParseDouble(value, settings.detection.adjacentHoldDuration);
        } else if (key == "singleRegularKeyHoldDuration") {
            settings.detection.singleRegularKeyHoldDuration = ParseDouble(value, settings.detection.singleRegularKeyHoldDuration);
        } else if (key == "singleModifierKeyHoldDuration") {
            settings.detection.singleModifierKeyHoldDuration = ParseDouble(value, settings.detection.singleModifierKeyHoldDuration);
        } else if (key == "launchAtLogin") {
            settings.launchAtLogin = ParseBool(value, settings.launchAtLogin);
        }
    }

    return settings;
}

void SettingsStore::Save(const AppSettings& settings) const {
    const auto path = SettingsPath();
    std::filesystem::create_directories(path.parent_path());

    std::ofstream file(path, std::ios::trunc);
    if (!file) {
        return;
    }

    file << "language=" << (settings.language == AppLanguage::Chinese ? "chinese" : "english") << "\n";
    file << "adjacentHoldDuration=" << settings.detection.adjacentHoldDuration << "\n";
    file << "singleRegularKeyHoldDuration=" << settings.detection.singleRegularKeyHoldDuration << "\n";
    file << "singleModifierKeyHoldDuration=" << settings.detection.singleModifierKeyHoldDuration << "\n";
    file << "launchAtLogin=" << (settings.launchAtLogin ? "true" : "false") << "\n";
}

std::filesystem::path SettingsStore::SettingsPath() const {
    auto path = std::filesystem::path(GetKnownFolderPath(FOLDERID_RoamingAppData));
    path /= L"Expecto Purronum";
    path /= L"settings.ini";
    return path;
}
