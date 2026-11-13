#include <exception>
#include <iostream>
#include <string>

#include "io_writer.h"
#include "placement.h"
#include "suraj_parser.h"
#include "system_builder.h"

namespace {

int run(const std::string& basename) {
    if (basename.empty()) {
        std::cerr << "Input basename cannot be empty.\n";
        return 2;
    }

    std::string areaFile = basename + ".are";
    std::string netFile = basename + ".net";
    std::string padFile = basename + ".kiaPad";
    if (parseIbmFile(areaFile.data(), netFile.data(), padFile.data()) != 0) {
        return 1;
    }

    const fastplace::PlacementProblem problem =
        fastplace::buildPlacementProblem();
    const fastplace::Placement initial =
        fastplace::makeInitialPlacement(problem);
    const double initialWirelength =
        fastplace::calculateWirelength(problem, initial);

    const std::string outputPrefix = basename + "_phase1";
    fastplace::writePlacementCsv(outputPrefix + "_initial.csv", problem, initial);
    fastplace::writeWirelengthCsv(outputPrefix + "_wirelength.csv",
                                  initialWirelength);

    std::cout << "Q dimensions: " << problem.q.rows() << " x "
              << problem.q.columns() << '\n'
              << "Q nonzero entries: " << problem.q.values().size() << '\n'
              << "Initial-center wirelength: " << initialWirelength << '\n'
              << "Wrote " << outputPrefix << "_initial.csv and "
              << outputPrefix << "_wirelength.csv\n";
    return 0;
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <benchmark-basename>\n";
        return 2;
    }

    try {
        return run(argv[1]);
    } catch (const std::exception& error) {
        std::cerr << "FastPlace: " << error.what() << '\n';
        return 1;
    }
}