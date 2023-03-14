#ifndef GRADDESCENT_H
#define GRADDESCENT_H

#include <QtGui>

#include "tile/tile.h"

using namespace std;

/**
 * Threshold for a value to be considered zero.
 */
#define MIN_THRESHOLD 0.00000000001
/**
 * Threshold for a value to be considered one.
 */
#define MAX_THRESHOLD 0.9999999

/**
 * @brief Gradient descent optimization.
 */
class GradientDescent {
public:
    /**
     * @brief Gradient Descent constructor.
     * @param tiles Tiles to run optimization.
     * @param nrows Number of columns of the puzzle.
     * @param nrows Number of rows of the puzzle.
     */
    GradientDescent(Tile *tiles, int ncols, int nrows);
    ~GradientDescent();

    /**
     * @brief Run optimization to solve puzzle.
     * @param hCompat Horizontal compatibility matrix.
     * @param vCompat Vertical compatibility matrix.
     * @param pInit Initial permutation matrix.
     * @return Solution's permutation.
     */
    int *solve(float** hCompat, float** vCompat, float **pInit);

private:
    /**
     * @brief Compute descent vector.
     */
    void computeDescentVector();
    
    /**
     * @brief Constrain descent vector to comply to the problem's 
     *        constraints (each column/row must sum up to 1).
     */
    void constrainDescentVector();

    /**
     * @brief Update current permutation based on gradient and step.
     */
    void updatePermutation();

    /**
     * @brief Compute step (gradient vector multiplier).
     */
    double computeStep();

    /**
     * @brief Restart permutation disregarding clamped tiles.
     */
    void restartPermutation();

    /**
     * Tiles to run optimization.
     */
    Tile *tiles;

    /**
     * Information about puzzle.
     */
    int ncols, nrows;
    int ntiles, ntiles2, ntilesx2;

    /**
     * Descent vector (negative gradient).
     */
    float **dF; 

    /**
     * Current permutation matrix (solution).
     */
    float **p;

    /**
     * Compatibility matrices.
     */
    float** hCompat, **vCompat;

    /**
     * Number of clamped tiles.
     */
    int clampCount;

    /**
     * Whether a tile has been clamped.
     */
    bool *clampedTile;
    
    /**
     * Whether a position has been assigned a tile.
     */
    bool *clampedPosition;

    /**
     * Whether a value in the permutation matrix is close to zero.
     */
    bool **zeroed;

    /**
     * Solution's permutation.
     */
    int *solution;

    /**
     * Whether the stop criteria has been reached.
     */
    bool stopCriteria;

    /**
     * Step for the optimization (gradient vector multiplier when 
     * updating current permutation).
     */
    double step;
};

#endif // GRADDESCENT_H
