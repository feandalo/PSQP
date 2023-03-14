#ifndef TILEDESC_H
#define TILEDESC_H

#include <QtGui>

/**
 * @brief Tile Descriptor template.
 */
class TileDescriptor {
public:
	/**
     * @brief Tile Descriptor constructor.
     * @param paramP Parameter P of the descriptor.
     * @param paramQ Parameter Q of the descriptor.
     */
	TileDescriptor(float paramP, float paramQ): tileDescriptor(NULL), paramP(paramP), paramQ(paramQ) {
	}
	virtual ~TileDescriptor() {
	}

	/**
     * @brief Create feature vector for one border of this descriptor's tile's image.
     * @param image Image.
     * @param border Image's border.
     */
	virtual void createFeatureVector(QImage tile, int border) = 0;

	/**
     * @brief Compute distance between this descriptor and another descriptor.
     * @param otherDescriptor Other descriptor.
     * @param param Whether paramP and paramQ should be considered.
     * @return Computed distance.
     */
	virtual float computeDistance(TileDescriptor *otherDescriptor, bool param=true) {
		return 0.0;
	}

	/**
     * @brief Get tile descriptor.
     * @return Feature vector for this descriptor's tile.
     */
	float **getTileDescriptor() {
		return tileDescriptor;
	}
	int getSize() {
		return size;
	}
	QImage getImage() {
		return image;
	}
	
private:

protected:
	float **tileDescriptor;
	float paramP, paramQ;
	int size, border;
	QImage image;
};

#endif // TILEDESC_H
