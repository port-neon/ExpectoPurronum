//
//  ContentView.swift
//  Expecto Purronum
//
//  Created by 隼 on 2026/4/26.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        SettingsView()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
            .environmentObject(AppState())
    }
}
