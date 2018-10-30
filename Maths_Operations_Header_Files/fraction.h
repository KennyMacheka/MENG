#ifndef FRACTION_H_INCLUDED
#define FRACTION_H_INCLUDED

//The fraction class is used to avoid losing precision when handling floating-point numbers
//The default fraction is a numerator of 1 and denominator of 1
class fraction{

    public:
        //Default initialiser sets fraction to 1/1
        fraction();

        fraction(double decimal);

        //Upon instantiation a value can be given for numerator and denominator
        fraction(long long num, long long denom);

        //'operator' is a keyword in c++, that allows objects to be treated as operands, and to use other operands
        /**The following operator declarations are fore adding, multiplying, subtracting, dividing
           and assigning a new fraction to the fraction object**/
        //The operations involve modifying the current fraction object, or returning a new fraction object

        void operator=(int x);
        void operator=(long long int x);
        void operator=(double x);
        void operator=(fraction x);

        void operator+=(double x);
        void operator+=(int x);
        void operator+=(long long int x);
        void operator+=(fraction otherFrac);
        fraction operator+(double x);
        fraction operator+(int x);
        fraction operator+(long long int x);
        fraction operator+(fraction otherFrac);

        void operator-=(double x);
        void operator-=(int x);
        void operator-=(long long int x);
        void operator-=(fraction otherFrac);
        fraction operator-(double x);
        fraction operator-(int x);
        fraction operator-(long long int x);
        fraction operator-(fraction otherFrac);

        void operator*=(double x);
        void operator*=(int x);
        void operator*=(long long int x);
        void operator*=(fraction otherFrac);
        fraction operator*(double x);
        fraction operator*(int x);
        fraction operator*(long long int x);
        fraction operator*(fraction otherFrac);

        void operator/=(double x);
        void operator/=(int x);
        void operator/=(long long int x);
        void operator/=(fraction otherFrac);
        fraction operator/(double x);
        fraction operator/(int x);
        fraction operator/(long long int x);
        fraction operator/(fraction otherFrac);

        //Operator used to check for quality
        bool operator==(int x);
        bool operator==(long long int x);
        bool operator==(fraction otherFrac);
        bool operator == (double x);

        //Operator used to check for inequality
        bool operator!=(int x);
        bool operator!=(long long int x);
        bool operator!=(fraction otherFrac);
        bool operator!= (double x);

        //Operator use to check for greater than or less than
        bool operator<(int x);
        bool operator<(long long int x);
        bool operator<(fraction x);
        bool operator<=(int x);
        bool operator<=(long long int x);
        bool operator<=(fraction x);

        bool operator>(int x);
        bool operator>(long long int x);
        bool operator>(fraction x);
        bool operator>=(int x);
        bool operator>=(long long int x);
        bool operator>=(fraction x);

        //Return the value of the numerator and denominator
        long long numerator();
        long long denominator();

        //Set a new value for numerator
        void numerator (long long newNum);
        //Sets a new value for denominator. Returns false upon failure (will occur if value of 0 is passed)
        bool denominator (long long newDenom);

        fraction absoluteValue();

        //Returns the decimal version of the fraction
        double decimalVersion();


    private:
        long long _numerator;
        long long _denominator;
        //Simplifies the numerator and denominator by finding the highest common factor between both
        void simplify();

};
#endif // FRACTION_H_INCLUDED
