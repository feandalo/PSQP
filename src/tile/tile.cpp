#include "tile/tile.h"
#include "tile/descriptor/pomeranzDescriptor.h"
#include "tile/descriptor/gallagherDescriptor.h"

Tile::Tile() {
}

Tile::~Tile() {
    if (descriptors != NULL)
        for (int i = 0; i < 4; i++)
            delete descriptors[i];
        delete[] descriptors;
}

void Tile::createDescriptors(QString desc, float paramP, float paramQ) {
    if (image.isNull())
        qFatal("Call Tile::setImage before Tile::createDescriptors.\n");

    descriptors = new TileDescriptor*[4];
    for (int i = 0; i < 4; i++) {
        if (desc == "Gallagher") {
            descriptors[i] = new GallagherDescriptor(paramP, paramQ);
        } else {
            descriptors[i] = new PomeranzDescriptor(paramP, paramQ);
        }
    }

    for (int i = 0; i < 4; i++)
        descriptors[i]->createFeatureVector(image, i);
}

TileDescriptor **Tile::getDescriptors() {
    if (image.isNull())
        return NULL;
    return descriptors;
}

double Tile::computeDistance(Tile *t, int border) {
    int otherBorder = 0;
    if ((border == R) || (border == B))
        otherBorder = border + 1;
    else
        otherBorder = border - 1;
    return descriptors[border]->computeDistance(
            (t->getDescriptors()[otherBorder]));
}

void Tile::setImage(QImage image) {
    if (image.isNull())
        return;
    this->image = image;
}

int Tile::getHeight() {
    if (image.isNull())
        return 0;
    return image.height();
}

int Tile::getWidth() {
    if (image.isNull())
        return 0;
    return image.width();
}

QImage Tile::getImage() {
    return image;
}
