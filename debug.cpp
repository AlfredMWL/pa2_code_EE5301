#include "debug.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>

#include "sparse_matrix.h"

namespace fastplace {
namespace debug {

bool enabled() {
    const char* value = std::getenv("FASTPLACE_DEBUG");
    return value != nullptr && value[0] != '\0' && value[0] != '0';
}

void log(const std::string& message) {
    if (enabled()) {
        std::cerr << "[debug] " << message << '\n';
    }
}
//
void dumpSystem(const SparseMatrix& matrix,
                const std::vector<double>& dx,
                const std::vector<double>& dy) {
    if (!enabled() || matrix.rows() > 32) {
        return;
    }

    std::cerr << "Q matrix (" << matrix.rows() << " x " << matrix.columns()
              << "):\n";
    for (std::size_t row = 0; row < matrix.rows(); ++row) {
        for (std::size_t column = 0; column < matrix.columns(); ++column) {
            std::cerr << std::setw(11) << matrix.get(row, column);
        }
        std::cerr << '\n';
    }

    std::cerr << "dx:";
    for (double value : dx) {
        std::cerr << ' ' << value;
    }
    std::cerr << "\ndy:";
    for (double value : dy) {
        std::cerr << ' ' << value;
    }
    std::cerr << '\n';
}

}  // namespace debug
}  // namespace fastplace