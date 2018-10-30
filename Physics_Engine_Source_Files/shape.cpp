#include <stdio.h>
#include <cmath>
#include "shape.h"
#include "binary_search_shapes.h"
#include "utility.h"

using std::abs;

namespace physicsEngine{


Shape::Shape (int frameR){

    frameRate = frameR;
    velocity.x = 0.0;
    velocity.y = 0.0;
    acceleration.x = 0.0;
    acceleration.y = 0.0;

    mass = DEFAULT_MASS;

    positionSetup = false;
    xSpeedSetup = false;
    ySpeedSetup = false;
    xAccelerationSetup = false;
    yAccelerationSetup = false;
    affectedByGravity = false;

    instantiationNumber = ShapeInstantiationNumber;

    //Shapes are white by default
    for (int i = R; i<=A;i++)
        colour[i] = 255;

    //This is incremented, so the next shape object takes the next possible value
    ShapeInstantiationNumber++;

    shouldHighlightShape = false;

}


bool Shape::changeFrameRate(int frameR){
    //If frame rate needs to be changed

    if (frameR<=0)
        return false;

    frameRate = frameR;

    /**
        If acceleration or velocity in any component is greater than current frame rate,
        then this will be capped

        The ternary operators check if the motion is greater than or
        less than 0, so they are capped accordingly

        For example, if an object had an x velocity of -75, then I change
        frame rate to 63, the velocity must be capped to -63

    **/
    if (abs(velocity.x) > frameRate){
        velocity.x = velocity.x > 0 ? frameRate:-frameRate;
    }

    if (abs(velocity.y) > frameRate){
        velocity.y = velocity.y > 0 ? frameRate:-frameRate;
    }

    if (abs(acceleration.x) > frameRate){
        acceleration.x = acceleration.x > 0 ? frameRate:-frameRate;
    }

    if (abs(acceleration.y) > frameRate){
        acceleration.y = acceleration.y > 0 ? frameRate:-frameRate;
    }

    resetMotionHandler();

    return true;
}

bool Shape::changeSpeedx (double x, bool collision){

    //If absolute value is greater than frame rate, speed will be capped to max or min value
    //The function will return false to signify the velocity was not changed as requested
    bool status = true;

    if (collision)
        xSpeedsPreCollision.push_back(velocity.x);

    if (x > frameRate){
        velocity.x = frameRate;
        status = false;
    }

    else if (x < -frameRate){
        velocity.x = -frameRate;
        status = false;
    }

    else
        velocity.x = x;

    return status;
}

bool Shape::changeSpeedy (double y, bool collision){

    bool status = true;

    if (collision)
        ySpeedsPreCollision.push_back(velocity.y);

    if (y > frameRate){
        velocity.y = frameRate;
        status = false;
    }

    else if (y < -frameRate){
        velocity.y = -frameRate;
        status = false;
    }

    else
        velocity.y = y;

    return status;
}

bool Shape::changeXSpeedPreCollision(){

    if (xSpeedsPreCollision.size() == 0)
        return false;

    velocity.x = xSpeedsPreCollision[xSpeedsPreCollision.size()-1];
    xSpeedsPreCollision.erase(xSpeedsPreCollision.end()-1,xSpeedsPreCollision.end());

    return true;
}

bool Shape::changeYSpeedPreCollision(){

    if (ySpeedsPreCollision.size() == 0)
        return false;

    velocity.y = ySpeedsPreCollision[ySpeedsPreCollision.size()-1];
    ySpeedsPreCollision.erase(ySpeedsPreCollision.end()-1,ySpeedsPreCollision.end());

    return true;

}

bool Shape::changeAccelerationx (double x){

    if (abs(x) > frameRate)
        return false;

    acceleration.x = x;

    return true;
}

bool Shape::changeAccelerationy (double y){

    if (abs(y) > frameRate)
        return false;

    acceleration.y = y;

    return true;
}

void Shape::changeColour (uint8_t r, uint8_t g, uint8_t b, uint8_t a){

    colour[R] = r;
    colour[G] = g;
    colour[B] = b;
    colour[A] = a;
}



bool Shape::changeMass (double m){
    if (m <= 0.0 || m > MAX_MASS)
        return false;

    mass = m;

    return true;
}

void Shape::gravityEffect(bool affected){

    affectedByGravity = affected;
}

bool Shape::changePosition (int x, int y){

    //Check if either coordinates lie outside boundary of physics engine
    if (x > simulationWidth/2 || x < -simulationWidth/2)
        return false;

    if (y > simulationHeight/2 || y < -simulationHeight/2)
        return false;

    referencePoint.x = x;
    referencePoint.y = y;
    positionSetup = true;

    return true;
}

double Shape::xPosition(){

    return referencePoint.x;
}

double Shape::yPosition(){

    return referencePoint.y;
}

double Shape::xSpeed(){

    if (abs(velocity.x) <= frameRate)
        return velocity.x;

    return velocity.x>0 ? frameRate:-frameRate;
}

double Shape::ySpeed(){

    if (abs(velocity.y) <= frameRate)
        return velocity.y;

    return velocity.y> 0 ? frameRate:-frameRate;
}

bool Shape::getGravityEffect(){

    return affectedByGravity;
}

fraction Shape::xSpeed_f(){

    if (abs(velocity.x) <= frameRate){
        fraction f(velocity.x);
        return f;
    }

    if (velocity.x > 0){
        fraction f (frameRate);
        return f;
    }

    else{
        fraction f(-frameRate);
        return f;
    }
}

fraction Shape::ySpeed_f(){

    if (velocity.y < frameRate){
        fraction f(velocity.y);
        return f;
    }

     if (velocity.y > 0){
        fraction f (frameRate);
        return f;
    }

    else{
        fraction f(-frameRate);
        return f;
    }
}


double Shape::xAcceleration(){
    return acceleration.x;
}

double Shape::yAcceleration(){
    return acceleration.y;
}

fraction Shape::xAcceleration_f(){

    fraction f(acceleration.x);
    return f;
}

fraction Shape::yAcceleration_f(){

    fraction f(acceleration.y);
    return f;
}

double Shape::getMass(){
    return mass;

}

fraction Shape::getMass_f(){

    fraction f(mass);

    return f;
}

int Shape::Id(){
    return instantiationNumber;
}

uint8_t Shape::rValue(){

    uint8_t x = 0;

    if (shouldHighlightShape)
        x = 100;

    return colour[R]+x;
}

uint8_t Shape::gValue(){

    uint8_t x = 0;

    if (shouldHighlightShape)
        x = 100;

    return colour[G]+x;
}

uint8_t Shape::bValue(){

    uint8_t x = 0;

    if (shouldHighlightShape)
        x = 100;

    return colour[B]+x;
}

uint8_t Shape::aValue(){

    return colour[A];
}

bool Shape::delegateToMotionHandler (bool forwardMotion){

    //Function is to handle motion for each shape
    bool status = true;

    enum MOTION{
        SPEED = 0,
        ACCELERATE = 1
    };

    if (!positionSetup)
        return false;

	/**The initialiseClass() function for a Motion object
	   requires pointer to pointers in some parameters
	**/
    double* ptrx = &referencePoint.x;
    double* ptry = &referencePoint.y;

    double* ptrVelx = &velocity.x;
    double* ptrVely = &velocity.y;

    double* ptrAcclx = &acceleration.x;
    double* ptrAccly = &acceleration.y;


    if (!xSpeedSetup){
        if (!xMotionHandler[SPEED].initialiseClass(&ptrx, &ptrVelx, frameRate, true))
            status = false;
        else
            xSpeedSetup = true;
    }

    if (!xAccelerationSetup){
        if (affectedByGravity)
            acceleration.y += gravitationalAcceleration;

        if (!xMotionHandler[ACCELERATE].initialiseClass(&ptrVelx, &ptrAcclx, frameRate, false))
            status = false;

        else
            xAccelerationSetup = true;
    }

    if (!ySpeedSetup){
        if (!yMotionHandler[SPEED].initialiseClass(&ptry,&ptrVely, frameRate,true))
            status = false;

        else
            ySpeedSetup = true;

    }

    if (!yAccelerationSetup){

        if (!yMotionHandler[ACCELERATE].initialiseClass(&ptrVely,&ptrAccly, frameRate, false))
            status = false;

        else
            yAccelerationSetup = true;
    }

    //Going forward in time
    if (forwardMotion){

        if (!xMotionHandler[ACCELERATE].moveObjectForward())
            status = false;

        if (!xMotionHandler[SPEED].moveObjectForward())
            status = false;

        if (!yMotionHandler[ACCELERATE].moveObjectForward())
            status = false;

        if (!yMotionHandler[SPEED].moveObjectForward())
            status = false;


    }

    //Going backwards in time

    else{

        if(!xMotionHandler[SPEED].moveObjectBackward())
            status = false;

        if (!xMotionHandler[ACCELERATE].moveObjectBackward())
            status = false;

        if (!xMotionHandler[SPEED].checkForChangeMovingBackwards())
            status = false;

        if(!yMotionHandler[SPEED].moveObjectBackward())
            status = false;

        if (!yMotionHandler[ACCELERATE].moveObjectBackward())
            status = false;

        if (!yMotionHandler[SPEED].checkForChangeMovingBackwards())
            status = false;
    }

    return status;

}

void Shape::resetMotionHandler(){

    //This is called when restarting a simulation

    if (xSpeedSetup){
        xMotionHandler[0].resetAttributes();
        xSpeedSetup = false;
    }

    if (ySpeedSetup){
        yMotionHandler[0].resetAttributes();
        ySpeedSetup = false;
    }

    if (xAccelerationSetup){
        xMotionHandler[1].resetAttributes();
        xAccelerationSetup = false;
    }

    if (yAccelerationSetup){
        yMotionHandler[1].resetAttributes();
        yAccelerationSetup = false;
    }

    xSpeedsPreCollision.clear();
    ySpeedsPreCollision.clear();


}

//The constructor of Shape is called first
Rectangle::Rectangle (int topLeftx, int topLefty, int frameR,
					  int w, int h, double m):Shape(frameR),objectsInCollision(){

    width = w;
    height = h;
    changePosition(topLeftx,topLefty);

    restitutionCircle = &circleToRectangleRestitution;
    restitutionRect = &rectangleTorectangleRestitution;
    restitutionWall = &rectangleToBoundaryRestitution;
    restitutionTriangle = &rectangleToTriangleRestitution;


    mass = m;
    updateLineEquations();

}


void Rectangle::updateLineEquations(){

    int topLeftx = convertIntDouble(referencePoint.x);
    int topLefty = convertIntDouble(referencePoint.y);

    //Set up the four line equations of the rectangle
    verticalLine1.initialise(topLeftx,topLefty,topLeftx,topLefty-height);
    verticalLine2.initialise (topLeftx+width,topLefty,topLeftx+width,topLefty-height);
    horizontalLine1.initialise(topLeftx,topLefty,topLeftx+width,topLefty);
    horizontalLine2.initialise(topLeftx,topLefty-height,topLeftx+width,topLefty-height);

}

bool Rectangle::changePosition(int x, int y){

    if (!Shape::changePosition(x,y))
        return false;


    updateLineEquations();

}

int Rectangle::getWidth(){

    return width;
}

int Rectangle::getHeight(){

    return height;
}

bool Rectangle::changeWidth(unsigned int w){

    if (referencePoint.x + w > simulationWidth)
        return false;

    width = w;

    updateLineEquations();
    return true;
}

bool Rectangle::changeHeight(unsigned int h){

    if (referencePoint.y + h > simulationHeight)
        return false;

    height = h;
    updateLineEquations();

    return true;
}

bool Rectangle::delegateToMotionHandler(bool forwardMotion){

    double currentx = referencePoint.x;
    double currenty = referencePoint.y;

    //We extend the function from Shape to allow point1, point2 and point3 to be changed
    if (!Shape::delegateToMotionHandler(forwardMotion))
        return false;


    if (currentx != referencePoint.x || currenty != referencePoint.y)
        updateLineEquations();

    return true;

}

Coordinate Rectangle::centreOfMass(){

    Coordinate com;

    // centre of mass of a rectangle is calculated by adding the coordinates of the corners and dividing by 2
    com.x = (referencePoint.x*2 + width)/2.0;
    com.y = (referencePoint.y*2 + height)/2.0;

    return com;

}

straightLine Rectangle::getLine1(){
    return verticalLine1;
}

straightLine Rectangle::getLine2(){
    return verticalLine2;
}

straightLine Rectangle::getLine3(){
    return horizontalLine1;
}

straightLine Rectangle::getLine4(){
    return horizontalLine2;
}


Triangle::Triangle(int xpoint1, int ypoint1, int xpoint2,
                   int ypoint2, int xpoint3, int ypoint3, int frameR,
				   double m):Shape(frameR),objectsInCollision(){

    changePosition(xpoint1,ypoint1);
    changePoint (xpoint1,ypoint1,&point1);
    changePoint (xpoint2,ypoint2,&point2);
    changePoint (xpoint3,ypoint3,&point3);
    updateLineEquations();

    mass = m;

    restitutionCircle = &circleToTriangleRestitution;
    restitutionRect = &rectangleToTriangleRestitution;
    restitutionWall = &triangleToBoundaryRestitution;
    restitutionTriangle = &triangleToTriangleRestitution;

}

void Triangle::updateLineEquations(){

    int x1 = convertIntDouble (point1.x);
    int y1 = convertIntDouble (point1.y);

    int x2 = convertIntDouble(point2.x);
    int y2 = convertIntDouble(point2.y);

    int x3 = convertIntDouble(point3.x);
    int y3 = convertIntDouble(point3.y);

    point1Point2Line.initialise(x1,y1,x2,y2);
    point1Point3Line.initialise(x1,y1,x3,y3);
    point2Point3Line.initialise(x2,y2,x3,y3);
}

Coordinate Triangle::centreOfMass(){

    Coordinate com;

    com.x = (point1.x + point2.x + point3.x)/3;
    com.y = (point1.y + point2.y + point3.y)/3;

    return com;
}

void Triangle::changePoint (int x, int y, Coordinate* coord){

    coord->x = x;
    coord->y = y;
}

bool Triangle::checkPointValid(int x, int y){

    //Calls changePosition function defined in Shape class
    if (!changePosition(x,y))
        return false;

    //If changePosition is true, these referencePoint would have been changed to x and y
    //Here, we change it back again
    referencePoint.x = point1.x;
    referencePoint.y = point1.y;

    return true;
}

bool Triangle::changePoint1 (int x, int y){


    if (!checkPointValid(x,y))
        return false;

   changePoint(x,y,&point1);
   referencePoint.x = point1.x;
   referencePoint.y = point1.y;

   updateLineEquations();
   return true;

}

bool Triangle::changePoint2 (int x, int y){

    if (!checkPointValid(x,y))
        return false;

   changePoint(x,y,&point2);

   updateLineEquations();
   return true;
}

bool Triangle::changePoint3 (int x, int y){


    if (!checkPointValid(x,y))
        return false;

   changePoint(x,y,&point3);

   updateLineEquations();
   return true;

}

double Triangle::xcoordPoint1(){

    return point1.x;
}

double Triangle::ycoordPoint1(){

    return point1.y;
}


double Triangle::xcoordPoint2(){

    return point2.x;
}

double Triangle::ycoordPoint2(){

    return point2.y;
}

double Triangle::xcoordPoint3(){

    return point3.x;
}

double Triangle::ycoordPoint3(){

    return point3.y;
}

straightLine Triangle::getLine1(){

    return point1Point2Line;
}

straightLine Triangle::getLine2(){

    return point1Point3Line;
}

straightLine Triangle::getLine3(){

    return point2Point3Line;
}


bool Triangle::delegateToMotionHandler(bool forwardMotion){

    //We extend the function from Shape to allow point1, point2 and point3 to be changed
    if (!Shape::delegateToMotionHandler(forwardMotion))
        return false;

    if (point1.x != referencePoint.x || point1.y != referencePoint.y){

        //point1, point2 and point3 will also be changed by dx and dy
        int dx = referencePoint.x - point1.x;
        int dy = referencePoint.y - point1.y;

        point1.x += dx;
        point1.y += dy;

        point2.x += dx;
        point2.y += dy;

        point3.x += dx;
        point3.y += dy;

    }

    updateLineEquations();

    return true;

}

Circle::Circle(int centrex, int centrey, unsigned int r, int frameR, double m): Shape(frameR),objectsInCollision(){

    changePosition (centrex, centrey);
    radius = r;


    mass = m;

    restitutionCircle = &circleToCircleRestitution;
    restitutionRect = &circleToRectangleRestitution;
    restitutionWall = &circleToBoundaryRestitution;
    restitutionTriangle = &circleToBoundaryRestitution;

}

Coordinate Circle::centreOfMass(){

    Coordinate com;
    com.x = referencePoint.x;
    com.y = referencePoint.y;

    return com;
}

int Circle::getRadius(){

    return radius;
}

bool Circle::changeRadius (unsigned int r){

    //If new radius would cause shape to be out of bounds
    if (referencePoint.x + r > simulationWidth/2 || referencePoint.x - r < -simulationWidth/2)
        return false;

    if (referencePoint.y + r > simulationHeight/2 || referencePoint.y - r < -simulationHeight/2 )
        return false;

    radius = r;

    return true;

}

void straightLine::initialise (int x1, int y1, int x2, int y2){
    /**Function takes two points on a line, and then determines
       the type of line that is given.

       If the difference in x coordinates AND y coordinates is given,
       we do not have a line, and we simply have a point.

       If the difference in x is zero, but the difference in y is not zero, then we have a vertical line
       If the difference in y is zero but the difference in x is not zero, then we have a horizontal line

       If both the difference in and y are not zero, then we have a straight line in the form y = mx + c

    **/

	dx = x1 - x2;
	dy = y1 - y2;

	point1.x = x1;
	point1.y = y1;
	point2.x = x2;
	point2.y = y2;

	if (dy == 0 && dx == 0){

        line = singlePoint;
		return;
	}

	//Line is vertical
	else if (dx == 0 && dy != 0){
		xIntercept.numerator(x1);
		xIntercept.denominator(1);
		line = verticalLine;

	}

	//Line is horizontal
	else if (dy == 0 && dx != 0){
		yIntercept.numerator(y1);
		yIntercept.denominator(1);
		line = horizontalLine;
	}

	//Straight line of form y = mx + c
	else if (dy !=0 && dx != 0){

        //We need to get the y-Intercept, which is y - mx
        //We do this by substituting point1.x and point1.y into y = mx + c

		gradient.numerator(dy);
		gradient.denominator(dx);

		fraction gradientNegative = gradient*-1;
		fraction mxNegative = gradientNegative*point1.x;

		yIntercept = mxNegative + point1.y;

		xIntercept = ( (yIntercept*-1)+point1.y)/gradient;

		line = diagonalLine;
	}
}

bool straightLine::lineIntersection (straightLine otherLine, bool calledOnce){

	/**Function Checks if a  line intersects with another line, within the range of the points.

       These are the following types of lines that can be in contact:

            1. Horizontal-Horizontal
            2. Horizontal-Vertical
            3. Horizontal- single point
            4. Vertical- single point
            5. Diagonal- horizontal
            6. Diagonal- vertical
            7. Diagonal - diagonal
            8. Diagonal- single point

        As you can see however, the above eight are "combinations"
        and not "permutations". The below code may not work in some instances if we didn't use recursion.
        For instance, Vertical-Diagonal isn't a combination that is explicitly stated. But if I were
        to explicitly state the other 8 combinations, this would be redundant.

        So to avoid redundancy, if a combination isn't found in the current function call,
        the current function gets called one more time in the perspective of the object
        otherLine, and instead passing the instance of the current object as the first parameter,
        with calledOnce set as true. This will allow for the other 8 combinations to be checked,
        without having to repeat any code.

    **/

    //These are the intersection points of the two lines if they meet
    double decimalyIntercept;
    double decimalxIntercept;
    bool discreetIntersectionFound = false;
    bool continuousIntersectionFound = false;

	if (line == horizontalLine){

        /**
        If the line of the current object is horizontal, and the other line
        is also horizontal, then the only way they can intersect is if they are
        exactly the same line
        **/
        if (otherLine.line == horizontalLine){

            //
            if (yIntercept == otherLine.yIntercept)
                continuousIntersectionFound = true;

            else
                return false;
        }

        /**
            If the line of the current object is horizontal and the other
            line is a vertical line, then their intersection point
            is simply the y intercept of the current object and the x intercept of
            the other object
        **/

        else if (otherLine.line == verticalLine){

            decimalyIntercept = yIntercept.decimalVersion();
            decimalxIntercept = otherLine.xIntercept.decimalVersion();
            discreetIntersectionFound = true;
        }

        /**
        If the other line is a single point then there is only an
        intersection point if the y coordinate of that line is the y
        intercept of the horizontal line
        **/
        else if (otherLine.line == singlePoint){

            if (otherLine.point1.y == point1.y){
                decimalyIntercept = otherLine.point1.y;
                decimalxIntercept = otherLine.point1.x;
                discreetIntersectionFound = true;
            }

            else
                return false;
        }
	}

	else if (line == verticalLine){

        /**
           Like with two horizontal lines, two vertical lines only intersect
           if they are exactly the same line
        **/

        if (otherLine.line == verticalLine){

            if (xIntercept == otherLine.xIntercept)
                continuousIntersectionFound = true;

            else
                return false;
        }


        else if (otherLine.line == singlePoint){

            if (otherLine.point1.x == point1.x){
                decimalxIntercept = point1.x;
                decimalyIntercept = otherLine.point1.y;

                discreetIntersectionFound = true;
            }

            else
                return false;
        }
	}


	else if (line == diagonalLine){

        /**
            If the object is a diagonal line of the form y = mx + c
            and the other line is a horizontal line of the form y = b,
            we can see that the intersection point of the x coordinate
            is (b-c)/m
        **/

        if (otherLine.line == horizontalLine){

            fraction intersection = ((yIntercept*-1)+otherLine.yIntercept)/gradient;
            decimalxIntercept = intersection.decimalVersion();
            decimalyIntercept = otherLine.yIntercept.decimalVersion();

            discreetIntersectionFound = true;
        }


        /**
            If the other line is a vertical line of the form x = b,
            then the y intersection point will be y = mb + c
        **/
        else if (otherLine.line == verticalLine){

            fraction intersection = (gradient*otherLine.xIntercept)+yIntercept;
            decimalxIntercept = otherLine.xIntercept.decimalVersion();
            decimalyIntercept = intersection.decimalVersion();

            discreetIntersectionFound = true;
        }


        else if (otherLine.line == diagonalLine){

            //Checks if the two lines have the same gradient and y intercept
            //If they do, they are the same line
            if (otherLine.gradient == gradient){
                if (otherLine.yIntercept == yIntercept){
                    continuousIntersectionFound = true;
                }

                //Two lines that are parallel and don't have same y intercept do not meet
                else
                    return false;
            }

            else{

                /**
                    The point of x intersection of two lines
                    of y = mx + c can be computed by the formula
                    x = c2 - c1 / m1 - m2

                    where c1 and c2 are the y-intercepts of the two lines respectively
                    and m1 and m2 are the gradients of the two lines respectively
                **/

                fraction m1Minusm2 = gradient - otherLine.gradient;
                fraction c2Minusc1 = otherLine.yIntercept - yIntercept;

                fraction xIntersection = c2Minusc1 / m1Minusm2;
                fraction yIntersection = gradient*xIntersection + yIntercept;

                decimalxIntercept = xIntersection.decimalVersion();
                decimalyIntercept = yIntersection.decimalVersion();

                discreetIntersectionFound = true;

            }

        }

        /**
        If the other line is a single point, we simply check if it lies on the
        diagonal line, by plugging in its x coordinate into y = mx+c,
        and comparing the computed value with its y coordinate. If they are the same,
        then the point lies on the line. Otherwise it doesn't
        **/
        else if (otherLine.line == singlePoint){

            fraction intersection = gradient*otherLine.point1.x + yIntercept;

            if (intersection == otherLine.point1.y){
                decimalyIntercept = intersection.decimalVersion();
                decimalxIntercept = otherLine.point1.x;

                discreetIntersectionFound = true;
            }

            else
                return false;
        }

	}

	else if (line == singlePoint){

        if (otherLine.line == singlePoint){

            if (point1.x == otherLine.point1.x && point1.y == otherLine.point1.y)
                return true;

            else
                return false;
        }
	}

	/**
        At this stage, we need to check if decimalxIntercept and decimalyIntercept
        both lie within range of the points of both lines

        This is because the lines given will be the lines of rectangles
        and triangles, which will not be infinite in size. Therefore,
        while the equations of the lines may appear to have intersections,
        the lines themselves may not actually meet if they are too small.
	**/

	if (discreetIntersectionFound){
        //Check if coordinates of intersection are within range of the given lines
       if (pointInRange(decimalxIntercept,decimalyIntercept,point1.x, point1.y,
           point2.x,point2.y)){

            if (pointInRange(decimalxIntercept,decimalyIntercept,
                otherLine.point1.x,otherLine.point1.y,otherLine.point2.x,otherLine.point2.y))
                    return true;
           }


        return false;

	}

	else if (continuousIntersectionFound){

        if (pointInRange (point2.x,point2.y, otherLine.point1.x,otherLine.point1.y,
                          otherLine.point2.x,otherLine.point2.y))
            return true;

        if (pointInRange(otherLine.point2.x,otherLine.point2.y,
                         point1.x,point1.y,point2.x,point2.y))
            return true;


        return false;

	}

	else{

        if (!calledOnce){
            //Function is called one more time, this time swapping the two lines...
            //...so other 8 permutations can be checked
            return otherLine.lineIntersection(*this,true);
        }

        else
            return false;
	}


}

bool straightLine::pointInRange (double x, double y, double xStart,
                                 double yStart, double xEnd, double yEnd){

    /**
        This function checks whether a given coordinate is within range of two coordinates
        Used to check if an intersection point actually lies within the finite range
        of a given line
    **/

    double greaterPointx, lesserPointx;
    double greaterPointy, lesserPointy;

    if (xStart >= xEnd){
        greaterPointx = xStart;
        lesserPointx = xEnd;
    }
    else{

        greaterPointx = xEnd;
        lesserPointx = xStart;
    }

    if (yStart >= yEnd){
        greaterPointy = yStart;
        lesserPointy = yEnd;
    }
    else{

        greaterPointy = yEnd;
        lesserPointy = yStart;
    }

    if (x >= lesserPointx && x <= greaterPointx)
        if (y >= lesserPointy && y <= greaterPointy)
            return true;

    return false;

}

bool straightLine::circleIntersection (Circle& circle){
	//Checks if collision with circle

	int radius = circle.getRadius();
	int radiusSquared = radius*radius;
	int circlex = circle.xPosition();
	int circley = circle.yPosition();

	/**Simplyfy the horizontal line and vertical code by making a generic function**/
	if (line == horizontalLine){

        /**
        If the line in question is horizontal, then we need to
        check the x coordinates of the point of intersection with
        the circle in question

        A circle has equation in form (x-a)^2 + (y-b)^2 = r^2
        Where (a,b) is the centre of the circle, and r is the radius

        To check if the horizontal line has points of intersection with the circle,
        we rearrange the equation to x = a +/- (r^2 - (y-b)^2)^0.5

        We first check if r^2 - (y-b)^2 >= 0, otherwise we cannot square root it
        Then we check if the x coordinates of intersection lie within the range of the
        line in question, as the line is not infinite

        Refer to design for more info
        ***/


		double discriminant = radiusSquared - (point1.y - circley)*(point1.y-circley);

		if (discriminant < 0)
			return false;

		double root = sqrt(discriminant);
		double solution1 = circlex + root;
		double solution2 = circlex - root;

		if (pointInRange(solution1,point1.y,point1.x,point1.y,point2.x,point2.y))
            return true;

        if (pointInRange(solution2,point1.y,point1.x,point1.y,point2.x,point2.y))
            return true;

	}

	//Vertical line
	else if (line == verticalLine){
        //Here we do exactly the same thing as above, except with a vertical line

		double discriminant = radiusSquared - (point1.x - circlex)*(point1.x-circlex);

		if (discriminant < 0)
			return false;

		double root = sqrt(discriminant);

		double solution1 = circley + root;
		double solution2 = circley - root;

		if (pointInRange(point1.x,solution1,point1.x,point1.y,point2.x,point2.y))
            return true;

        if (pointInRange(point1.x,solution2,point1.x,point1.y,point2.x,point2.y))
            return true;

	}

	//Straight line of form y = mx + c
	else if (line == diagonalLine){
        /**
            Here, we plug in the straight line equation of y = mx +c
            into the equation of a circle and check for solutions

            Refer to design for full equations
        **/

		fraction a = (gradient*gradient)+1;
		fraction b = ((gradient*2)*(yIntercept-circley))- (circlex*2);
		fraction c = ( ((yIntercept*-1)+circley)*((yIntercept*-1)+circley)) + (circlex*circlex - radiusSquared);
		double bDecimal = b.decimalVersion();
		double aDecimal = a.decimalVersion();
		double cDecimal = c.decimalVersion();

		//b^2-4ac
		fraction discriminant = (b*b) - ( (a*c)*4);
		double discriminantDecimal = discriminant.decimalVersion();

		if (discriminantDecimal < 0)
			return false;

		double solution1 = (-bDecimal + sqrt(discriminantDecimal))/(2*aDecimal);
		double solution2 = (-bDecimal - sqrt(discriminantDecimal))/(2*aDecimal);

		double greaterPoint, lesserPoint;

		if (point1.x >= point2.x){

			greaterPoint = point1.x;
			lesserPoint = point2.x;
		}

		else{

			greaterPoint = point2.x;
			lesserPoint = point1.x;
		}

		if (solution1 >= lesserPoint && solution1 <= greaterPoint)
			return true;

		if (solution2 >= lesserPoint && solution2 <= greaterPoint)
			return true;

	}

	return false;
}

shapeHandler::shapeHandler(){

    numRects = 0;
    numTriangles = 0;
    numCircles = 0;
}

bool shapeHandler::locateTriangle (int instNum, int* position){

  return locateShape (triangles,instNum,numTriangles,position);

}

bool shapeHandler::locateCircle(int instNum, int* position){

    return locateShape (circles,instNum,numCircles,position);
}

bool shapeHandler::locateRectangle(int instNum, int *position){

    return locateShape (rectangles,instNum,numRects,position);
}

bool shapeHandler::addTriangle(Triangle* triangle){

    if (locateTriangle(triangle->Id(),nullptr))
        return false;

    triangles.push_back(triangle);
    numTriangles++;

    return true;
}

bool shapeHandler::removeTriangle(Triangle* triangle){

    int position;
    if (!locateTriangle(triangle->Id(),&position))
        return false;

    triangles.erase (triangles.begin()+position,triangles.begin()+position+1);
    numTriangles--;

    return true;

}

bool shapeHandler::addRectangle(Rectangle* rect){

    if (locateRectangle(rect->Id(),nullptr))
        return false;

    rectangles.push_back(rect);
    numRects++;

    return true;
}

bool shapeHandler::removeRectangle(Rectangle* rect){

    int position;


    if (!locateRectangle(rect->Id(),&position))
        return false;


    rectangles.erase (rectangles.begin()+position,rectangles.begin()+position+1);
    numRects--;

    return true;
}

bool shapeHandler::addCircle(Circle* circle){

    if (locateCircle(circle->Id(),nullptr))
        return false;

    circles.push_back(circle);
    numCircles++;

    return true;
}

bool shapeHandler::removeCircle(Circle* circle){

    int position;

    if (!locateCircle(circle->Id(),&position))
        return false;

    circles.erase (circles.begin()+position,circles.begin()+position+1);
    numCircles--;

    return true;
}

objectsInCollision::objectsInCollision():shapeHandler(){

    recentlyDetectedCollision = false;
    numBoundariesCollidedWith = 0;
}

bool objectsInCollision::addRectangle(Rectangle* rect){

    bool status = shapeHandler::addRectangle(rect);

    if (status){
        if (rect->getMass() == wallMass)
            numBoundariesCollidedWith++;
    }

    return status;
}

bool objectsInCollision::removeRectangle(Rectangle* rect){
    bool status = shapeHandler::removeRectangle(rect);

    if (status){
        if (rect->getMass() == wallMass)
            numBoundariesCollidedWith--;
    }
    return status;
}


//The rectangle objects used to represent the boundaries of the simulation
//All coordinates handled by the physics engine are standard cartesian coordinates
//The walls will be black
Rectangle topWall(-simulationWidth/2,simulationHeight/2,DEFAULT_FRAME_RATE,
                   simulationWidth, WALL_THICKNESS, wallMass);


Rectangle leftWall(-simulationWidth/2,(simulationHeight/2)-WALL_THICKNESS,
                   DEFAULT_FRAME_RATE,WALL_THICKNESS,
                   simulationHeight-WALL_THICKNESS, wallMass);

Rectangle bottomWall ((-simulationWidth/2)+WALL_THICKNESS,
                      (-simulationHeight/2)+WALL_THICKNESS,
                      DEFAULT_FRAME_RATE,
                      simulationWidth-WALL_THICKNESS,
                      WALL_THICKNESS, wallMass);

Rectangle rightWall ((simulationWidth/2)-WALL_THICKNESS,
                     (simulationHeight/2)-WALL_THICKNESS,DEFAULT_FRAME_RATE,
                      WALL_THICKNESS,simulationHeight-2*WALL_THICKNESS, wallMass);
}
