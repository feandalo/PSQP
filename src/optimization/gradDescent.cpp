#include <gmp.h>

#include "optimization/gradientDescent.h"

using namespace std;

GradientDescent::GradientDescent(Tile *tiles, int ncols, int nrows) {
    this->tiles = tiles;
    this->ncols = ncols;
    this->nrows = nrows;

    ntiles = ncols * nrows;
    ntiles2 = ntiles * ntiles;
    ntilesx2 = 2 * ntiles;
}

GradientDescent::~GradientDescent() {
}

int *GradientDescent::solve(float**hCompat, float **vCompat, float **pInit) {
    p = pInit;
    step = 0.0;
    this->clampCount = 0;
    this->hCompat = hCompat;
    this->vCompat = vCompat;
    this->stopCriteria = false;
    int iterations = 0;

    dF = new float*[ntiles];
    zeroed = new bool*[ntiles];
    for (int i = 0; i < ntiles; i++) {
        dF[i] = (float*) calloc(ntiles, sizeof(float));
        zeroed[i] = (bool*) calloc(ntiles, sizeof(bool));
    }
    solution = new int[ntiles];
    clampedTile = (bool*) calloc(ntiles, sizeof(bool)); // clamped tiles
    clampedPosition = (bool*) calloc(ntiles, sizeof(bool)); // clamped positions

    /* ---- INITIAL TIME ---- */
    clock_t s1, f1;
    double time;
    s1 = clock();
    /* ---------------------- */

    // Start iterating until stop criteria is reached
    while (!stopCriteria) {
        computeDescentVector();
        constrainDescentVector();
        updatePermutation();

        iterations++;
    }
    /* ---- FINAL TIME ---- */
    f1 = clock();
    time = (double(f1) - double(s1)) / CLOCKS_PER_SEC;
    qDebug() << "TOTAL TIME " << time;
    qDebug() << "ITER: " << (iterations-1);
    /* -------------------- */

    double maxCost = 0.0;
    int maxTile = -1;
    for (int i = 0; i < ntiles; i++) {
        if (!clampedTile[i]) {
            maxCost = -1.0;
            maxTile = -1;
            for (int j = 0; j < ntiles; j++) {
                if (!clampedPosition[j])
                    if (p[i][j] > maxCost) {
                        maxCost = p[i][j];
                        maxTile = j;
                    }
            }
            clampedTile[i] = 1;
            clampedPosition[maxTile] = 1;
            solution[i] = maxTile;
        }
    }

    for (int i = 0; i < ntiles; i++) {
        free(dF[i]);
        free(zeroed[i]);
    }
    delete[] dF;
    delete[] zeroed;
    free(clampedTile);
    free(clampedPosition);

    return solution;
}

void GradientDescent::computeDescentVector() {
    /**
     *  F(p) = p'*A*p
     * dF = - (A + A') * p = (2*A)*p
     * where A is the symmetric matrix representation of the quadratic form (ntiles2 x ntiles2)
     *       p is the concatenation of the columns of the permutation matrix P (ntiles2 x 1)
     *       p_i represents the column i of P (ntiles x 1)
     *       dF is the negative gradient of F evaluated at the current vector p (ntiles2 x 1)
     *       dF_i represents the element i of the descent vector dF
     * As we do not divide hCompat and vCompat by half as we should, there is no need to
     * multiply the elements of A by two.
     * 
     * Computation of dF
     * For each horizontal edge (i,j)
     *   dF_i -= H  * p_j;
     *   dF_j -= H' * p_i;
     * For each vertical edge (i,j)
     * dF_i -= V  * p_j;
     * dF_j -= V' * p_i;}
    * */

    // clear accumulator
    for (int i = 0; i < ntiles; i++) {
        if (!clampedTile[i])
            for (int j = 0; j < ntiles; j++)
                dF[i][j] = 0.0;
    }

    int i, j;
    double sum;
    // add horizontal cost contributions
    for (int r = 0; r < nrows; r++) {
        for (int c = 0; c + 1 < ncols; c++) {
            // horizontal edge (i,j)
            i = c + r * ncols;
            j = i + 1;
            // dF_i -= H  * p_j;
            if (!clampedTile[i])
                for (int k = 0; k < ntiles; k++) {
                    if (!clampedPosition[k]) {
                        sum = 0.0;
                        // dF_i[k] = inner product of k-th row of H and p_j
                        for (int h = 0; h < ntiles; h++)
                            sum += hCompat[k][h] * p[j][h];
                        dF[i][k] += sum;
                    }
                }
            // dF_j -= H' * p_i;
            if (!clampedTile[j]) {
                for (int k = 0; k < ntiles; k++) {
                    if (!clampedPosition[k]) {
                        sum = 0.0;
                        // dF_j[k] = inner product of k-th column of H and p_i
                        for (int h = 0; h < ntiles; h++)
                            sum += hCompat[h][k] * p[i][h];
                        dF[j][k] += sum;
                    }
                }
            }
        }
    }

    // add vertical cost contributions
    for (int c = 0; c < ncols; c++) {
        for (int r = 0; r + 1 < nrows; r++) {
            // vertical edge (i,j)
            i = c + r * ncols;
            j = i + ncols;
            // dF_i -= V  * p_j;
            if (!clampedTile[i]) {
                for (int k = 0; k < ntiles; k++) {
                    if (!clampedPosition[k]) {
                        sum = 0.0;
                        // dF_i[k] = inner product of k-th row of V and p_j
                        for (int h = 0; h < ntiles; h++)
                            sum += vCompat[k][h] * p[j][h];
                        dF[i][k] += sum;
                    }
                }
            }
            // dF_j -= V' * p_i;
            if (!clampedTile[j]) {
                for (int k = 0; k < ntiles; k++) {
                    if (!clampedPosition[k]) {
                        sum = 0.0;
                        // dF_j[k] = inner product of k-th column of V and p_i
                        for (int h = 0; h < ntiles; h++)
                            sum += vCompat[h][k] * p[i][h];
                        dF[j][k] += sum;
                    }
                }
            }
        }
    }
}

void GradientDescent::restartPermutation() {
    float p0 = 1.0 / (float) (ntiles - clampCount);
    for (int i = 0; i < ntiles; i++) {
        if (!clampedTile[i]) {
            for (int j = 0; j < ntiles; j++) {
                if (!clampedPosition[j]) {
                    p[i][j] = p0;
                }
            }
        }
    }
}

void GradientDescent::constrainDescentVector() {
    int i, j;
    float lambda;
    bool isZero = true;

    float naux = 1.0 / (float) (ntiles - clampCount);
    // apply row constraints
    for (j = 0; j < ntiles; j++) {
        lambda = 0.0;
        if (!clampedPosition[j]) {
            for (i = 0; i < ntiles; i++) {
                if (!clampedTile[i])
                    lambda += dF[i][j];
            }
            lambda *= naux;
            for (i = 0; i < ntiles; i++) {
                if (!clampedTile[i]) {
                    dF[i][j] -= lambda;
                }
            }
        }
    }

    // apply column constraints
    for (j = 0; j < ntiles; j++) {
        lambda = 0.0f;
        if (!clampedTile[j]) {
            for (i = 0; i < ntiles; i++)
                if (!clampedPosition[i])
                    lambda += dF[j][i];
            lambda *= naux;
            for (i = 0; i < ntiles; i++) {
                if (!clampedPosition[i]) {
                    dF[j][i] -= lambda;
                    if (dF[j][i] > MIN_THRESHOLD)
                        isZero = false;
                }
            }
        }
    }

    if (isZero)
        stopCriteria = true;
}

void GradientDescent::updatePermutation() {
    // p = p - step*dF;

    if (stopCriteria)
        return;

    step = computeStep();

    float aux;
    for (int i = 0; i < ntiles; i++) {
        if (!clampedTile[i]) {
            for (int j = 0; j < ntiles; j++) {
                if ((!clampedPosition[j]) || (!zeroed[i][j])) {
                    aux = step * dF[i][j];
                    p[i][j] -= aux;
                }
            }
        }
    }

    bool clamp = false;
    for (int i = 0; i < ntiles; i++) {
        if (!clampedTile[i]) {
            for (int j = 0; j < ntiles; j++) {
                if (!clampedPosition[j]) {
                    if (p[i][j] < MIN_THRESHOLD) {
                        p[i][j] = 0.0;
                        zeroed[i][j] = true;
                    } else if (p[i][j] > MAX_THRESHOLD) {
                        p[i][j] = 1.0;
                        clampedTile[i] = true;
                        clampedPosition[j] = true;
                        clampCount++;
                        solution[i] = j;
                        clamp = true;
                        qDebug() << "CLAMP: " << i << " " << j;
                        for (int k = 0; k < ntiles; k++) {
                            if (k != j) {
                                p[i][k] = 0.0;
                            }
                            if (k != i) {
                                p[k][j] = 0.0;
                            }
                        }
                    }
                }
            }
        }
    }

    if (clamp) {
        restartPermutation();
        qDebug() << clampCount << " / " << ntiles;
    }

    // If all tiles are clamped, stop criteria is reached
    if (clampCount == ntiles)
        stopCriteria = true;
}

double GradientDescent::computeStep() {
    // p = p - step*dF;

    /**
     * Search for the largest step to multiply the gradient.
     * The line search must identify points a and b,
     * such that the largest possible step lies between them.
     * The largest possible step is the largest value
     * while maintaning all values of the permutation matrix
     * between 0 and 1.
     * */ 

    mpf_t a, b, x, step, aux, aux2;
    int i = 0, j = 0, pos;

    mpf_init(a);
    mpf_init_set_d(b, 0.5);
    mpf_init(x);
    mpf_init(aux2);
    mpf_init(step);
    mpf_init(aux);
    bool doBreak = false;

    pos = 0;
    do {
        doBreak = false;
        mpf_add(x, a, b);
        mpf_div_ui(x, x, 2);
        for (i = 0; i < ntiles; i++) {
            if (!clampedTile[i]) {
                for (j = 0; j < ntiles; j++) {
                    if ((clampedPosition[j]) || (zeroed[i][j]))
                        continue;
                    mpf_set_d(aux, dF[i][j]);
                    mpf_mul(aux2, aux, x);
                    mpf_set_d(aux, p[i][j]);
                    mpf_sub(aux2, aux, aux2);
                    if (mpf_sgn(aux2) < 0) {
                        doBreak = true;
                        break;
                    }
                    if (mpf_cmp_d(aux2, 1.0) > 0) {
                        doBreak = true;
                        break;
                    }
                }
                if (doBreak)
                    break;
            }
        }
        if (doBreak) {
            mpf_set(b, x);
            pos = 0;
        } else {
            mpf_set(a, x);
            pos = 1;
        }
        mpf_sub(aux2, b, a);
    } while ((mpf_cmp_d(aux2, MIN_THRESHOLD) > 0) || !pos);

    double finalStep = mpf_get_d(x);
    mpf_clear(aux2);
    mpf_clear(a);
    mpf_clear(b);
    mpf_clear(x);
    mpf_clear(aux);
    mpf_clear(step);

    return finalStep;
}
