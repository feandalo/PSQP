#ifndef COLORCONVERSION_H_
#define COLORCONVERSION_H_

class ColorConversion {
public:
    /**
     * @brief Convert RGB to LAB.
     * @param rgb RGB values.
     * @param lab LAB values.
     */
    void static RGBtoLAB(float rgb[], float lab[]) {
        rgb[0] /= 255.0;
        rgb[1] /= 255.0;
        rgb[2] /= 255.0;

        lab[0] = 0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2];
        lab[1] = 7.0 * (0.596 * rgb[0] - 0.274 * rgb[1] - 0.322 * rgb[2]);
        lab[2] = 7.0 * (0.211 * rgb[0] - 0.523 * rgb[1] + 0.312 * rgb[2]);
    }

private:
};

#endif /* COLORCONVERSION_H_ */
