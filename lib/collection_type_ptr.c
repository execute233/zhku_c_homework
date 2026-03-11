#include "collection_type_ptr.h"
#include <string.h>
#include <stdlib.h>

int* int_ptr(const int value)  {
    int * result = malloc(sizeof(int));
    *result = value;
    return result;
}
unsigned int* uint_ptr(const unsigned int value) {
    unsigned int * result = malloc(sizeof(unsigned int));
    *result = value;
    return result;
}
long long* long_ptr(long long value) {
    long long * result = malloc(sizeof(long long));
    *result = value;
    return result;
}
unsigned long long * ulong_ptr(unsigned long long value) {
    unsigned long long * result = malloc(sizeof(unsigned long long));
    *result = value;
    return result;
}
float* float_ptr(float value) {
    float * result = malloc(sizeof(float));
    *result = value;
    return result;
}
double* double_ptr(double value) {
    double * result = malloc(sizeof(double));
    *result = value;
    return result;
}
long double* long_double_ptr(long double value) {
    long double * result = malloc(sizeof(long double));
    *result = value;
    return result;
}
char* char_ptr(char value) {
    char * result = malloc(sizeof(char));
    *result = value;
    return result;
}
char* string_ptr(char * value) {
    char * result = malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(result, value);
    return result;
}

