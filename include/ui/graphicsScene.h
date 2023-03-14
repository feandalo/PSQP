#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QtGui>
#include "tile/tiledImage.h"

namespace Ui {
class GraphicsScene;
}

using namespace std;

/**
 * @brief Graphics scene to show tiledImage.
 */
class GraphicsScene: public QGraphicsScene {
    Q_OBJECT

public:
    /**
     * @brief GraphicsScene constructor.
     * @param parent Parent widget.
     */
    GraphicsScene(QWidget *parent);
    ~GraphicsScene();

    /**
     * @brief Show tiled image in scene.
     * @param tiledImage Tiled image to show.
     */
    void showTiledImage(TiledImage *tiledImage);

    /**
     * @brief Get center of scene.
     * @return The center point of the scene.
     */
    QPointF getCenter();
};

#endif // GRAPHICSSCENE_H
