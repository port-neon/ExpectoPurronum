//
//  KeyboardLockState.swift
//  Expecto Purronum
//

enum KeyboardLockState: Equatable {
    case unlocked
    case suspicious
    case locked

    var isLocked: Bool {
        self == .locked
    }

    var title: String {
        switch self {
        case .unlocked:
            "Unlocked"
        case .suspicious:
            "Suspicious"
        case .locked:
            "Locked"
        }
    }
}
