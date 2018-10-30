#ifndef __UTILITY_H
#define __UTILITY_H

#include <stdio.h>
#include <stdlib.h>

char* convert_str (long long int num);
char* convert_str_d (double num , int decimalPlace = 1);
bool convert_str_to_uint (const char* num, unsigned int *placeHolder);
bool convert_str_to_int (const char* num, int *placeHolder);
bool convert_str_to_double (const char* num, double* placeHolder);
int convertIntDouble (double x);

#endif


