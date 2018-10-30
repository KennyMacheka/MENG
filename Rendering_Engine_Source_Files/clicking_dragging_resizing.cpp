#include "clicking_dragging_resizing.h"
#include "rendering_engine_global_vars.h"
#include "utility.h"
#include <stdio.h>

using namespace physicsEngine;
using std::vector;
//Note: mousex and  mousey must be in cartesian coordinates
//Use convertX and use shapesBox.w as the width

shapeClicked::shapeClicked(bool isResizing){
    resizingNeeded = isResizing;
    shapeSetup = false;
    shapeNum = NO_SHAPE;
    resizeShape.rect.resizeNum = NO_RECT_RESIZE;
    resizeShape.tri.resizeNum = NO_TRI_RESIZE;
    mouseState = NOT_CLICKED;

    //An id number will never be negative, so we can assume this is unique
    instNum = -5;
}

shapeClicked::~shapeClicked(){

    removeShape();
}

bool shapeClicked::isSetup(){
    return shapeSetup;
}

bool shapeClicked::isShapeHighlighted(){

    if (!shapeSetup)
        return false;

    if (shapeNum == RECTANGLE)
        return shape.rect->shouldHighlightShape;

    else if (shapeNum == TRIANGLE)
        return shape.tri->shouldHighlightShape;

    else if (shapeNum == CIRCLE)
        return shape.circle->shouldHighlightShape;

}

void shapeClicked::highlightShape(){
    if (!shapeSetup)
        return;

    if (shapeNum == RECTANGLE)
        shape.rect->shouldHighlightShape = true;

    else if (shapeNum == TRIANGLE)
        shape.tri->shouldHighlightShape = true;

    else if (shapeNum == CIRCLE)
        shape.tri->shouldHighlightShape = true;

}

void shapeClicked::unhilightShape(){

    if (!shapeSetup)
        return;

    if (shapeNum == RECTANGLE)
        shape.rect->shouldHighlightShape = false;

    else if (shapeNum == TRIANGLE)
        shape.tri->shouldHighlightShape = false;

    else if (shapeNum == CIRCLE)
        shape.circle->shouldHighlightShape = false;
}

void shapeClicked::addRect(Rectangle* rect, int cartesianMousex, int cartesianMousey){

    if (rect->Id() != instNum){
        removeShape();
        shape.rect = rect;
        shapeNum = RECTANGLE;
        instNum = rect->Id();
        shapeSetup = true;

        if (resizingNeeded)
            setupRectCircles();
    }
    else{
        if (resizingNeeded)
            updateRectCircles();
    }

    //If the same shape is being clicked on, we still want the mouse coordinates to update
    //Hence why this is outside the if statement
    differenceMousex = convertIntDouble(rect->xPosition()) - cartesianMousex;
    differenceMousey = convertIntDouble(rect->yPosition()) - cartesianMousey;

}

void shapeClicked::addTri(Triangle* tri, renderingCoordinates* triCoords,
                        int cartesianMousex, int cartesianMousey){

    if (tri->Id() != instNum){
        removeShape();
        shape.tri= tri;
        shapeRenderCoords.tri = triCoords;
        shapeNum = TRIANGLE;
        instNum = tri->Id();
        shapeSetup = true;

        if (resizingNeeded)
            setupTriCircles();
    }

    else{
        if (resizingNeeded)
            updateTriCircles();
    }

    /**
         xCoordPoint1() and yCoordPoint1() is the point on the triangle
         used to get the difference between a triangle and the mouse coordinate
    **/
    differenceMousex = convertIntDouble(tri->xcoordPoint1()) - cartesianMousex;
    differenceMousey = convertIntDouble(tri->ycoordPoint1()) - cartesianMousey;
}

void shapeClicked::addCircle(Circle* circle, renderingCoordinates* circleCoords,
                             int cartesianMousex, int cartesianMousey){
    //If shape is not already being clicked on
    if (circle->Id() != instNum){
        removeShape();
        shape.circle = circle;
        shapeRenderCoords.circle = circleCoords;
        shapeNum = CIRCLE;
        instNum = circle->Id();
        shapeSetup = true;
    }

    differenceMousex = convertIntDouble(circle->xPosition()) - cartesianMousex;
    differenceMousey = convertIntDouble(circle->yPosition()) - cartesianMousey;
}



void shapeClicked::removeShape(){

    unhilightShape();

    if (shapeNum == RECTANGLE && resizingNeeded){

        delete resizeShape.rect.topLeftCorner;
        delete resizeShape.rect.topMiddle;
        delete resizeShape.rect.topRightCorner;
        delete resizeShape.rect.rightMiddle;
        delete resizeShape.rect.bottomRightCorner;
        delete resizeShape.rect.bottomMiddle;
        delete resizeShape.rect.bottomLeftCorner;
        delete resizeShape.rect.leftMiddle;

        coords.circleRect.topLeftCorner.coordinates.clear();
        coords.circleRect.topMiddle.coordinates.clear();
        coords.circleRect.topRightCorner.coordinates.clear();
        coords.circleRect.rightMiddle.coordinates.clear();
        coords.circleRect.bottomRightCorner.coordinates.clear();
        coords.circleRect.bottomMiddle.coordinates.clear();
        coords.circleRect.bottomLeftCorner.coordinates.clear();
        coords.circleRect.leftMiddle.coordinates.clear();

        resizeShape.rect.resizeNum = NO_RECT_RESIZE;

    }

    else if (shapeNum == TRIANGLE && resizingNeeded){

        delete resizeShape.tri.point1Circle;
        delete resizeShape.tri.point2Circle;
        delete resizeShape.tri.point3Circle;

        coords.circleTri.point1Circle.coordinates.clear();
        coords.circleTri.point2Circle.coordinates.clear();
        coords.circleTri.point3Circle.coordinates.clear();
        resizeShape.tri.resizeNum = NO_TRI_RESIZE;
    }

    shapeNum = NO_SHAPE;

    shapeSetup = false;
    instNum = -5;

}

void shapeClicked::getDifferenceMousex (int* placeHolder){

    if (shapeSetup && placeHolder != nullptr){
        *placeHolder = differenceMousex;
    }
}

void shapeClicked::getDifferenceMousey (int* placeHolder){

    if (shapeSetup && placeHolder != nullptr){
        *placeHolder = differenceMousey;
    }
}

void shapeClicked::updateResizeCircles(){

    if (!shapeSetup || !resizingNeeded)
        return;

    if (shapeNum == RECTANGLE)
        updateRectCircles();

    else if (shapeNum == TRIANGLE)
        updateTriCircles();

}

void shapeClicked::updateRectCircles(){

    int rectx = convertIntDouble(shape.rect->xPosition());
    int recty = convertIntDouble(shape.rect->yPosition());
    int w = shape.rect->getWidth();
    int h = shape.rect->getHeight();

    int midx = (rectx+rectx+w)/2;
    int midy = (recty+(recty-h))/2;

    resizeShape.rect.topLeftCorner->changePosition(rectx,recty);
    resizeShape.rect.topMiddle->changePosition(midx,recty);
    resizeShape.rect.topRightCorner->changePosition(rectx+w,recty);
    resizeShape.rect.rightMiddle->changePosition(rectx+w,midy);
    resizeShape.rect.bottomRightCorner->changePosition(rectx+w,recty-h);
    resizeShape.rect.bottomMiddle->changePosition(midx,recty-h);
    resizeShape.rect.bottomLeftCorner->changePosition(rectx,recty-h);
    resizeShape.rect.leftMiddle->changePosition(rectx,midy);

}

void shapeClicked::updateTriCircles(){


    int x1 = convertIntDouble(shape.tri->xcoordPoint1());
    int y1 = convertIntDouble(shape.tri->ycoordPoint1());

    int x2 = convertIntDouble(shape.tri->xcoordPoint2());
    int y2 = convertIntDouble(shape.tri->ycoordPoint2());

    int x3 = convertIntDouble(shape.tri->xcoordPoint3());
    int y3 = convertIntDouble(shape.tri->ycoordPoint3());

    resizeShape.tri.point1Circle->changePosition(x1,y1);
    resizeShape.tri.point2Circle->changePosition(x2,y2);
    resizeShape.tri.point3Circle->changePosition(x3,y3);


}

void shapeClicked::setupTriCircles(){

    int x1 = convertIntDouble(shape.tri->xcoordPoint1());
    int y1 = convertIntDouble(shape.tri->ycoordPoint1());

    int x2 = convertIntDouble(shape.tri->xcoordPoint2());
    int y2 = convertIntDouble(shape.tri->ycoordPoint2());

    int x3 = convertIntDouble(shape.tri->xcoordPoint3());
    int y3 = convertIntDouble(shape.tri->ycoordPoint3());

    resizeShape.tri.point1Circle = new Circle (x1,y1,resizeCirclesRadius,DEFAULT_FRAME_RATE);
    resizeShape.tri.point2Circle = new Circle (x2,y2,resizeCirclesRadius,DEFAULT_FRAME_RATE);
    resizeShape.tri.point3Circle = new Circle (x3,y3,resizeCirclesRadius,DEFAULT_FRAME_RATE);

    setupCircleRendering(resizeShape.tri.point1Circle,&coords.circleTri.point1Circle);
    setupCircleRendering(resizeShape.tri.point2Circle,&coords.circleTri.point2Circle);
    setupCircleRendering(resizeShape.tri.point3Circle,&coords.circleTri.point3Circle);


}

void shapeClicked::setupRectCircles(){

    int rectx = convertIntDouble(shape.rect->xPosition());
    int recty = convertIntDouble(shape.rect->yPosition());
    int w = shape.rect->getWidth();
    int h = shape.rect->getHeight();
    int midx = (rectx+rectx+w)/2;
    int midy = (recty+(recty-h))/2;


    resizeShape.rect.topLeftCorner = new Circle (rectx,recty,resizeCirclesRadius,DEFAULT_FRAME_RATE);
    resizeShape.rect.topMiddle = new Circle(midx,recty,resizeCirclesRadius,DEFAULT_FRAME_RATE);
    resizeShape.rect.topRightCorner = new Circle(rectx+w,recty,resizeCirclesRadius,DEFAULT_FRAME_RATE);
    resizeShape.rect.rightMiddle = new Circle(rectx+w,midy,resizeCirclesRadius,DEFAULT_FRAME_RATE);
    resizeShape.rect.bottomRightCorner = new Circle(rectx+w,recty-h,resizeCirclesRadius,DEFAULT_FRAME_RATE);
    resizeShape.rect.bottomMiddle = new Circle(midx,recty-h,resizeCirclesRadius,DEFAULT_FRAME_RATE);
    resizeShape.rect.bottomLeftCorner = new Circle(rectx,recty-h,resizeCirclesRadius,DEFAULT_FRAME_RATE);
    resizeShape.rect.leftMiddle = new Circle(rectx,midy,resizeCirclesRadius,DEFAULT_FRAME_RATE);

    setupCircleRendering(resizeShape.rect.topLeftCorner,&coords.circleRect.topLeftCorner);
    setupCircleRendering(resizeShape.rect.topMiddle,&coords.circleRect.topMiddle);
    setupCircleRendering(resizeShape.rect.topRightCorner,&coords.circleRect.topRightCorner);
    setupCircleRendering(resizeShape.rect.rightMiddle,&coords.circleRect.rightMiddle);
    setupCircleRendering(resizeShape.rect.bottomRightCorner,&coords.circleRect.bottomRightCorner);
    setupCircleRendering(resizeShape.rect.bottomMiddle,&coords.circleRect.bottomMiddle);
    setupCircleRendering(resizeShape.rect.bottomLeftCorner,&coords.circleRect.bottomLeftCorner);
    setupCircleRendering(resizeShape.rect.leftMiddle,&coords.circleRect.leftMiddle);


}

double shapeClicked::getMass(){

    if (!shapeSetup)
        return -10000;

    if (shapeNum == RECTANGLE)
        return shape.rect->getMass();

    else if (shapeNum == TRIANGLE)
        return shape.tri->getMass();

    else if (shapeNum == CIRCLE)
        return shape.circle->getMass();

}

double shapeClicked::getXVelocity(){

    if (!shapeSetup)
        return -100000;

    if (shapeNum == RECTANGLE)
        return shape.rect->xSpeed();

    else if (shapeNum == TRIANGLE)
        return shape.tri->xSpeed();

    else if (shapeNum == CIRCLE)
        return shape.circle->xSpeed();

}

double shapeClicked::getYVelocity(){

    if (!shapeSetup)
        return -100000;

    if (shapeNum == RECTANGLE)
        return shape.rect->ySpeed();

    else if (shapeNum == TRIANGLE)
        return shape.tri->ySpeed();

    else if (shapeNum == CIRCLE)
        return shape.circle->ySpeed();

}

double shapeClicked::getXAcceleration(){

    if (!shapeSetup)
        return -100000;

    if (shapeNum == RECTANGLE)
        return shape.rect->xAcceleration();

    else if (shapeNum == TRIANGLE)
        return shape.tri->xAcceleration();

    else if (shapeNum == CIRCLE)
        return shape.circle->xAcceleration();

}

double shapeClicked::getYAcceleration(){

    if (!shapeSetup)
        return -100000;

    if (shapeNum == RECTANGLE)
        return shape.rect->yAcceleration();

    else if (shapeNum == TRIANGLE)
        return shape.tri->yAcceleration();

    else if (shapeNum == CIRCLE)
        return shape.circle->yAcceleration();

}

bool shapeClicked::getGravityEffect(){

    if (!shapeSetup)
        return false;

    if (shapeNum == RECTANGLE)
        return shape.rect->getGravityEffect();

    else if (shapeNum == TRIANGLE)
        return shape.tri->getGravityEffect();

    else if (shapeNum == CIRCLE)
        return shape.circle->getGravityEffect();

}
uint8_t shapeClicked::getRValue(){


    if (!shapeSetup)
        return 0;

    bool shouldRehilight = false;
    if (isShapeHighlighted()){
        shouldRehilight = true;
        unhilightShape();
    }

    uint8_t val;

    if (shapeNum == RECTANGLE)
        val = shape.rect->rValue();

    else if (shapeNum == TRIANGLE)
        val =  shape.tri->rValue();

    else if (shapeNum == CIRCLE)
        val =  shape.circle->rValue();

    if (shouldRehilight)
        highlightShape();

    return val;
}

uint8_t shapeClicked::getGValue(){

    if (!shapeSetup)
        return 0;

    bool shouldRehilight = false;
    if (isShapeHighlighted()){
        shouldRehilight = true;
        unhilightShape();
    }

    uint8_t val;

    if (shapeNum == RECTANGLE)
        val =  shape.rect->gValue();

    else if (shapeNum == TRIANGLE)
        val =  shape.tri->gValue();

    else if (shapeNum == CIRCLE)
        val =  shape.circle->gValue();

    if (shouldRehilight)
        highlightShape();

    return val;
}

uint8_t shapeClicked::getBValue(){

    if (!shapeSetup)
        return 0;

    bool shouldRehilight = false;
    uint8_t val;
    if (isShapeHighlighted()){
        shouldRehilight = true;
        unhilightShape();
    }

    if (shapeNum == RECTANGLE)
        val =  shape.rect->bValue();

    else if (shapeNum == TRIANGLE)
        val =  shape.tri->bValue();

    else if (shapeNum == CIRCLE)
        val =  shape.circle->bValue();

    if (shouldRehilight)
        highlightShape();
    return val;
}

uint8_t shapeClicked::getAValue(){
    if (!shapeSetup)
        return 0;

    if (shapeNum == RECTANGLE)
        return shape.rect->aValue();

    else if (shapeNum == TRIANGLE)
        return shape.tri->aValue();

    else if (shapeNum == CIRCLE)
        return shape.circle->aValue();
}

void shapeClicked::changeColour(int dr, int dg, int db, int da){
    if (!shapeSetup)
        return;

    if (shapeNum == RECTANGLE)
        shape.rect->changeColour(shape.rect->rValue()+dr,shape.rect->gValue()+dg,
                                 shape.rect->bValue()+db,shape.rect->aValue()+da);

    else if (shapeNum == CIRCLE)
        shape.circle->changeColour(shape.circle->rValue()+dr,shape.circle->gValue()+dg,
                                   shape.circle->bValue()+db,shape.circle->aValue()+da);

    else if (shapeNum == TRIANGLE)
        shape.tri->changeColour(shape.tri->rValue()+dr,shape.tri->gValue()+dg,
                                shape.tri->bValue()+db,shape.tri->aValue()+da);
}

void shapeClicked::changeGravityEffect(){
    if (!shapeSetup)
        return;

    bool change = getGravityEffect() == true? false:true;

    if (shapeNum == RECTANGLE)
        shape.rect->gravityEffect(change);

    else if (shapeNum == TRIANGLE)
        shape.tri->gravityEffect(change);

    else if (shapeNum == CIRCLE)
        shape.tri->gravityEffect(change);
}



