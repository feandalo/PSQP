#ifndef PSQP_H
#define PSQP_H

#include <QtGui>
#include <QObject>

#include "tile/tile.h"
#include "tile/tiledImage.h"
#include "optimization/solver.h"

using namespace std;

/**
 * @brief Main PSQP class to set puzzle parameters.
 */
class PSQP: public QObject {
Q_OBJECT

public:
    /**
     * @brief PSQP constructor.
     */
    PSQP();
    ~PSQP();

    /**
     * @brief Set puzzle image.
     * @param imageFile Path to image file.
     */
    void setImage(QString imageFile);
    
    /**
     * @brief Set puzzle size.
     * @param ncols Number of columns to divide the puzzle image.
     * @param nrows Number of rows to divide the puzzle image.
     */
    void setPuzzleSize(int ncols, int nrows);

    /**
     * @brief Set descriptor to describe tiles.
     * @param desc Name of the descriptor to compute distances between the tiles.
     * @param paramP Parameter P for descriptor.
     * @param paramP Parameter Q for descriptor.
     */
    void setDescriptor(QString desc, float paramP, float paramQ);

    /**
     * @brief Get puzzle tiled image.
     * @return Puzzle tiled image.
     */
    TiledImage *getTiledImage();

    /**
     * @brief Run quadratic programming solver.
     */
    void runSolver();

    /**
     * @brief Run shift optimization.
     */
    void optimizeShift();

    /**
     * @brief Get descriptors' names.
     */
    QStringList getDescriptorNames();

private:
    /**
     * Descriptors' names.
     * */ 
       QStringList descriptorOptions;

    /**
     * Puzzle image.
     * */ 
    QString imageFile;
    QImage *image;

    /**
     * Puzzle image broken down into ncols x nrows tiles.
     * */ 
    TiledImage *tiledImage;
    int ncols, nrows, ntiles;

    /**
     * Quadratic programming solver.
     * */ 
    Solver *solver;
};

#endif // PSQP_H
