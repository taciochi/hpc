#include <omp.h>

void stencil(float *inputvec, int m, int n, float *filtervec, int k, float *outputvec, int b) {
    int batch, i, j, x, y;

    int km = (k + 1) / 2;
    int blower = (k - 1) / 2;
    int bupper = (k - 1) / 2;

    float (*input)[m][n] = (float(*)[m][n])inputvec;
    float (*filter)[k] = (float(*)[k])filtervec;
    float (*output)[m][n] = (float(*)[m][n])outputvec;

    #pragma omp parallel for private(batch, i, j, x, y) shared(input, filter, output)
    for (batch = 0; batch < b; batch++) {
        #pragma omp parallel for private(i, j, x, y) shared(input, filter, output)
        for (i = blower; i < m - bupper; i++) {
            #pragma omp parallel for private(j, x, y) shared(input, filter, output)
            for (j = blower; j < n - bupper; j++) {
                output[batch][i][j] = 0.0;
                for (x = 0; x < k; x++) {
                    for (y = 0; y < k; y++) {
                        output[batch][i][j] += input[batch][i - blower + x][j - blower + y] * filter[x][y];
                    }
                }
            }
        }
    }
}