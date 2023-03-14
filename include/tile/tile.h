#ifndef TILE_H_
#define TILE_H_

#include <QtGui>

#include "tile/descriptor/tileDescriptor.h"

/**
 * @brief A tile from a tiled image.
 */
class Tile {
public:
    /**
     * @brief Tile constructor.
     */
    Tile();
    ~Tile();

    /**
    * Tile's borders
    */
    static const int R = 0; // right border
    static const int L = 1; // left border
    static const int B = 2; // bottom border
    static const int T = 3; // top border

    /**
     * @brief Set tile image.
     * @param image Image for tile.
     */
    void setImage(QImage image);
    
    /**
     * @brief Compute distance between this tile and another tile.
     * @param otherTile Other tile.
     * @param border This tile's border that neighbors the other tile.
     * @return Computed distance.
     */
    double computeDistance(Tile *otherTile, int border);

    /**
     * @brief Create descriptor for the four borders of this tile.
     * @param desc Descriptor name.
     * @param paramP Descriptor's parameter P.
     * @param paramP Descriptor's parameter Q.
     */
    void createDescriptors(QString desc, float paramP, float paramQ);

    /**
     * @brief Get this tile's descriptors.
     * @return This tile's descriptors.
     */
    TileDescriptor **getDescriptors();

    /**
     * @brief Get this tile's height.
     * @return This tile's height.
     */
    int getHeight();

    /**
     * @brief Get this tile's width.
     * @return This tile's width.
     */
    int getWidth();

    /**
     * @brief Get this tile's image.
     * @return This tile's image.
     */
    QImage getImage();
    
private:
    /**
     * This tile's image.
     */
    QImage image;

    /**
     * Descriptor for the four borders of this tile.
     */
    TileDescriptor **descriptors;
};

#endif /* TILE_H_ */
