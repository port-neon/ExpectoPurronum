//
//  AppState.swift
//  Expecto Purronum
//

import Combine
import AppKit
import Foundation

@MainActor
final class AppState: ObservableObject {
    private static let languageDefaultsKey = "appLanguage"

    @Published private(set) var isMonitoring = false
    @Published private(set) var lockState: KeyboardLockState = .unlocked
    @Published private(set) var launchAtLoginEnabled = LaunchAtLoginService.isEnabled()
    @Published var language = AppState.savedLanguage() {
        didSet {
            UserDefaults.standard.set(language.rawValue, forKey: Self.languageDefaultsKey)
        }
    }
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
            return language.lockedStatus
        }

        if lockState == .suspicious {
            return language.suspicious
        }

        return isMonitoring ? language.monitoringOn : language.monitoringOff
    }

    var menuBarIconName: String {
        if isLocked {
            return "locked"
        }

        return isMonitoring ? "monitor on" : "monitor off"
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

    func setLaunchAtLoginEnabled(_ enabled: Bool) {
        do {
            try LaunchAtLoginService.setEnabled(enabled)
            launchAtLoginEnabled = LaunchAtLoginService.isEnabled()
        } catch {
            print("[AppState] failed to update launch at login: \(error)")
            launchAtLoginEnabled = LaunchAtLoginService.isEnabled()
        }
    }

    func refreshLaunchAtLoginStatus() {
        launchAtLoginEnabled = LaunchAtLoginService.isEnabled()
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
        alert.messageText = language.lockAlertTitle
        alert.informativeText = language.lockAlertMessage
        alert.addButton(withTitle: language.lockAlertUnlockButton)
        alert.addButton(withTitle: language.lockAlertKeepLockedButton)

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

        print("[AppState] requesting \(permission.logName) permission")
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

    private static func savedLanguage() -> AppLanguage {
        guard let rawValue = UserDefaults.standard.string(forKey: languageDefaultsKey),
              let language = AppLanguage(rawValue: rawValue) else {
            return .english
        }

        return language
    }
}
