#include "math_utilities.h"
#include "rendering_engine_global_vars.h"
#include "physics_engine_global_vars.h"
#include <cmath>
#include "stdio.h"

namespace physicsEngine{
//Function computes the upper and lower pixel per frames an object must move at if not divisible by the frame rate
//It assumes the frameRate/speed gives a decimal number. If not, it will return false, meaning execution was not successful
//Likewise, if the speed is greater than the frame rate, it will also return false
bool pixelAverage (int frameRate, double speed,
                   int* numLess, int* numMore,
                   int *numLessOccurrences, int *numMoreOccurrences){

    //If the object has a negative velocity, the caller of this function should convert this into a positive value before calling the function
    if (speed == 0.0 || speed < 0 || frameRate == 0 || frameRate < 0 )
        return false;

    if (isInteger(frameRate/speed))
        return false;

    if (speed > frameRate)
        speed = frameRate;

    //We'll need to compute the gcd of frameRate and speed, so the numerator and denominator must be in integer form
    if ((frameRate/speed) > frameRate){

        while (speed < 1.0){
            speed *= 10;
            frameRate *= 10;
        }
    }

    int numDecimals = 0;
    //The user may enter a non-integer speed greater than 1, so this needs to be accounted for
    while (!isInteger(speed) && numDecimals < decimalPlaces ){
        //The fraction will still be the same, but we are just ensuring they are both integers
        speed *= 10;
        frameRate *= 10;

        numDecimals++;
    }

    int highestCommonFactor = gcd (frameRate,speed);
    frameRate /= highestCommonFactor;
    speed /= highestCommonFactor;

    *numLess = floor(frameRate/speed);
    *numMore = ceil(frameRate/speed);
    *numLessOccurrences = (*numMore) * speed - frameRate;
    *numMoreOccurrences = frameRate - (*numLess)*speed;


    return true;
}

//This function is used to distribute the pixel per frames of a moving object
//It is to ensure that an object moves as close as possible to its inputted over a smaller period of time if the speed is not divisble by the frame rate
void framesDistribution ( double totalAverage, int numA, int numB,
						  int sizeA, int sizeB,
						  int position, int distributionArray[]){
    using std::abs;
    //If the number of numA is 0, fill the rest of the array with numB and terminate function
	if (sizeA == 0 && sizeB > 0){

		for (int i = position; i >= 0; i--)
			distributionArray[i] = numB;

		return;
	}

    //If the number of numB is 0, fill the rest of the array with numA and terminate function
	if (sizeB == 0 && sizeA > 0){
		for (int i = position; i >= 0; i--)
			distributionArray[i] = numA;

		return;
	}

	//Gets average pixel per Frames for the current size of the array, if we consider adding each number in turn
	double minus_numB_average =   (sizeA*numA + (sizeB-1)*numB)  / (double)  (sizeA + sizeB -1);
	double minus_numA_average =   ( (sizeA-1)*numA + sizeB*numB)  / (double)  (sizeA + sizeB -1);

    //How close the computed pixel per Frames for the current size of the array is to the actual pixel per Frames (for each above scenario)
	double diffB = abs (totalAverage - minus_numB_average);
	double diffA = abs (totalAverage - minus_numA_average);

	//Which ever value leads to a closer value to the actual pixel per Frames will be added to the array
	//The function then gets called again, now with the amount of one of the numbers smaller by one, with the other still having the same amount left to add to the array
	if (diffB <= diffA){
		distributionArray[position] = numB;

		framesDistribution(totalAverage,numA,numB,
						   sizeA,sizeB-1,
						   position-1,distributionArray);
	}
	else{
		distributionArray[position] = numA;
		framesDistribution(totalAverage,numA,numB,
						   sizeA-1,sizeB,
						   position-1,distributionArray);
	}
}

}


