#include "psqp.h"
#include "optimization/gradientDescent.h"

PSQP::PSQP() {
    image = NULL;
    tiledImage = NULL;
    solver = NULL;

    descriptorOptions << "Pomeranz" << "Gallagher";
}

PSQP::~PSQP() {
    qDebug() << "Deleting PSQP.";
    delete image;
    delete tiledImage;
    delete solver;

    image = NULL;
    tiledImage = NULL;
    solver = NULL;
}

void PSQP::setImage(QString imgFile) {
    qDebug() << imgFile;
    this->imgFile = imgFile;
    image = new QImage(imgFile);
}

void PSQP::setPuzzleSize(int ncols, int nrows) {
    this->ncols = ncols;
    this->nrows = nrows;
    ntiles = this->ncols * this->nrows;

    if (image == NULL)
        qFatal("Call PSQP::setImage before PSQP::setPuzzleSize.\n");

    delete tiledImage;
    tiledImage = new TiledImage(image, ncols, nrows);
}

void PSQP::setDescriptor(QString desc, float paramP, float paramQ) {
    if (tiledImage == NULL)
        qFatal("Call PSQP::setPuzzleSize before PSQP::setDescriptor.\n");
    
    tiledImage->setDescriptor(desc, paramP, paramQ);
    delete solver;
    solver = new Solver(tiledImage);
}

TiledImage* PSQP::getTiledImage() {
    return tiledImage;
}

QStringList PSQP::getDescriptorNames() {
       return descriptorOptions;
}

void PSQP::optimizeShift() {
    if (solver == NULL)
        qFatal("Call PSQP::setDescriptor before PSQP::optimizeShift.\n");

    int *perm = solver->optimizeShift();
    tiledImage->permutTiles(perm);
    tiledImage->computeMetrics();
}

void PSQP::runSolver() {
    if (solver == NULL)
        qFatal("Call PSQP::setDescriptor before PSQP::runSolver.\n");

    int *perm = solver->solve();
    tiledImage->permutTiles(perm);
    tiledImage->computeMetrics();

    qDebug() << "-------------------------------------------------------";
}
