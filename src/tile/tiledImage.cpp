#include <sstream>

#include "tile/tiledImage.h"

TiledImage::TiledImage(QImage *image, int ncols, int nrows) {
    this->ncols = ncols;
    this->nrows = nrows;
    ntiles = ncols * nrows;

    tileH = floor(image->height() / (double) nrows);
    tileW = floor(image->width() / (double) ncols);

    qDebug() << "Image size: " << image->width() << "x" << image->height();
    qDebug() << "Tile size: " << tileW << "x" << tileH;
    qDebug() << "# of tiles: " << "(" << ncols << "*" << nrows << ") = "
            << (ncols * nrows);

    tiles = new Tile[ntiles];
    int index = 0;
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++, index++) {
            tiles[index].setImage(image->copy(j * tileW, i * tileH, tileW, tileH));
        }
    }

    tileTranslation = new int[ntiles];
    for (int i = 0; i < ntiles; i++) {
        tileTranslation[i] = i;
    }
}

TiledImage::~TiledImage() {
    delete[] tiles;
    delete[] tileTranslation;
}


Tile *TiledImage::getTiles() {
    return tiles;
}

int TiledImage::getNCols() {
    return ncols;
}

int TiledImage::getNRows() {
    return nrows;
}

void TiledImage::setDescriptor(QString desc, double param_p, double param_q) {
    qDebug() << "Descriptor: " << desc;

    for (int i = 0; i < ntiles; i++)
        tiles[i].createDescriptors(desc, param_p, param_q);
}

void TiledImage::permutTiles(int *perm) {
    Tile *auxTiles = new Tile[ntiles];
    int *auxTranslation = new int[ntiles];

    // Generate random permutation
    if (perm == NULL) {
        perm = new int[ntiles];
        int auxNum, randNum;

        for (int i = 0; i < ntiles; i++)
            perm[i] = i;

        srand(time(0));
        for (int i = 0; i < ntiles; ++i) {
            randNum = rand() % (i + 1);

            auxNum = perm[i];
            perm[i] = perm[randNum];
            perm[randNum] = auxNum;
        }
    }
    // apply permutation
    for (int i = 0; i < ntiles; i++) {
        auxTiles[i] = tiles[perm[i]];
        auxTranslation[i] = tileTranslation[perm[i]];
    }
    tiles = auxTiles;
    tileTranslation = auxTranslation;    
}

void TiledImage::computeMetrics() {
    int correctDirect = 0;
    for (int i = 0; i < ntiles; i++) {
        if (tileTranslation[i] == i)
            correctDirect++;
    }

    qDebug() << "Direct: " << ((correctDirect / (double) ntiles) * 100.0);

    int iaux, jaux;
    int correctNgb = 0;
    int contCorrect = 0;
    int contTotal = 0;
    for (int i = 0; i < nrows; i++) {
        iaux = i * ncols;
        for (int j = 0; j < ncols; j++) {
            jaux = iaux + j;
            contCorrect = 0;
            if (j + 1 < ncols) {
                contTotal++;
                if (tileTranslation[jaux + 1] == tileTranslation[jaux] + 1)
                    contCorrect++;
            }
            if (j - 1 >= 0) {
                contTotal++;
                if (tileTranslation[jaux - 1] == tileTranslation[jaux] - 1) {
                    contCorrect++;
                }
            }
            if (i + 1 < nrows) {
                contTotal++;
                if (tileTranslation[jaux + ncols] == tileTranslation[jaux] + ncols)
                    contCorrect++;
            }
            if (i - 1 >= 0) {
                contTotal++;
                if (tileTranslation[jaux - ncols] == tileTranslation[jaux] - ncols) {
                    contCorrect++;
                }
            }

            correctNgb += contCorrect;
        }
    }

    qDebug() << "Neighbor: " << ((correctNgb / (double) contTotal) * 100.0);
}
