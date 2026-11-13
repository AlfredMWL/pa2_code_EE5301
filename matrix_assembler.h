#ifndef FASTPLACE_MATRIX_ASSEMBLER_H
#define FASTPLACE_MATRIX_ASSEMBLER_H

#include <cstddef>
#include <vector>

#include "sparse_matrix.h"

namespace fastplace {

class MatrixAssembler {
public:
    explicit MatrixAssembler(std::size_t dimension);

    void add(std::size_t row, std::size_t column, double value);
    void addLaplacianEdge(std::size_t first,
                          std::size_t second,
                          double weight);
    void addGroundedDiagonal(std::size_t variable, double weight);
    std::vector<bool> ungroundedNodes() const;
    SparseMatrix build();

private:
    std::size_t dimension_;
    std::vector<MatrixEntry> entries_;
    std::vector<std::vector<std::size_t>> adjacency_;
    std::vector<bool> grounded_;
};

}  // namespace fastplace

#endif
