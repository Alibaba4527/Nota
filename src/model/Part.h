// One piano part. The name is Piano. Measures are in order.

#pragma once

#include "model/Measure.h"

#include <string>
#include <vector>

namespace nota {

struct Part {
    std::string name = "Piano";
    std::vector<Measure> measures;
};

} // namespace nota
