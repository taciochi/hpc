#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>
#include<mpi.h>

int *read_dims(char *filename);
float *read_array(char *filename, int *dims, int num_dims);
void write_to_output_file(char *filename, float *output, int *dims, int num_dims);
long int product(int *array, int n);
void stencil(float* inputvec, int m, int n, float* filtervec, int k, float* output, int b);

int main(int argc, char *argv[]){
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *input_file = "input_64_512_960.dat";
    char *output_file = "output_64_512_960x5.dat";
    char *filter_file = "kernel_5.dat";

    int *input_dimensions = read_dims(input_file);
    int input_num_of_dimensions = input_dimensions[0];
    int *input_dims = &input_dimensions[1];
    int input_size = product(input_dims, input_num_of_dimensions);

    float *input = read_array(input_file, input_dims, input_num_of_dimensions);

    int *filter_dimensions = read_dims(filter_file);
    int filter_num_of_dimensions = filter_dimensions[0];
    int *filter_dims = &filter_dimensions[1];
    int filter_size = product(filter_dims, filter_num_of_dimensions);

    float *filter = read_array(filter_file, filter_dims, filter_num_of_dimensions);

    int b = input_dims[0];
    int m = input_dims[1];
    int n = input_dims[2];
    int k = filter_dims[1];

    float *output = malloc(sizeof(float) * input_size);

    double start_time = MPI_Wtime();
    stencil(input, m, n, filter, k, output, b);
    double end_time = MPI_Wtime();

    if (rank == 0) {
        write_to_output_file(output_file, output, input_dims, input_num_of_dimensions);
    }

    free(input_dimensions);
    free(input);
    free(filter_dimensions);
    free(filter);
    free(output);

    MPI_Finalize();

    if (rank == 0) {
        printf("Time taken for %d threads: %f seconds\n", size, end_time - start_time);
    }

    return 0;
}

/*Code for reading and writing to the files*/

/*Gets the dimensions of the matrices. This will return a 1d array of 4 elements [0] = number of dimensions [1] = batch [2] = m [3] = n*/
int *read_dims(char *filename) {
    FILE *file = fopen(filename,"r");
    int i;
    
    if(file == NULL) {
        printf("Unable to open file: %s", filename);
        return NULL;
    }

    char firstline[500];
    fgets(firstline, 500, file);
    
    int line_length = strlen(firstline);

    int num_dims = 0;
    
    for(i=0; i<line_length; i++) {  
        if(firstline[i] == ' ') {
            num_dims++;
        }
    }
    
    int *dims = malloc((num_dims+1)*sizeof(int));
    dims[0] = num_dims;
    const char s[2] = " ";
    char *token;
    token = strtok(firstline, s);
    i = 0;
    while( token != NULL ) {
        dims[i+1] = atoi(token);
        i++;
        token = strtok(NULL, s);
    }
    fclose(file);
    return dims;
}

/*Gets the data from the file and returns it as a 1 dimensional array*/
float * read_array(char *filename, int *dims, int num_dims) {
    FILE *file = fopen(filename,"r");
    int i;
    
    if(file == NULL) {
        printf("Unable to open file: %s", filename);
        return NULL;
    }

    char firstline[500];
    fgets(firstline, 500, file);

    //Ignore first line and move on since first line contains 
    //header information and we already have that. 

    long int total_elements = product(dims, num_dims);

    float *one_d = malloc(sizeof(float) * total_elements);
    
    for(i=0; i<total_elements; i++) {
        fscanf(file, "%f", &one_d[i]);
    }
    fclose(file);
    return one_d;
}

/*Writes to the output file*/
void write_to_output_file(char *filename, float *output, int *dims, int num_dims){
    FILE *file = fopen(filename,"w");
    int i;
    
    if(file == NULL) {
        printf("Unable to open file: %s", filename);
        return;
    }

    if (file != NULL) {
        for(i=0; i<num_dims; i++) {
            fprintf(file, "%d ", dims[i]);
        }
        fprintf(file, "\n");
    }
      
    long int total_elements = product(dims, num_dims);
      
    for(i=0; i<total_elements; i++) {
        fprintf(file, "%.7f ", output[i]);
    }

    fclose(file);
}

/*Returns the number of elements by multiplying the dimensions*/
long int product(int *array, int n) {
    long int product = 1;
    int i;
    for(i=0; i<n; i++) {
        product *= array[i];
    }
    return product;
}

/*Performs the stencil operation on the input array using the filter array*/
void stencil(float* inputvec, int m, int n, float* filtervec, int k, float* output, int b) {
    int i, j, l, p, q;
    int h = k / 2;
    int input_size = m * n * sizeof(float);

    float* input = inputvec;
    float* filter = filtervec;
    float* output = outputvec;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    for (p=0; p<b; p++) {
        for (i=rank; i<m; i+=size) {
            for (j=0; j<n; j++) {
                float sum = 0.0f;
                for (l=-h; l<=h; l++) {
                    for (q=-h; q<=h; q++) {
                        int ii = i + l;
                        int jj = j + q;
                        if (ii >= 0 && ii < m && jj >= 0 && jj < n) {
                            sum += input[p*m*n + ii*n + jj] * filter[(l+h)*k + (q+h)];
                        }
                    }
                }
                output[p*m*n + i*n + j] = sum;
            }
        }
    }

    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, output, input_size/size, MPI_FLOAT, MPI_COMM_WORLD);
}