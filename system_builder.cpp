#include "system_builder.h"

#include <algorithm>
#include <stdexcept>
#include <string>

#include "config.h"
#include "debug.h"
#include "net_model.h"
#include "suraj_parser.h"

namespace fastplace {

PlacementProblem::PlacementProblem()
    : movableCellCount(0),
      auxiliaryNodeCount(0),
      chipWidth(1.0),
      chipHeight(1.0) {}

std::size_t PlacementProblem::variableCount() const {
    return movableCellCount + auxiliaryNodeCount;
}
// Builds the system of equations for the placement problem based on the netlist and pin locations.
PlacementProblem buildPlacementProblem() {
    if (numCells_noPads < 0 || numCellsAndPads < numCells_noPads ||
        numhyper < 0 || numCellPins < 0) {
        throw std::runtime_error("Parser returned invalid circuit dimensions");
    }
    if ((numhyper > 0 &&
         (hEdge_idxToFirstEntryInPinArray == nullptr || hyperwts == nullptr)) ||
        (numCellPins > 0 && cellPinArray == nullptr)) {
        throw std::runtime_error("Parser did not initialize the netlist arrays");
    }

    PlacementProblem problem;
    problem.movableCellCount = static_cast<std::size_t>(numCells_noPads);
    const std::size_t padCount =
        static_cast<std::size_t>(numCellsAndPads - numCells_noPads);
    if (padCount > 0 && pinLocations == nullptr) {
        throw std::runtime_error("Parser did not initialize I/O pad locations");
    }
    problem.padLocations.resize(padCount);
    problem.nodeNames.resize(static_cast<std::size_t>(numCellsAndPads));

    for (const auto& entry : nodeNameToNodeNum_map) {
        if (entry.second < 0 ||
            static_cast<std::size_t>(entry.second) >= problem.nodeNames.size()) {
            throw std::runtime_error("Parser returned an invalid node index");
        }
        problem.nodeNames[static_cast<std::size_t>(entry.second)] = entry.first;
    }
    for (const std::string& name : problem.nodeNames) {
        if (name.empty()) {
            throw std::runtime_error(
                "A parser node index has no corresponding name");
        }
    }

    double maximumPadX = 0.0;
    double maximumPadY = 0.0;
    for (std::size_t pad = 0; pad < padCount; ++pad) {
        const SPinLocation& parsedLocation = pinLocations[pad];
        problem.padLocations[pad] = {
            static_cast<double>(parsedLocation.x),
            static_cast<double>(parsedLocation.y)};
        maximumPadX = std::max(maximumPadX, problem.padLocations[pad].x);
        maximumPadY = std::max(maximumPadY, problem.padLocations[pad].y);
    }
    problem.chipWidth = std::max(kCellWidth, maximumPadX + kCellWidth);
    problem.chipHeight = std::max(kCellHeight, maximumPadY + kCellHeight);

    buildNetSystem(problem);
    debug::log("Built Q with " + std::to_string(problem.q.rows()) +
               " variables and " + std::to_string(problem.q.values().size()) +
               " nonzero entries");
    debug::dumpSystem(problem.q, problem.dx, problem.dy);
    return problem;
}

}  // namespace fastplace
