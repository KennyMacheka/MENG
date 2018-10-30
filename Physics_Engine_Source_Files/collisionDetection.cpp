#include <cmath>
#include <stdio.h>
#include "collisionDetection.h"
#include "collisionHandling.h"
#include "math_utilities.h"

using std::vector;
namespace physicsEngine{
//Contains all of the functions for detecting collisions between all of the respective shapes
double distance (double x1, double y1, double x2, double y2){

	//Function gets the distance between two coordinates using Pythagoras' theorem
    double diffx = x1 - x2;
    double diffy = y1 - y2;

    //Pythahoras' Theorem
    double squared = diffx*diffx + diffy*diffy;

    return sqrt(squared);

}

bool isCircleRectCollided(Circle& circle, Rectangle& rect){

    straightLine lines [] = {rect.getLine1(),rect.getLine2(),rect.getLine3(),rect.getLine4()};

    return checkCircleLinesIntersection(lines,4,circle);
}

bool isCircleTriangleCollided (Circle& circle, Triangle& tri){

    straightLine lines[] = {tri.getLine1(),tri.getLine2(),tri.getLine3()};

    return checkCircleLinesIntersection(lines,3,circle);
}

bool isCirclesCollided(Circle& c1, Circle& c2){
	/**Checks if circls have collided by adding up their radii
		The function then checks the distance between the two circles by taking their centres
		If this distance is less than or equal to the sum of their radii then they have collided**/

    int radiiSum = c1.getRadius() + c2.getRadius();


    if (distance(c1.xPosition(), c1.yPosition(),
        c2.xPosition(),c2.yPosition()) <= radiiSum)
        return true;

    return false;
}

bool isTrianglesCollided(Triangle& tri1, Triangle& tri2){

    straightLine tri1Lines[] = {tri1.getLine1(),tri1.getLine2(),tri1.getLine3()};
    straightLine tri2Lines[] = {tri2.getLine1(),tri2.getLine2(),tri2.getLine3()};

    return checkLinesIntersection(tri1Lines,tri2Lines,3,3);

}

bool isRectangleTriangleCollided(Rectangle& rect, Triangle& tri){

    straightLine triLines[] =  {tri.getLine1(),tri.getLine2(),tri.getLine3()};
    straightLine rectLines[] = {rect.getLine1(),rect.getLine2(),rect.getLine3(),rect.getLine4()};

    return checkLinesIntersection(rectLines,triLines,4,3);


}

bool isRectanglesCollided(Rectangle& rect1, Rectangle& rect2){
    straightLine rect1Lines[] = {rect1.getLine1(),rect1.getLine2(),rect1.getLine3(),rect1.getLine4()};
    straightLine rect2Lines[] =  {rect2.getLine1(),rect2.getLine2(),rect2.getLine3(),rect2.getLine4()};

    return checkLinesIntersection(rect1Lines,rect2Lines,4,4);

}

bool checkLinesIntersection (straightLine lines1[], straightLine lines2[], int lenLines1, int lenLines2){

    for (int i = 0; i < lenLines1; i++){
        for (int j = 0; j < lenLines2; j++){
            if (lines1[i].lineIntersection(lines2[j]))
                return true;

        }
    }

    return false;
}

bool checkCircleLinesIntersection (straightLine lines[], int lenLines, Circle& circle){

    for (int i = 0; i< lenLines;i++){
        if (lines[i].circleIntersection(circle))
            return true;
    }

    return false;
}

Graph::Graph(){
    numObjects = 0;
    restitutionValue = -1;
    sameRestitution = -1;
    shouldHandleCollision = false;
}

physicsEngineHandler::physicsEngineHandler(int decimalPlace, int frameR):shapeHandler(){

    frameNumber = 0;

    if (frameR <= 0)
        frameRate = DEFAULT_FRAME_RATE;

    if (decimalPlace >= 0)
        speedDecimalPlace = decimalPlace;
    else
        speedDecimalPlace = 1;

    addRectangle(&topWall);
    addRectangle(&leftWall);
    addRectangle(&bottomWall);
    addRectangle(&rightWall);
}

bool physicsEngineHandler::changeFrameRate(int frameR){

    if (frameR <= 0)
        return false;

    frameRate = frameR;

    int i;

    for (i = 0; i<numCircles;i++)
        circles[i]->changeFrameRate(frameRate);

    for (i = 0; i<numTriangles;i++)
        triangles[i]->changeFrameRate(frameRate);

    for (i = 0; i<numRects;i++)
        rectangles[i]->changeFrameRate(frameRate);

    return true;

}

int physicsEngineHandler::getFrameRate(){

    return frameRate;
}

void physicsEngineHandler::clearShapes(){

    rectangles.clear();
    numRects = 0;

    circles.clear();
    numCircles = 0;

    triangles.clear();
    numTriangles = 0;

    previousCollisions.clear();


    addRectangle(&topWall);
    addRectangle(&bottomWall);
    addRectangle(&leftWall);
    addRectangle(&rightWall);

}

void physicsEngineHandler::checkCollisions(bool forwardMotion){

    /**This function checks for collisions between every shape**/

    //Circle and rectangle collision check
    bool (Rectangle::*locateCirclePtr)(int,int*);
    bool (Circle::*locateRectPtr)(int,int*);
    locateCirclePtr = &Rectangle::locateCircle;
    locateRectPtr = &Circle::locateRectangle;


    bool (Rectangle::*addCirclePtr)(Circle*);
    bool (Circle::*addRectPtr)(Rectangle*);
    addCirclePtr = &Rectangle::addCircle;
    addRectPtr = &Circle::addRectangle;


    bool (Rectangle::*removeCirclePtr)(Circle*);
    bool (Circle::*removeRectPtr)(Rectangle*);
    removeCirclePtr = &Rectangle::removeCircle;
    removeRectPtr = &Circle::removeRectangle;


    //Shape 1 = circle, Shape 2 = rectangle
    checkCollision<Circle,Rectangle>  (circles, rectangles, numCircles, numRects,
                    locateCirclePtr, locateRectPtr,&isCircleRectCollided,
                    addRectPtr, addCirclePtr, removeCirclePtr,removeRectPtr,false);


    //Circle and triangle collision check
    bool (Triangle::*locateCirclePtr2)(int,int*);
    bool (Circle::*locateTriPtr)(int,int*);
    locateCirclePtr2 = &Triangle::locateCircle;
    locateTriPtr = &Circle::locateTriangle;

    bool (Triangle::*addCirclePtr2)(Circle*);
    bool (Circle::*addTriPtr)(Triangle*);
    addCirclePtr2 = &Triangle::addCircle;
    addTriPtr = &Circle::addTriangle;

    bool (Triangle::*removeCirclePtr2)(Circle*);
    bool (Circle::*removeTriPtr)(Triangle*);
    removeCirclePtr2 = &Triangle::removeCircle;
    removeTriPtr = &Circle::removeTriangle;


    //Shape 1 = circle, Shape 2 = triangle
    checkCollision<Circle,Triangle>  (circles, triangles, numCircles, numTriangles,
                    locateCirclePtr2, locateTriPtr,&isCircleTriangleCollided,
                    addTriPtr, addCirclePtr2, removeCirclePtr2,removeTriPtr,false);



    //Triangle and Rectangle Collision check
    bool (Triangle::*locateRectPtr2)(int,int*);
    bool (Rectangle::*locateTriPtr2)(int,int*);
    locateRectPtr2 = &Triangle::locateRectangle;
    locateTriPtr2 = &Rectangle::locateTriangle;

    bool (Triangle::*addRectPtr2)(Rectangle*);
    bool (Rectangle::*addTriPtr2)(Triangle*);
    addRectPtr2 = &Triangle::addRectangle;
    addTriPtr2 = &Rectangle::addTriangle;

    bool (Triangle::*removeRectPtr2)(Rectangle*);
    bool (Rectangle::*removeTriPtr2)(Triangle*);
    removeRectPtr2 = &Triangle::removeRectangle;
    removeTriPtr2 = &Rectangle::removeTriangle;

    //Shape 1 = Rectangle, Shape 2 = Triangle
    checkCollision<Rectangle,Triangle> (rectangles,triangles,numRects,numTriangles,
                    locateRectPtr2,locateTriPtr2,&isRectangleTriangleCollided,
                    addTriPtr2,addRectPtr2,removeRectPtr2,removeTriPtr2,false);

    //Two triangles collision check
    bool (Triangle::*locateTriPtr3)(int,int*);
    locateTriPtr3 = &Triangle::locateTriangle;

    bool (Triangle::*addTriPtr3)(Triangle*);
    addTriPtr3 = &Triangle::addTriangle;

    bool (Triangle::*removeTriPtr3)(Triangle*);
    removeTriPtr3 = &Triangle::removeTriangle;

    checkCollision<Triangle,Triangle> (triangles,triangles,numTriangles,numTriangles,
                    locateTriPtr3,locateTriPtr3,&isTrianglesCollided,
                    addTriPtr3,addTriPtr3,removeTriPtr3,removeTriPtr3,true);


    //Two rectangles collision check
    bool (Rectangle::*locateRectPtr3)(int,int*);
    locateRectPtr3 = &Rectangle::locateRectangle;

    bool (Rectangle::*addRectPtr3)(Rectangle*);
    addRectPtr3 = &Rectangle::addRectangle;

    bool (Rectangle::*removeRectPtr3)(Rectangle*);
    removeRectPtr3 = &Rectangle::removeRectangle;

    checkCollision<Rectangle,Rectangle> (rectangles,rectangles,numRects,numRects,
                    locateRectPtr3,locateRectPtr3,&isRectanglesCollided,
                    addRectPtr3,addRectPtr3,removeRectPtr3,removeRectPtr3,true);


    //Two Circles collision check

    bool (Circle::*locateCirclePtr3)(int,int*);
    locateCirclePtr3 = &Circle::locateCircle;

    bool (Circle::*addCirclePtr3)(Circle*);
    addCirclePtr3 = &Circle::addCircle;

    bool (Circle::*removeCirclePtr3)(Circle*);
    removeCirclePtr3 = &Circle::removeCircle;


    checkCollision<Circle,Circle> (circles,circles,numCircles,numCircles,
                    locateCirclePtr3,locateCirclePtr3,&isCirclesCollided,
                    addCirclePtr3,addCirclePtr3,removeCirclePtr3,removeCirclePtr3,true);


    establishConnections(forwardMotion);

    /**Use this commenyed example of checking collisions between all circles and triangles
	   to guage how it is the checkCollisions function works, as it is quite abstract.

	   I designed that function to avoid repetition, as I'd have to repeat the code
	   below for every shape combination otherwise**/

	/*
    int i,j;
    printf("Hello %d %d\n",numRects,numCircles);
    for (i = 0; i < numRects; i++){
        for ( j = 0; j < numCircles; j++){

            //First we check if objects are in each other's lists
            //If they are, and no collision is detected between them, we remove them
            int location1, location2;

            //Boolean to check whether objects were previously collided
            bool alreadyConnected = false;
            bool collisionDetected = false;
            if (rectangles[i]->locateCircle(circles[j], &location1)){
                circles[j]->locateRectangle(rectangles[i],&location2);
                alreadyConnected = true;
            }

            if (isCircleRectCollided(*circles[j],*rectangles[i])){
                printf("Detected collision\n");
                collisionDetected = true;

                if (!alreadyConnected){

                    circles[j]->addRectangle(rectangles[i]);
                    rectangles[i]->addCircle(circles[j]);
                }

                circles[j]->recentlyDetectedCollision = true;
                rectangles[i]->recentlyDetectedCollision = true;

            }

            else{

                printf("No collision detected\n");

                if (alreadyConnected){

                    rectangles[i]->removeCircle(circles[j]);
                    circles[j]->removeRectangle(rectangles[i]);
                }
            }
        }
    }

    */
}

long physicsEngineHandler::getCircleHash(int instNum){

    long circleHash = (long) (instNum) *10 +1;
    return circleHash;
}

long physicsEngineHandler::getTriangleHash(int instNum){

    long triangleHash = (long) (instNum) *10 +2;
    return triangleHash;
}

long physicsEngineHandler::getRectangleHash(int instNum){

    long rectangleHash = (long) (instNum)*10 +3;
    return rectangleHash;
}

void physicsEngineHandler::establishConnections(bool forwardMotion){


    /**
        This function creates a list of graphs to differentiate between the different connected bodies

        Note that the objects which represent the boundaries/walls won't have any objects connected from them
        Hence, they'll be a one-off directed edge to a boundary/wall
        This is to allow multiple unconnected objects to collide with the same wall and yield the same results
        as if only one of those objects were there

    **/

    int i,j;
    vector<Graph> connections;
    int numConnections = 0;

    //We get the maximum instantiation number so they can all have a 0 or 1 value in an array
    //0 for not yet checked, 1 for checked

    //The maximum number of shapes currently in the physics engine
    int maxNum = ShapeInstantiationNumber;

    int shapesLoopedThrough[maxNum];

    //Set each element  to 0 initially, as they haven't yet been checked
    for (i = 0; i<maxNum;i++)
        shapesLoopedThrough[i] = 0;

    //Loop through all 3 shapes
    for (j = 0; j<3;j++){
        //Inner loop loops a set number of times based on the shape currently being considered
        //If j is 0 we're dealing with triangles
        //If 1 circles
        //If 2 rectangles

        int numberOfShapes;
        if (j == 0)
            numberOfShapes = numTriangles;
        else if (j == 1)
            numberOfShapes = numCircles;

        else if (j == 2)
            numberOfShapes = numRects;

        for (i = 0; i<numberOfShapes;i++){

            if (j == 2){
                if (rectangles[i]->getMass() == wallMass){
                    shapesLoopedThrough[rectangles[i]->Id()] = 1;
                    continue;

                }
            }

            int instNum;
            int hashNum;

            //Number of shapes the current object has collided with
            int shapeNumCircles;
            int shapeNumTriangles;
            int shapeNumRects;
            int numBoundaries;

            switch (j){
                case 0:
                    instNum = triangles[i]->Id();
                    hashNum = getTriangleHash(instNum);
                    shapeNumCircles = triangles[i]->numCircles;
                    shapeNumTriangles = triangles[i]->numTriangles;
                    shapeNumRects = triangles[i]->numRects;
                    numBoundaries = triangles[i]->numBoundariesCollidedWith;
                    break;

                case 1:
                    instNum = circles[i]->Id();
                    hashNum = getCircleHash(instNum);
                    shapeNumCircles = circles[i]->numCircles;
                    shapeNumTriangles = circles[i]->numTriangles;
                    shapeNumRects = circles[i]->numRects;
                    numBoundaries = circles[i]->numBoundariesCollidedWith;
                    break;

                case 2:
                    instNum = rectangles[i]->Id();
                    hashNum = getRectangleHash(instNum);
                    shapeNumCircles = rectangles[i]->numCircles;
                    shapeNumTriangles = rectangles[i]->numTriangles;
                    shapeNumRects = rectangles[i]->numRects;
                    numBoundaries = rectangles[i]->numBoundariesCollidedWith;
                    break;
            }

            if ( shapesLoopedThrough[instNum] == 1)
                continue;

            //Object has no shapes connected to it

            if (shapeNumCircles == 0 && shapeNumTriangles == 0 && shapeNumRects == 0){
                shapesLoopedThrough[instNum] = 1;
                continue;
            }

            Graph graph;
            graph.hashNum = hashNum;

            if (j == 0)
                connectionsTraversal<Triangle> (&graph,triangles[i],shapesLoopedThrough);

            else if (j == 1)
                connectionsTraversal<Circle> (&graph,circles[i],shapesLoopedThrough);

            else if (j == 2)
                connectionsTraversal<Rectangle> (&graph,rectangles[i],shapesLoopedThrough);


            connections.push_back(graph);
            numConnections++;
        }
    }


    for (i = 0; i<maxNum;i++)
        shapesLoopedThrough[i] = 0;

    for (i = 0; i<numConnections;i++){
        if (!connections[i].shouldHandleCollision && forwardMotion)
            continue;


        int pos;
        int shapeNum = connections[i].hashNum % 10;
        //instNum should be long just to ensure compute with hashNum in case its value...
        //...is bigger than 2^32
        long instNum = (connections[i].hashNum - shapeNum)/10;
            //Circle
            if (shapeNum == 1)
                locateCircle((int)(instNum),&pos);

            //Triangle
            else if (shapeNum == 2)
                locateTriangle ((int) (instNum), &pos);

            //Rectangle
            else if (shapeNum == 3)
                locateRectangle((int)(instNum),&pos);

        if (forwardMotion){

            if (connections[i].sameRestitution){
                if (shapeNum == 1)
                    handleCollisionSameResitution(circles[pos],
                                                  connections[i],
                                                  speedDecimalPlace,
                                                  shapesLoopedThrough,
                                                  maxNum);

                else if (shapeNum == 2)
                    handleCollisionSameResitution(triangles[pos],
                                                  connections[i],
                                                  speedDecimalPlace,
                                                  shapesLoopedThrough,
                                                  maxNum);

                else if (shapeNum == 3)
                    handleCollisionSameResitution(rectangles[pos],
                                                  connections[i],
                                                  speedDecimalPlace,
                                                  shapesLoopedThrough,
                                                  maxNum);
            }

            //Coefficient of restitution between objects is not the same
            else{
                if (shapeNum == 1)
                    handleCollisionDifferentRestitution(circles[pos],
                                                        connections[i],
                                                        speedDecimalPlace,
                                                        shapesLoopedThrough,
                                                        maxNum);

                else if (shapeNum == 2)
                    handleCollisionDifferentRestitution(triangles[pos],
                                                        connections[i],
                                                        speedDecimalPlace,
                                                        shapesLoopedThrough,
                                                        maxNum);
                else if (shapeNum == 3)
                    handleCollisionDifferentRestitution(rectangles[pos],
                                                        connections[i],
                                                        speedDecimalPlace,
                                                        shapesLoopedThrough,
                                                        maxNum);
            }

            CollisionInfo collision;
            collision.connection = connections[i];
            previousCollisions.push_back(collision);

        }

        //Going backwards in time
        else{
            //Here we look at the most recently detected collisions
            int i;

            for (i = previousCollisions.size()-1; i >=0; i--){
                if (connections[i].hashNum == previousCollisions[i].connection.hashNum)
                    break;
            }


            //Handle collision
            if (previousCollisions[i].counter == 0){

                if (shapeNum == 1)
                    reverseCollisions (circles[pos],shapesLoopedThrough);

                else if (shapeNum == 2)
                    reverseCollisions (triangles[pos],shapesLoopedThrough);

                else if (shapeNum == 3)
                    reverseCollisions (rectangles[pos],shapesLoopedThrough);

                previousCollisions.erase(previousCollisions.begin()+i,previousCollisions.begin()+i+1);

            }
        }

    }
}

bool physicsEngineHandler::moveObjects(bool forwardMotion){


    int i;

    //Can't go back in time if time is at 0
    if (!forwardMotion && frameNumber == 0)
        return false;

    if (!forwardMotion){
        checkCollisions(forwardMotion);
        for (i = 0;i<previousCollisions.size();i++)
            previousCollisions[i].counter -= 1;

        frameNumber -= 1;
    }


    for (i =0; i<numTriangles;i++)
        triangles[i]->delegateToMotionHandler(forwardMotion);


    for (i=0;i<numCircles;i++)
        circles[i]->delegateToMotionHandler(forwardMotion);

    for (i=0;i<numRects;i++)
        if (rectangles[i]->getMass() != wallMass){
            rectangles[i]->delegateToMotionHandler(forwardMotion);

    }


    /**
        Increase counter of previously collided objects as we've just moved
        time by one frame by moving every object
    **/
    if (forwardMotion){

        for (i = 0; i<previousCollisions.size();i++)
            previousCollisions[i].counter += 1;

        checkCollisions();

        frameNumber += 1;
    }

    return true;
}

//Defunct (this function is redundant but I kept it in anyway)
bool isCircleRectCollidedPrevious (Circle circle, Rectangle rect){
	/**This function checks if a circle has collided with a rectangle by considering the equation of
		each line of the rectangle. It checks if the equation of each line has a solution when plugged
		into the equation of the given circle. If the coordinates of this solution lie on the rectangle,
		then there is a collision**/

    struct intersectionParameter{
        double coordinate;
        double discriminant;
        double pointA;
        double pointB;
    };

    int radius = circle.getRadius();
    int radiusSquared = radius * radius;
    double circlex = circle.xPosition();
    double circley = circle.yPosition();
    double rectx = rect.xPosition();
    double recty = rect.yPosition();

    double discriminant1 = radiusSquared - (rectx- circlex)*(rectx-circlex);
    double discriminant2 = radiusSquared - (rectx + rect.getWidth() - circlex)*(rectx + rect.getWidth() - circlex);
    double discriminant3 = radiusSquared - (recty - circley)*(recty-circley);
    double discriminant4 = radiusSquared - (recty + rect.getHeight() - circley)*(recty + rect.getHeight() - circley);

    intersectionParameter intersections[4] = { {circley, discriminant1, recty, recty+rect.getHeight()},
                                               {circley, discriminant2, recty, recty+rect.getHeight()},
                                               {circlex, discriminant3, rectx, rectx+rect.getWidth()},
                                               {circlex, discriminant4, rectx, rectx+rect.getWidth()}};

    for (int i = 0; i < 4; i++){
        if (intersections[i].discriminant < 0)
            continue;

        double root = sqrt(intersections[i].discriminant);
        double solution1 = intersections[i].coordinate + root;
        double solution2 = intersections[i].coordinate - root;

        if (solution1 >= intersections[i].pointA && solution1 <= intersections[i].pointB)
            return true;

        if (solution2 >= intersections[i].pointA && solution2 <= intersections[i].pointB)
            return true;

    }

    return false;
}
}
