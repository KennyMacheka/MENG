#include "fraction.h"
#include "math_utilities.h"
#include <stdio.h>

//As a general rule, if fraction is negative, the numerator will take the negative value

fraction::fraction(){
     _numerator = 1;
    _denominator = 1;
}

fraction::fraction (long long num, long long denom){

    if (denom == 0)
        *this = fraction();

    _numerator = num;
    _denominator = denom;

    simplify();
}

fraction::fraction (double decimal){

    int numerator;
    int denominator = 1;

    if (decimal == 0)
        fraction();

    while (!isInteger(decimal)){
        decimal *= 10;
        denominator *= 10;
    }

    numerator = decimal;
    fraction f (numerator, denominator);

    *this = f;

}

void fraction::operator=(int x){

    *this = (long long int)(x);
}

void fraction::operator=(long long int x){

    fraction f (x);
    *this = f;
}

void fraction::operator=(double x){

    fraction f(x);
    *this = f;
}

void fraction::operator=(fraction x){

    _numerator = x._numerator;
    _denominator = x._denominator;
}

void fraction::operator += (int x){

    *this = (*this) + x;

}

void fraction::operator += (long long int x){

    *this = (*this) + x;
}

void fraction::operator += (double x){

    fraction f(x);

    *this += f;
}

void fraction::operator+=(fraction otherFrac){

    *this = (*this) + otherFrac;
}

fraction fraction::operator+(int x){

    return *this + (long long int) (x);

}

fraction fraction::operator+(long long int x){

    long long newNum = _numerator + x*_denominator;
    fraction newFrac (newNum,_denominator);
    return newFrac;

}

fraction fraction::operator+(double x){

    fraction f (x);

    return *this + f;
}

fraction fraction::operator+(fraction otherFrac){

    long long newNumerator = _numerator*otherFrac._denominator + otherFrac._numerator*_denominator;
    long long newDenominator = _denominator * otherFrac._denominator;

    fraction newFrac = fraction (newNumerator, newDenominator);


    return newFrac;
}


fraction fraction::operator-(int x){

    return *this - (long long int)(x);

}

fraction fraction::operator-(long long int x){

    long long newNum = _numerator - x*_denominator;
    fraction newFrac = fraction (newNum,_denominator);

    return newFrac;

}

fraction fraction::operator-(double x){

    fraction f (x);

    return *this - f;
}

fraction fraction::operator-(fraction otherFrac){

    long long newNum = _numerator*otherFrac._denominator - otherFrac._numerator*_denominator;
    long long newDenom = _denominator * otherFrac._denominator;

    fraction newFrac = fraction (newNum, newDenom);

    return newFrac;
}


void fraction::operator -= (double x){

    fraction f(x);

    *this -= f;
}

void fraction::operator-=(int x){

    (*this) = (*this) - x;
}

void fraction::operator-=(long long int x){

    *this = (*this) - x;
}

void fraction::operator-=(fraction otherFrac){

    (*this) = (*this) - otherFrac;
}


fraction fraction::operator*(int x){

    return (*this) * (long long int)(x);

}

fraction fraction::operator*(long long int x){

    long long newNum = _numerator * x;
    fraction newFrac = fraction (newNum,_denominator);

    return newFrac;

}

fraction fraction::operator*(fraction otherFrac){

    long long newNum = _numerator * otherFrac._numerator;
    long long newDenom = _denominator * otherFrac._denominator;

    fraction newFrac = fraction (newNum, newDenom);

    return newFrac;
}

void fraction:: operator*=(int x){

    (*this) = (*this) * x;
}

void fraction:: operator*=(long long int x){

    (*this) = (*this) * x;
}


void fraction:: operator*=(fraction otherFrac){

    (*this) = (*this) * otherFrac;
}

fraction fraction::operator*(double x){

    fraction f (x);

    return *this * f;
}

void fraction::operator*=(double x){

    fraction f (x);

    *this *= f;
}

fraction fraction::operator/(int x){

   return (*this) / (long long int) (x);

}

fraction fraction::operator/(long long int x){
    //If division by zero is attempted, the result will be the original fraction
    if (x == 0)
        return *this;

    long long newDenom = _denominator * x;
    fraction newFrac = fraction (_numerator,newDenom);

    return newFrac;

}

fraction fraction::operator/(double x){

    if (x == 0.0)
        return *this;

    fraction f (x);

    return *this / f;
}

fraction fraction::operator/(fraction otherFrac){

    long long newNum = _numerator * otherFrac._denominator;
    long long newDenom = _denominator * otherFrac._numerator;

    fraction newFrac = fraction (newNum,newDenom);

    return newFrac;

}

void fraction::operator/=(int x){

    (*this) = (*this) / x;
}

void fraction::operator/=(long long int x){

    (*this) = (*this) / x;
}

void fraction::operator/=(double x){

    fraction f (x);

    *this /= f;
}

void fraction::operator/=(fraction otherFrac){

    (*this) = (*this) / otherFrac;
}

void fraction::simplify(){

    if (_numerator == 0){
        _denominator = 1;
        return;
    }

    //Highest common factor
    int hcf = gcd (_numerator,_denominator);

    //If the numerator is 0, then the hcf will be 0. We don't want to do a division by zero
    //Likewise, a division by 1 is redundant
    if (hcf != 0 && hcf != 1){
        _numerator /= hcf;
        _denominator /= hcf;
    }

    //Remove the negative signs, as to negatives make a positive
    if (_numerator < 0 && _denominator < 0){
        _numerator *= -1;
        _denominator *= -1;
    }

    //Make numerator negative
    else if (_numerator > 0 && _denominator < 0){
        _numerator *= -1;
        _denominator *= -1;
    }
}

void fraction::numerator (long long newNum){

    _numerator = newNum;
    simplify();

}

bool fraction::denominator (long long newDenom){
	
	

    if (newDenom == 0)
        return false;

    _denominator = newDenom;
    simplify();

    return true;
}

bool fraction::operator==(int x){

    return (*this) == (long long int)(x);
}

bool fraction::operator==(long long int x){


    if (_numerator == x && _denominator == 1)
        return true;

    return false;

}

bool fraction::operator==(fraction otherFrac){

    //Checks if fraction object is equal to another fraction object
    if (_numerator == otherFrac._numerator && _denominator == otherFrac._denominator)
        return true;

    return false;
}

bool fraction::operator==(double x){

    fraction f(x);

    return (*this) == f;

}

bool fraction::operator!=(int x){

    return (*this) != (long long int )(x);
}

bool fraction::operator!=(long long int x){

     if (!( *this == x ))
        return true;

    return false;

}

bool fraction::operator!=(fraction otherFrac){
    if (!(*this == otherFrac))
        return true;

    return false;
}

bool fraction::operator!= (double x){

    fraction f(x);

    return (*this) != f;
}

bool fraction::operator<(int x){

    return (*this) < (long long int)(x);
}

bool fraction::operator< (long long int x){

    if (decimalVersion() < x)
        return true;

    return false;
}

bool fraction::operator<(fraction x){

    if (decimalVersion() < x.decimalVersion())
        return true;

    return false;
}

bool fraction::operator<=(int x){

    return (*this) <= (long long int)(x);
}

bool fraction::operator<=(long long int x){

    if (*this < x || *this == x)
        return true;

    return false;
}

bool fraction::operator<=(fraction x){

    if (*this < x || *this == x)
        return true;

    return false;
}

bool fraction::operator>(int x){

    return (*this) > (long long int)(x);
}

bool fraction::operator> (long long int x){

    if (decimalVersion() > x)
        return true;

    return false;
}

bool fraction::operator>(fraction x){

    if (decimalVersion() > x.decimalVersion())
        return true;

    return false;
}

bool fraction::operator>=(int x){

    return (*this) >= (long long int)(x);
}

bool fraction::operator>=(long long int x){

    if (*this > x || *this == x)
        return true;

    return false;
}

bool fraction::operator>=(fraction x){

    if (*this > x || *this == x)
        return true;

    return false;
}

fraction fraction::absoluteValue(){

    if ((*this) < 0)
        return *this*-1;
    return *this;

}
long long fraction::numerator(){

    return _numerator;
}

long long fraction::denominator(){

    return _denominator;
}

double fraction::decimalVersion(){

    //Casting occurs, numerator and _denominators are converted into floating-points so a division can be carried out on them
    return (double) (_numerator) / (double) (_denominator);
}
