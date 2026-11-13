#ifndef FASTPLACE_SYSTEM_BUILDER_H
#define FASTPLACE_SYSTEM_BUILDER_H

#include <cstddef>
#include <string>
#include <vector>

#include "sparse_matrix.h"

namespace fastplace {

struct Point {
    double x;
    double y;
};

struct PlacementProblem {
    SparseMatrix q;
    std::vector<double> dx;
    std::vector<double> dy;
    std::vector<std::string> nodeNames;
    std::vector<Point> padLocations;
    std::size_t movableCellCount;
    std::size_t auxiliaryNodeCount;
    double chipWidth;
    double chipHeight;

    PlacementProblem();
    std::size_t variableCount() const;
};

PlacementProblem buildPlacementProblem();

}  // namespace fastplace

#endif