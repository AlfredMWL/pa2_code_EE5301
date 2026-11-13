#ifndef FASTPLACE_PLACEMENT_H
#define FASTPLACE_PLACEMENT_H

#include <vector>

#include "system_builder.h"

namespace fastplace {

struct Placement {
    std::vector<double> x;
    std::vector<double> y;
};

Placement makeInitialPlacement(const PlacementProblem& problem);
double calculateWirelength(const PlacementProblem& problem,
                           const Placement& placement);

}  // namespace fastplace

#endif