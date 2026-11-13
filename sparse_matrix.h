#ifndef FASTPLACE_SPARSE_MATRIX_H
#define FASTPLACE_SPARSE_MATRIX_H

#include <cstddef>
#include <vector>

namespace fastplace {

struct MatrixEntry {
    std::size_t row;
    std::size_t column;
    double value;
};

class SparseMatrix {
public:
    SparseMatrix();
    SparseMatrix(std::size_t dimension, std::vector<MatrixEntry> entries);

    std::size_t rows() const;
    std::size_t columns() const;
    double get(std::size_t row, std::size_t column) const;

    const std::vector<std::size_t>& rowOffsets() const;
    const std::vector<std::size_t>& columnIndices() const;
    const std::vector<double>& values() const;

private:
    std::size_t dimension_;
    std::vector<std::size_t> rowOffsets_;
    std::vector<std::size_t> columnIndices_;
    std::vector<double> values_;
};

}  // namespace fastplace

#endif