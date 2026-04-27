//
//  PermissionService.swift
//  Expecto Purronum
//

import ApplicationServices
import AppKit
import Foundation

enum PermissionService {
    static func isAccessibilityGranted() -> Bool {
        AXIsProcessTrusted()
    }

    static func requestAccessibility() -> Bool {
        print("[PermissionService] requestAccessibility called")

        let options = [
            kAXTrustedCheckOptionPrompt.takeUnretainedValue() as String: true
        ] as CFDictionary

        let isGranted = AXIsProcessTrustedWithOptions(options)
        print("[PermissionService] requestAccessibility returned \(isGranted)")

        if !isGranted {
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
                openAccessibilitySettings()
            }
        }

        return isGranted
    }

    static func isInputMonitoringGranted() -> Bool {
        CGPreflightListenEventAccess()
    }

    static func requestInputMonitoring() -> Bool {
        print("[PermissionService] requestInputMonitoring called")

        let isGranted = CGRequestListenEventAccess()
        print("[PermissionService] requestInputMonitoring returned \(isGranted)")

        if !isGranted {
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
                openInputMonitoringSettings()
            }
        }

        return isGranted
    }

    static func openAccessibilitySettings() {
        openSettingsPane("x-apple.systempreferences:com.apple.preference.security?Privacy_Accessibility")
    }

    static func openInputMonitoringSettings() {
        openSettingsPane("x-apple.systempreferences:com.apple.preference.security?Privacy_ListenEvent")
    }

    private static func openSettingsPane(_ urlString: String) {
        guard let url = URL(string: urlString) else {
            print("[PermissionService] invalid System Settings URL: \(urlString)")
            return
        }

        if !NSWorkspace.shared.open(url) {
            openSystemSettingsApp()
        }
    }

    private static func openSystemSettingsApp() {
        let systemSettingsURL = URL(fileURLWithPath: "/System/Applications/System Settings.app")

        if !NSWorkspace.shared.open(systemSettingsURL) {
            print("[PermissionService] failed to open System Settings")
        }
    }
}
