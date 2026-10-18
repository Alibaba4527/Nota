// A voice is notes in onset order. The onset of a note is the sum of durations before it.

#pragma once

#include "model/Note.h"

#include <vector>

namespace nota {

struct Voice {
    std::vector<Note> notes;
};

[[nodiscard]] inline Duration voiceDuration(const Voice& voice) {
    Duration sum{0, 1};
    for (const Note& note : voice.notes) {
        sum = sum + note.duration;
    }
    return sum;
}

[[nodiscard]] inline Duration onsetBefore(const Voice& voice, int index) {
    Duration sum{0, 1};
    const int count = static_cast<int>(voice.notes.size());
    const int last = index < count ? index : count;
    for (int i = 0; i < last; ++i) {
        sum = sum + voice.notes[static_cast<size_t>(i)].duration;
    }
    return sum;
}

} // namespace nota
