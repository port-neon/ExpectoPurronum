#pragma once

enum class KeyboardLockState {
    Unlocked,
    Suspicious,
    Locked
};

inline bool IsLocked(KeyboardLockState state) {
    return state == KeyboardLockState::Locked;
}
