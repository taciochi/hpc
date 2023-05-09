#include <omp.h>

void stencil(float *inputvec, int m, int n, float *filtervec, int k, float *outputvec, int b) {
    int batch, i, j, x, y;

    int km = (k + 1) / 2;
    int blower = (k - 1) / 2;
    int bupper = (k - 1) / 2;

    float (*input)[m][n] = (float(*)[m][n])inputvec;
    float (*filter)[k] = (float(*)[k])filtervec;
    float (*output)[m][n] = (float(*)[m][n])outputvec;

    #pragma omp parallel for private(batch, i, j, x, y)
    for (batch = 0; batch < b; batch++) {
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                output[batch][i][j] = 0.0;

                for (x = 0; x < k; x++) {
                    for (y = 0; y < k; y++) {
                        int input_i = i - km + x;
                        int input_j = j - km + y;
                        int filter_x = x;
                        int filter_y = y;

                        if (input_i >= 0 && input_i < m && input_j >= 0 && input_j < n) {
                            output[batch][i][j] += input[batch][input_i][input_j] * filter[filter_x][filter_y];
                        }
                    }
                }
            }
        }
    }
}
