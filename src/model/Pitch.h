// Pitch is a MIDI number plus spelling. Callers use pitch(), not field writes.
// midi matches step + alter + octave. Middle C is C4, MIDI 60. Naturals are alter 0.

#pragma once

namespace nota {

struct Pitch {
    int midi = 60;
    int step = 0;   // 0 = C … 6 = B
    int alter = 0;  // -1, 0, +1
    int octave = 4; // scientific pitch, C4 = middle C
};

[[nodiscard]] inline int floorDivInt(int numerator, int denominator) {
    const int quotient = numerator / denominator;
    const int remainder = numerator % denominator;
    if (remainder != 0 && ((remainder < 0) != (denominator < 0))) {
        return quotient - 1;
    }
    return quotient;
}

[[nodiscard]] inline int floorModInt(int numerator, int denominator) {
    const int remainder = numerator % denominator;
    if (remainder < 0) {
        return remainder + (denominator < 0 ? -denominator : denominator);
    }
    return remainder;
}

// step is 0 = C … 6 = B. alter is -1, 0, or +1. octave is scientific pitch.
[[nodiscard]] inline Pitch pitch(int step, int alter, int octave) {
    static constexpr int kSemitone[7] = {0, 2, 4, 5, 7, 9, 11};
    Pitch result;
    result.step = step;
    result.alter = alter;
    result.octave = octave;
    result.midi = (octave + 1) * 12 + kSemitone[step] + alter;
    return result;
}

// diatonic 0 is C0. Positive steps move up the letters.
[[nodiscard]] inline Pitch pitchFromDiatonic(int diatonic, int alter = 0) {
    return pitch(floorModInt(diatonic, 7), alter, floorDivInt(diatonic, 7));
}

} // namespace nota
