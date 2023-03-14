#include "tile/tile.h"
#include "tile/descriptor/colorConversion.h"
#include "tile/descriptor/pomeranzDescriptor.h"

PomeranzDescriptor::PomeranzDescriptor(float paramP, float paramQ) 
                : TileDescriptor(paramP, paramQ) {
    /*
     * Tile
     *  0   1   2   3
     *  4   5   6   7
     *  8   9  10  11
     * 12  13  14  15
     *
     * if (border == R)
     * 	tileDesc = {3  7 11 15 ,  2  6 10 14}
     * else if (border == L)
     * 	tileDesc = {0  4  8 12 ;  1  5  9 13}
     * else if (border == B)
     * 	tileDesc = {12  13 14 15 ; 8 9 10 11}
     * else if (border == T)
     * 	tileDesc = {0  1  2  3 ;  4  5  6  7}
     */
    nx2 = 0;
}

PomeranzDescriptor::~PomeranzDescriptor() {
    for (int i = 0; i < nx2; i++)
        free(tileDescriptor[i]);
    free(tileDescriptor);
}

void PomeranzDescriptor::createFeatureVector(QImage image, int border) {
    if (tileDescriptor != NULL)
        return;

    this->image = image;
    this->border = border;

    if (border == Tile::R) {
        size = image.height();
        dataExt = image.width() - 1;
        dataInt = image.width() - 2;
    } else if (border == Tile::L) {
        size = image.height();
        dataExt = 0;
        dataInt = 1;
    } else if (border == Tile::B) {
        size = image.width();
        dataExt = image.height() - 1;
        dataInt = image.height() - 2;
    } else {
        size = image.width();
        dataExt = 0;
        dataInt = 1;
    }
    nx2 = 2 * size;

    tileDescriptor = (float**) calloc(nx2, sizeof(float*));
    for (int i = 0; i < nx2; i++)
        tileDescriptor[i] = (float*) calloc(3, sizeof(float));

    int tx1, ty1, tx2, ty2;
    float tileDescAux[3];
    for (int i = 0; i < size; i++) {
        if ((border == Tile::R) || (border == Tile::L)) {
            tx1 = dataExt;
            tx2 = dataInt;
            ty1 = ty2 = i;
        } else {
            ty1 = dataExt;
            ty2 = dataInt;
            tx1 = tx2 = i;
        }

        QColor pixel1(image.pixel(tx1, ty1));
        QColor pixel2(image.pixel(tx2, ty2));

        tileDescAux[0] = ((float) pixel1.red());
        tileDescAux[1] = ((float) pixel1.green());
        tileDescAux[2] = ((float) pixel1.blue());
        ColorConversion::RGBtoLAB(tileDescAux, tileDescriptor[i]);

        tileDescAux[0] = ((float) pixel2.red());
        tileDescAux[1] = ((float) pixel2.green());
        tileDescAux[2] = ((float) pixel2.blue());
        ColorConversion::RGBtoLAB(tileDescAux, tileDescriptor[size + i]);
    }
}

float PomeranzDescriptor::computeDistance(TileDescriptor *otherDescriptor, bool param) {
    PomeranzDescriptor *desc1 = NULL;
    PomeranzDescriptor *desc2 = NULL;

    bool localP, localQ;
    if (param) {
        localP = paramP;
        localQ = paramQ;
    } else
        localP = localQ = 1.0;
    

    if (border == Tile::R || border == Tile::B) {
        desc1 = this;
        desc2 = (PomeranzDescriptor*) otherDescriptor;
    } else {
        desc1 = (PomeranzDescriptor*) otherDescriptor;
        desc2 = this;
    }

    float **td1 = desc1->getTileDescriptor();
    float **td2 = desc2->getTileDescriptor();

    float dist = 0.0;
    float pred1, pred2;

    if (size != otherDescriptor->getSize())
        return -1.0;

    float aux;

    aux = 0.0;
    pred1 = 0.0;
    pred2 = 0.0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 3; j++) {
            aux = fabsf(td1[i][j] + td1[i][j] - td1[size + i][j] - td2[i][j]);
            pred1 += powf(aux, localP);

            aux = fabsf(td2[i][j] + td2[i][j] - td1[i][j] - td2[size + i][j]);
            pred2 += powf(aux, localP);

        }
    }
    pred1 /= (float) size;
    pred2 /= (float) size;
    dist = powf(pred1, localQ) + powf(pred2, localQ);

    return dist;
}
