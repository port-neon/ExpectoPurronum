#pragma once

#include <filesystem>

#include "AppLanguage.h"
#include "DetectionSettings.h"

struct AppSettings {
    AppLanguage language = AppLanguage::English;
    DetectionSettings detection;
    bool launchAtLogin = false;
};

class SettingsStore {
public:
    AppSettings Load() const;
    void Save(const AppSettings& settings) const;

private:
    std::filesystem::path SettingsPath() const;
};
