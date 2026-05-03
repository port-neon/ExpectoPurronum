#include "Localization.h"

const LocalizedText& TextFor(AppLanguage language) {
    static const LocalizedText english{
        L"Expecto Purronum",
        L"English",
        L"Protection",
        L"Keyboard",
        L"On",
        L"Paused",
        L"Protection On",
        L"Protection Paused",
        L"Start Protection",
        L"Pause Protection",
        L"Lock",
        L"Unlock",
        L"Keyboard Lock",
        L"Locked",
        L"Unlocked",
        L"Meow meow meow",
        L"Settings",
        L"Quit",
        L"Adjacent hold",
        L"Single key hold",
        L"Modifier hold",
        L"Launch at Login",
        L"Start Expecto Purronum automatically when you sign in to Windows.",
        L"Launch at Login enabled",
        L"Launch at Login disabled",
        L"Failed to update Launch at Login.",
        L"Language",
        L"seconds",
        L"Save",
        L"Cancel",
        L"Saved",
        L"Expecto Purronum",
        L"Meow meow~ Expecto Purronum spell has been cast by the cat-wizard.\n\n"
        L"Unlock the keyboard with Purrhomora, or pause protection with Expelliarmeows.\n"
        L"Don't forget to restart protection later.",
        L"Purrhomora\r\n(Unlock Keyboard)",
        L"Expelliarmeows\r\n(Pause Protection)"
    };

    static const LocalizedText chinese{
        L"Expecto Purronum",
        L"\u4e2d\u6587",
        L"\u952e\u76d8\u5b88\u62a4\u72b6\u6001",
        L"\u952e\u76d8\u72b6\u6001",
        L"\u5b88\u62a4\u5df2\u5f00\u542f",
        L"\u5b88\u62a4\u5df2\u6682\u505c",
        L"\u952e\u76d8\u5b88\u62a4\u4e2d",
        L"\u6682\u505c\u62a4\u952e\u4e2d",
        L"\u5f00\u59cb\u5b88\u62a4\u952e\u76d8",
        L"\u6682\u505c\u5b88\u62a4\u952e\u76d8",
        L"\u5c01\u5370\u952e\u76d8",
        L"\u89e3\u9664\u5c01\u5370",
        L"\u952e\u76d8\u9501",
        L"\u5df2\u5c01\u5370",
        L"\u672a\u5c01\u5370",
        L"\u55b5\u54aa\u55b5\u54aa\u54c4",
        L"\u8bbe\u7f6e",
        L"\u9000\u51fa",
        L"\u76f8\u90bb\u6309\u952e\u6301\u7eed\u65f6\u95f4",
        L"\u5355\u4e2a\u6309\u952e\u6301\u7eed\u65f6\u95f4",
        L"\u7279\u6b8a\u952e\u6301\u7eed\u65f6\u95f4",
        L"\u5f00\u673a\u81ea\u542f\u52a8",
        L"\u767b\u5f55 Windows \u65f6\u81ea\u52a8\u542f\u52a8 Expecto Purronum\u3002",
        L"\u5df2\u5f00\u542f\u5f00\u673a\u81ea\u542f\u52a8",
        L"\u5df2\u5173\u95ed\u5f00\u673a\u81ea\u542f\u52a8",
        L"\u66f4\u65b0\u5f00\u673a\u81ea\u542f\u52a8\u5931\u8d25\u3002",
        L"\u8bed\u8a00",
        L"\u79d2",
        L"\u4fdd\u5b58",
        L"\u53d6\u6d88",
        L"\u5df2\u4fdd\u5b58",
        L"\u547c\u55b5\u62a4\u952e",
        L"\u55b5\u54e6\u4e0d\uff01\u55b5\u55b5\u65bd\u5c55\u4e86\u547c\u55b5\u62a4\u952e\u5492\u3002\n\n"
        L"\u5feb\u4f7f\u7528\u55b5\u55b5\u73b0\u5f62\u5492\u6765\u89e3\u9501\u952e\u76d8\uff0c"
        L"\u6216\u8005\u4f7f\u7528\u9664\u55b5\u6b66\u5668\u5492\u6765\u6682\u505c\u62a4\u952e\u3002\n"
        L"\u540e\u7eed\u522b\u5fd8\u4e86\u624b\u52a8\u5f00\u542f\u62a4\u952e\u72b6\u6001\u54e6\u3002",
        L"\u55b5\u55b5\u73b0\u5f62\u5492\r\n\uff08\u89e3\u9664\u952e\u76d8\u5c01\u5370\uff09",
        L"\u9664\u55b5\u6b66\u5668\u5492\r\n\uff08\u6682\u505c\u5b88\u62a4\uff09"
    };

    return language == AppLanguage::Chinese ? chinese : english;
}

std::wstring TitleForState(AppLanguage language, KeyboardLockState state) {
    const auto& text = TextFor(language);
    switch (state) {
    case KeyboardLockState::Unlocked:
        return text.unlocked;
    case KeyboardLockState::Suspicious:
        return text.suspicious;
    case KeyboardLockState::Locked:
        return text.lockedStatus;
    }

    return text.unlocked;
}
