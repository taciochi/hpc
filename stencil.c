#include <omp.h>
#include <math.h>

void stencil_omp(float* inputvec, int m, int n, float* filtervec, int k, float* outputvec, int b) {
    // Convert input, filter, and output to 3D arrays
    float (input)[m][n] = (float (*)[m][n])inputvec;
    float (filter)[k] = (float (*)[k])filtervec;
    float (output)[m][n] = (float (*)[m][n])outputvec;
    // Compute boundary region size
    int blower = floor((k - 1) / 2.0);
    int bupper = ceil((k - 1) / 2.0);

    // Process each batch
    #pragma omp parallel for
    for (int batch = 0; batch < b; batch++) {
        // Copy boundary elements from input to output
        #pragma omp for collapse(2)
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (i < blower || i >= m - bupper || j < blower || j >= n - bupper) {
                    output[batch][i][j] = input[batch][i][j];
                }
            }
        }
        
        // Apply filter to input and store result in output
        #pragma omp for collapse(2)
        for (int i = blower; i < m - bupper; i++) {
            for (int j = blower; j < n - bupper; j++) {
                float sum = 0.0;
                for (int x = 0; x < k; x++) {
                    for (int y = 0; y < k; y++) {
                        sum += input[batch][i + x - blower][j + y - blower] * filter[x][y];
                    }
                }
                output[batch][i][j] = sum;
            }
        }
    }
}