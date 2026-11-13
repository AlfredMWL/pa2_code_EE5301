#ifndef FASTPLACE_CONFIG_H
#define FASTPLACE_CONFIG_H

namespace fastplace {

enum class HyperedgeModel {
    Clique,
    Star
};

constexpr HyperedgeModel kHyperedgeModel = HyperedgeModel::Star;
constexpr double kCellWidth = 1.0;
constexpr double kCellHeight = 1.0;
constexpr double kDiagonalRegularization = 1e-6;

}  // namespace fastplace

#endif