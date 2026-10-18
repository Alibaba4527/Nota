// The score is exactly one piano part.

#pragma once

#include "model/Part.h"

namespace nota {

struct Score {
    Part part;
};

// A new document: four empty 4/4 measures.
[[nodiscard]] inline Score emptyPianoScore() {
    Score score;
    score.part.name = "Piano";
    score.part.measures.assign(4, Measure{});
    return score;
}

} // namespace nota
