#include "matrix_assembler.h"

#include <utility>

namespace fastplace {
// Constructs a MatrixAssembler for a square matrix of the given dimension.
MatrixAssembler::MatrixAssembler(std::size_t dimension)
    : dimension_(dimension),
      adjacency_(dimension),
      grounded_(dimension, false) {}

void MatrixAssembler::add(std::size_t row,
                          std::size_t column,
                          double value) {
    entries_.push_back({row, column, value});
}

void MatrixAssembler::addLaplacianEdge(std::size_t first,
                                       std::size_t second,
                                       double weight) {
    if (first == second) {
        return;
    }

    add(first, first, weight);
    add(second, second, weight);
    add(first, second, -weight);
    add(second, first, -weight);
    adjacency_[first].push_back(second);
    adjacency_[second].push_back(first);
}
// Adds a grounded diagonal entry to the matrix and marks the variable as grounded.
void MatrixAssembler::addGroundedDiagonal(std::size_t variable,
                                          double weight) {
    add(variable, variable, weight);
    grounded_[variable] = true;
}
// Returns a vector of booleans indicating which nodes are ungrounded.
std::vector<bool> MatrixAssembler::ungroundedNodes() const {
    std::vector<bool> visited(dimension_, false);
    std::vector<bool> ungrounded(dimension_, false);

    for (std::size_t start = 0; start < dimension_; ++start) {
        if (visited[start]) {
            continue;
        }

        std::vector<std::size_t> component;
        std::vector<std::size_t> pending{start};
        bool hasFixedConnection = false;
        visited[start] = true;
        while (!pending.empty()) {
            const std::size_t node = pending.back();
            pending.pop_back();
            component.push_back(node);
            hasFixedConnection = hasFixedConnection || grounded_[node];
            for (std::size_t neighbor : adjacency_[node]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    pending.push_back(neighbor);
                }
            }
        }

        if (!hasFixedConnection) {
            for (std::size_t node : component) {
                ungrounded[node] = true;
            }
        }
    }
    return ungrounded;
}
// Builds and returns the sparse matrix from the accumulated entries.
SparseMatrix MatrixAssembler::build() {
    return SparseMatrix(dimension_, std::move(entries_));
}

}  // namespace fastplace
