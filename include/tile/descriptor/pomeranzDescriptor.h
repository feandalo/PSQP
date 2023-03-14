#ifndef POMERANZDESC_H
#define POMERANZDESC_H

#include "tile/descriptor/tileDescriptor.h"
#include "tile/descriptor/colorConversion.h"

/**
 * @brief Descriptor for the Pomeranz compatibiilty metric.
 * 		  Source: A fully automated greedy square jigsaw puzzle solver, CVPR 2011.
 *        https://www.cs.bgu.ac.il/~obs/Publications/2011-Pomeranz_Shemesh_and_Ben_Shahar-A_Fully_Automated_Greedy_Square_Jigsaw_Puzzle_Solver.pdf
 */
class PomeranzDescriptor: public TileDescriptor {
public:
    /**
     * @brief Descriptor constructor.
     * @param paramP Parameter P of the descriptor.
     * @param paramQ Parameter Q of the descriptor.
     */
	PomeranzDescriptor(float paramP, float paramQ);
	~PomeranzDescriptor();

	/**
     * @brief Create feature vector for one border of this descriptor's tile's image.
     * @param image Image.
     * @param border Image's border.
     */
    void createFeatureVector(QImage image, int border);

    /**
     * @brief Compute distance between this descriptor and another descriptor.
     * @param otherDescriptor Other descriptor.
     * @param param Whether paramP and paramQ should be considered.
     * @return Computed distance.
     */
	float computeDistance(TileDescriptor *otherDescriptor, bool param=true);

private:
    /**
     * Auxiliary variables.
     */
	int dataExt, dataInt;
	int nx2;
};

#endif // POMERANZDESC_H
