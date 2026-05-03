//
//  DetectionSettings.swift
//  Expecto Purronum
//

import Foundation

struct DetectionSettings: Equatable {
    var adjacentHoldDuration: TimeInterval = 1.0
    var singleRegularKeyHoldDuration: TimeInterval = 2.5
    var singleModifierKeyHoldDuration: TimeInterval = 2.0
}
