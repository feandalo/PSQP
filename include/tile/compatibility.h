#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

#include <QtGui>
#include <math.h>
#include <sstream>

#include "tile/tile.h"

using namespace std;

/**
 * @brief Compatibility between tiles.
 */
class Compatibility {
public:
    /**
     * Struct to store information on a tile's neighbor
     */
    struct neighbor {
        int num;
        float distance;
    };

    /**
     * @brief Compatibility constructor.
     * @param tiles Tiles to compute compatibility.
     * @param ncols Number of columns of the puzzle.
     * @param nrows Number of rown of the puzzle.
     */
    Compatibility(Tile *tiles, int ncols, int nrows);
    ~Compatibility();

    /**
     * Horizontal border.
    */
    static const int HORIZONTAL = 0;
    /**
     * Vertical border.
    */
    static const int VERTICAL = 1;

    /**
     * @brief Get compatibility matrix.
     * @return Compatibillity matrix.
     */
    float ***getCompatibilityMatrix();

private:
    /**
     * @brief Compute information on each tile's neighbors,
     *        and sort them by distance.
     */
    void computeNeighbors();

    /**
     * @brief Find tiles with constant borders.
     */
    void findConstantBorders();

    /**
     * @brief Compute compatibility values to populate
     *        compatibility matrix.
     */
    void computeCompatibilityMatrix();

    /**
     * @brief Auxililary function to store value in compatibility matrix 
     *        for tiles tile1 and tile2, across the given border.
     * @param tile1
     * @param tile2
     * @param border
     * @param value
     */
    void saveCompatibility(int tile1, int tile2, int border, float value);

    /**
     * @brief Find a tile's neighbor position, ordered by distance,
     *        across given border.
     * @param tile
     * @param neighbor
     * @param border
     * @return Neighbor's position (rank)
     */
    int findNeighborPosition(int tile, int neighbor, int border);

    /**
     * @brief Get the opposite border to given border.
     * @param border
     * @return Opposite border.
     */
    int otherBorder(int border);

    /**
     * Auxiliary variables.
     */
    stringstream time;
    int quartile;
    bool quartileEven;
    
    /**
     * Tiles to compute compatibility
     */
    Tile *tiles;

    /**
     * Information about puzzle.
     */
    int ncols, nrows, ntiles;

    /**
     * Information on each tile's neighbors.
     */
    neighbor ***neighbors;

    /**
     * Compatibility matrix.
     */
    float***compatibilityMatrix;

    /**
     * Tiles with constant borders.
     */
    bool **constantBorders;
};

#endif // COMPATIBILITY_H
