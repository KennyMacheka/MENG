#include "utility.h"
#include <cstring>
#include <stdio.h>

char* convert_str (long long int num){

    /**
        We divide the number repeatedly by 10 until it is under 0.
        In fact the number will reach exactly zero because temp_num is an int
        and fractional parts are always truncated on an int.

        The number of times we divide by 10 is the number of digits in the number,
        which tells us how big our string array should be in bytes (plus an extra incase number is negative)
    **/
    long long int temp_num = num;

    int arr_size = 0;

    while (temp_num > 0){
        temp_num /= 10;

        arr_size ++;
    }

    //Create an array of chars
    char* str_num = (char *)  ( malloc (sizeof(char)*arr_size + sizeof(char)*2) );

    sprintf (str_num,"%I64d",num);
    return str_num;
}

char* convert_str_d (double num, int decimalPlace){

    double roundingError = 0.001;

    //When we truncate the decimal part, we don't want there to a rounding error. e.g. a Value of 46.0 may give 45.0 when truncated
    double temp_num = num+roundingError;
    int int_num = (int) (temp_num);

    int arr_size = 0;

    while (int_num > 0){
        int_num /= 10;
        arr_size ++;
    }

    //An extra 4 bytes for the decimal point, the number after the decimal point, null character and just incase the number is negative for the - sign
    char* str_num = (char *) ( malloc (sizeof(char)*arr_size + sizeof(char)*4));

    if (decimalPlace == 1)
        sprintf (str_num,"%.1f", num);

    //2 dp max
    else
        sprintf(str_num,"%.2f",num);

    return str_num;
}


bool convert_str_to_int (const char* num, int *placeHolder){
    //Function converts a string to an int
    int length = strlen(num);

    int power10 = 1;
    int converted = 0;

    for (int i = length-1;i>=0;i--){

		if (num[i]<='9' && num[i] >= '0'){
			converted += power10* (num[i]-'0');

			power10 *= 10;

		}

		else if (num[i] == '-' && i == 0){
			converted *= -1;

		}

		else
			return false;

    }

	*placeHolder = converted;

    return true;

}

//We need another, as unsigned integers cannot have a negative number
bool convert_str_to_uint (const char* num, unsigned int *placeHolder){
    //Function converts a string to an int

    int length = strlen(num);

    int power10 = 1;
    unsigned int converted = 0;

    for (int i = length-1;i>=0;i--){

		if (num[i]<='9' && num[i] >= '0'){
			converted += power10* (num[i]-'0');

			power10 *= 10;

		}

		else
			return false;

    }

	*placeHolder = converted;

    return true;

}

bool convert_str_to_double (const char* num, double* placeHolder){
    if(sscanf(num,"%lf", placeHolder) == EOF)
        return false;

    return true;

}

int convertIntDouble (double x){
    /**Function converted a double variable that is an integer to an actual integer type
        Caller must be 100% sure that x is in actual fact an integer**/

    if (x > 0)
        return (int) (x+0.5);

    else
        return (int) (x-0.5);
}
