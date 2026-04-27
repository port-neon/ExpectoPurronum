//
//  SettingsView.swift
//  Expecto Purronum
//

import SwiftUI

struct SettingsView: View {
    @EnvironmentObject private var appState: AppState

    var body: some View {
        VStack(alignment: .leading, spacing: 18) {
            Text("Expecto Purronum")
                .font(.title2)
                .fontWeight(.semibold)

            VStack(alignment: .leading, spacing: 8) {
                statusRow(title: "Monitoring", value: appState.isMonitoring ? "On" : "Off")
                statusRow(title: "Keyboard", value: appState.lockState.title)
            }

            HStack {
                Button(appState.isMonitoring ? "Stop Monitoring" : "Start Monitoring") {
                    if appState.isMonitoring {
                        appState.stopMonitoring()
                    } else {
                        appState.startMonitoring()
                    }
                }

                Toggle("Locked", isOn: Binding(
                    get: { appState.isLocked },
                    set: { appState.setLocked($0) }
                ))
                .toggleStyle(.switch)
            }

            Divider()

            VStack(alignment: .leading, spacing: 12) {
                durationStepper(
                    title: "Adjacent hold",
                    value: $appState.detectionSettings.adjacentHoldDuration,
                    range: 0.5...3.0
                )

                durationStepper(
                    title: "Single key hold",
                    value: $appState.detectionSettings.singleRegularKeyHoldDuration,
                    range: 1.0...8.0
                )

                durationStepper(
                    title: "Modifier hold",
                    value: $appState.detectionSettings.singleModifierKeyHoldDuration,
                    range: 1.0...8.0
                )
            }
        }
        .frame(minWidth: 420, alignment: .leading)
        .padding(24)
    }

    private func statusRow(title: String, value: String) -> some View {
        HStack {
            Text(title)
                .foregroundStyle(.secondary)
            Spacer()
            Text(value)
                .fontWeight(.medium)
        }
    }

    private func durationStepper(
        title: String,
        value: Binding<TimeInterval>,
        range: ClosedRange<TimeInterval>
    ) -> some View {
        Stepper(value: value, in: range, step: 0.5) {
            HStack {
                Text(title)
                Spacer()
                Text(String(format: "%.1fs", value.wrappedValue))
                    .fontWeight(.medium)
            }
        }
    }
}

struct SettingsView_Previews: PreviewProvider {
    static var previews: some View {
        SettingsView()
            .environmentObject(AppState())
    }
}
