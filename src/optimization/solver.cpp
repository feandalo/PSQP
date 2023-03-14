#include "tile/compatibility.h"
#include "optimization/solver.h"
#include "optimization/gradientDescent.h"

Solver::Solver(TiledImage *tiledImage) {
    this->tiledImage = tiledImage;
    ncols = tiledImage->getNCols();
    nrows = tiledImage->getNRows();
    ntiles = ncols * nrows;
}

int *Solver::solve() {
    int *perm;
    
    qDebug() << "Computing compatibility...";
    Compatibility *compat = new Compatibility(tiledImage->getTiles(), ncols, nrows);
    qDebug() << "Done!";

    float **hCompat = compat->getCompatibilityMatrix()[0];
    float **vCompat = compat->getCompatibilityMatrix()[1];

    // Initializing solver
    float p0 = 1.0f / ((float) ntiles);
    float **pInit = new float*[ntiles];
    for (int i = 0; i < ntiles; i++) {
        pInit[i] = new float[ntiles];
        for (int j = 0; j < ntiles; j++)
            pInit[i][j] = p0;
    }

    // Run Gradient Descent and get optimum permutation
    qDebug() << "Solving puzzle...";
    GradientDescent gd(tiledImage->getTiles(), ncols, nrows);
    perm = gd.solve(hCompat, vCompat, pInit);
    qDebug() << "Done!";

    for (int i = 0; i < ntiles; i++)
        delete[] pInit[i];
    delete[] pInit;

    qDebug() << "Solution cost: " << computeCost(perm);

    return perm;
}

Solver::~Solver() {
}

float Solver::computeCost(int *perm) {
    float totalCost = 0.0, totalHCost = 0.0, totalVCost = 0.0, cost = 0.0;

    int pos = 0;
    int tile1, tile2;
    Tile *tiles = tiledImage->getTiles();
    // Horizontal cost
    for (int j = 0; j < nrows; j++) {
        for (int i = 0; i < ncols - 1; i++) {
            pos = i + j * ncols;
            tile1 = perm[pos];
            tile2 = perm[pos + 1];
            cost =
                    (tiles[tile1].getDescriptors()[Tile::R])->computeDistance(
                        (tiles[tile2].getDescriptors()[Tile::L]), false);
            totalHCost += cost;
        }
    }
    // Vertical cost
    for (int j = 0; j < nrows - 1; j++) {
        for (int i = 0; i < ncols; i++) {
            pos = i + j * ncols;
            tile1 = perm[pos];
            tile2 = perm[pos + ncols];
            cost =
                    (tiles[tile1].getDescriptors()[Tile::B])->computeDistance(
                        (tiles[tile2].getDescriptors()[Tile::T]), false);
            totalVCost += cost;
        }
    }

    // Sum and normalize costs
    if (ncols > 1 && nrows > 1)
        totalCost = totalHCost * tiles[0].getHeight() / (nrows * (ncols - 1))
                  + totalVCost * tiles[0].getWidth() / (ncols * (nrows - 1));
    else if (ncols > 1)
        totalCost = totalHCost * tiles[0].getHeight() / (nrows * (ncols - 1));
    else
        totalCost = totalVCost * tiles[0].getWidth() / (ncols * (nrows - 1));

    return totalCost;
}

int *Solver::optimizeShift() {
    int col, row, pos, sHmin = 0, sVmin = 0;
    double cost, costMin;

    int *auxPermutation = new int[ntiles];
    for (int i = 0; i < ntiles; i++)
        auxPermutation[i] = i;
    costMin = computeCost(auxPermutation);

    // Look for best shift (min cost solution) horizontally and vertically, based on solution cost
    for (int shiftH = 0; shiftH < (ncols - 1); shiftH++) {
        for (int shiftV = 0; shiftV < (nrows - 1); shiftV++) {
            for (int i = 0; i < ntiles; i++) {
                row = (i / ncols) + shiftV;
                col = (i % ncols) + shiftH;
                if (row > (nrows - 1))
                    row -= nrows;
                if (col > (ncols - 1))
                    col -= ncols;
                pos = col + row * ncols;
                auxPermutation[pos] = i;
            }
            cost = computeCost(auxPermutation);
            if (cost < costMin) {
                costMin = cost;
                sHmin = shiftH;
                sVmin = shiftV;
            }
        }
    }

    // Compute permutation based on best shift
    for (int i = 0; i < ntiles; i++) {
        row = (i / ncols) + sVmin;
        col = (i % ncols) + sHmin;
        if (row > (nrows - 1))
            row -= nrows;
        if (col > (ncols - 1))
            col -= ncols;
        pos = col + row * ncols;
        auxPermutation[pos] = i;
    }

    qDebug() << "Solution cost: " << computeCost(auxPermutation);

    return auxPermutation;
}
