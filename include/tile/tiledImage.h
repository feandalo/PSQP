#ifndef TILEDIMAGE_H
#define TILEDIMAGE_H

#include <QtGui>

#include "tile/tile.h"

using namespace std;

/**
 * @brief Tiled image.
 */
class TiledImage {
public:
	/**
     * @brief Tiled image constructor.
     * @param image Image to be tiled.
     * @param ncols Number of colums of the tiled image.
     * @param nrows Number of rows of the tiled image.
     */
    TiledImage(QImage *image, int ncols, int nrows);
    ~TiledImage();

    /**
     * @brief Get tiled image's tiles.
     * @return This tililed image's tiles.
     */
    Tile *getTiles();
    
    /**
     * @brief Set descriptor for the tiles.
     * @param desc Descriptor name.
     * @param paramP Descriptor's parameter P.
     * @param paramQ Descriptor's parameter Q.
     */
    void setDescriptor(QString desc, double paramP, double paramQ);

    /**
     * @brief Permute tiles.
     * @param perm Permutation for the tiles or NULL for random permutation.
     */
    void permutTiles(int *perm=NULL);

    /**
     * @brief Compute puzzle metrics with current permutation.
     */
    void computeMetrics();

    /**
     * @brief Get number of columns of tiled image.
     * @return Number of columns of the tiled image.
     */
    int getNCols();

    /**
     * @brief Get number of rows of tiled image.
     * @return Number of rows of the tiled image.
     */
    int getNRows();

private:
    /**
     * @brief Information about tiled image's tiles.
     */
    Tile *tiles;
    int ncols, nrows, ntiles;
    int tileH, tileW;

    /**
     * Translation between tiles positions before 
     * and after permutation.
     * */ 
    int *tileTranslation;
};

#endif // TILEDIMAGE_H
