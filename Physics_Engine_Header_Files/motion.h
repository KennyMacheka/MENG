#ifndef MOTION_H_INCLUDED
#define MOTION_H_INCLUDED

#include <vector>

namespace physicsEngine{
//Stores information that will be needed if an object has acceleration, and we...
//...want to move the simulation back in time
struct oldMotionInfo{
    int pixelCountdown;
    bool motionNonDivisbleByFrames;
    int ppfDistributionMark;
};

extern const short int OBJECT_NOT_MOVING;

class Motion{

private:
    //Used to check if the class was successfully initialised
    bool classInitialised;

    //The rate of change in seconds (pixels per second, pixels per pixels per second if acceleration)
    double motionPerSec;

    //Reference to the original rate of change in case it changes
    double* motionPerSecReference;

    //Rate of change in pixel per Frames
    int pixelPerFrames;

    //Countdown until 'integral' is altered
    int pixelCountdown;

    int frameRate;


    //Is rate of change not divisible by frame rate (60)
    bool motionNonDivisibleByFrames;


    //ppf = pixel-per-frames
    //These are used if motion per second is not divisible by frame rate
    int ppfLowerBound, ppfUpperBound;
    int ppfLowerBoundCount, ppfUpperBoundCount;
    int* ppfDistributionArray;
    int ppfDistributionMark;

    //A pointer to the integral of the rate of change which gets altered
    double* integral;

    //A boolean to check if the integral of the motion being considered is displacement (i.e. if this motion object is for velocity)
    //If we're using any higher derivatives, this should be set to false
    bool integralIsDisplacement;

    //Stores all the details about previous motions that were affected by a higher derivative
    //So if Motion is being used for velocity, it stores previous information for all different velocities...
    //...the object took, if it was affected by acceleration
    //This structure is treated as a stack- the top element will be read off it
    std::vector <oldMotionInfo> previousMotions;
    //Used to check the maximum index so the last element can be popped from the above stack when necessary
    int previousMotionsCount;

    bool initialiseAttributes();



public:
    Motion();
    ~Motion();

    //Reset everything
    void resetAttributes(bool nonDivisibleFramesAttributesOnly = false);

    bool initialiseClass (double** integralLocation, double** rateOfChange, int frameR, bool isIntegralDisplacement);
    bool moveObjectForward();
    //When going backwards in time
    bool moveObjectBackward();
    bool checkForChangeMovingBackwards();


    //bool moveObjectBackward_Defunct();

    //void checkForChangeMovingBackwards_Defunct();


};
}
#endif // MOTION_H_INCLUDED
