//
//  MenuBarView.swift
//  Expecto Purronum
//

import SwiftUI

struct MenuBarView: View {
    @Environment(\.openWindow) private var openWindow
    @EnvironmentObject private var appState: AppState

    var body: some View {
        Text(appState.menuStatusTitle)

        Divider()

        Button("Start Monitoring") {
            appState.startMonitoring()
        }
        .disabled(appState.isMonitoring)

        Button("Stop Monitoring") {
            appState.stopMonitoring()
        }
        .disabled(!appState.isMonitoring)

        Button("Lock") {
            appState.setLocked(true)
        }
        .disabled(appState.isLocked)

        Button("Unlock") {
            appState.unlock()
        }
        .disabled(!appState.isLocked)

        Divider()

        Button("Settings") {
            openWindow(id: "settings")
        }

        Button("Quit") {
            NSApplication.shared.terminate(nil)
        }
    }
}
