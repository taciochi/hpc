#include <omp.h>

void stencil(float *inputvec, int m, int n, float *filtervec, int k, float *outputvec, int b) {
    int batch, row, col, x, y;

    int km = (k + 1) / 2;
    int blower = (k - 1) / 2;
    int bupper = k / 2;

    float (*input)[m][n] = (float(*)[m][n])inputvec;
    float (*filter)[k] = (float(*)[k])filtervec;
    float (*output)[m][n] = (float(*)[m][n])outputvec;

    #pragma omp parallel for private(batch, row, col, x, y)
    for (batch = 0; batch < b; batch++) {
        for (row = blower; row < m - bupper; row++) {
            for (col = blower; col < n - bupper; col++) {

                for (x = 0; x < k; x++) {
                    for (y = 0; y < k; y++) {
                        int input_row = row - km + x;
                        int input_col = col - km + y;
                        int filter_x = x;
                        int filter_y = y;

                        if (input_row >= 0 && input_row < m && input_col >= 0 && input_col < n) {
                            output[batch][row][col] += input[batch][input_row][input_col] * filter[filter_x][filter_y];
                        }
                    }
                }
            }
        }
    }
}

