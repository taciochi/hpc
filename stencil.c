#include <omp.h>
#include <math.h>

void stencil(float* inputvec, int m, int n, float* filtervec, int k, float* outputvec, int b) {
    int i, j, l, x, y;

    int km = (k+1)/2;
    int blower = (k-1)/2;
    int bupper = (k-1)/2;

    float (*input)[m][n] = (float (*)[m][n]) inputvec;
    float (*filter)[k] = (float (*)[k]) filtervec;
    float (*output)[m][n] = (float (*)[m][n]) outputvec;

    #pragma omp parallel for schedule(dynamic)
    for (i=0; i<b; i++) {
        for (j=0; j<m; j++) {
            for (l=0; l<n; l++) {
                if (j<km-1+blower || j>=(m-km+1+bupper) || l<km-1+blower || l>=(n-km+1+bupper)) {
                    output[i][j][l] = input[i][j][l];
                } else {
                    float sum = 0.0f;
                    for (x=0; x<k; x++) {
                        for (y=0; y<k; y++) {
                            sum += input[i][j-km+1+x][l-km+1+y] * filter[x][y];
                        }
                    }
                    output[i][j][l] = sum;
                }
            }
        }
    }
}