//
//  Expecto_PurronumApp.swift
//  Expecto Purronum
//
//  Created by 隼 on 2026/4/26.
//

import SwiftUI

@main
struct Expecto_PurronumApp: App {
    @StateObject private var appState = AppState()

    var body: some Scene {
        MenuBarExtra(appState.menuStatusTitle, image: appState.menuBarIconName) {
            MenuBarView()
                .environmentObject(appState)
        }
        .menuBarExtraStyle(.menu)

        Window("Settings", id: "settings") {
            SettingsView()
                .environmentObject(appState)
        }
        .defaultSize(width: 420, height: 220)
    }
}
