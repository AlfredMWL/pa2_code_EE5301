#ifndef FASTPLACE_IO_WRITER_H
#define FASTPLACE_IO_WRITER_H

#include <string>

#include "placement.h"

namespace fastplace {

void writePlacementCsv(const std::string& filename,
                       const PlacementProblem& problem,
                       const Placement& placement);
void writeWirelengthCsv(const std::string& filename,
                        double initialWirelength);

}  // namespace fastplace

#endif