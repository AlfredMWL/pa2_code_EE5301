#include "placement.h"

#include <cmath>
#include <stdexcept>

namespace fastplace {

Placement makeInitialPlacement(const PlacementProblem& problem) {
    const std::size_t count = problem.variableCount();
    const double centerX = problem.chipWidth / 2.0;
    const double centerY = problem.chipHeight / 2.0;
    return {std::vector<double>(count, centerX),
            std::vector<double>(count, centerY)};
}
// Calculates the total wirelength of the placement based on the quadratic cost matrix.
double calculateWirelength(const PlacementProblem& problem,
                           const Placement& placement) {
    if (placement.x.size() != problem.variableCount() ||
        placement.y.size() != problem.variableCount()) {
        throw std::invalid_argument("Placement has the wrong variable count");
    }

    double squaredWirelength = 0.0;
    const std::vector<std::size_t>& offsets = problem.q.rowOffsets();
    const std::vector<std::size_t>& columns = problem.q.columnIndices();
    const std::vector<double>& values = problem.q.values();
    for (std::size_t row = 0; row < problem.q.rows(); ++row) {
        for (std::size_t entry = offsets[row]; entry < offsets[row + 1]; ++entry) {
            const std::size_t column = columns[entry];
            if (column <= row || values[entry] >= 0.0) {
                continue;
            }
            const double deltaX = placement.x[row] - placement.x[column];
            const double deltaY = placement.y[row] - placement.y[column];
            squaredWirelength += -values[entry] *
                                 (deltaX * deltaX + deltaY * deltaY);
        }
    }
    return std::sqrt(squaredWirelength);
}

}  // namespace fastplace