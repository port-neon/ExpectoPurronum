#pragma once

#include <string>

#include "AppLanguage.h"
#include "KeyboardLockState.h"

struct LocalizedText {
    std::wstring appName;
    std::wstring displayName;
    std::wstring protection;
    std::wstring keyboard;
    std::wstring on;
    std::wstring off;
    std::wstring monitoringOn;
    std::wstring monitoringOff;
    std::wstring startMonitoring;
    std::wstring stopMonitoring;
    std::wstring lock;
    std::wstring unlock;
    std::wstring keyboardLock;
    std::wstring lockedStatus;
    std::wstring unlocked;
    std::wstring suspicious;
    std::wstring settings;
    std::wstring quit;
    std::wstring adjacentHold;
    std::wstring singleKeyHold;
    std::wstring modifierHold;
    std::wstring launchAtLogin;
    std::wstring language;
    std::wstring save;
    std::wstring cancel;
    std::wstring lockedAlertTitle;
    std::wstring lockedAlertMessage;
};

const LocalizedText& TextFor(AppLanguage language);
std::wstring TitleForState(AppLanguage language, KeyboardLockState state);
