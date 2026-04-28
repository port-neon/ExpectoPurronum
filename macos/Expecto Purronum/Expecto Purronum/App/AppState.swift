//
//  AppState.swift
//  Expecto Purronum
//

import Combine
import AppKit
import Foundation

@MainActor
final class AppState: ObservableObject {
    @Published private(set) var isMonitoring = false
    @Published private(set) var lockState: KeyboardLockState = .unlocked
    @Published var detectionSettings = DetectionSettings() {
        didSet {
            keyboardGuard.updateSettings(detectionSettings)
        }
    }

    private let keyboardGuard = KeyboardGuard()
    private var notificationObservers: [NSObjectProtocol] = []
    private var isShowingLockAlert = false

    init() {
        keyboardGuard.updateSettings(detectionSettings)
        keyboardGuard.onLockStateChange = { [weak self] state in
            Task { @MainActor in
                self?.applyLockState(state)
            }
        }

        observeSessionChanges()

        Task { @MainActor [weak self] in
            self?.startMonitoring()
        }
    }

    deinit {
        for observer in notificationObservers {
            NSWorkspace.shared.notificationCenter.removeObserver(observer)
        }
    }

    var isLocked: Bool {
        lockState.isLocked
    }

    var menuStatusTitle: String {
        if isLocked {
            return "Locked"
        }

        if lockState == .suspicious {
            return "Suspicious"
        }

        return isMonitoring ? "Monitoring On" : "Monitoring Off"
    }

    var menuBarIconName: String {
        if isLocked {
            return "locked"
        }

        return isMonitoring ? "monitor on" : "monitor off"
    }

    func startMonitoring() {
        guard !isMonitoring else { return }

        let didStart = keyboardGuard.start()
        isMonitoring = didStart

        if !didStart {
            print("[AppState] failed to start monitoring")
        }
    }

    func stopMonitoring() {
        guard isMonitoring else { return }

        keyboardGuard.stop()
        isMonitoring = false
        applyLockState(.unlocked)
    }

    func setLocked(_ locked: Bool) {
        applyLockState(locked ? .locked : .unlocked, showsAlert: false)
        if !locked {
            keyboardGuard.resetDetection()
        }
    }

    func unlock() {
        applyLockState(.unlocked)
        keyboardGuard.resetDetection()
    }

    private func applyLockState(_ state: KeyboardLockState, showsAlert: Bool = true) {
        let didBecomeLocked = lockState != .locked && state == .locked

        lockState = state

        if didBecomeLocked && showsAlert {
            showLockedAlert()
        }
    }

    private func showLockedAlert() {
        guard !isShowingLockAlert else { return }

        isShowingLockAlert = true

        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.messageText = "Expecto Purronum"
        alert.informativeText = """
        Expecto Purronum spell has been cast by the cat-wizard.
        You can unlock the keyboard anytime with the spell Purrhomora.
        Or keep it locked with the spell Immeowbulus.
        """
        alert.addButton(withTitle: "Purrhomora(Unlock)")
        alert.addButton(withTitle: "Immeowbulus(Keep locked)")

        NSApp.activate(ignoringOtherApps: true)
        let response = alert.runModal()

        isShowingLockAlert = false

        if response == .alertFirstButtonReturn {
            unlock()
        }
    }

    private func observeSessionChanges() {
        let workspaceCenter = NSWorkspace.shared.notificationCenter

        notificationObservers.append(
            workspaceCenter.addObserver(
                forName: NSWorkspace.sessionDidResignActiveNotification,
                object: nil,
                queue: .main
            ) { [weak self] _ in
                guard let self else { return }
                Task { @MainActor [self] in
                    self.unlockForSessionTransition("session resigned active")
                }
            }
        )

        notificationObservers.append(
            workspaceCenter.addObserver(
                forName: NSWorkspace.willSleepNotification,
                object: nil,
                queue: .main
            ) { [weak self] _ in
                guard let self else { return }
                Task { @MainActor [self] in
                    self.unlockForSessionTransition("system will sleep")
                }
            }
        )

        notificationObservers.append(
            workspaceCenter.addObserver(
                forName: NSWorkspace.screensDidSleepNotification,
                object: nil,
                queue: .main
            ) { [weak self] _ in
                guard let self else { return }
                Task { @MainActor [self] in
                    self.unlockForSessionTransition("screens did sleep")
                }
            }
        )
    }

    private func unlockForSessionTransition(_ reason: String) {
        guard lockState != .unlocked else { return }

        print("[AppState] unlocking because \(reason)")
        unlock()
    }
}
