#ifndef COLLISIONDETECTION_H_INCLUDED
#define COLLISIONDETECTION_H_INCLUDED

#include "shape.h"
#include "matrix.h"
#include "math_utilities.h"
#include "coordinate.h"
#include "collisionStructures.h"
#include <stdio.h>

namespace physicsEngine{
//The class which will be used to handle control of the physics engine
class physicsEngineHandler: public shapeHandler{

    /**

        The physics engine will use a hashing numbering system for identifying triangles,
        circles and rectangles when handling a collision

        The rule is:
            1. The hash of a circle will end in a 1
            2. The hash of a triangle will end in a 2
            3. The hash of a rectangle will end in a 3

        The hash is computed by multiplying a shape's instantiation number by 10, and adding 1,2 or 3
        As every instantiation number is unique, every hash will also be unique

        When given a hash, one can then determine the original instantiation number simply subtracting its modulo 10
        and then dividing by 10
    **/

    //All circles will end in a 1
    //All triangles will end in a 2
    //All rectangles will end in a 3

    public:
        physicsEngineHandler(int decimalPlace, int frameR = DEFAULT_FRAME_RATE);

        bool moveObjects(bool forwardMotion = true);
        void checkCollisions(bool forwardMotion = true);
        int getFrameRate();
        bool changeFrameRate (int frameR);

        void clearShapes();

    private:

        void establishConnections(bool forwardMotion = true);
        long getCircleHash(int instNum);
        long getTriangleHash (int instNum);
        long getRectangleHash (int instNum);
        int speedDecimalPlace;

        long long int frameNumber;
        int frameRate;

        std::vector<CollisionInfo> previousCollisions;

};

//Calculates distance between two points
double distance (double x1, double y1, double x2, double y2);

bool isCirclesCollided (Circle& c1, Circle& c2);
bool isCircleRectCollided (Circle& circle, Rectangle& rect);
bool isCircleRectCollidedPrevious (Circle& circle, Rectangle& rect);
bool isCircleTriangleCollided (Circle& circle, Triangle& tri);
bool isTrianglesCollided (Triangle& tri1, Triangle& tri2);
bool isRectanglesCollided (Rectangle& rect1, Rectangle& rect2);
bool isRectangleTriangleCollided (Rectangle& rect, Triangle& tri);
bool checkCircleLinesIntersection (straightLine lines[], int lenLines, Circle& circle);
bool checkLinesIntersection (straightLine lines1[], straightLine lines2[], int lenLines1, int lenLines2);
}

#endif // COLLISIONDETECTION_H_INCLUDED
