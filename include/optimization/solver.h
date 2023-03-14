#ifndef SOLVER_H
#define SOLVER_H

#include <QtGui>

#include "tile/tiledImage.h"

using namespace std;

/**
 * @brief Solver for Quadratic Programming procedure.
 */
class Solver {
public:
    /**
     * @brief Solver constructor.
     * @param tiledImage Tiled puzzle image for solver.
     */
    Solver(TiledImage *tiledImage);
    ~Solver();

    /**
     * @brief Solve puzzle.
     * @return Solution's permutation.
     */
    int *solve();

    /**
     * @brief Find the best shift for current solution.
     * @return Solution's permutation shifted by the best shift.
     */
    int *optimizeShift();

private:
    /**
     * @brief Compute total cost for given permutation.
     * @param Permutation of the tiles.
     * @return Computed cost.
     */
    float computeCost(int *perm);

    /**
     * Information about tiled image.
     */
    TiledImage *tiledImage;
    int ncols, nrows, ntiles;
};

#endif // SOLVER_H
