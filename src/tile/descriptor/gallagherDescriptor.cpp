#include "tile/tile.h"
#include "tile/descriptor/gallagherDescriptor.h"
#include "tile/descriptor/colorConversion.h"

GallagherDescriptor::GallagherDescriptor(float paramP, float paramQ) 
                  : TileDescriptor(paramP, paramQ) {
    /*
     * Tile
     *  0   1   2   3
     *  4   5   6   7
     *  8   9  10  11
     * 12  13  14  15
     *
     * if (border == R)
     * 	tileDescriptor = {3  7 11 15 ,  2  6 10 14}
     * else if (border == L)
     * 	tileDescriptor = {0  4  8 12 ;  1  5  9 13}
     * else if (border == B)
     * 	tileDescriptor = {12  13 14 15 ; 8 9 10 11}
     * else if (border == T)
     * 	tileDescriptor = {0  1  2  3 ;  4  5  6  7}
     */
}

GallagherDescriptor::~GallagherDescriptor() {
    for (int i = 0; i < size; i++)
        free(tileDescriptor[i]);
    free(tileDescriptor);
}

void GallagherDescriptor::createFeatureVector(QImage image, int border) {
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

    tileDescriptor = (float**) calloc(size, sizeof(float*));
    for (int i = 0; i < size; i++)
        tileDescriptor[i] = (float*) calloc(3, sizeof(float));

    float **diff = (float**) calloc(size+9, sizeof(float*));
    for (int i = 0; i < size+9; i++)
        diff[i] = (float*) calloc(3, sizeof(float));

    // Computing mean difference between descriptors
    int tx1, ty1, tx2, ty2;
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

        tileDescriptor[i][0] = ((float) pixel1.red());
        tileDescriptor[i][1] = ((float) pixel1.green());
        tileDescriptor[i][2] = ((float) pixel1.blue());

        diff[i][0] = ((float) pixel1.red())   - ((float) pixel2.red());
        diff[i][1] = ((float) pixel1.green()) - ((float) pixel2.green());
        diff[i][2] = ((float) pixel1.blue())  - ((float) pixel2.blue());
    }

    /**
     * To avoid numerical problems related to the inversion of the covariance,
     * and the inherent issues of quantized pixel values, we include nine 
     * dummy gradients in the calculations.
     */
    diff[size+0][0] = 0;  diff[size+0][1] = 0;  diff[size+0][2] = 0;
    diff[size+1][0] = 1;  diff[size+1][1] = 1;  diff[size+1][2] = 1;
    diff[size+2][0] = -1; diff[size+2][1] = -1; diff[size+2][2] = -1;
    diff[size+3][0] = 0;  diff[size+3][1] = 0;  diff[size+3][2] = 1;
    diff[size+4][0] = 0;  diff[size+4][1] = 1;  diff[size+4][2] = 0;
    diff[size+5][0] = 1;  diff[size+5][1] = 0;  diff[size+5][2] = 0;
    diff[size+6][0] = -1; diff[size+6][1] = 0;  diff[size+6][2] = 0;
    diff[size+7][0] = 0;  diff[size+7][1] = -1; diff[size+7][2] = 0;
    diff[size+8][0] = 0;  diff[size+8][1] = 0;  diff[size+8][2] = -1;

    mean[0] = mean[1] = mean[2] = 0.0;
    for (int i = 0; i < size; i++) {
        mean[0] += diff[i][0];
        mean[1] += diff[i][1];
        mean[2] += diff[i][2];
    }
    mean[0] /= (float)size;
    mean[1] /= (float)size;
    mean[2] /= (float)size;

    // Computing covariance matrix
    float cov[3][3];
    memset(cov,0,sizeof(cov));
    for (int i = 0; i < size+9; i++) {
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++) // color channel
                cov[j][k] += (diff[i][j]-mean[j])*(diff[i][k]-mean[k]);
    }
    for (int j = 0; j < 3; j++)
        for (int k = 0; k < 3; k++) // color channel
            cov[j][k] /= ((float)size+8.0);

    // Inverting covariance matrix
    float det = 0.0;
    for (int i = 0; i < 3; i++)
        det += (cov[0][i]*(cov[1][(i+1)%3]*cov[2][(i+2)%3] - cov[1][(i+2)%3]*cov[2][(i+1)%3]));

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            invcov[i][j] =  ((cov[(i+1)%3][(j+1)%3] * cov[(i+2)%3][(j+2)%3]) - (cov[(i+1)%3][(j+2)%3]*cov[(i+2)%3][(j+1)%3])) / det;
}

float GallagherDescriptor::computeDistance(TileDescriptor *td, bool param) {
    GallagherDescriptor *desc1 = NULL;
    GallagherDescriptor *desc2 = NULL;

    bool localP, localQ;
    if (param) {
        localP = paramP;
        localQ = paramQ;
    } else
        localP = localQ = 1.0;

    if (border == Tile::R || border == Tile::B) {
        desc1 = this;
        desc2 = (GallagherDescriptor*) td;
    } else {
        desc1 = (GallagherDescriptor*) td;
        desc2 = this;
    }

    float **td1 = desc1->getTileDescriptor();
    float **td2 = desc2->getTileDescriptor();

    float dist = 0.0;
    float diff1, diff2;
    float aux1, aux2, aux3, aux4, aux5, aux6;

    if (size != td->getSize())
        return -1.0;

    diff1 = 0.0;
    for (int i = 0; i < size; i++) {
            aux1 = td2[i][0] - td1[i][0] - desc1->getMean(0);
            aux2 = td2[i][1] - td1[i][1] - desc1->getMean(1);
            aux3 = td2[i][2] - td1[i][2] - desc1->getMean(2);

            aux4 = aux1*desc1->getSinv(0,0) + aux2*desc1->getSinv(1,0) + aux3*desc1->getSinv(2,0);
            aux5 = aux1*desc1->getSinv(0,1) + aux2*desc1->getSinv(1,1) + aux3*desc1->getSinv(2,1);
            aux6 = aux1*desc1->getSinv(0,2) + aux2*desc1->getSinv(1,2) + aux3*desc1->getSinv(2,2);

            diff1 += aux4*aux1 + aux5*aux2 + aux6*aux3;
    }

    diff2 = 0.0;
    for (int i = 0; i < size; i++) {
            aux1 = td1[i][0] - td2[i][0] - desc2->getMean(0);
            aux2 = td1[i][1] - td2[i][1] - desc2->getMean(1);
            aux3 = td1[i][2] - td2[i][2] - desc2->getMean(2);

            aux4 = aux1*desc2->getSinv(0,0) + aux2*desc2->getSinv(1,0) + aux3*desc2->getSinv(2,0);
            aux5 = aux1*desc2->getSinv(0,1) + aux2*desc2->getSinv(1,1) + aux3*desc2->getSinv(2,1);
            aux6 = aux1*desc2->getSinv(0,2) + aux2*desc2->getSinv(1,2) + aux3*desc2->getSinv(2,2);

            diff2 += aux4*aux1 + aux5*aux2 + aux6*aux3;
    }

    diff1 = powf(diff1, localP);
    diff2 = powf(diff2, localP);
    dist = diff1 + diff2;
    dist = powf(dist, localQ);

    return dist;
}
