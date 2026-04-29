//
//  AppLanguage.swift
//  Expecto Purronum
//

enum AppLanguage: String, CaseIterable, Identifiable {
    case chinese
    case english

    var id: String {
        rawValue
    }

    var displayName: String {
        switch self {
        case .chinese:
            return "中文"
        case .english:
            return "English"
        }
    }

    var monitoring: String {
        switch self {
        case .chinese:
            return "键盘守护状态"
        case .english:
            return "Protection"
        }
    }

    var keyboard: String {
        switch self {
        case .chinese:
            return "键盘状态"
        case .english:
            return "Keyboard"
        }
    }

    var on: String {
        switch self {
        case .chinese:
            return "守护已开启"
        case .english:
            return "On"
        }
    }

    var off: String {
        switch self {
        case .chinese:
            return "守护已暂停"
        case .english:
            return "Paused"
        }
    }

    var monitoringOn: String {
        switch self {
        case .chinese:
            return "键盘守护中"
        case .english:
            return "Protection On"
        }
    }

    var monitoringOff: String {
        switch self {
        case .chinese:
            return "暂停护键中"
        case .english:
            return "Protection Paused"
        }
    }

    var startMonitoring: String {
        switch self {
        case .chinese:
            return "开始守护键盘"
        case .english:
            return "Start Protection"
        }
    }

    var stopMonitoring: String {
        switch self {
        case .chinese:
            return "暂停守护键盘"
        case .english:
            return "Pause Protection"
        }
    }

    var lock: String {
        switch self {
        case .chinese:
            return "封印键盘"
        case .english:
            return "Lock"
        }
    }

    var unlock: String {
        switch self {
        case .chinese:
            return "解除封印"
        case .english:
            return "Unlock"
        }
    }

    var locked: String {
        switch self {
        case .chinese:
            return "键盘锁"
        case .english:
            return "Keyboard Lock"
        }
    }

    var lockedStatus: String {
        switch self {
        case .chinese:
            return "已封印"
        case .english:
            return "Locked"
        }
    }

    var unlocked: String {
        switch self {
        case .chinese:
            return "未封印"
        case .english:
            return "Unlocked"
        }
    }

    var suspicious: String {
        switch self {
        case .chinese:
            return "✨喵咪喵咪哄✨"
        case .english:
            return "Meow meow meow"
        }
    }

    var settings: String {
        switch self {
        case .chinese:
            return "设置"
        case .english:
            return "Settings"
        }
    }

    var quit: String {
        switch self {
        case .chinese:
            return "退出"
        case .english:
            return "Quit"
        }
    }

    var adjacentHold: String {
        switch self {
        case .chinese:
            return "相邻按键持续时间"
        case .english:
            return "Adjacent hold"
        }
    }

    var singleKeyHold: String {
        switch self {
        case .chinese:
            return "单个按键持续时间"
        case .english:
            return "Single key hold"
        }
    }

    var modifierHold: String {
        switch self {
        case .chinese:
            return "特殊键持续时间"
        case .english:
            return "Modifier hold"
        }
    }

    var launchAtLogin: String {
        switch self {
        case .chinese:
            return "开机自启动"
        case .english:
            return "Launch at Login"
        }
    }

    var lockAlertTitle: String {
        switch self {
        case .chinese:
            return "呼喵护键"
        case .english:
            return "Expecto Purronum"
        }
    }

    var lockAlertMessage: String {
        switch self {
        case .chinese:
            return """
            😳哦不！喵喵施展了呼喵护键咒 
            🪄快使用喵喵现形咒来解锁键盘
            🪄或者使用除喵武器咒来暂停护键
            ✨后续别忘了手动开启护键状态哦～
            """
        case .english:
            return """
            🧙Meow meow~ Expecto Purronum spell has been cast by the cat-wizard.
            🪄Unlock the keyboard with the spell Purrhomora.
            🪄Or pause the protection with the spell Expelliarmeows.
            ✨Don't forget to restart the Protection later ~
            """
        }
    }

    var lockAlertUnlockButton: String {
        switch self {
        case .chinese:
            return "喵喵现形咒（解除键盘封印）"
        case .english:
            return "Purrhomora (Unlock Keyboard)"
        }
    }

    var lockAlertKeepLockedButton: String {
        switch self {
        case .chinese:
            return "除喵武器咒（暂停守护）"
        case .english:
            return "Expelliarmeows (Pause Protection)"
        }
    }

    func title(for state: KeyboardLockState) -> String {
        switch state {
        case .unlocked:
            return unlocked
        case .suspicious:
            return suspicious
        case .locked:
            return lockedStatus
        }
    }
}
