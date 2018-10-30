#include "math_utilities.h"
#include <cmath>
#include <stdio.h>

//Function checks if number is technically an integer
bool isInteger (double n){

    double roundingError = 0.00001;

    /**If n is an integer, it will have no fractional part, so floor(n) will be equal to n
        However, it's generally not a good idea to directly compare doubles,as there may be a
        rounding error so they are not precisely the same at the machine level.
        So I subtract the two and see if they are less than the rounding error, which I set
        as 0.00001.
    **/

    if (std::abs(n - floor(n+0.5)) < roundingError)
        return true;


    return false;
}

//Function computes the highest common factor of two numbers using Euclid's algorithm
long long int gcd (long long int m, long long int n){

    if (m < 0)
        m *= -1;

    if (n < 0)
        n *= -1;

    if (m<n){
        int temp = n;
        n = m;
        m = temp;
    }

    if (n == 0)
        return m;

	//Recursive
    return gcd (n, m%n);

}


double roundNumber (double number, int decimalPlaces){

    /**

        Work out decimal places of the number by increasingly multiplying by 10
        Once we have an integer, that's the decimal places

        If decimal places is less than target decimal place, return that number
    **/

    int i;
    int decimalPlacesOfNumber = 0;
    double temp = number;
    double base  = 1.0;
    long long int rounder;

    //Keep multiplying the decimal by 10 so the last unit digits becomes the number we check if it should be rounded
    for (i = 0; i < decimalPlaces+1;i++){

        temp *= 10;

        if (isInteger(temp) && i < decimalPlaces)
            return number;
    }

    rounder = (long long int) (temp) %10;


    temp = floor( (temp/10.0) );
    double divider = 1;

    for (i = 0; i<decimalPlaces;i++){
        divider /= 10;
        temp /= 10;
    }

    if (rounder >= 5)
        return temp + divider;

    else
        return temp;
}


