#include "ui/graphicsScene.h"

GraphicsScene::GraphicsScene(QWidget *parent) :
		QGraphicsScene(parent) {
}

GraphicsScene::~GraphicsScene() {
}

QPointF GraphicsScene::getCenter() {
	return this->sceneRect().center();
}

void GraphicsScene::showTiledImage(TiledImage *tiledImage) {
	Tile *tiles = tiledImage->getTiles();
	int ncols = tiledImage->getNCols();
	int nrows = tiledImage->getNRows();

	this->clear();
	int index = 0;
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++, index++) {
			QGraphicsPixmapItem *pix = this->addPixmap(QPixmap::fromImage(tiles[index].getImage()));
			pix->setOffset(j * tiles[index].getWidth(),
					i * tiles[index].getHeight());
		}
	}
}
