#pragma once

struct DetectionSettings {
    double adjacentHoldDuration = 1.0;
    double singleRegularKeyHoldDuration = 1.5;
    double singleModifierKeyHoldDuration = 2.0;

    bool operator==(const DetectionSettings& other) const {
        return adjacentHoldDuration == other.adjacentHoldDuration &&
            singleRegularKeyHoldDuration == other.singleRegularKeyHoldDuration &&
            singleModifierKeyHoldDuration == other.singleModifierKeyHoldDuration;
    }

    bool operator!=(const DetectionSettings& other) const {
        return !(*this == other);
    }
};
