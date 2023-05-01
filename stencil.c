#include <omp.h>
#include <math.h>

void stencil(float* inputvec, int m, int n, float* filtervec, int k, float* outputvec, int b) {
    int batch, i, j, x, y;

    // Convert input, filter, and output to 3D arrays
    float (*input)[n][m] = (float (*)[n][m]) inputvec;
    float (*filter)[k] = (float (*)[k]) filtervec;
    float (*output)[n][m] = (float (*)[n][m]) outputvec;
    
    // Compute boundary region size
    int blower = floor((k - 1) / 2.0);
    int bupper = ceil((k - 1) / 2.0);

    // Process each batch
    #pragma omp parallel for
    for (batch = 0; batch < b; batch++) {
        // Copy boundary elements from input to output
        // #pragma omp for collapse(2)
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                if (i < blower || i >= m - bupper || j < blower || j >= n - bupper) {
                    output[batch][i][j] = input[batch][i][j];
                }
            }
        }
        
        // Apply filter to input and store result in output
        // #pragma omp for collapse(2)
        for (i = blower; i < m - bupper; i++) {
            for (j = blower; j < n - bupper; j++) {
                float sum = 0.0;
                for (x = 0; x < k; x++) {
                    for (y = 0; y < k; y++) {
                        sum += input[batch][i + x - blower][j + y - blower] * filter[x][y];
                    }
                }
                output[batch][i][j] = sum;
            }
        }
    }
}