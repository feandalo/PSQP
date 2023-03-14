#ifndef GALLAGHERDESC_H
#define GALLAGHERDESC_H

#include "tile/descriptor/tileDescriptor.h"
#include "tile/descriptor/colorConversion.h"

/**
 * @brief Descriptor for the Gallagher compatibility metric.
 *        Source: Jigsaw Puzzles with Pieces of Unknown Orientation, CVPR 2012.
 *        http://chenlab.ece.cornell.edu/people/Andy/publications/Andy_files/Gallagher_cvpr2012_puzzleAssembly.pdf
 */
class GallagherDescriptor: public TileDescriptor {
public:
    /**
     * @brief Descriptor constructor.
     * @param paramP Parameter P of the descriptor.
     * @param paramQ Parameter Q of the descriptor.
     */
    GallagherDescriptor(float paramP, float paramQ);
    ~GallagherDescriptor();

    /**
     * @brief Create feature vector for one border of this descriptor's tile's image.
     * @param image Image.
     * @param border Image's border.
     */
    void createFeatureVector(QImage image, int border);

    /**
     * @brief Compute distance between this descriptor's tile and another descriptor's tile.
     * @param otherDescriptor Other descriptor.
     * @param param Whether paramP and paramQ should be considered.
     * @return Computed distance.
     */
    float computeDistance(TileDescriptor *otherDescriptor, bool param=true);

    /**
     * @brief Get value in position (r,c) of the invertdd covariance matrix.
     * @param r
     * @param c
     * @return Value of the inverted covariance matrix at position (r,c).
     */
    float getSinv(int r, int c) { return invcov[r][c]; }

    /**
     * @brief Get mean difference value for channel c.
     * @param c
     * @return Mean difference value for channel c.
     */
    float getMean(int c) { return mean[c]; }

private:
    /**
     * Auxiliary variables.
     */
    int dataExt, dataInt;

    /**
     * 3x3 covariance that captures the relationship of the gradients
     * near the edge of the tile between the color channels.
     */
    float invcov[3][3];

    /**
     * For each color channel, mean is the mean difference between
       the two columns of the descriptor.
    */
    float mean[3];
};


#endif // GALLAGHERDESC_H
