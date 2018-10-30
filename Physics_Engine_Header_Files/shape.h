#ifndef SHAPE_H_INCLUDED
#define SHAPE_H_INCLUDED

#include <cstdint>
#include <vector>
#include "coordinate.h"
#include "motion.h"
#include "physics_engine_global_vars.h"
#include "fraction.h"

namespace physicsEngine{

class Shape;
class objectsInCollision;
class Triangle;
class Rectangle;
class Circle;
class straightLine;
class shapeHandler;

//The base class used to store attributes common to all shapes
class Shape{

    protected:

        //Used for indexing the colour array
        enum COLOUR{
            R = 0,
            G = 1,
            B = 2,
            A = 3
        };

        //ID number of a shape
        int instantiationNumber;
        int frameRate;

        //A reference coordinate of a shape
        Coordinate referencePoint;
        Coordinate velocity;
        Coordinate acceleration;

        bool affectedByGravity;
        uint8_t colour[4];

        double mass;

        Motion xMotionHandler[2];
        Motion yMotionHandler[2];

        //Used to check if position, speed and acceleration of a shape have been established
        bool positionSetup;
        bool xSpeedSetup;
        bool ySpeedSetup;
        bool xAccelerationSetup;
        bool yAccelerationSetup;

        std::vector<double> xSpeedsPreCollision;
        std::vector<double> ySpeedsPreCollision;


    public:
        Shape (int frameR);

        bool changeFrameRate (int frameR);
        bool changeSpeedx (double x, bool collision = false);
        bool changeSpeedy (double y, bool collision = false);
        bool changeXSpeedPreCollision();
        bool changeYSpeedPreCollision();

        bool changePosition (int x, int y);
        bool changeAccelerationx (double x);
        bool changeAccelerationy (double y);
        void changeColour (uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        bool changeMass (double m);
        void gravityEffect (bool affected);


        bool delegateToMotionHandler (bool forwardMotion = true);
        void resetMotionHandler();

        //Returns values for RGBA colours
        uint8_t rValue();
        uint8_t gValue();
        uint8_t bValue();
        uint8_t aValue();

        double xPosition();
        double yPosition();
        double xSpeed();
        double ySpeed();
        bool getGravityEffect();
        fraction xSpeed_f();
        fraction ySpeed_f();
        double xAcceleration();
        double yAcceleration();
        fraction xAcceleration_f();
        fraction yAcceleration_f();
        double getMass();
        fraction getMass_f();

        //Returns the instantiation number of a shape object
        int Id();

        bool shouldHighlightShape;

};

//Class is used to handle a collection of shape objects
class shapeHandler{

    public:

        shapeHandler();

        //Subroutines to add shapes to the collection of shape objects
        bool addTriangle (Triangle* triangle);
        bool addRectangle (Rectangle* rect);

        bool addCircle (Circle* circle);

        bool removeTriangle(Triangle* triangle);
        bool removeRectangle(Rectangle* rect);
        bool removeCircle (Circle* circle);

        //These vectors store the all three shape objects
        std::vector<Rectangle*> rectangles;
        std::vector<Triangle*> triangles;
        std::vector<Circle*> circles;

        //Number of each shapes
        int numRects;
        int numTriangles;
        int numCircles;

        //Check if a specific shape is present in the collection, and if it is...
        //...place its position in the variable position
        bool locateTriangle (int instNum, int* position);
        bool locateCircle (int instNum, int* position);
        bool locateRectangle (int instNum, int* position);


};



/**
    This class is used by every derived Shape Class
    It is used so every shape contains a reference to every other shape it has collided with

**/

class objectsInCollision: public shapeHandler{

    public:

        objectsInCollision();

        //Polymorphism to deal with wall collisions
        bool addRectangle(Rectangle* rect);
        bool removeRectangle(Rectangle* rect);
        bool recentlyDetectedCollision;

        //The number of boundaries an object has collided with (at mass it would be 2)
        int numBoundariesCollidedWith;


};


class Circle: public Shape, public objectsInCollision{


    public:
        Circle (int x, int y, unsigned int r, int frameR, double m = DEFAULT_MASS);
        Coordinate centreOfMass();
        int getRadius();
        bool changeRadius (unsigned int r);

        double* restitutionRect;
        double* restitutionWall;
        double* restitutionCircle;
        double* restitutionTriangle;


    private:
        unsigned int radius;

};


/**
    This class is used to check for intersections between triangles, circles and rectangles
**/
class straightLine{

	public:

	    enum LINE_TYPE{
            horizontalLine = 1,
            verticalLine,
            diagonalLine,
            singlePoint,
	    };

        Coordinate point1;
		Coordinate point2;

		LINE_TYPE line;

		double dx;
		double dy;
		fraction yIntercept;
		fraction xIntercept;
		fraction gradient;

		void initialise (int x1, int y1, int x2, int y2);

		bool lineIntersection (straightLine otherLine, bool calledOnce = false);
        bool circleIntersection (Circle& circle);

    private:
        bool pointInRange (double x, double y, double xStart,double yStart, double xEnd, double yEnd);

};


//Class used for rectangle objects, a derived class of Shape
class Rectangle: public Shape, public objectsInCollision{

    /** If a user resizes a square the other way, I may need to let changeWidth and
        changeHeight accept negative numbers, and then change the reference point **/

    public:

        //Called when an object is called. Coordinates of top left corner and frame rate are required
        Rectangle (int topLeftx, int topLefty, int frameR,
                   int w = DEFAULT_RECT_WIDTH, int h = DEFAULT_RECT_HEIGHT,
                   double m = DEFAULT_MASS);


        int getWidth();
        int getHeight();
        bool changeWidth (unsigned int w);
        bool changeHeight (unsigned int h);
        bool changePosition (int x, int y);
        Coordinate centreOfMass();

        straightLine getLine1();
        straightLine getLine2();
        straightLine getLine3();
        straightLine getLine4();


        //This function needs to have a slight modification in Rectangle...
        //...to update line equations
        bool delegateToMotionHandler(bool forwardMotion = true);


        double* restitutionRect;
        double* restitutionWall;
        double* restitutionCircle;
        double* restitutionTriangle;

    private:
        unsigned int width,height;
        //The four line equations of the rectangle
        straightLine verticalLine1;
        straightLine verticalLine2;
        straightLine horizontalLine1;
        straightLine horizontalLine2;

        void updateLineEquations();


};

class Triangle: public Shape, public objectsInCollision{
    //Calling xPosition() or yPosition for this class will return the Coordinates of Point 1
    public:

        Triangle (int xpoint1, int ypoint1, int xpoint2, int ypoint2, int xpoint3, int ypoint3,
                  int frameR, double m = DEFAULT_MASS);

        bool changePoint1 (int x, int y);
        bool changePoint2 (int x, int y);
        bool changePoint3 (int x, int y);

        straightLine getLine1();
        straightLine getLine2();
        straightLine getLine3();

        Coordinate centreOfMass();

        double xcoordPoint1();
        double ycoordPoint1();

        double xcoordPoint2();
        double ycoordPoint2();

        double xcoordPoint3();
        double ycoordPoint3();

        //This function needs to have a slight modification in Triangle...
        //...to allow point1, point2 and point3 to be changed
        //And to update line coordinates
        bool delegateToMotionHandler(bool forwardMotion = true);

        double* restitutionRect;
        double* restitutionWall;
        double* restitutionCircle;
        double* restitutionTriangle;

    private:
        Coordinate point1;
        Coordinate point2;
        Coordinate point3;

        straightLine point1Point2Line;
        straightLine point1Point3Line;
        straightLine point2Point3Line;

        //Generic functions to avoid having to repeat code for:...
        //...changePoint1, changePoint2, changePoint3
        void changePoint (int x, int y, Coordinate* coord);
        bool checkPointValid (int x, int y);
        void updateLineEquations();
};



extern Rectangle topWall;
extern Rectangle leftWall;
extern Rectangle bottomWall;
extern Rectangle rightWall;

}
#endif // SHAPE_H_INCLUDED
