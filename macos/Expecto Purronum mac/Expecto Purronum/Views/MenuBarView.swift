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

        Button(appState.language.startMonitoring) {
            appState.startMonitoring()
        }
        .disabled(appState.isMonitoring)

        Button(appState.language.stopMonitoring) {
            appState.stopMonitoring()
        }
        .disabled(!appState.isMonitoring)

        Button(appState.language.lock) {
            appState.setLocked(true)
        }
        .disabled(appState.isLocked)

        Button(appState.language.unlock) {
            appState.unlock()
        }
        .disabled(!appState.isLocked)

        Divider()

        Button(appState.language.settings) {
            openWindow(id: "settings")
        }

        Button(appState.language.quit) {
            NSApplication.shared.terminate(nil)
        }
    }
}
