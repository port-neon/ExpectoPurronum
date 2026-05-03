#pragma once

#include <Windows.h>

#include <functional>
#include <map>
#include <optional>
#include <set>

#include "DetectionSettings.h"
#include "KeyboardLockState.h"

class KeyboardGuard {
public:
    explicit KeyboardGuard(HWND notificationWindow);
    ~KeyboardGuard();

    KeyboardGuard(const KeyboardGuard&) = delete;
    KeyboardGuard& operator=(const KeyboardGuard&) = delete;

    bool Start();
    void Stop();
    void ResetDetection();
    void UpdateSettings(const DetectionSettings& newSettings);
    void HandleDetectionTimer();
    void SetLocked(bool locked);

    KeyboardLockState State() const { return detectionState_; }
    bool IsRunning() const { return hook_ != nullptr; }

private:
    enum class CandidateKind {
        AdjacentCluster,
        SingleRegularKey,
        SingleModifierKey
    };

    struct KeyPosition {
        double x = 0.0;
        double y = 0.0;
    };

    struct DetectionCandidate {
        CandidateKind kind = CandidateKind::SingleRegularKey;
        std::set<DWORD> keys;
        double delay = 0.0;

        bool operator==(const DetectionCandidate& other) const {
            return kind == other.kind && keys == other.keys && delay == other.delay;
        }
    };

    static LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam);
    static KeyboardGuard* activeInstance_;

    LRESULT HandleKeyboardEvent(WPARAM wParam, const KBDLLHOOKSTRUCT& event);
    bool ShouldSuppress(WPARAM wParam) const;
    void EvaluatePressedKeys();
    void ScheduleLockCheck(const DetectionCandidate& candidate);
    void CancelDetection();
    void CancelLockCheck();
    void UpdateDetectionState(KeyboardLockState state);
    std::optional<DetectionCandidate> BestDetectionCandidate() const;
    bool IsCandidateStillActive(const DetectionCandidate& candidate) const;
    std::optional<std::set<DWORD>> LargestAdjacentCluster() const;
    bool AreNeighborKeys(DWORD first, DWORD second) const;
    bool IsModifierKey(DWORD key) const;
    bool IsRegularKey(DWORD key) const;

    static const std::map<DWORD, KeyPosition>& KeyPositions();

    HWND notificationWindow_ = nullptr;
    HHOOK hook_ = nullptr;
    std::set<DWORD> pressedKeys_;
    std::set<DWORD> pressedModifierKeys_;
    std::optional<DetectionCandidate> activeCandidate_;
    KeyboardLockState detectionState_ = KeyboardLockState::Unlocked;
    DetectionSettings settings_;
    const size_t minimumAdjacentKeyCount_ = 2;
};
