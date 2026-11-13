#include "net_model.h"

#include <stdexcept>

#include "config.h"
#include "matrix_assembler.h"
#include "suraj_parser.h"

namespace fastplace {
namespace {

Point padPoint(const PlacementProblem& problem, int node) {
    const std::size_t padIndex =
        static_cast<std::size_t>(node) - problem.movableCellCount;
    if (padIndex >= problem.padLocations.size()) {
        throw std::runtime_error(
            "Parser returned an I/O pad index outside pinLocations");
    }
    return problem.padLocations[padIndex];
}

void addMovableToFixedEdge(MatrixAssembler& assembler,
                           PlacementProblem& problem,
                           std::size_t variable,
                           Point fixedPoint,
                           double weight) {
    assembler.addGroundedDiagonal(variable, weight);
    problem.dx[variable] -= weight * fixedPoint.x;
    problem.dy[variable] -= weight * fixedPoint.y;
}

bool isMovable(const PlacementProblem& problem, int node) {
    return node >= 0 &&
           static_cast<std::size_t>(node) < problem.movableCellCount;
}

void addTwoPinNet(MatrixAssembler& assembler,
                  PlacementProblem& problem,
                  int first,
                  int second,
                  double weight) {
    if (isMovable(problem, first) && isMovable(problem, second)) {
        assembler.addLaplacianEdge(static_cast<std::size_t>(first),
                                   static_cast<std::size_t>(second), weight);
    } else if (isMovable(problem, first)) {
        addMovableToFixedEdge(assembler, problem,
                              static_cast<std::size_t>(first),
                              padPoint(problem, second), weight);
    } else if (isMovable(problem, second)) {
        addMovableToFixedEdge(assembler, problem,
                              static_cast<std::size_t>(second),
                              padPoint(problem, first), weight);
    }
}

void addCliqueNet(MatrixAssembler& assembler,
                  PlacementProblem& problem,
                  int begin,
                  int end,
                  double edgeWeight) {
    for (int firstPin = begin; firstPin < end; ++firstPin) {
        const int first = cellPinArray[firstPin];
        for (int secondPin = firstPin + 1; secondPin < end; ++secondPin) {
            const int second = cellPinArray[secondPin];
            addTwoPinNet(assembler, problem, first, second, edgeWeight);
        }
    }
}

void addStarNet(MatrixAssembler& assembler,
                PlacementProblem& problem,
                int begin,
                int end,
                std::size_t& nextAuxiliaryNode,
                double edgeWeight) {
    const std::size_t starNode = nextAuxiliaryNode++;
    for (int pin = begin; pin < end; ++pin) {
        const int node = cellPinArray[pin];
        if (isMovable(problem, node)) {
            assembler.addLaplacianEdge(static_cast<std::size_t>(node),
                                       starNode, edgeWeight);
        } else {
            addMovableToFixedEdge(assembler, problem, starNode,
                                  padPoint(problem, node), edgeWeight);
        }
    }
}

}  // namespace
// Builds the system of equations for the placement problem based on the netlist and pin locations.
void buildNetSystem(PlacementProblem& problem) {
    for (int net = 0; net < numhyper; ++net) {
        const int begin = hEdge_idxToFirstEntryInPinArray[net];
        const int end = hEdge_idxToFirstEntryInPinArray[net + 1];
        if (begin < 0 || end < begin || end > numCellPins) {
            throw std::runtime_error("Parser returned an invalid net pin range");
        }
        for (int pin = begin; pin < end; ++pin) {
            if (cellPinArray[pin] < 0 ||
                cellPinArray[pin] >= numCellsAndPads) {
                throw std::runtime_error("Parser returned an invalid net pin index");
            }
        }
        if (kHyperedgeModel == HyperedgeModel::Star && end - begin >= 3) {
            ++problem.auxiliaryNodeCount;
        }
    }

    const std::size_t variableCount = problem.variableCount();
    MatrixAssembler assembler(variableCount);
    problem.dx.assign(variableCount, 0.0);
    problem.dy.assign(variableCount, 0.0);
    std::size_t nextAuxiliaryNode = problem.movableCellCount;

    for (int net = 0; net < numhyper; ++net) {
        const int begin = hEdge_idxToFirstEntryInPinArray[net];
        const int end = hEdge_idxToFirstEntryInPinArray[net + 1];
        const int degree = end - begin;
        if (degree < 2) {
            continue;
        }

        const double netWeight = static_cast<double>(hyperwts[net]);
        if (!(netWeight > 0.0)) {
            throw std::runtime_error("Net weights must be positive");
        }
        if (degree == 2) {
            addTwoPinNet(assembler, problem,
                         cellPinArray[begin], cellPinArray[begin + 1],
                         netWeight);
            continue;
        }

        const double gamma = 1.0 / static_cast<double>(degree - 1);
        if (kHyperedgeModel == HyperedgeModel::Clique) {
            addCliqueNet(assembler, problem, begin, end,
                         gamma * netWeight);
        } else {
            addStarNet(assembler, problem, begin, end, nextAuxiliaryNode,
                       static_cast<double>(degree) * gamma * netWeight);
        }
    }

    const double centerX = problem.chipWidth / 2.0;
    const double centerY = problem.chipHeight / 2.0;
    const std::vector<bool> ungrounded = assembler.ungroundedNodes();
    for (std::size_t variable = 0; variable < variableCount; ++variable) {
        if (ungrounded[variable]) {
            assembler.add(variable, variable, kDiagonalRegularization);
            problem.dx[variable] -= kDiagonalRegularization * centerX;
            problem.dy[variable] -= kDiagonalRegularization * centerY;
        }
    }
    problem.q = assembler.build();
}

}  // namespace fastplace
