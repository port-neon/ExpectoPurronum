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
        MenuBarExtra {
            MenuBarView()
                .environmentObject(appState)
        } label: {
            Image(appState.menuBarIconName)
                .resizable()
                .aspectRatio(contentMode: .fit)
                .frame(width: 18, height: 18)
                .accessibilityLabel(appState.menuStatusTitle)
        }
        .menuBarExtraStyle(.menu)

        Window("Settings", id: "settings") {
            SettingsView()
                .environmentObject(appState)
        }
        .defaultSize(width: 420, height: 220)
    }
}
