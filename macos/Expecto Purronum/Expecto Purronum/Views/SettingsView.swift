//
//  SettingsView.swift
//  Expecto Purronum
//

import SwiftUI

struct SettingsView: View {
    @EnvironmentObject private var appState: AppState

    var body: some View {
        VStack(alignment: .leading, spacing: 18) {
            HStack {
                Text("Expecto Purronum")
                    .font(.title2)
                    .fontWeight(.semibold)

                Spacer()

                Picker("", selection: $appState.language) {
                    ForEach(AppLanguage.allCases) { language in
                        Text(language.displayName)
                            .tag(language)
                    }
                }
                .labelsHidden()
                .frame(width: 120)
            }

            VStack(alignment: .leading, spacing: 8) {
                statusRow(
                    title: appState.language.monitoring,
                    value: appState.isMonitoring ? appState.language.on : appState.language.off
                )
                statusRow(
                    title: appState.language.keyboard,
                    value: appState.language.title(for: appState.lockState)
                )
            }

            VStack(alignment: .leading, spacing: 10) {
                Button(appState.isMonitoring ? appState.language.stopMonitoring : appState.language.startMonitoring) {
                    if appState.isMonitoring {
                        appState.stopMonitoring()
                    } else {
                        appState.startMonitoring()
                    }
                }

                Toggle(appState.language.locked, isOn: Binding(
                    get: { appState.isLocked },
                    set: { appState.setLocked($0) }
                ))
                .toggleStyle(.switch)
            }

            Divider()

            VStack(alignment: .leading, spacing: 12) {
                durationStepper(
                    title: appState.language.adjacentHold,
                    value: $appState.detectionSettings.adjacentHoldDuration,
                    range: 0.5...3.0
                )

                durationStepper(
                    title: appState.language.singleKeyHold,
                    value: $appState.detectionSettings.singleRegularKeyHoldDuration,
                    range: 1.0...8.0
                )

                durationStepper(
                    title: appState.language.modifierHold,
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
