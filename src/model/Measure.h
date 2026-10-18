// A measure has one duration of beats and two voices. 4/4 beats are 1/1, one whole note.
// Treble is staff 0. Bass is staff 1. A new note that would pass the beats is refused.

#pragma once

#include "model/Voice.h"

namespace nota {

enum class Staff { Treble, Bass };

struct Measure {
    Duration beats = wholeDuration();
    Voice treble;
    Voice bass;
};

[[nodiscard]] inline Voice& voiceOf(Measure& measure, Staff staff) {
    return staff == Staff::Treble ? measure.treble : measure.bass;
}

[[nodiscard]] inline const Voice& voiceOf(const Measure& measure, Staff staff) {
    return staff == Staff::Treble ? measure.treble : measure.bass;
}

// True when the voice can take another note of this duration without passing the beats.
[[nodiscard]] inline bool accepts(const Measure& measure, Staff staff, Duration added) {
    return compare(voiceDuration(voiceOf(measure, staff)) + added, measure.beats) <= 0;
}

// Treble middle line is B4. Bass middle line is D3. stepsFromMiddle is positive upward.
[[nodiscard]] inline Pitch pitchOnStaff(Staff staff, int stepsFromMiddle) {
    const int origin = staff == Staff::Treble ? (4 * 7 + 6) : (3 * 7 + 1);
    return pitchFromDiatonic(origin + stepsFromMiddle, 0);
}

// Steps from the staff's middle line back to a spelled pitch. Alter does not move the step.
[[nodiscard]] inline int stepsFromMiddle(Staff staff, Pitch spelled) {
    const int origin = staff == Staff::Treble ? (4 * 7 + 6) : (3 * 7 + 1);
    return spelled.octave * 7 + spelled.step - origin;
}

} // namespace nota
