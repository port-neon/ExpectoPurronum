#pragma once

#include <string>

class LaunchAtLoginService {
public:
    static bool IsEnabled();
    static bool SetEnabled(bool enabled);

private:
    static std::wstring ExecutablePath();
};
