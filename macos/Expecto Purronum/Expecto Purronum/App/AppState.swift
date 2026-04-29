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
    private var isRequestingMonitoringPermissions = false
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
            await self?.startMonitoringWithPermissionFallback()
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

    func startMonitoring() {
        Task { @MainActor [weak self] in
            await self?.startMonitoringWithPermissionFallback()
        }
    }

    private func startMonitoringWithPermissionFallback() async {
        if isMonitoring {
            guard hasMonitoringPermissions() else {
                print("[AppState] monitoring permissions were revoked while monitoring was on")
                keyboardGuard.stop()
                isMonitoring = false
                applyLockState(.unlocked, showsAlert: false)
                return await startMonitoringWithPermissionFallback()
            }

            return
        }

        guard await ensureMonitoringPermissions() else {
            print("[AppState] monitoring permissions were not granted")
            return
        }

        let didStart = keyboardGuard.start()
        isMonitoring = didStart

        if !didStart {
            print("[AppState] failed to start monitoring")

            guard !hasMonitoringPermissions() else { return }

            print("[AppState] retrying monitoring permission flow after start failure")
            guard await ensureMonitoringPermissions() else { return }

            let didRetryStart = keyboardGuard.start()
            isMonitoring = didRetryStart

            if !didRetryStart {
                print("[AppState] failed to start monitoring after permission retry")
            }
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

    private enum RequiredPermission {
        case accessibility
        case inputMonitoring

        var alertMessage: String {
            switch self {
            case .accessibility:
                return "Accessibility permission is required so Expecto Purronum can monitor keyboard state and lock the keyboard when needed."
            case .inputMonitoring:
                return "Input Monitoring permission is required so Expecto Purronum can observe keyboard input while monitoring is on."
            }
        }

        var logName: String {
            switch self {
            case .accessibility:
                return "Accessibility"
            case .inputMonitoring:
                return "Input Monitoring"
            }
        }
    }

    private func ensureMonitoringPermissions() async -> Bool {
        guard !isRequestingMonitoringPermissions else { return false }

        isRequestingMonitoringPermissions = true
        defer { isRequestingMonitoringPermissions = false }

        guard !hasMonitoringPermissions() else { return true }
        guard await ensurePermission(.accessibility) else { return false }
        guard await ensurePermission(.inputMonitoring) else { return false }

        return true
    }

    private func hasMonitoringPermissions() -> Bool {
        PermissionService.isAccessibilityGranted() &&
            PermissionService.isInputMonitoringGranted()
    }

    private func ensurePermission(_ permission: RequiredPermission) async -> Bool {
        if isPermissionGranted(permission) {
            return true
        }

        guard showPermissionAlert(for: permission) else {
            print("[AppState] user denied \(permission.logName) permission prompt")
            return false
        }

        requestPermission(permission)
        openSystemSettings(for: permission)

        return await waitForPermission(permission)
    }

    private func isPermissionGranted(_ permission: RequiredPermission) -> Bool {
        switch permission {
        case .accessibility:
            return PermissionService.isAccessibilityGranted()
        case .inputMonitoring:
            return PermissionService.isInputMonitoringGranted()
        }
    }

    private func requestPermission(_ permission: RequiredPermission) {
        switch permission {
        case .accessibility:
            _ = PermissionService.requestAccessibility()
        case .inputMonitoring:
            _ = PermissionService.requestInputMonitoring()
        }
    }

    private func openSystemSettings(for permission: RequiredPermission) {
        switch permission {
        case .accessibility:
            PermissionService.openAccessibilitySettings()
        case .inputMonitoring:
            PermissionService.openInputMonitoringSettings()
        }
    }

    private func showPermissionAlert(for permission: RequiredPermission) -> Bool {
        let alert = NSAlert()
        alert.alertStyle = .warning
        alert.messageText = "\(permission.logName) Permission Required"
        alert.informativeText = permission.alertMessage
        alert.addButton(withTitle: "Open System Settings")
        alert.addButton(withTitle: "Deny")

        NSApp.activate(ignoringOtherApps: true)
        return alert.runModal() == .alertFirstButtonReturn
    }

    private func waitForPermission(_ permission: RequiredPermission) async -> Bool {
        if isPermissionGranted(permission) {
            return true
        }

        for _ in 0..<120 {
            try? await Task.sleep(nanoseconds: 1_000_000_000)

            if Task.isCancelled {
                return false
            }

            if isPermissionGranted(permission) {
                print("[AppState] \(permission.logName) permission granted")
                return true
            }
        }

        print("[AppState] timed out waiting for \(permission.logName) permission")
        return false
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
