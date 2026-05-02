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
        L"Language",
        L"Save",
        L"Cancel",
        L"Expecto Purronum",
        L"Expecto Purronum has locked the keyboard.\n\nChoose Yes to unlock the keyboard.\nChoose No to pause protection."
    };

    static const LocalizedText chinese{
        L"Expecto Purronum",
        L"中文",
        L"键盘守护状态",
        L"键盘状态",
        L"守护已开启",
        L"守护已暂停",
        L"键盘守护中",
        L"暂停护键中",
        L"开始守护键盘",
        L"暂停守护键盘",
        L"封印键盘",
        L"解除封印",
        L"键盘锁",
        L"已封印",
        L"未封印",
        L"喵咪喵咪哄",
        L"设置",
        L"退出",
        L"相邻按键持续时间",
        L"单个按键持续时间",
        L"特殊键持续时间",
        L"开机自启动",
        L"语言",
        L"保存",
        L"取消",
        L"呼喵护键",
        L"呼喵护键已锁定键盘。\n\n选择“是”解除键盘封印。\n选择“否”暂停守护。"
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
