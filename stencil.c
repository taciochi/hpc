#include <omp.h>

void stencil(float input_vec, int m, int n, floatfilter_vec, int k, float output_vec, int b) {
    float (input)[m][n] = (float ()[m][n]) input_vec;
    float (filter)[k] = (float ()[k]) filter_vec;
    float (output)[m][n] = (float ()[m][n]) output_vec;

    int km = (k + 1) / 2;
    int blower = (k - 1) / 2;
    int bupper = k / 2;

    #pragma omp parallel for shared(input, filter, output) private(b_idx, i, j, p, q) schedule(static)
    for (int b_idx = 0; b_idx < b; b_idx++) {
        for (int i = blower; i < m - bupper; i++) {
            #pragma omp simd private(sum)
            for (int j = blower; j < n - bupper; j++) {
                float sum = 0.0;
                for (int p = 0; p < k; p++) {
                    #pragma omp simd reduction(+:sum)
                    for (int q = 0; q < k; q++) {
                        sum += input[b_idx][i + p - km][j + q - km] filter[p][q];
                    }
                }
                output[b_idx][i][j] = sum;
            }
        }
    }

    #pragma omp parallel for shared(input, output) private(b_idx, i, j) schedule(static)
    for (int b_idx = 0; b_idx < b; b_idx++) {
        for (int i = 0; i < blower; i++) {
            #pragma omp simd
            for (int j = 0; j < n; j++) {
                output[b_idx][i][j] = input[b_idx][i][j];
            }
        }
        for (int i = m - bupper; i < m; i++) {
            #pragma omp simd
            for (int j = 0; j < n; j++) {
                output[b_idx][i][j] = input[b_idx][i][j];
            }
        }
        for (int i = blower; i < m - bupper; i++) {
            #pragma omp simd
            for (int j = 0; j < blower; j++) {
                output[b_idx][i][j] = input[b_idx][i][j];
            }
            #pragma omp simd
            for (int j = n - bupper; j < n; j++) {
                output[b_idx][i][j] = input[b_idx][i][j];
            }
        }
    }
}
