#include "io_writer.h"

#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace fastplace {
// Writes the placement to a CSV file with the following columns:
    // name,x,y,type
void writePlacementCsv(const std::string& filename,
                       const PlacementProblem& problem,
                       const Placement& placement) {
    std::ofstream output(filename);
    if (!output) {
        throw std::runtime_error("Cannot open placement output file: " + filename);
    }

    output << "name,x,y,type\n" << std::setprecision(12);
    for (std::size_t cell = 0; cell < problem.movableCellCount; ++cell) {
        output << problem.nodeNames[cell] << ',' << placement.x[cell] << ','
               << placement.y[cell] << ",cell\n";
    }
    for (std::size_t pad = 0; pad < problem.padLocations.size(); ++pad) {
        const std::size_t node = problem.movableCellCount + pad;
        output << problem.nodeNames[node] << ',' << problem.padLocations[pad].x
               << ',' << problem.padLocations[pad].y << ",pad\n";
    }
    if (!output) {
        throw std::runtime_error("Failed while writing placement output file: " +
                                 filename);
    }
}

void writeWirelengthCsv(const std::string& filename,
                        double initialWirelength) {
    std::ofstream output(filename);
    if (!output) {
        throw std::runtime_error("Cannot open wirelength output file: " + filename);
    }
    output << "stage,wirelength\n"
           << "initial_center," << std::setprecision(12) << initialWirelength
           << '\n';
    if (!output) {
        throw std::runtime_error("Failed while writing wirelength output file: " +
                                 filename);
    }
}

}  // namespace fastplace