#include <cmath>
#include "motion.h"
#include "pixelMovement.h"
#include "math_utilities.h"
#include "physics_engine_global_vars.h"
#include "rendering_engine_global_vars.h"
#include <stdio.h>

//Using this for absolute values
using std::abs;

namespace physicsEngine{
//A default value for objects whose motion value is 0
const short int OBJECT_NOT_MOVING = -1;


Motion::Motion(){
    //This is called automatically whenever an object of Motion is created
    //We need to automatically set classInitialised to false to prevent any invalid member function calls...
    //...before all appropriate variables have been set up
    //We set all pointers to nullptr, so they can be properly allocated memory later on
    classInitialised = false;
    motionNonDivisibleByFrames = false;
    ppfDistributionArray = nullptr;;
    integral = nullptr;
    motionPerSecReference = nullptr;
}

bool Motion::initialiseClass(double** integralLocation, double** rateOfChange,
							 int frameR, bool isIntegralDisplacement){


    //If parameters given are not valid the function returns false
    if (integralLocation == nullptr || rateOfChange == nullptr)
        return false;

    if (*integralLocation == nullptr || *rateOfChange == nullptr)
        return false;

    //We don't want a negative frame rate
    if (frameR <= 0)
        return false;

    integralIsDisplacement = isIntegralDisplacement;

    //We  need a direct pointer to the integral of the motion being considered
    integral = *integralLocation;

    //We want the actual value and not the pointer, so we indirect (*) twice
    motionPerSec = **rateOfChange;

    //We want the original pointer. A local pointer would lose meaning after this function call, which is why we have a pointer to a pointer
    motionPerSecReference = *rateOfChange;

    frameRate = frameR;

    previousMotionsCount = -1;

    if (initialiseAttributes())
        classInitialised = true;

    return classInitialised;

}

bool Motion::initialiseAttributes(){

    //If object isn't moving, or has no acceleration (physics engine is 1 dp, so anything under 0.01 is not moving)
    if (abs(motionPerSec) <= 0.01){
        pixelCountdown = OBJECT_NOT_MOVING;
        pixelPerFrames = OBJECT_NOT_MOVING;
        return true;
    }

    //Set up the pixel per frames distribution array
    double motionPerFrame;

    //If the motion is greater than the frame rate, then since this is impossible...
    //...we treat the motion as if its speed was equal to the frame rate
    if ( abs(motionPerSec) > frameRate)
        motionPerFrame = 1;
    else
        motionPerFrame = frameRate/abs(motionPerSec);


    //If motion per sec (speed or acceleration is not divisible by frame rate, which will be 60
    //Note, that I don't use modulus, as motionPerSec may not be an integer...
    //...So I use my custom isInteger function instead
    if (!isInteger (motionPerFrame)){

        motionNonDivisibleByFrames = true;
        //Set up pointers to single integers

        //Set up values for pixel per Frames to compensate for being a non-integer frame
        //If this fails then revert everything back
        if (!pixelAverage (frameRate,abs(motionPerSec),&ppfLowerBound,&ppfUpperBound,
                          &ppfLowerBoundCount,&ppfUpperBoundCount)){
            resetAttributes();
            return false;

        }

        if (ppfLowerBoundCount == 0 || ppfUpperBoundCount == 0){
            motionNonDivisibleByFrames = false;
            pixelPerFrames = frameRate / abs(motionPerSec);
            pixelCountdown = pixelPerFrames;

            return true;

        }

        //Set up the pixel per frames distribution array
        ppfDistributionArray = new int[ppfLowerBoundCount+ppfUpperBoundCount];


        framesDistribution (motionPerFrame, ppfLowerBound, ppfUpperBound,
                            ppfLowerBoundCount, ppfUpperBoundCount,
                            ppfLowerBoundCount + ppfUpperBoundCount-1, ppfDistributionArray);


        //Pointers to the location of the current current
        ppfDistributionMark = 0;

        pixelCountdown = ppfDistributionArray[0];
        pixelPerFrames = ppfDistributionArray[0];


    }


    else{
        //Motion cannot be greater than the frame rate
        //So if this is so, the motion gets capped at the value of the frame rate
        if (abs(motionPerSec) > frameRate)
            pixelPerFrames = 1;

        else
            pixelPerFrames = frameRate / abs(motionPerSec);

        pixelCountdown = pixelPerFrames;
    }

    return true;
}


bool Motion::moveObjectForward(){

    //Either this one, or moveobjectBackward is called for each object every frame
    //A check must be done to check if the motion has changed or not

    if (!classInitialised)
        return false;


    //If motion has been changed due to a higher derivative (e.g. velocity changing as a result of acceleration)
    if (motionPerSec != *motionPerSecReference){

        if (!(abs(motionPerSec)>=frameRate &&
              abs(*motionPerSecReference) >= frameRate)){

            //Store the current values for the countdown and motion as they are about to be changed
            int oldCountdown = pixelCountdown;
            int oldMotion = motionPerSec;
            //Get the new value

            //Creates a structure, containing info on the current velocity
            oldMotionInfo currentMotion;
            currentMotion.pixelCountdown = oldCountdown;
            currentMotion.motionNonDivisbleByFrames = motionNonDivisibleByFrames;

            if (motionNonDivisibleByFrames)
                currentMotion.ppfDistributionMark = ppfDistributionMark;

            //Push info on current velocity onto stack
            previousMotions.push_back(currentMotion);
            previousMotionsCount++;

            //Attributes are reset
            motionPerSec = *motionPerSecReference;
            resetAttributes(true);

            if(!initialiseAttributes())
                return false;

            //Old object was moving, and current object is moving
            if (oldCountdown != OBJECT_NOT_MOVING && pixelCountdown != OBJECT_NOT_MOVING){

                bool revertCountdown = true;

                //Object is moving faster
                //Check if countdown should not be reverted as it would slow down object...
                //...when it's meant to be moving faster
                if (abs(motionPerSec) > abs(oldMotion)){
                    if (pixelCountdown < oldCountdown)
                        revertCountdown = false;
                }

                if (revertCountdown)
                    pixelCountdown = oldCountdown;

            }

        }

        else
            motionPerSec = *motionPerSecReference;
    }

    //We cannot move an object that's not supposed to be moving
    if (pixelPerFrames == OBJECT_NOT_MOVING)
        return true;


    if (pixelCountdown == 1){

        //Whether to increase or decrease the object's axis
        if (motionPerSec < 0)
            *integral -= 1;

        else if (motionPerSec > 0)
            *integral += 1;

        if (!motionNonDivisibleByFrames)
            pixelCountdown = pixelPerFrames;

        else{
			//Get rid of this. Have a
            ppfDistributionMark += 1;
            ppfDistributionMark %= (ppfUpperBoundCount + ppfLowerBoundCount);
            pixelPerFrames = ppfDistributionArray[ppfDistributionMark];
            pixelCountdown = pixelPerFrames;
        }
    }

    else
        pixelCountdown -=1;


    return true;
}


bool Motion::moveObjectBackward(){

    if (!classInitialised)
        return false;

    if (motionPerSec == OBJECT_NOT_MOVING)
        return true;

    if (pixelCountdown == pixelPerFrames){

        //Whether to increase or decrease a object's axis in the other direction...
        //...it's meant to be travelling in if going forward

        if (motionPerSec<0)
            *integral += 1;

        else if (motionPerSec>0)
            *integral -= 1;


        //Countdown goes back to 1
        pixelCountdown = 1;

        //Here, ppfDistribution mark is decremented by 1
        if (motionNonDivisibleByFrames){
            ppfDistributionMark -= 1;
            if (ppfDistributionMark == -1)
                ppfDistributionMark = ppfLowerBoundCount + ppfUpperBoundCount -1;

            pixelPerFrames = ppfDistributionArray[ppfDistributionMark];
        }
    }
    //Instead of decreasing pixelCountdown by 1, we increase it by 1
    else
        pixelCountdown += 1;

    return true;

}

bool Motion::checkForChangeMovingBackwards(){
    //This function is used to check if the motion has changed or not while moving backwards
    /**
        Unlike the moveForward function, we cannot put this section of code in the
        moveBackward function. This is because when moving backwards in time,
        we do the following:

        1. Move velocity back
        2. Move acceleration back
        3. Change the velocity based on the acceleration

        This function is step 3 We cannot have it come before step 2 so it must be in its
        own function
    **/
    if (!classInitialised)
        return false;

    if (motionPerSec != *motionPerSecReference){
        //If the current motion value and new motion value are greater than frame rate...
        //...Then we don't need to do anything, as max speed has already been reached
        if (!(abs(motionPerSec)>=frameRate &&
              abs(*motionPerSecReference) >= frameRate)){

            motionPerSec = *motionPerSecReference;
            resetAttributes(true);

            if (!initialiseAttributes())
                return false;

            //Obtain previous info on velocity from previous stack
            pixelCountdown = previousMotions[previousMotionsCount].pixelCountdown;

            if (previousMotions[previousMotionsCount].motionNonDivisbleByFrames){
                ppfDistributionMark = previousMotions[previousMotionsCount].ppfDistributionMark;
                pixelPerFrames = ppfDistributionArray[ppfDistributionMark];
            }

            //Pop the stack by erasing the last element
            previousMotions.erase(previousMotions.begin() + previousMotionsCount);
            previousMotionsCount--;
        }
        //Simply allow the motion to reflect the motion change
        //However, nothing is changed internally
        else
            motionPerSec = *motionPerSecReference;
    }

    return true;

}

void Motion::resetAttributes(bool nonDivisibleFramesAttributesOnly){



    //If nonDivisibleFramesAttributes only is false...
    //...then we are completely resetting the class
    //Otherwise, only a part of it needs to be reset, due to a change in motion

    //Here, all the pointers have the memories they were pointing to deallocated
    if (motionNonDivisibleByFrames){

        if (ppfDistributionArray != nullptr){
            delete [] ppfDistributionArray;
            ppfDistributionArray = nullptr;
        }

        motionNonDivisibleByFrames = false;

    }

    //For the event that the motion has been changed, the below attributes won't need to be changed
    if (!nonDivisibleFramesAttributesOnly){
        integral = nullptr;
        motionPerSecReference = nullptr;

        classInitialised = false;
        previousMotionsCount = -1;

        //The stack gets completely erased
        previousMotions.erase(previousMotions.begin(),previousMotions.end());

    }
}


Motion::~Motion(){
    //This is automatically called when an object of Motion reaches its end of scope
    resetAttributes();
}
}

/*
void Motion::checkForChangeMovingBackwards_Defunct(){
    if (motionPerSec != *motionPerSecReference){
        if (!(abs(motionPerSec)>=frameRate &&
              abs(*motionPerSecReference) >= frameRate)){

            int oldCountdown = pixelCountdown;
            int oldMotion = motionPerSec;

            motionPerSec = *motionPerSecReference;


            resetAttributes(true);

            if (!initialiseAttributes())
                return;

            if (oldCountdown != OBJECT_NOT_MOVING){
                if (abs(motionPerSec) > abs(oldMotion)){
                    if (oldCountdown > pixelCountdown)
                        pixelCountdown = oldCountdown;

                }

                //If object is going slower
                else if ( abs(motionPerSec) < abs(oldMotion) || oldCountdown){
                    if (oldCountdown < pixelCountdown || oldCountdown)
                        pixelCountdown = oldCountdown;

                }
            }
        }

        else
            motionPerSec = *motionPerSecReference;
    }

}

bool Motion::moveObjectBackward_Defunct(){

	//This needs to be removed
	//Have a list containing all previous positions and pixel countdown


    if (!classInitialised)
        return false;

    if (pixelCountdown >= pixelPerFrames && pixelPerFrames != OBJECT_NOT_MOVING){
        //Whether to increase or decrease a object's axis in the other direction...
        //it's meant to be travelling in if going forward

        if (motionPerSec<0)
            *integral += 1;

        else if (motionPerSec>0)
            *integral -= 1;

        //Countdown goes back to 1
        pixelCountdown = 1;

        if (motionNonDivisibleByFrames){

            pixelPerFrames = ppfDistributionArray[*ppfDistributionMarkBackward];

            *ppfDistributionMarkForward -= 1;
            if (*ppfDistributionMarkForward == -1)
                *ppfDistributionMarkForward = *ppfLowerBoundCount + *ppfUpperBoundCount -1;

            *ppfDistributionMarkBackward -= 1;
            if (*ppfDistributionMarkBackward == -1)
                *ppfDistributionMarkBackward = *ppfLowerBoundCount + *ppfUpperBoundCount -1;

        }
    }

    else
        pixelCountdown += 1;

    return true;
}
*/


