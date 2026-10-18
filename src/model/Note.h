// One note: a pitch and a duration. Onset is not stored; it is the sum of earlier durations.

#pragma once

#include "model/Duration.h"
#include "model/Pitch.h"

namespace nota {

struct Note {
    Pitch pitch;
    Duration duration;
};

} // namespace nota
