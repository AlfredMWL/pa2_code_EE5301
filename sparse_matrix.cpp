#include "sparse_matrix.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace fastplace {

SparseMatrix::SparseMatrix() : dimension_(0), rowOffsets_(1, 0) {}
// Constructs a sparse matrix from a list of entries, combining duplicates and ignoring zero values.
SparseMatrix::SparseMatrix(std::size_t dimension,
                           std::vector<MatrixEntry> entries)
    : dimension_(dimension), rowOffsets_(dimension + 1, 0) {
    for (const MatrixEntry& entry : entries) {
        if (entry.row >= dimension_ || entry.column >= dimension_) {
            throw std::out_of_range("Sparse matrix entry is outside its dimensions");
        }
    }

    std::sort(entries.begin(), entries.end(),
              [](const MatrixEntry& left, const MatrixEntry& right) {
                  if (left.row != right.row) {
                      return left.row < right.row;
                  }
                  return left.column < right.column;
              });

    std::size_t index = 0;
    while (index < entries.size()) {
        const std::size_t row = entries[index].row;
        const std::size_t column = entries[index].column;
        double value = 0.0;
        do {
            value += entries[index].value;
            ++index;
        } while (index < entries.size() && entries[index].row == row &&
                 entries[index].column == column);

        if (value != 0.0) {
            columnIndices_.push_back(column);
            values_.push_back(value);
            ++rowOffsets_[row + 1];
        }
    }

    for (std::size_t row = 0; row < dimension_; ++row) {
        rowOffsets_[row + 1] += rowOffsets_[row];
    }
}

std::size_t SparseMatrix::rows() const {
    return dimension_;
}

std::size_t SparseMatrix::columns() const {
    return dimension_;
}
//
double SparseMatrix::get(std::size_t row, std::size_t column) const {
    if (row >= dimension_ || column >= dimension_) {
        throw std::out_of_range("Sparse matrix lookup is outside its dimensions");
    }
    for (std::size_t i = rowOffsets_[row]; i < rowOffsets_[row + 1]; ++i) {
        if (columnIndices_[i] == column) {
            return values_[i];
        }
    }
    return 0.0;
}

const std::vector<std::size_t>& SparseMatrix::rowOffsets() const {
    return rowOffsets_;
}

const std::vector<std::size_t>& SparseMatrix::columnIndices() const {
    return columnIndices_;
}

const std::vector<double>& SparseMatrix::values() const {
    return values_;
}

}  // namespace fastplace