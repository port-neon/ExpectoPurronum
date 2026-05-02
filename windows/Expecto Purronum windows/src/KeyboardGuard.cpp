#include "KeyboardGuard.h"

#include "AppMessages.h"

#include <algorithm>
#include <cmath>
#include <vector>

KeyboardGuard* KeyboardGuard::activeInstance_ = nullptr;

KeyboardGuard::KeyboardGuard(HWND notificationWindow)
    : notificationWindow_(notificationWindow) {
}

KeyboardGuard::~KeyboardGuard() {
    Stop();
}

bool KeyboardGuard::Start() {
    if (hook_ != nullptr) {
        return true;
    }

    activeInstance_ = this;
    hook_ = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandleW(nullptr), 0);
    if (hook_ == nullptr) {
        activeInstance_ = nullptr;
        return false;
    }

    return true;
}

void KeyboardGuard::Stop() {
    if (hook_ != nullptr) {
        UnhookWindowsHookEx(hook_);
        hook_ = nullptr;
    }

    if (activeInstance_ == this) {
        activeInstance_ = nullptr;
    }

    pressedKeys_.clear();
    pressedModifierKeys_.clear();
    CancelDetection();
    UpdateDetectionState(KeyboardLockState::Unlocked);
}

void KeyboardGuard::ResetDetection() {
    pressedKeys_.clear();
    pressedModifierKeys_.clear();
    CancelDetection();
    UpdateDetectionState(KeyboardLockState::Unlocked);
}

void KeyboardGuard::UpdateSettings(const DetectionSettings& newSettings) {
    if (settings_ == newSettings) {
        return;
    }

    settings_ = newSettings;
    if (detectionState_ == KeyboardLockState::Locked) {
        return;
    }

    CancelDetection();
    EvaluatePressedKeys();
}

void KeyboardGuard::HandleDetectionTimer() {
    if (!activeCandidate_ || detectionState_ != KeyboardLockState::Suspicious) {
        return;
    }

    const auto candidate = *activeCandidate_;
    if (IsCandidateStillActive(candidate)) {
        UpdateDetectionState(KeyboardLockState::Locked);
    }
}

void KeyboardGuard::SetLocked(bool locked) {
    if (locked) {
        CancelDetection();
        UpdateDetectionState(KeyboardLockState::Locked);
    } else {
        ResetDetection();
    }
}

LRESULT CALLBACK KeyboardGuard::LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && activeInstance_ != nullptr) {
        const auto* event = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (activeInstance_->HandleKeyboardEvent(wParam, *event) == 1) {
            return 1;
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

LRESULT KeyboardGuard::HandleKeyboardEvent(WPARAM wParam, const KBDLLHOOKSTRUCT& event) {
    const DWORD key = event.vkCode;
    const bool isDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    const bool isUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

    if (isDown) {
        if (IsModifierKey(key)) {
            if (pressedModifierKeys_.insert(key).second || !activeCandidate_) {
                EvaluatePressedKeys();
            }
        } else if (IsRegularKey(key)) {
            if (pressedKeys_.insert(key).second || !activeCandidate_) {
                EvaluatePressedKeys();
            }
        }
    } else if (isUp) {
        bool changed = false;
        if (IsModifierKey(key)) {
            changed = pressedModifierKeys_.erase(key) > 0;
        } else {
            changed = pressedKeys_.erase(key) > 0;
        }

        if (changed) {
            EvaluatePressedKeys();
        }
    }

    if (ShouldSuppress(wParam)) {
        return 1;
    }

    return 0;
}

bool KeyboardGuard::ShouldSuppress(WPARAM wParam) const {
    if (detectionState_ != KeyboardLockState::Locked) {
        return false;
    }

    return wParam == WM_KEYDOWN ||
        wParam == WM_SYSKEYDOWN ||
        wParam == WM_KEYUP ||
        wParam == WM_SYSKEYUP;
}

void KeyboardGuard::EvaluatePressedKeys() {
    const auto candidate = BestDetectionCandidate();
    if (!candidate) {
        CancelDetection();
        if (detectionState_ != KeyboardLockState::Locked) {
            UpdateDetectionState(KeyboardLockState::Unlocked);
        }
        return;
    }

    if (detectionState_ == KeyboardLockState::Locked) {
        return;
    }

    if (detectionState_ == KeyboardLockState::Suspicious && activeCandidate_ && *activeCandidate_ == *candidate) {
        return;
    }

    activeCandidate_ = *candidate;
    UpdateDetectionState(KeyboardLockState::Suspicious);
    ScheduleLockCheck(*candidate);
}

void KeyboardGuard::ScheduleLockCheck(const DetectionCandidate& candidate) {
    CancelLockCheck();
    const auto delayMs = static_cast<UINT>(std::max(100.0, candidate.delay * 1000.0));
    SetTimer(notificationWindow_, IDT_KEYBOARD_DETECTION, delayMs, nullptr);
}

void KeyboardGuard::CancelDetection() {
    CancelLockCheck();
    activeCandidate_.reset();
}

void KeyboardGuard::CancelLockCheck() {
    if (notificationWindow_ != nullptr) {
        KillTimer(notificationWindow_, IDT_KEYBOARD_DETECTION);
    }
}

void KeyboardGuard::UpdateDetectionState(KeyboardLockState state) {
    if (detectionState_ == state) {
        return;
    }

    detectionState_ = state;
    if (notificationWindow_ != nullptr) {
        PostMessageW(notificationWindow_, WM_APP_GUARD_STATE_CHANGED, static_cast<WPARAM>(state), 0);
    }
}

std::optional<KeyboardGuard::DetectionCandidate> KeyboardGuard::BestDetectionCandidate() const {
    const auto cluster = LargestAdjacentCluster();
    if (cluster && cluster->size() >= minimumAdjacentKeyCount_) {
        return DetectionCandidate{ CandidateKind::AdjacentCluster, *cluster, settings_.adjacentHoldDuration };
    }

    if (pressedKeys_.empty() && pressedModifierKeys_.size() == 1) {
        return DetectionCandidate{ CandidateKind::SingleModifierKey, pressedModifierKeys_, settings_.singleModifierKeyHoldDuration };
    }

    if (pressedModifierKeys_.empty() && pressedKeys_.size() == 1) {
        return DetectionCandidate{ CandidateKind::SingleRegularKey, pressedKeys_, settings_.singleRegularKeyHoldDuration };
    }

    return std::nullopt;
}

bool KeyboardGuard::IsCandidateStillActive(const DetectionCandidate& candidate) const {
    switch (candidate.kind) {
    case CandidateKind::AdjacentCluster:
        return std::all_of(candidate.keys.begin(), candidate.keys.end(), [this](DWORD key) {
            return pressedKeys_.contains(key);
        });
    case CandidateKind::SingleRegularKey:
        return pressedModifierKeys_.empty() && pressedKeys_ == candidate.keys;
    case CandidateKind::SingleModifierKey:
        return pressedKeys_.empty() && pressedModifierKeys_ == candidate.keys;
    }

    return false;
}

std::optional<std::set<DWORD>> KeyboardGuard::LargestAdjacentCluster() const {
    std::vector<DWORD> positionedKeys;
    for (const auto key : pressedKeys_) {
        if (KeyPositions().contains(key)) {
            positionedKeys.push_back(key);
        }
    }

    if (positionedKeys.empty()) {
        return std::nullopt;
    }

    std::set<DWORD> visited;
    std::set<DWORD> largestCluster;

    for (const auto key : positionedKeys) {
        if (visited.contains(key)) {
            continue;
        }

        std::set<DWORD> cluster;
        std::vector<DWORD> stack{ key };
        visited.insert(key);

        while (!stack.empty()) {
            const auto current = stack.back();
            stack.pop_back();
            cluster.insert(current);

            for (const auto neighbor : positionedKeys) {
                if (!visited.contains(neighbor) && AreNeighborKeys(current, neighbor)) {
                    visited.insert(neighbor);
                    stack.push_back(neighbor);
                }
            }
        }

        if (cluster.size() > largestCluster.size()) {
            largestCluster = std::move(cluster);
        }
    }

    return largestCluster;
}

bool KeyboardGuard::AreNeighborKeys(DWORD first, DWORD second) const {
    const auto& positions = KeyPositions();
    const auto firstIt = positions.find(first);
    const auto secondIt = positions.find(second);
    if (firstIt == positions.end() || secondIt == positions.end()) {
        return false;
    }

    const auto dx = std::abs(firstIt->second.x - secondIt->second.x);
    const auto dy = std::abs(firstIt->second.y - secondIt->second.y);
    return (dx > 0.0 || dy > 0.0) && dx <= 1.35 && dy <= 1.1;
}

bool KeyboardGuard::IsModifierKey(DWORD key) const {
    switch (key) {
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
    case VK_MENU:
    case VK_LMENU:
    case VK_RMENU:
    case VK_LWIN:
    case VK_RWIN:
        return true;
    default:
        return false;
    }
}

bool KeyboardGuard::IsRegularKey(DWORD key) const {
    return KeyPositions().contains(key) && !IsModifierKey(key);
}

const std::map<DWORD, KeyboardGuard::KeyPosition>& KeyboardGuard::KeyPositions() {
    static const std::map<DWORD, KeyPosition> positions{
        { VK_OEM_3, { -1.0, 0.0 } }, { '1', { 0.0, 0.0 } }, { '2', { 1.0, 0.0 } },
        { '3', { 2.0, 0.0 } }, { '4', { 3.0, 0.0 } }, { '5', { 4.0, 0.0 } },
        { '6', { 5.0, 0.0 } }, { '7', { 6.0, 0.0 } }, { '8', { 7.0, 0.0 } },
        { '9', { 8.0, 0.0 } }, { '0', { 9.0, 0.0 } }, { VK_OEM_MINUS, { 10.0, 0.0 } },
        { VK_OEM_PLUS, { 11.0, 0.0 } }, { VK_BACK, { 12.0, 0.0 } },

        { 'Q', { 0.5, 1.0 } }, { 'W', { 1.5, 1.0 } }, { 'E', { 2.5, 1.0 } },
        { 'R', { 3.5, 1.0 } }, { 'T', { 4.5, 1.0 } }, { 'Y', { 5.5, 1.0 } },
        { 'U', { 6.5, 1.0 } }, { 'I', { 7.5, 1.0 } }, { 'O', { 8.5, 1.0 } },
        { 'P', { 9.5, 1.0 } }, { VK_OEM_4, { 10.5, 1.0 } }, { VK_OEM_6, { 11.5, 1.0 } },

        { 'A', { 0.75, 2.0 } }, { 'S', { 1.75, 2.0 } }, { 'D', { 2.75, 2.0 } },
        { 'F', { 3.75, 2.0 } }, { 'G', { 4.75, 2.0 } }, { 'H', { 5.75, 2.0 } },
        { 'J', { 6.75, 2.0 } }, { 'K', { 7.75, 2.0 } }, { 'L', { 8.75, 2.0 } },
        { VK_OEM_1, { 9.75, 2.0 } }, { VK_OEM_7, { 10.75, 2.0 } },

        { 'Z', { 1.25, 3.0 } }, { 'X', { 2.25, 3.0 } }, { 'C', { 3.25, 3.0 } },
        { 'V', { 4.25, 3.0 } }, { 'B', { 5.25, 3.0 } }, { 'N', { 6.25, 3.0 } },
        { 'M', { 7.25, 3.0 } }, { VK_OEM_COMMA, { 8.25, 3.0 } },
        { VK_OEM_PERIOD, { 9.25, 3.0 } }, { VK_OEM_2, { 10.25, 3.0 } },

        { VK_SPACE, { 5.0, 4.0 } }, { VK_ESCAPE, { -1.0, -1.0 } }
    };

    return positions;
}
