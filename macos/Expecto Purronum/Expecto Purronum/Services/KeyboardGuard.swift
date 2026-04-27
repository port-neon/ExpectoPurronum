//
//  KeyboardGuard.swift
//  Expecto Purronum
//

import ApplicationServices
import Foundation

final class KeyboardGuard {
    var onLockStateChange: ((KeyboardLockState) -> Void)?

    private var eventTap: CFMachPort?
    private var runLoopSource: CFRunLoopSource?
    private var pressedKeys = Set<CGKeyCode>()
    private var pressedModifierKeys = Set<CGKeyCode>()
    private var detectionWorkItem: DispatchWorkItem?
    private var activeCandidate: DetectionCandidate?
    private var detectionState: KeyboardLockState = .unlocked
    private let minimumAdjacentKeyCount = 2
    private var settings = DetectionSettings()

    func start() -> Bool {
        guard eventTap == nil else {
            print("[KeyboardGuard] monitoring already started")
            return true
        }

        print("[KeyboardGuard] bundleIdentifier=\(Bundle.main.bundleIdentifier ?? "nil")")
        print("[KeyboardGuard] bundlePath=\(Bundle.main.bundlePath)")
        print("[KeyboardGuard] executablePath=\(Bundle.main.executablePath ?? "nil")")

        let isAccessibilityGranted = AXIsProcessTrusted()
        let isInputMonitoringGranted = CGPreflightListenEventAccess()

        print("[KeyboardGuard] AXIsProcessTrusted() returned \(isAccessibilityGranted)")
        print("[KeyboardGuard] CGPreflightListenEventAccess() returned \(isInputMonitoringGranted)")

        guard isAccessibilityGranted else {
            print("[KeyboardGuard] Accessibility not granted")
            return false
        }

        guard isInputMonitoringGranted else {
            print("[KeyboardGuard] Input Monitoring not granted")
            return false
        }

        let eventMask = CGEventMask(
            (1 << UInt64(CGEventType.keyDown.rawValue)) |
            (1 << UInt64(CGEventType.keyUp.rawValue)) |
            (1 << UInt64(CGEventType.flagsChanged.rawValue))
        )
        let callback: CGEventTapCallBack = { proxy, type, event, userInfo in
            guard let userInfo else {
                return Unmanaged.passUnretained(event)
            }

            let guardInstance = Unmanaged<KeyboardGuard>.fromOpaque(userInfo).takeUnretainedValue()
            guardInstance.handle(proxy: proxy, type: type, event: event)

            if guardInstance.shouldSuppress(type: type) {
                print("[KeyboardGuard] suppressed \(type)")
                return nil
            }

            return Unmanaged.passUnretained(event)
        }

        guard let tap = CGEvent.tapCreate(
            tap: .cgSessionEventTap,
            place: .headInsertEventTap,
            options: .defaultTap,
            eventsOfInterest: eventMask,
            callback: callback,
            userInfo: Unmanaged.passUnretained(self).toOpaque()
        ) else {
            print("[KeyboardGuard] CGEvent.tapCreate failed")
            return false
        }

        guard let source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, tap, 0) else {
            print("[KeyboardGuard] failed to create run loop source for CGEventTap")
            CFMachPortInvalidate(tap)
            return false
        }

        eventTap = tap
        runLoopSource = source
        CFRunLoopAddSource(CFRunLoopGetCurrent(), source, .commonModes)
        CGEvent.tapEnable(tap: tap, enable: true)

        print("[KeyboardGuard] started keyboard event listening")
        return true
    }

    func stop() {
        if let tap = eventTap {
            CGEvent.tapEnable(tap: tap, enable: false)
        }

        if let source = runLoopSource {
            CFRunLoopRemoveSource(CFRunLoopGetCurrent(), source, .commonModes)
        }

        if let tap = eventTap {
            CFMachPortInvalidate(tap)
        }

        pressedKeys.removeAll()
        pressedModifierKeys.removeAll()
        cancelDetection()
        updateDetectionState(.unlocked)
        runLoopSource = nil
        eventTap = nil

        print("[KeyboardGuard] stopped keyboard event listening")
    }

    func resetDetection() {
        pressedKeys.removeAll()
        pressedModifierKeys.removeAll()
        cancelDetection()
        updateDetectionState(.unlocked)
        print("[KeyboardGuard] detection reset")
    }

    func updateSettings(_ newSettings: DetectionSettings) {
        guard settings != newSettings else { return }

        settings = newSettings

        if detectionState == .locked {
            print("[KeyboardGuard] settings updated while locked")
            return
        }

        cancelDetection()
        evaluatePressedKeys()
        print("[KeyboardGuard] settings updated: adjacentHold=\(settings.adjacentHoldDuration), singleKeyHold=\(settings.singleRegularKeyHoldDuration), modifierHold=\(settings.singleModifierKeyHoldDuration)")
    }

    private func handle(proxy: CGEventTapProxy, type: CGEventType, event: CGEvent) {
        switch type {
        case .keyDown:
            let keyCode = CGKeyCode(event.getIntegerValueField(.keyboardEventKeycode))
            let didInsert = pressedKeys.insert(keyCode).inserted
            print("[KeyboardGuard] keyDown keyCode=\(keyCode) pressedKeys=\(formatPressedKeys()) modifiers=\(formatKeys(pressedModifierKeys.sorted()))")
            if didInsert || activeCandidate == nil {
                evaluatePressedKeys()
            }

        case .keyUp:
            let keyCode = CGKeyCode(event.getIntegerValueField(.keyboardEventKeycode))
            let didRemove = pressedKeys.remove(keyCode) != nil
            print("[KeyboardGuard] keyUp keyCode=\(keyCode) pressedKeys=\(formatPressedKeys())")
            if didRemove {
                evaluatePressedKeys()
            }

        case .flagsChanged:
            let keyCode = CGKeyCode(event.getIntegerValueField(.keyboardEventKeycode))
            let didChange = updateModifierState(keyCode: keyCode, flags: event.flags)
            print("[KeyboardGuard] flagsChanged keyCode=\(keyCode) pressedKeys=\(formatPressedKeys()) modifiers=\(formatKeys(pressedModifierKeys.sorted()))")
            if didChange {
                evaluatePressedKeys()
            }

        case .tapDisabledByTimeout, .tapDisabledByUserInput:
            if let eventTap {
                CGEvent.tapEnable(tap: eventTap, enable: true)
                print("[KeyboardGuard] event tap re-enabled after \(type)")
            }

        default:
            break
        }
    }

    private func shouldSuppress(type: CGEventType) -> Bool {
        guard detectionState == .locked else {
            return false
        }

        switch type {
        case .keyDown, .keyUp, .flagsChanged:
            return true
        default:
            return false
        }
    }

    private func formatPressedKeys() -> String {
        let sortedKeys = pressedKeys.sorted()
        return "[\(sortedKeys.map(String.init).joined(separator: ", "))]"
    }

    private func evaluatePressedKeys() {
        guard let candidate = bestDetectionCandidate() else {
            cancelDetection()
            if detectionState != .locked {
                updateDetectionState(.unlocked)
            }
            return
        }

        if detectionState == .locked {
            return
        }

        if detectionState == .suspicious, candidate == activeCandidate {
            return
        }

        activeCandidate = candidate
        updateDetectionState(.suspicious)
        scheduleLockCheck(for: candidate)

        print("[KeyboardGuard] suspicious \(candidate.logDescription); waiting \(candidate.delay)s")
    }

    private func scheduleLockCheck(for candidate: DetectionCandidate) {
        cancelLockCheck()

        let workItem = DispatchWorkItem { [weak self] in
            guard let self else { return }
            guard self.activeCandidate == candidate,
                  self.isCandidateStillActive(candidate),
                  self.detectionState == .suspicious else {
                return
            }

            self.updateDetectionState(.locked)
            print("[KeyboardGuard] locked after sustained \(candidate.logDescription)")
        }

        detectionWorkItem = workItem
        DispatchQueue.main.asyncAfter(deadline: .now() + candidate.delay, execute: workItem)
    }

    private func cancelLockCheck() {
        detectionWorkItem?.cancel()
        detectionWorkItem = nil
    }

    private func cancelDetection() {
        cancelLockCheck()
        activeCandidate = nil
    }

    private func updateDetectionState(_ state: KeyboardLockState) {
        guard detectionState != state else { return }

        detectionState = state
        onLockStateChange?(state)
        print("[KeyboardGuard] state=\(state.title)")
    }

    private func bestDetectionCandidate() -> DetectionCandidate? {
        if let cluster = largestAdjacentCluster(in: pressedKeys),
           cluster.count >= minimumAdjacentKeyCount {
            return DetectionCandidate(
                kind: .adjacentCluster,
                keys: Set(cluster),
                delay: settings.adjacentHoldDuration
            )
        }

        if pressedKeys.isEmpty,
           pressedModifierKeys.count == 1,
           let key = pressedModifierKeys.first {
            return DetectionCandidate(
                kind: .singleModifierKey,
                keys: [key],
                delay: settings.singleModifierKeyHoldDuration
            )
        }

        guard pressedModifierKeys.isEmpty,
              pressedKeys.count == 1,
              let key = pressedKeys.first else {
            return nil
        }

        return DetectionCandidate(
            kind: .singleRegularKey,
            keys: [key],
            delay: settings.singleRegularKeyHoldDuration
        )
    }

    private func isCandidateStillActive(_ candidate: DetectionCandidate) -> Bool {
        switch candidate.kind {
        case .adjacentCluster:
            return candidate.keys.isSubset(of: pressedKeys)
        case .singleRegularKey:
            return pressedModifierKeys.isEmpty && pressedKeys == candidate.keys
        case .singleModifierKey:
            return pressedKeys.isEmpty && pressedModifierKeys == candidate.keys
        }
    }

    private func largestAdjacentCluster(in keys: Set<CGKeyCode>) -> [CGKeyCode]? {
        let positionedKeys = keys.filter { Self.keyPositions[$0] != nil }
        guard !positionedKeys.isEmpty else { return nil }

        var visited = Set<CGKeyCode>()
        var largestCluster = [CGKeyCode]()

        for key in positionedKeys.sorted() where !visited.contains(key) {
            var cluster = [CGKeyCode]()
            var stack = [key]
            visited.insert(key)

            while let current = stack.popLast() {
                cluster.append(current)

                for neighbor in positionedKeys where !visited.contains(neighbor) && areNeighborKeys(current, neighbor) {
                    visited.insert(neighbor)
                    stack.append(neighbor)
                }
            }

            if cluster.count > largestCluster.count {
                largestCluster = cluster
            }
        }

        return largestCluster.sorted()
    }

    private func areNeighborKeys(_ first: CGKeyCode, _ second: CGKeyCode) -> Bool {
        guard let firstPosition = Self.keyPositions[first],
              let secondPosition = Self.keyPositions[second] else {
            return false
        }

        let dx = abs(firstPosition.x - secondPosition.x)
        let dy = abs(firstPosition.y - secondPosition.y)
        return dx > 0 || dy > 0 ? dx <= 1.35 && dy <= 1.1 : false
    }

    private func updateModifierState(keyCode: CGKeyCode, flags: CGEventFlags) -> Bool {
        guard let modifier = Self.modifierKeys[keyCode] else {
            return false
        }

        if flags.contains(modifier.flag) {
            return pressedModifierKeys.insert(keyCode).inserted
        } else {
            return pressedModifierKeys.remove(keyCode) != nil
        }
    }

    private static let modifierKeys: [CGKeyCode: ModifierKey] = [
        54: ModifierKey(flag: .maskCommand),
        55: ModifierKey(flag: .maskCommand),
        56: ModifierKey(flag: .maskShift),
        58: ModifierKey(flag: .maskAlternate),
        59: ModifierKey(flag: .maskControl),
        60: ModifierKey(flag: .maskShift),
        61: ModifierKey(flag: .maskAlternate),
        62: ModifierKey(flag: .maskControl),
        63: ModifierKey(flag: .maskSecondaryFn)
    ]

    private static let keyPositions: [CGKeyCode: KeyPosition] = [
        50: KeyPosition(x: -1.0, y: 0.0), 18: KeyPosition(x: 0.0, y: 0.0),
        19: KeyPosition(x: 1.0, y: 0.0), 20: KeyPosition(x: 2.0, y: 0.0),
        21: KeyPosition(x: 3.0, y: 0.0), 23: KeyPosition(x: 4.0, y: 0.0),
        22: KeyPosition(x: 5.0, y: 0.0), 26: KeyPosition(x: 6.0, y: 0.0),
        28: KeyPosition(x: 7.0, y: 0.0), 25: KeyPosition(x: 8.0, y: 0.0),
        29: KeyPosition(x: 9.0, y: 0.0), 27: KeyPosition(x: 10.0, y: 0.0),
        24: KeyPosition(x: 11.0, y: 0.0), 51: KeyPosition(x: 12.0, y: 0.0),

        12: KeyPosition(x: 0.5, y: 1.0), 13: KeyPosition(x: 1.5, y: 1.0),
        14: KeyPosition(x: 2.5, y: 1.0), 15: KeyPosition(x: 3.5, y: 1.0),
        17: KeyPosition(x: 4.5, y: 1.0), 16: KeyPosition(x: 5.5, y: 1.0),
        32: KeyPosition(x: 6.5, y: 1.0), 34: KeyPosition(x: 7.5, y: 1.0),
        31: KeyPosition(x: 8.5, y: 1.0), 35: KeyPosition(x: 9.5, y: 1.0),
        33: KeyPosition(x: 10.5, y: 1.0), 30: KeyPosition(x: 11.5, y: 1.0),

        0: KeyPosition(x: 0.75, y: 2.0), 1: KeyPosition(x: 1.75, y: 2.0),
        2: KeyPosition(x: 2.75, y: 2.0), 3: KeyPosition(x: 3.75, y: 2.0),
        5: KeyPosition(x: 4.75, y: 2.0), 4: KeyPosition(x: 5.75, y: 2.0),
        38: KeyPosition(x: 6.75, y: 2.0), 40: KeyPosition(x: 7.75, y: 2.0),
        37: KeyPosition(x: 8.75, y: 2.0), 41: KeyPosition(x: 9.75, y: 2.0),
        39: KeyPosition(x: 10.75, y: 2.0), 42: KeyPosition(x: 11.75, y: 2.0),

        6: KeyPosition(x: 1.25, y: 3.0), 7: KeyPosition(x: 2.25, y: 3.0),
        8: KeyPosition(x: 3.25, y: 3.0), 9: KeyPosition(x: 4.25, y: 3.0),
        11: KeyPosition(x: 5.25, y: 3.0), 45: KeyPosition(x: 6.25, y: 3.0),
        46: KeyPosition(x: 7.25, y: 3.0), 43: KeyPosition(x: 8.25, y: 3.0),
        47: KeyPosition(x: 9.25, y: 3.0), 44: KeyPosition(x: 10.25, y: 3.0),

        59: KeyPosition(x: 0.0, y: 4.0), 58: KeyPosition(x: 1.0, y: 4.0),
        55: KeyPosition(x: 2.0, y: 4.0), 49: KeyPosition(x: 5.0, y: 4.0),
        54: KeyPosition(x: 8.0, y: 4.0), 61: KeyPosition(x: 9.0, y: 4.0),
        62: KeyPosition(x: 10.0, y: 4.0), 63: KeyPosition(x: 11.0, y: 4.0),
        56: KeyPosition(x: 0.25, y: 3.0), 60: KeyPosition(x: 11.25, y: 3.0),
        57: KeyPosition(x: -0.25, y: 2.0), 53: KeyPosition(x: -1.0, y: -1.0)
    ]

    private struct KeyPosition {
        let x: Double
        let y: Double
    }

    private struct ModifierKey {
        let flag: CGEventFlags
    }

    private struct DetectionCandidate: Equatable {
        let kind: DetectionCandidateKind
        let keys: Set<CGKeyCode>
        let delay: TimeInterval

        var logDescription: String {
            "\(kind.rawValue) keys=[\(keys.sorted().map(String.init).joined(separator: ", "))]"
        }
    }

    private enum DetectionCandidateKind: String {
        case adjacentCluster
        case singleRegularKey
        case singleModifierKey
    }

    private func formatKeys(_ keys: [CGKeyCode]) -> String {
        "[\(keys.map(String.init).joined(separator: ", "))]"
    }
}
