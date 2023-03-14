#include "float.h"

#include "tile/tile.h"
#include "tile/compatibility.h"

Compatibility::Compatibility(Tile *tiles, int ncols, int nrows) {
    this->ncols = ncols;
    this->nrows = nrows;
    this->ntiles = ncols * nrows;
    this->tiles = tiles;

    // Horizontal and Vertical compatibilities
    compatibilityMatrix = new float**[2];
    for (int i = 0; i < 2; i++) {
        compatibilityMatrix[i] = new float*[ntiles];
        for (int j = 0; j < ntiles; j++) {
            compatibilityMatrix[i][j] = new float[ntiles];
            for (int k = 0; k < ntiles; k++) {
                compatibilityMatrix[i][j][k] = 0.0;
            }
        }
    }

    neighbors = (neighbor***) calloc(ntiles, sizeof(neighbor**));
    for (int i = 0; i < ntiles; i++) {
        neighbors[i] = (neighbor**) calloc(4, sizeof(neighbor*));
        for (int j = 0; j < 4; j++)
            neighbors[i][j] = (neighbor*) calloc(ntiles - 1, sizeof(neighbor));
    }

    quartileEven = false;
    quartile = (int) ceilf((float) ntiles / 2.0);
    if (quartile % 2 == 0) {
        quartile = (int) quartile / 2.0;
        quartileEven = true;
    } else
        quartile = (int) (quartile + 1.0) / 2.0;

    computeNeighbors();
    findConstantBorders();
    computeCompatibilityMatrix();
}

Compatibility::~Compatibility() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < ntiles; j++) {
            delete[] compatibilityMatrix[i][j];
        }
        delete[] compatibilityMatrix[i];
    }
    delete[] compatibilityMatrix;

    for (int i = 0; i < ntiles; i++)
        free(constantBorders[i]);
    free(constantBorders);

    for (int i = 0; i < ntiles; i++) {
        for (int j = 0; j < 4; j++)
            free(neighbors[i][j]);
        free(neighbors[i]);
    }
    free(neighbors);
}

/**
 * Compare neighboring tiles by their feature vector distance.
 * */ 
int neighborsComparisonByDistance(const void *a, const void *b) {
    struct Compatibility::neighbor *ia = (struct Compatibility::neighbor *) a;
    struct Compatibility::neighbor *ib = (struct Compatibility::neighbor *) b;
    if (ia->num == -1)
        return 1;
    if (ib->num == -1)
        return -1;
    if (ia->distance - ib->distance > 0.0)
        return 1;
    return -1;
}

void Compatibility::computeNeighbors() {
    int pos = 0;
    float dist;
    for (int i = 0; i < ntiles; i++) { // For each tile
        for (int j = 0; j < 4; j++) { // For each border
            pos = 0;
            for (int k = 0; k < ntiles; k++) { // Compute distance to every other tile
                if (k == i)
                    continue;
                neighbors[i][j][pos].num = k;
                dist = tiles[i].computeDistance(&tiles[k], j);
                neighbors[i][j][pos].distance = dist;
                pos++;
            }
            // Sort the distances
            qsort(neighbors[i][j], (ntiles - 1), sizeof(struct neighbor),
                  neighborsComparisonByDistance);
        }
    }
}

void Compatibility::findConstantBorders() {
    constantBorders = (bool**) calloc(ntiles, sizeof(bool*));
    for (int i = 0; i < ntiles; i++)
        constantBorders[i] = (bool*) calloc(4, sizeof(bool));

    /**
     * If the distance to a neighbor, in a given border, is 
     * low, then that border is constant.
     * */ 
    int num1, num2, num3, num4;
    int n1 = ntiles - 1;
    float rad = 0;

    for (int i = 0; i < ntiles; i++) {
        num1 = num2 = num3 = num4 = 0;
        for (int j = 0; j < n1; j++) {
            if (neighbors[i][Tile::R][j].distance <= rad)
                num1++;
            if (neighbors[i][Tile::L][j].distance <= rad)
                num2++;
            if (neighbors[i][Tile::B][j].distance <= rad)
                num3++;
            if (neighbors[i][Tile::T][j].distance <= rad)
                num4++;
        }

        if (num1 > 0) {
            constantBorders[i][Tile::R] = true;
        }
        if (num2 > 0) {
            constantBorders[i][Tile::L] = true;
        }
        if (num3 > 0) {
            constantBorders[i][Tile::B] = true;
        }
        if (num4 > 0) {
            constantBorders[i][Tile::T] = true;
        }
   }
}


void Compatibility::computeCompatibilityMatrix() {
    float sigma, value, aux;
    int jNeighbor, oBorder;

    for (int i = 0; i < ntiles; i++) {
        // do not consider constant tiles
        if ((constantBorders[i][Tile::R]) && (constantBorders[i][Tile::L])
                && (constantBorders[i][Tile::B])
                && (constantBorders[i][Tile::T]))
            continue;

        for (int border = 0; border < 4; border++) {
            if (constantBorders[i][border])
                continue;
            oBorder = otherBorder(border);

            sigma = 0.0;
            if (quartileEven)
                sigma = (neighbors[i][border][quartile].distance
                         + neighbors[i][border][quartile + 1].distance) / 2.0;
            else
                sigma = neighbors[i][border][quartile].distance;
            if (sigma == 0.0)
                sigma = FLT_MIN;

            for (int j = 0; j < (ntiles - 1); j++) {
                jNeighbor = neighbors[i][border][j].num;
                if (constantBorders[jNeighbor][oBorder])
                    continue;

                aux = (float) findNeighborPosition(jNeighbor, i, oBorder);
                aux = j + aux;
                value = -expf(-aux - (neighbors[i][border][j].distance+0.0001) / (sigma+0.0001));

                if (value > -0.00001)
                    break;
                saveCompatibility(i, jNeighbor, border, value);
            }
        }
    }
}

void Compatibility::saveCompatibility(int tile1, int tile2, int border,
                                      float value) {
    int orient = 0;
    if (border < 2)
        orient = HORIZONTAL;
    else
        orient = VERTICAL;

    if ((border == Tile::R) || (border == Tile::B)) {
        if (value < compatibilityMatrix[orient][tile1][tile2])
            compatibilityMatrix[orient][tile1][tile2] = value;
    } else {
        if (value < compatibilityMatrix[orient][tile2][tile1])
            compatibilityMatrix[orient][tile2][tile1] = value;
    }
}

int Compatibility::findNeighborPosition(int tile, int neighbor, int border) {
    for (int i = 0; i < (ntiles - 1); i++) {
        if (neighbors[tile][border][i].num == neighbor)
            return i;
    }
    return -1;
}

int Compatibility::otherBorder(int border) {
    int otherBorder = 0;
    if ((border == Tile::R) || (border == Tile::B))
        otherBorder = border + 1;
    else
        otherBorder = border - 1;
    return otherBorder;
}

float*** Compatibility::getCompatibilityMatrix() {
    return compatibilityMatrix;
}
