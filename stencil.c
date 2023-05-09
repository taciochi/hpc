#include <omp.h>

void stencil(float input_vec, int m, int n, floatfilter_vec, int k, float output_vec, int b) {
    int b_idx, i, j, p, q;
    float (input)[m][n] = (float ()[m][n]) input_vec;
    float (filter)[k] = (float ()[k]) filter_vec;
    float (output)[m][n] = (float ()[m][n]) output_vec;

    int km = (k + 1) / 2;
    int blower = (k - 1) / 2;
    int bupper = k / 2;

    #pragma omp for shared(input, filter, output) private(b_idx, i, j, p, q) schedule(static)
    for (b_idx = 0; b_idx < b; b_idx++) {
        for (i = blower; i < m - bupper; i++) {
            #pragma omp simd private(sum)
            for (j = blower; j < n - bupper; j++) {
                float sum = 0.0;
                for (p = 0; p < k; p++) {
                    #pragma omp simd reduction(+:sum)
                    for (q = 0; q < k; q++) {
                        sum += input[b_idx][i + p - km][j + q - km] filter[p][q];
                    }
                }
                output[b_idx][i][j] = sum;
            }
        }
    }

    #pragma omp parallel for shared(input, output) private(b_idx, i, j) schedule(static)
    for (b_idx = 0; b_idx < b; b_idx++) {
        for (i = 0; i < blower; i++) {
            #pragma omp simd
            for (j = 0; j < n; j++) {
                output[b_idx][i][j] = input[b_idx][i][j];
            }
        }
        for (i = m - bupper; i < m; i++) {
            #pragma omp simd
            for (j = 0; j < n; j++) {
                output[b_idx][i][j] = input[b_idx][i][j];
            }
        }
        for (i = blower; i < m - bupper; i++) {
            #pragma omp simd
            for (j = 0; j < blower; j++) {
                output[b_idx][i][j] = input[b_idx][i][j];
            }
            #pragma omp simd
            for (j = n - bupper; j < n; j++) {
                output[b_idx][i][j] = input[b_idx][i][j];
            }
        }
    }
}

// void stencil(float *inputvec, int m, int n, float *filtervec, int k, float *outputvec, int b) {
//     int batch, row, col, sub_row, sub_col;

//     int km = (k + 1) / 2;
//     int blower = (k - 1) / 2;
//     int bupper = (k - 1) / 2;

//     float (*input)[m][n] = (float(*)[m][n])inputvec;
//     float (*filter)[k] = (float(*)[k])filtervec;
//     float (*output)[m][n] = (float(*)[m][n])outputvec;

//     #pragma omp parallel for private(batch, i, j, x, y)
//     for (batch = 0; batch < b; batch++) {
        
//         for (row = 0; row < m-1; row++) {
//             for (col = 0; col < n-1; col++) {

//                 float sum = 0.0;
//                 int k_row = 0;
//                 int k_col = 0;

//                 for (sub_row = row; sub_row < row + k; sub_row++) {
//                     for (sub_col = 0; sub_col < col + k; sub_col++) {
//                         sum += input[batch][sub_row][sub_col] * kernel[k_row][k_col];
                        

//                         int input_i = i - km + x;
//                         int input_j = j - km + y;
//                         int filter_x = x;
//                         int filter_y = y;

//                         if (input_i >= 0 && input_i < m && input_j >= 0 && input_j < n) {
//                             output[batch][i][j] += input[batch][input_i][input_j] * filter[filter_x][filter_y];
//                         }
//                         k_col++;
//                     }
//                     k_row++;
//                     k_col=0;
//                 }
//             }
//         }
//     }
// }
