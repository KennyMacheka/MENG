#include <stdio.h>
#include <vector>
#include <iostream>
#include "math_utilities.h"
#include "pixelMovement.h"
#include "fraction.h"
#include "matrix.h"
#include "motion.h"
#include "shape.h"
#include "collisionDetection.h"
#include "database.h"
#include "colourHashing.h"

using std::vector;
using namespace physicsEngine;

void pixelMovementTests (int frameRate, double speed){

    int numLess;
    int numMore;
    int numLessOccurrences;
    int numMoreOccurrences;

    bool result = pixelAverage (frameRate,speed, &numLess, &numMore, &numLessOccurrences, &numMoreOccurrences);

    if (result)
        printf ("Floor of speed: %d. Number of times to go at this speed: %d\n"
            "Ceil of speed: %d. Number of times to go at this speed: %d\n\n\n", numLess, numLessOccurrences, numMore, numMoreOccurrences);

    else
        printf ("The function returned false\n");
}

void gcd_t (int m ,int n){



    int val = gcd (m,n);

    printf ("Highest common factor of %d and %d is %d\n",m,n, val);
}

void isInteger_t(double m){

    bool result = isInteger(m);

    if (result)
        printf ("Value is integer");

    else
        printf ("Value is not an integer");
}

void fraction_t (){

    /**Test 1: initialise a fraction with values and return these values.**/
    fraction frac1 (5,6);
    //printf ("Numerator of frac1: %d. Denominator of frac1: %d\n", frac1.numerator(), frac1.denominator());

    /**Test 2: get decimal value of a fraction **/
    //printf ("Decimal value of frac1: %.4f", frac1.decimalVersion());

    /**Test 3: Simplification**/
    fraction frac2 (5,10);
    //printf ("Simplified numerator: %d. Simplified denominator: %d ",frac2.numerator(),frac2.denominator());


    /**Test 4 addition**/
    //fraction frac3 = frac1 + 1;
    //printf ("5/6 + 1 gives: %d/%d \n", frac3.numerator(),frac3.denominator());
    //frac3 = frac1 + frac2;
    //printf("5/6 + 5/10 gives: %d/%d \n", frac3.numerator(),frac3.denominator());

    /**Test 5 subtraction**/
    //fraction frac3 = frac1 - 1;
    //printf ("5/6 - 1 gives: %d/%d \n", frac3.numerator(),frac3.denominator());
    //frac3 = frac1 - frac2;
    //printf("5/6 - 5/10 gives: %d/%d \n", frac3.numerator(),frac3.denominator());

    /**Test 6 multiplication**/
    //fraction frac3 = frac1 * 1;
    //printf ("5/6 * 1 gives: %d/%d \n", frac3.numerator(),frac3.denominator());
    //frac3 = frac1 * frac2;
    //printf("5/6 * 5/10 gives: %d/%d \n", frac3.numerator(),frac3.denominator());

    /**Test 7 division**/
    //fraction frac3 = frac1-1;
    //printf ("5/6 -- 1 gives: %d/%d \n", frac3.numerator(),frac3.denominator());
    //frac3 = frac1 / frac2;
    //printf("5/6 * 5/10 gives: %d/%d \n", frac3.numerator(),frac3.denominator());

    /**Test 8 append addition**/
    //frac1 += 1;
    //printf ("New numerator of frac1: %d. New denominator of frac1: %d\n",frac1.numerator(), frac1.denominator());

    /**Test 9 append subtraction**/
    frac1 -= 2;
    printf("New value of frac1: %d/%d\n",frac1.numerator(),frac1.denominator());
    //Reset for the next fraction example
    frac1 += 2;
    frac1 -= frac2;
    printf("New value of frac1: %d/%d\n",frac1.numerator(),frac1.denominator());


    fraction frac5 (5.55);
    printf ("Num: %d/%d\n",frac5.numerator(),frac5.denominator());


}

void matrix_t(){
    /*
    int matrix3x3 [9] = {-1,1,0,0,-1,1,10,5,4};

    vector2d matrix;
    vector2d inverseM;

    for (int i = 0; i< 3; i++){
        vector<fraction> e;
        matrix.push_back(e);
        inverseM.push_back(e);
    }

    for (int i = 0; i<9;i++){
        fraction frac(matrix3x3[i],1);
        matrix[i/3].push_back(frac);
        inverseM[i/3].push_back(frac);
    }

    inverse (3,matrix,inverseM);
    for (int i = 0; i<3; i++){
        for (int j = 0; j < 3; j++){
            printf("%2d/%2d ", inverseM[i][j].numerator(),
                    inverseM[i][j].denominator());
        }
        printf("\n");
    }

    */
    /**
    int matrix2x2[4] = {10,7,-5,4};
    vector2d matrix;
    vector2d inverseM;

    for (int i = 0; i< 2; i++){
        vector<fraction> e;
        matrix.push_back(e);
        inverseM.push_back(e);
    }

    for (int i = 0; i<4;i++){
        fraction frac(matrix2x2[i],1);
        matrix[i/2].push_back(frac);
        inverseM[i/2].push_back(frac);
    }

    inverse (2,matrix,inverseM);
    for (int i = 0; i<2; i++){
        for (int j = 0; j < 2; j++){
            printf("%3lld/%3lld ", inverseM[i][j].numerator(),
                    inverseM[i][j].denominator());
        }
        printf("\n");
    }
    **/


    /**
    int matrix4x4[16] = {1,-1,0,0,3,4,2,1,-1,2,3,7,0,-1,6,5 };
    vector2d matrix;
    vector2d inverseM;
    //Works
    for (int i = 0; i< 4; i++){
        vector<fraction> e;
        matrix.push_back(e);
        inverseM.push_back(e);
    }

    for (int i = 0; i<16;i++){
        fraction frac(matrix4x4[i],1);
        matrix[i/4].push_back(frac);
        inverseM[i/4].push_back(frac);
    }

    inverse (4,matrix,inverseM);
    for (int i = 0; i<4; i++){
        for (int j = 0; j < 4; j++){
            printf("%3lld/%3lld ", inverseM[i][j].numerator(),
                    inverseM[i][j].denominator());
        }
        printf("\n");
    }

    **/

    /**
    int matrix2x2[4] = {10,0,0,0};
    vector2d matrix;
    vector2d inverseM;

    for (int i = 0; i< 2; i++){
        vector<fraction> e;
        matrix.push_back(e);
        inverseM.push_back(e);
    }

    for (int i = 0; i<4;i++){
        fraction frac(matrix2x2[i],1);
        matrix[i/2].push_back(frac);
        inverseM[i/2].push_back(frac);
    }

    printf ("Function returned: %s\n",
            inverse (2,matrix,inverseM) == true?
            "True":"False");*/



    int matrix3x3[9] = {5,5,5,1,1,1,0,0,0};
    vector2d matrix;
    vector2d inverseM;

    for (int i = 0; i< 3; i++){
        vector<fraction> e;
        matrix.push_back(e);
        inverseM.push_back(e);
    }

    for (int i = 0; i<9;i++){
        fraction frac(matrix3x3[i],1);
        matrix[i/3].push_back(frac);
        inverseM[i/3].push_back(frac);
    }

    printf ("Function returned: %s\n",
            inverse (3,matrix,inverseM) == true?
            "True":"False");



}

void motion_frame_test(){

    int frame = 60;
    double* speed = new double;
    double* position = new double;
    double* acceleration = new double;

    *speed = 10;
    *position = 0.0;
    *acceleration = 59;

    /**General motion with acceleration
    Motion velocity;
    Motion acc;

    velocity.initialiseClass(&position,&speed,frame, true );
    acc.initialiseClass(&speed,&acceleration,frame,false);
    printf("Starting position: %.1f. Speed: %.1f\n",*position,*speed);
    //printf(" Velocity speed: %.1f. Velocity countdown: %d", *speed,velocity.pixelCountdown);
    //printf(" Acceleration countdown: %d\n", acc.pixelCountdown);
    for (int i = 0; i < 60; i++){

        //if (i == 0)
            //printf("Frame: %d. Pixel Per frame: %d. Pixel countdown: %d. Speed: %.1f. Also speed: %.1f\n",i,velocity.pixelPerFrames,velocity.pixelCountdown,velocity.motionPerSec,*speed);
        acc.moveObjectForward();
        velocity.moveObjectForward();

        //printf("Frame %d. Starting position: %.1f",i+1,*position);
        //printf(" Velocity speed: %.1f. Velocity countdown: %d", *speed,velocity.pixelCountdown);
        //printf(" Acceleration countdown: %d\n", acc.pixelCountdown);
        //printf("Frame: %d. Pixel Per frame: %d. Pixel countdown: %d. Speed: %.1f. Also speed: %.1f\n",i+1,velocity.pixelPerFrames,velocity.pixelCountdown,velocity.motionPerSec,*speed);

    }


    printf ("Position of object: %.1f. Speed: %.1f\n", *position, *speed);
    printf("Going backwards: \n\n");

    for (int j = 59; j>=0;j--){
        //if (j == 60)
            //printf("Frame start . Pixel Per frame: %d. Pixel countdown: %d. Speed: %.1f. Also speed: %.1f\n",velocity.pixelPerFrames,velocity.pixelCountdown,velocity.motionPerSec,*speed);
        velocity.moveObjectBackward();
        acc.moveObjectBackward();
        velocity.checkForChangeMovingBackwards();

        //printf("Frame %d. Starting position: %.1f",j,*position);
        //printf(" Velocity speed: %.1f. Velocity countdown: %d", *speed,velocity.pixelCountdown);
        //printf(" Acceleration countdown: %d\n", acc.pixelCountdown);

        //printf("Frame: %d. Pixel Per frame: %d. Pixel countdown: %d. Speed: %.1f. Also speed: %.1f\n",j,velocity.pixelPerFrames,velocity.pixelCountdown,velocity.motionPerSec,*speed);

    }


    printf("New position of object: %.1f. Speed is: %.1f\n\n",*position,*speed);

    **/

    /**Testing a general shape object

    Shape shape(60);
    shape.changePosition(0,0);
    shape.changeSpeedx(10);
    shape.changeSpeedy(10);
    shape.changeAccelerationx(59);
    shape.changeAccelerationy(59);

    printf("Shape object. Position x: %d. Position y: %d\n", shape.xPosition(),shape.yPosition());

    for (int i = 0; i<60;i++){
        if(!shape.delegateToMotionHandler())
            printf("Returned false\n");

    }

    printf("New position x: %.1f. New position y: %.1f. Speed x: %.1f\n", shape.xPosition(),shape.yPosition(),shape.xSpeed());

    printf("Going backwards\n");

    for (int j = 59; j >= 0; j--){
        if(!shape.delegateToMotionHandler(false))
            printf("Returned false\n");

    }

    printf("New position x: %.1f. New position y: %.1f\n", shape.xPosition(), shape.yPosition());

    **/

    /**Rectangle motion
    printf("\n\nUsing the rectangle class: ");
    Rectangle rectangle (0,0,60);

    rectangle.changeSpeedx(10);
    rectangle.changeSpeedy(10);
    rectangle.changeAccelerationx(59);
    rectangle.changeAccelerationy(59);

    printf("Rectangle object. Position x: %d. Position y: %d\n", rectangle.xPosition(),rectangle.yPosition());


        for (int i = 0; i<60;i++){
        if(!rectangle.delegateToMotionHandler())
            printf("Returned false\n");

    }

    printf("New position x: %.1f. New position y: %.1f. Speed x: %.1f\n", rectangle.xPosition(),rectangle.yPosition(),rectangle.xSpeed());

    printf("Going backwards\n");

    for (int j = 59; j >= 0; j--){
        if(!rectangle.delegateToMotionHandler(false))
            printf("Returned false\n");

    }

    printf("New position x: %.1f. New position y: %.1f\n", rectangle.xPosition(), rectangle.yPosition());

    **/


    /**
    printf ("\n\nNew object: Triangle\n");

    Triangle tri (0,20,0,10,10,0,60);
    tri.changeSpeedx(10);
    tri.changeSpeedy(10);


    tri.changeAccelerationx(59);
    tri.changeAccelerationy(59);

    tri.changePoint1(0,0);

    printf("Point1: (%.1f,%.1f). Point2: (%.1f,%.1f).  Point3: (%.1f,%.1f). ", tri.xcoordPoint1(), tri.ycoordPoint1(),
            tri.xcoordPoint2(), tri.ycoordPoint2(), tri.xcoordPoint3(), tri.ycoordPoint3());


    for (int i = 0; i<60;i++){
        if(!tri.delegateToMotionHandler())
            printf("Returned false\n");

    }

    printf ("\nNew position: \n");

      printf("Point1: (%.1f,%.1f). Point2: (%.1f,%.1f).  Point3: (%.1f,%.1f). ", tri.xcoordPoint1(), tri.ycoordPoint1(),
            tri.xcoordPoint2(), tri.ycoordPoint2(), tri.xcoordPoint3(), tri.ycoordPoint3());

    for (int i  = 60; i>0;i--)
        tri.delegateToMotionHandler(false);

    printf("\nFinal\n");

    printf("Point1: (%.1f,%.1f). Point2: (%.1f,%.1f).  Point3: (%.1f,%.1f). ", tri.xcoordPoint1(), tri.ycoordPoint1(),
            tri.xcoordPoint2(), tri.ycoordPoint2(), tri.xcoordPoint3(), tri.ycoordPoint3());


    **/

    /*
    printf("\n\nNew object: circle\n");

    Circle circ(0,0,10,60);
    circ.changeSpeedx(10);
    circ.changeSpeedy(10);

    circ.changeAccelerationx(0);
    circ.changeAccelerationy(0);

    printf("Circle initial position:\nx: %.1f. y: %.1f\n",
           circ.xPosition(), circ.yPosition());

    for (int i = 0; i<60;i++)
        circ.delegateToMotionHandler();

    printf ("\nNew position x:\n %.1f. y: %.1f\n",
            circ.xPosition(),circ.yPosition());*/


    /*
    printf("\n\nNew object: Rect\n");

    Rectangle rect(25,10,60);
    rect.changeSpeedx(49);
    rect.changeSpeedy(-38);

    printf("Rect initial position:\nx: %.1f. y: %.1f\n",
           rect.xPosition(), rect.yPosition());

    for (int i = 0; i<60;i++)
        rect.delegateToMotionHandler();

    printf ("\nNew position x:\n %.1f. y: %.1f\n",
            rect.xPosition(),rect.yPosition());*/

    /*
    Triangle tri (0,0,10,10,20,20,60);
    tri.changeSpeedx(10);
    tri.changeSpeedy(42);
    tri.changeAccelerationx(10);
    tri.changeAccelerationy(-59);

    printf ("tri initial position:\nx: %.1f. y:%.1f\n",
            tri.xPosition(),tri.yPosition());

    printf ("velocity- x: %.1f. y: %.1f\n",
            tri.xSpeed(),tri.ySpeed());


    for (int i = 0; i<60;i++){
        tri.delegateToMotionHandler();
    }

    printf ("\nNew position\n x: %.1f. y: %.1f\n",
            tri.xPosition(),tri.yPosition());

    printf ("New velocity x: %.1f. y: %.1f\n",
            tri.xSpeed(),tri.ySpeed());*/


    /*

    Rectangle rect(10,15,60);
    rect.changeSpeedx(-59);
    rect.changeSpeedy(-28);
    rect.changeAccelerationx(50);
    rect.changeAccelerationy(-24);

    printf("Rect initial position:\nx: %.1f. y: %.1f\n",
           rect.xPosition(), rect.yPosition());

    printf ("Velocity- x: %.1f. y: %.1f\n",
            rect.xSpeed(),rect.ySpeed());

    for (int i = 0; i<60;i++)
        rect.delegateToMotionHandler();

    printf ("\nNew position\n x: %.1f. y: %.1f\n",
            rect.xPosition(),rect.yPosition());

    printf ("New velocity- x: %.1f. y: %.1f\n",
            rect.xSpeed(),rect.ySpeed());*/


    /*
    Circle circ(0,0,10,60);
    circ.changeSpeedx(-50);
    circ.changeAccelerationx(-50);


    printf("Circle initial position:\nx: %.1f. y: %.1f\n",
           circ.xPosition(), circ.yPosition());

    for (int i = 0; i<60;i++)
        circ.delegateToMotionHandler();

    printf ("\nNew position x:\n %.1f. y: %.1f\n",
            circ.xPosition(),circ.yPosition());

    printf("New velocity x: %.1f\n", circ.xSpeed());*/

    Circle circ(0,0,10,60);
    circ.changeSpeedx(-100);

    printf("Circle x speed: %.1f\n",
           circ.xSpeed());

    /*
    printf("Going backwards\n");

    for (int i =60; i>0;i--)
        circ.delegateToMotionHandler(false);

    printf("New position x: %.1f. y: %.1f\n", circ.xPosition(),circ.yPosition());*/


    delete speed;
    delete acceleration;
    delete position;

}

void collisionsTests(){

    /*
    Circle c1(10, 10, 10,60);
    Rectangle rect (25,10,60);
    Circle c2 (31,10,10,60);
    Triangle tri(30,5,22,7,40,27,60);
    Triangle tri2(21,10,26,30,40,50,60);
    Rectangle rect2 (500,600,60);
    Triangle tri3(21,10,10,10,10,0,60);
    Circle c3 (25,10,10,60);*/


    /*
    Circle c1 (10,10,40,60);
    Rectangle rect (50,23,60,20,20);

    printf ("Collision result:\n %s\n",
            isCircleRectCollided(c1,rect) == true?
            "Circle and Rectangle Collided":
            "No collision");*/

    /*
    Rectangle rect (-100,100,60,60,50);
    Triangle tri (-58,64,-22,115,36,64,60);

    printf ("Collision result:\n %s\n",
            isRectangleTriangleCollided(rect,tri) == true?
            "Triangle and Rectangle Collided":
            "No collision");*/

    /*
    Rectangle rect1 (-534,359,60,242,153);
    Rectangle rect2 (-292,304,60,156,44);


    printf ("Collision result:\n %s\n",
            isRectanglesCollided(rect1,rect2) == true?
            "Rectangles Collided":
            "No collision");*/

    /*
    Rectangle rect (-1749,930,60,156,44);

      printf ("Collision result:\n %s\n",
            isRectanglesCollided(rect,leftWall) == true?
            "Rectangles Collided":
            "No collision");*/


    /*
     Triangle tri (-436,-372,-436,-328,-342,-372,60);
     Circle circle(-396,-401,31,60);

    printf ("Collision result:\n %s\n",
            isCircleTriangleCollided(circle,tri) == true?
            "Circle and triangle Collided":
            "No collision");*/

    /*
    Triangle tri1 (-87,69,72,233,379,70,60);
    Triangle tri2 (93,-10,65,80,-75,-22,60);

    printf ("Collision result:\n %s\n",
            isTrianglesCollided(tri1,tri2) == true?
            "Triangles Collided":
            "No collision");*/

    /*
    Circle circle1(-120,9,216,60);
    Circle circle2(134,1,51,60);
    printf ("Collision result:\n %s\n",
            isCirclesCollided(circle1,circle2) == true?
            "Circles Collided":
            "No collision");*/

    /*
    Circle c1 (1,11,40,60);
    Rectangle rect (50,23,60,20,20);

    printf ("Collision result:\n %s\n",
            isCircleRectCollided(c1,rect) == true?
            "Circle and Rectangle Collided":
            "No collision- circle and rectangle");*/


    /*
    Rectangle rect1 (-534,359,60,242,153);
    Rectangle rect2 (-276,304,60,156,44);


    printf ("Collision result:\n %s\n",
            isRectanglesCollided(rect1,rect2) == true?
            "Rectangles Collided":
            "No rectangles collision");*/

    /*
    Rectangle rect (-132,100,60,60,50);
    Triangle tri (-58,64,-22,115,36,64,60);

    printf ("Collision result:\n %s\n",
            isRectangleTriangleCollided(rect,tri) == true?
            "Triangle and Rectangle Collided":
            "No tri & rect collision");*/


    Triangle tri (-31,264,-31,308,63,264,60);
    Circle circle(145,277,31,60);

    printf ("Collision result:\n %s\n",
            isCircleTriangleCollided(circle,tri) == true?
            "Circle and triangle Collided":
            "No circle-triangle collision");

    //printf ("Result: %d\n",isCirclesCollided(c1,c2));
    //printf("Rect circle collision: %d\n", isCircleRectCollidedPrevious(c1,rect));


    //printf ("Now with polygon: %d\n", isCircleRectCollided(c1,rect));
    //printf ("Triangle Rectangle collision : %d\n",isCircleTriangleCollided(c1,tri));
   // printf ("Triangle rectangle collision: %d\n", isRectangleTriangleCollided(rect,tri2));
    //printf ("Rectangle rectangle: %d\n", isRectanglesCollided(rect,rect2));
//    printf ("Triangle triangle collision: %d\n", isTrianglesCollided(tri,tri2));
   // printf("Other triangle triangle collision: %d\n",isTrianglesCollided(tri2,tri3));
   // printf ("Triangle circle collision: %d\n", isCircleTriangleCollided(c3,tri3));


}

void checkMixingObjects(){

    Circle c1(10, 10, 10,60);
    Rectangle rect (25,10,60);
    Circle c2 (31,10,10,60);
    Triangle tri(30,5,22,7,40,27,60);
    Triangle tri2(21,10,26,30,40,50,60);
    Rectangle rect2 (500,600,60);
    Triangle tri3(21,10,10,10,10,0,60);
    Circle c3 (25,10,10,60);

    vector<Shape*> shapes;

    shapes.push_back(&c1);
    shapes.push_back(&rect);
    shapes.push_back(&c2);
    shapes.push_back(&tri);
    shapes.push_back(&tri2);
    shapes.push_back(&rect2);
    shapes.push_back(&tri3);
    shapes.push_back(&c3);

    printf("Size of circle: %d\n", sizeof(Circle));
    printf("Size of rect: %d\n",sizeof(Rectangle));
    printf("Size of tri: %d\n", sizeof(Triangle));
    printf("Size of shape: %d\n",sizeof(Shape));

    for (int i =0; i<8;i++){

        printf ("Size: %d\n", sizeof (*shapes[i]));
    }


}

void collisionTestsPhysicsEngine(){

    physicsEngineHandler shapes(1);

    Circle c1(10, 10, 10,60);
    Rectangle rect (21,10,60);
    Triangle tri (19,10,30,10,25,0,60);
    Triangle tri2 (9,0,26,0,20,-5,60);

    /*
    Circle c2 (31,10,10,60);
    Triangle tri(30,5,22,7,40,27,60);
    Triangle tri2(21,10,26,30,40,50,60);
    Rectangle rect2 (500,600,60);
    Triangle tri3(21,10,10,10,10,0,60);
    Circle c3 (25,10,10,60);*/

    shapes.addCircle(&c1);
    shapes.addRectangle(&rect);
    shapes.addTriangle(&tri);
    shapes.addTriangle(&tri2);

    /*
    shapes.addCircle(&c2);
    shapes.addTriangle(&tri);
    shapes.addTriangle(&tri2);
    shapes.addRectangle(&rect2);
    shapes.addTriangle(&tri3);
    shapes.addCircle(&c3);*/

    shapes.checkCollisions();

    printf ("Rectangle circle: %d\n", isCircleRectCollided(c1,rect));
    printf ("Triangle circle: %d\n", isCircleTriangleCollided(c1,tri));
    printf ("Rectangle triangles: %d\n", isRectangleTriangleCollided(rect,tri));
    printf ("Triangle triangle: %d\n", isTrianglesCollided(tri,tri2));



}



void decimalPlaceTests(){

    double b = 5.635;
    double c = 5.9999;
    b = roundNumber(b,2);
    c = roundNumber(c,2);

    printf("%.3f\n",b);
    printf ("%.3f\n",c);

}

void collisionHandlingTestsSameRest(){

    /*
     physicsEngineHandler engine(1);

     Circle c (10,10,10,60,10);
     Triangle t(10,10,5,14,20,10,60,10);

     c.changeSpeedx(20);

     engine.addCircle(&c);
     engine.addTriangle(&t);

     engine.checkCollisions();

     printf ("Circle speed x: %.1f\n"
             "Triangle speed x: %.1f\n",
             c.xSpeed(),
             t.xSpeed());*/


    physicsEngineHandler engine(1);
    Rectangle rect(0,0,60,50,50,100);
    rect.changeSpeedx(50);

    Circle c (50,0,20,60,20);
    c.changeSpeedx(-50);
    c.changeSpeedy(20);

    engine.addRectangle(&rect);
    engine.addCircle(&c);

    circleToRectangleRestitution = 0.5;
    engine.checkCollisions();

    printf ("Rectangle x speed: %.1f\n"
            "Rectangle y speed: %.1f\n"
            "Circle x speed: %.1f\n"
            "Circle y speed: %.1f\n",
            rect.xSpeed(),rect.ySpeed(),
            c.xSpeed(),c.ySpeed());

    /*

    physicsEngineHandler engine(1);


    Rectangle r1 (20,30,60,5,5,10);
    Rectangle r2 (24,30,60,5,5,5);
    Rectangle r3 (28,30,60,5,5,4);

    r1.changeSpeedx(20);
    r2.changeSpeedx(15);
    r3.changeSpeedx(-25);

    engine.addRectangle(&r1);
    engine.addRectangle(&r2);
    engine.addRectangle(&r3);


    Rectangle r4 ((simulationWidth/2)-WALL_THICKNESS-5,30,60,5,5,10);
    r4.changeSpeedx(30);
    engine.addRectangle(&r4);

    printf ("R1 and r2 %d\n", isRectanglesCollided(r1,r3));


    engine.checkCollisions();


    printf ("R1 final speed: %.2f\n", r1.xSpeed());
    printf ("R2 final speed: %.2f\n", r2.xSpeed());
    printf ("R3 final speed: %.2f\n", r3.xSpeed());
    printf ("R4 final speed: %.2f\n", r4.xSpeed());

    printf ("Speeds should remain the same\n");

    engine.checkCollisions();


    printf ("R1 final speed: %.2f\n", r1.xSpeed());
    printf ("R2 final speed: %.2f\n", r2.xSpeed());
    printf ("R3 final speed: %.2f\n", r3.xSpeed());
    printf ("R4 final speed: %.2f\n", r4.xSpeed());*/


}

void collisionHandlingTestsDifferentRest(){

    circleToRectangleRestitution = 0.5;
    circleToTriangleRestitution = 0.4;
    physicsEngineHandler engine(1);
    Rectangle r1 (20,30,60,5,5,10);
    Circle c1 (30,30,5,60,5);
    Triangle t1 (34,30,40,50,40,20,60,4);

    r1.changeSpeedx(20);
    c1.changeSpeedx(15);
    t1.changeSpeedx(-25);

    engine.addRectangle(&r1);
    engine.addCircle(&c1);
    engine.addTriangle(&t1);
    engine.checkCollisions();

    printf ("R1 final speed: %.2f, %.2f\n",
            r1.xSpeed(),r1.ySpeed());

    printf ("C1 final speed: %.2f, %.2f\n",
            c1.xSpeed(),c1.ySpeed());

    printf ("T1 final speed: %.2f, %.2f\n",
            t1.xSpeed(),t1.ySpeed());

    /**
    printf("Speeds should remain the same\n");

    engine.checkCollisions();


    printf ("R1 final speed: %.2f, %.2f\n", r1.xSpeed(),r1.ySpeed());
    printf ("C1 final speed: %.2f, %.2f\n", c1.xSpeed(),c1.ySpeed());
    printf ("T1 final speed: %.2f, %.2f\n", t1.xSpeed(),t1.ySpeed());**/


}

void collisionHandlingTestsDiffRestPhysicsEngineOwn(){

    circleToRectangleRestitution = 0.5;
    circleToTriangleRestitution = 0.4;
    circleToCircleRestitution = 0.5;

    physicsEngineHandler engine(1);

    Rectangle r1 (20,30,60,5,5,10);
    Circle c1 (30,30,5,60,5);
    Triangle t1 (34,30,40,50,40,20,60,4);

    Circle c2(500,20,5,60,20);
    Circle c3(500,30,5,60,5);

    r1.changeSpeedx(20);
    c1.changeSpeedx(15);
    t1.changeSpeedx(-25);

    c2.changeSpeedx(10);
    c2.changeSpeedy(7);

    c3.changeSpeedx(-10);
    c3.changeSpeedy(-5);

    engine.addCircle(&c2);
    engine.addCircle(&c3);

    engine.addRectangle(&r1);
    engine.addCircle(&c1);
    engine.addTriangle(&t1);

    engine.moveObjects();



    printf ("R1 final speed: %.2f, %.2f\n", r1.xSpeed(),r1.ySpeed());
    printf ("C1 final speed: %.2f, %.2f\n", c1.xSpeed(),c1.ySpeed());
    printf ("T1 final speed: %.2f, %.2f\n", t1.xSpeed(),t1.ySpeed());
    printf ("C2 final speed: %.2f, %.2f\n", c2.xSpeed(),c2.ySpeed());
    printf ("C3 final speed: %.2f, %.2f\n", c3.xSpeed(),c3.ySpeed());

    printf("Speeds should remain the same\n");

    engine.moveObjects();

    printf ("R1 final speed: %.2f, %.2f\n", r1.xSpeed(),r1.ySpeed());
    printf ("C1 final speed: %.2f, %.2f\n", c1.xSpeed(),c1.ySpeed());
    printf ("T1 final speed: %.2f, %.2f\n", t1.xSpeed(),t1.ySpeed());
    printf ("C2 final speed: %.2f, %.2f\n", c2.xSpeed(),c2.ySpeed());
    printf ("C3 final speed: %.2f, %.2f\n", c3.xSpeed(),c3.ySpeed());

    printf ("Going backwards in time\n");
    engine.moveObjects(false);
    engine.moveObjects(false);

    printf ("R1 final speed: %.2f, %.2f\n", r1.xSpeed(),r1.ySpeed());
    printf ("C1 final speed: %.2f, %.2f\n", c1.xSpeed(),c1.ySpeed());
    printf ("T1 final speed: %.2f, %.2f\n", t1.xSpeed(),t1.ySpeed());
    printf ("C2 final speed: %.2f, %.2f\n", c2.xSpeed(),c2.ySpeed());
    printf ("C3 final speed: %.2f, %.2f\n", c3.xSpeed(),c3.ySpeed());




}

void collisionTestSameCoordinateAxis(){

    //Test this using the different restitution function to see if that same axis thing worked
    //Bypass first if statement of same restitution with something impossible like 5==4
    circleToCircleRestitution = 0.5;
    Circle c1(20,20,5,60,20);
    Circle c2(20,30,5,60,5);
    physicsEngineHandler engine(1);

    c1.changeSpeedx(10);
    c1.changeSpeedy(7);

    c2.changeSpeedx(-10);
    c2.changeSpeedy(-5);

    engine.addCircle(&c1);
    engine.addCircle(&c2);

    engine.checkCollisions();

    printf ("C1 final speed: %.2f, %.2f\n", c1.xSpeed(),c1.ySpeed());
    printf ("C2 final speed: %.2f, %.2f\n", c2.xSpeed(),c2.ySpeed());

}

void databaseSaveTest(){

    using std::cout;

    circleToRectangleRestitution = 0.5;
    circleToTriangleRestitution = 0.4;
    circleToCircleRestitution = 0.5;

    //sqlite3* db = setupDatabase("template2.db");

    sqlite3* db = nullptr;
    sqlite3_open("template2.db", &db);

    if (!db)
        cout << "Failure";

    else
        cout <<"Database created";

    Rectangle r1 (20,30,60,5,5,10);
    Circle c1 (30,30,5,60,5);
    Triangle t1 (34,30,40,50,40,20,60,4);


    Circle c2(500,20,5,60,20);
    Circle c3(500,30,5,60,5);

    r1.changeSpeedx(20);
    c1.changeSpeedx(15);
    t1.changeSpeedx(-25);

    c2.changeSpeedx(10);
    c2.changeSpeedy(7);

    c3.changeSpeedx(-10);
    c3.changeSpeedy(-5);


    vector<Circle> circles;
    vector<Rectangle> rectangles;
    vector<Triangle> triangles;

    circles.push_back(c1);
    circles.push_back(c2);
    circles.push_back(c3);

    rectangles.push_back(r1);
    triangles.push_back(t1);

    bool r = saveData(db,&rectangles,&circles,&triangles);

    printf ("Result returned is: %d\n",r);



}

void loadDatabaseTest(){

    using std::cout;

    vector<Circle> circles;
    vector<Triangle> triangles;
    vector<Rectangle> rectangles;

    sqlite3* db = nullptr;
    sqlite3_open("template2.db", &db);

    physicsEngineHandler physicsEngine(1);

    loadData(db,&rectangles,&circles,&triangles);

    int i;

    for (i = 0; i<rectangles.size();i++){

        printf("R%d. x: %.2f y: %.2f speed x: %.2f speed y: %.2f accel x: %.2f accel y: %.2f mass: %.2f\n", i+1,
               rectangles[i].xPosition(),rectangles[i].yPosition(), rectangles[i].xSpeed(), rectangles[i].ySpeed(),
               rectangles[i].xAcceleration(), rectangles[i].yAcceleration(), rectangles[i].getMass());

        physicsEngine.addRectangle(&rectangles[i]);
    }


    printf("Now for triangles:\n");

     for (i = 0; i<triangles.size();i++){

        printf("T%d. x: %.2f y: %.2f speed x: %.2f speed y: %.2f accel x: %.2f accel y: %.2f mass: %.2f\n", i+1,
               triangles[i].xPosition(),triangles[i].yPosition(), triangles[i].xSpeed(), triangles[i].ySpeed(),
               triangles[i].xAcceleration(), triangles[i].yAcceleration(), triangles[i].getMass());

        physicsEngine.addTriangle(&triangles[i]);
    }

    printf("Now for circles:\n");

     for (i = 0; i<circles.size();i++){

        printf("C%d. x: %.2f y: %.2f speed x: %.2f speed y: %.2f accel x: %.2f accel y: %.2f mass: %.2f\n", i+1,
               circles[i].xPosition(),circles[i].yPosition(), circles[i].xSpeed(), circles[i].ySpeed(),
               circles[i].xAcceleration(), circles[i].yAcceleration(), circles[i].getMass());

        physicsEngine.addCircle(&circles[i]);
    }


    physicsEngine.moveObjects();
    printf("Speed tests: \n");


    for (i = 0; i<rectangles.size();i++){

        printf("R%d.speed x: %.2f speed y: %.2f\n", i+1,rectangles[i].xSpeed(), rectangles[i].ySpeed());
    }

    for (i = 0; i<triangles.size();i++){

        printf("T%d. speed x: %.2f speed y: %.2f \n", i+1,triangles[i].xSpeed(), triangles[i].ySpeed());
    }

    for (i = 0; i<circles.size();i++){

        printf("C%d.speed x: %.2f speed y: %.2f\n", i+1,circles[i].xSpeed(), circles[i].ySpeed());
    }
}

void randTest(){

    Rectangle r(100,100,60,100,100,5);
    r.changeSpeedx(50);


    Circle a (100,-1000,50,60);
    a.changeSpeedx(50);


    vector<Rectangle> b;

    b.push_back(r);

    physicsEngineHandler c(1);

    c.addRectangle(&r);
    c.addCircle(&a);



    //Rectangle x(100,100,60);
    //x.changeSpeedx(50);

    double* pos = new double;
    //*pos = 100.0;

    double* vel = new double;
    //*vel = 50.0;

    Motion de;

    //de.initialiseClass(&pos,&vel,60,true);

    for (int i =0; i < 60;i++){
        c.moveObjects();
        //x.delegateToMotionHandler();
       // de.moveObjectForward();


    }

    printf("rect: %.2f\n",r.xPosition());

    //printf("Other pos %.2f\n",x.xPosition());
    printf("circle pos %.2f\n",a.xPosition());
    //printf("de pos: %.2f\n",*pos);

    delete pos;
    delete vel;

}

void collideRectEmergency(){

    Rectangle r1(15,20,60,50,50);
    r1.changeSpeedx(60);

    Rectangle r2 (70,20,60,50,50);


    physicsEngineHandler a(1);

    a.addRectangle(&r1);
    a.addRectangle(&r2);

    for (int i =0; i<60;i++)
        a.moveObjects();

}
