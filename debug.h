#ifndef FASTPLACE_DEBUG_H
#define FASTPLACE_DEBUG_H

#include <string>
#include <vector>

namespace fastplace {

class SparseMatrix;

namespace debug {

bool enabled();
void log(const std::string& message);
void dumpSystem(const SparseMatrix& matrix,
                const std::vector<double>& dx,
                const std::vector<double>& dy);

}  // namespace debug
}  // namespace fastplace

#endif