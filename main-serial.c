#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>
#include <omp.h>

int *read_dims(char *filename);
float * read_array(char *filename, int *dims, int num_dims);
void *write_to_output_file(char *filename, float *output, int *dims, int num_dims);
long int product(int *array, int n);
void stencil(float *inputvec, int m, int n, float *filtervec, int k, float *outputvec, int b);


int main(int argc, char *argv[]){
    double dtime = omp_get_wtime(); // start time recording
    int * input_dims;
    int * kernel_dims;
    float * output_data;
    float * input_data;
    float * kernel_data;


    // read input and kernel dimensions
    input_dims = read_dims("input_64_512_960.dat");
    kernel_dims = read_dims("kernel_5.dat");

    // read input and kernel data
    input_data = read_array("input_64_512_960.dat", input_dims, input_dims[0]);
    kernel_data = read_array("kernel_5.dat", kernel_dims, kernel_dims[0]);

    // allocate memory for the output data
    output_data = (float *)malloc(product(input_dims, input_dims[0]) * sizeof(float));


    // call stencil function
    stencil(input_data, input_dims[1], input_dims[2], kernel_data, kernel_dims[0], output_data, input_dims[0]);

    // write output data to file
    write_to_output_file("output_64_512_960x3.dat", output_data, input_dims, input_dims[0]);

    // free memory
    free(input_dims);
    free(kernel_dims);
    free(input_data);
    free(kernel_data);
    free(output_data);

    dtime = omp_get_wtime() - dtime; // total run time
    printf("\nOpenMP stencil %d\n", dtime);

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
void *write_to_output_file(char *filename, float *output, int *dims, int num_dims){
    FILE *file = fopen(filename,"w");
    int i;
    
    if(file == NULL) {
        printf("Unable to open file: %s", filename);
        return NULL;
    }
    printf("File opened, writing dims");
    if (file != NULL) {
        for(i=0; i<num_dims; i++) {
            fprintf(file, "%d ", dims[i]);
        }
        fprintf(file, "\n");
    }
      
    long int total_elements = product(dims, num_dims);
      
    printf("Writing output data");
    for(i=0; i<total_elements; i++) {
        fprintf(file, "%.7f ", output[i]);
    }
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