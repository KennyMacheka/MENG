#include "renderingShapes.h"
#include "collisionDetection.h"
#include "utility.h"
#include "fraction.h"
#include <cmath>

using std::abs;
using std::vector;
using namespace physicsEngine;

void renderPoint (SDL_Renderer* renderer, int x, int y, int mapWidth, int mapHeight, int dx, int dy);
void renderLine (SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int mapWidth,
                 int mapHeight, int dx, int dy);

//If a triangle object is provided instead of direct coordinates
bool pointInTriangle (Triangle* tri, int x, int y){
    int x1 = convertIntDouble(tri->xcoordPoint1());
    int y1 = convertIntDouble(tri->ycoordPoint1());

    int x2 = convertIntDouble(tri->xcoordPoint2());
    int y2 = convertIntDouble(tri->ycoordPoint2());

    int x3 = convertIntDouble(tri->xcoordPoint3());
    int y3 = convertIntDouble(tri->ycoordPoint3());

    return pointInTriangle(x1,y1,x2,y2,x3,y3,x,y);

}

bool pointInTriangle (int x1, int y1, int x2, int y2,
                      int x3, int y3, int x, int y){

    /**
    To determine whether a point lies inside a triangle or not, I've used a a coordinate system
    known as Barycentric Coordinates. It's application in this instance works as follows:

        1. For any point "r" inside a triangle with three vertices r1 (x1,y1), r2 (x2,y2) and r3(x3,y3),
           there exists three numbers greater than or equal to 0 that sum to 1,  p1 + p2 + p3 = 1,
           and that r = r1*p1 + r2*p2 + r3*p3

        2. If any of the three numbers are less than 0, then the point does not lie inside the triangle

        These key facts are used to fill in a triangle with colour.

        The numbers p1, p2 and p3 for a point (x,y) are determined via the following formulae:

        p1 = ( (y2-y3)*(x-x3) + (x3-x2)*(y-y3) ) / ( (y2-y3)*(x1-x3) + (x3-x2)*(y1-y3) )
        p2 = ( (y3-y1)*(x-x3) + (x1-x3)*(y-y3) ) / ( (y2-y3)*(x1-x3) + (x3-x2)*(y1-y3) )
        p3 = 1 - p1 -p2

        If a point lies outside the triangle, at least one of p1, p2 and p3 will be less than 1
    **/

    fraction p1 ( (y2-y3)*(x-x3) + (x3-x2)*(y-y3), (y2-y3)*(x1-x3) + (x3-x2)*(y1-y3) );
    fraction p2 ((y3-y1)*(x-x3) + (x1-x3)*(y-y3), (y2-y3)*(x1-x3) + (x3-x2)*(y1-y3));
    fraction p3 = (p1+p2)*-1 +1;

    if (p1 < 0 || p2 < 0 || p3 < 0)
        return false;


    return true;

}

bool pointInRect (Rectangle* rect, int x, int y){

    int rectx = convertIntDouble(rect->xPosition());
    int recty = convertIntDouble(rect->yPosition());
    int w = convertIntDouble(rect->getWidth());
    int h = convertIntDouble(rect->getHeight());

    return pointInRect (rectx, recty, w,h,x,y);

}


bool pointInRect (int topLeftx, int topLefty, int w ,int h, int x, int y){

    /**
        This function checks if a point lies inside a rectangle.

        It isn't like the one in sdl_utilities.cpp, as that assumes the
        coordinates are screen coordinates. This one assumed the coordinates
        provided are normal Cartesian coordinates, where going up is y increasing
        and going down is y decreasing
    **/

    if (x >= topLeftx && x <= topLeftx +w &&
        y >= topLefty - h && y <= topLefty)
            return true;

    return false;

}

bool pointInCircle(Circle* circle, int x, int y){

    int centrex = convertIntDouble(circle->xPosition());
    int centrey = convertIntDouble(circle->yPosition());
    int radius = circle->getRadius();

    return pointInCircle(centrex,centrey,radius,x,y);
}

bool pointInCircle (int centrex, int centrey, int radius, int x, int y){

    //Function checks if a point is in circle

    if ( (centrex-x)*(centrex-x) + (centrey-y)*(centrey-y) < radius*radius)
        return true;

    return false;
}


/**
    When rendering shapes onto the screen, the normal cartesian coordinates must be converted
    into screen coordinates. When the renderer is provided in each of these functions, it is assumed
    that the portion of the screen where the shape will be rendered has been calibrated so its top left corner
    has coordinates (0,0)

    The general formula I devised for converting between normal cartesian coordinates
    to screen coordinates is:

    F(x,y)=(x+0.5w- dx,-y+0.5h-dy)
	Where (x,y) are the Cartesian coordinates used by the physics engine,
    w is the width of the simulation map,
    h is the height of the simulation map,
    dx is the amount the map has been shifted to the right or left
    dy is the amount the simulation map has been shifted up or down

*/

int convertX (int cartesianX, int mapWidth, int dx){
    //Converts Cartesian x coordinate to screen coordinates
    //This is relative to a specific portion of the application screen
    return (cartesianX+mapWidth/2)-dx;
}

int convertY (int cartesianY, int mapHeight, int dy){
    //Converts Cartesian y coordinate to screen coordinates
    //This is relative to a specific portion of the application screen
    return -cartesianY + (mapHeight/2) -dy;

}

int inverseConvertX(int screenX, int mapWidth, int dx){
    //Screen x coordinate to Cartesian x coordinate
    return screenX + dx -mapWidth*0.5;

}

int inverseConvertY (int screenY, int mapHeight, int dy){
    //Screen y coordinates to Cartesian y coordinate
    return -screenY + (mapHeight/2) -dy;
}

void renderPoint (SDL_Renderer* renderer, int x, int y, int mapWidth, int mapHeight, int dx, int dy){

    //Function takes a single point in normal cartesian form, and converts it into screen coordinates
    //Then the SDL_RenderDrawPoint function is called and a point is coloured on the screen

    int convertedX = convertX (x,mapWidth,dx);
    int convertedY = convertY(y,mapHeight,dy);

    if (convertedX < 0 || convertedX >= mapWidth)
        return;

    if (convertedY < 0 || convertedY >= mapHeight)
        return;

    SDL_RenderDrawPoint (renderer,convertedX,convertedY);


}

void renderLine (SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int mapWidth,
                 int mapHeight, int dx, int dy){

    //Function takes two coordinates in normal cartesian form and converts them to screen coordinates
    //Then the SDL_RenderDrawLine function is called, and a line is drawn between the points

    int convertedX1 = convertX (x1,mapWidth,dx);
    int convertedY1 = convertY (y1,mapHeight,dy);

    int convertedX2 = convertX (x2,mapWidth,dx);
    int convertedY2 = convertY (y2,mapHeight,dy);

    SDL_RenderDrawLine(renderer,convertedX1,convertedY1,convertedX2,convertedY2);

}

void renderRectangle(SDL_Renderer* renderer, Rectangle* rect,
                     Rectangle* simulationRect, int mapWidth, int mapHeight,
                     int dx, int dy){

    int xPos = convertIntDouble(rect->xPosition());
    int yPos = convertIntDouble(rect->yPosition());
    int w = convertIntDouble(rect->getWidth());
    int h = convertIntDouble(rect->getHeight());

    int xConverted = convertX(xPos,mapWidth,dx);
    int yConverted =convertY(yPos,mapHeight,dy);


    /**
        If the rectangle to be rendered is one that is displayed
        with a known position on the screen, there is no need
        to check if it is valid to render, so the caller can
        provide simulationRect as known
    **/
    if (simulationRect != nullptr){


        int simXPos = convertIntDouble(simulationRect->xPosition());
        int simYPos = convertIntDouble(simulationRect->yPosition());
        int simW = convertIntDouble(simulationRect->getWidth());
        int simH = convertIntDouble(simulationRect->getHeight());

        /*
        SDL_Point points[4] = {{xConverted,yConverted},
                               {xConverted+rect.getWidth(), yConverted},
                               {xConverted, yConverted + rect.getHeight()},
                               {xConverted+rect.getWidth(),yConverted + rect.getHeight()}};*/

        SDL_Point points[4] = {{xPos,yPos},
                               {xPos + w, yPos},
                               {xPos, yPos-h},
                               {xPos + w,yPos-h}};

        SDL_Point simPoints[4] = {{simXPos,simYPos},
                                  {simXPos + simW, simYPos},
                                  {simXPos, simYPos -simH},
                                  {simXPos + simW,simYPos -simH}};

        bool validToRender = false;
        int i;
        //Normal Cartesian coordinates comparisons
        for (i = 0; i< 4 ; i++){
            if (pointInRect(simXPos,simYPos,simW,simH,
                            points[i].x,points[i].y)){
                validToRender = true;
                break;
                }
        }

        if (!validToRender){
            for (i = 0; i< 4 ; i++){
                if (pointInRect(xPos,yPos,w,h,
                                simPoints[i].x,simPoints[i].y)){
                    validToRender = true;
                    break;
                }
            }

        }

        if (!validToRender){
            if (!isRectanglesCollided(*rect,*simulationRect)){
                return;
            }

        }
    }

    SDL_Rect toRender = {xConverted,yConverted,w,h};
    SDL_SetRenderDrawColor(renderer,rect->rValue(),rect->gValue(),rect->bValue(),rect->aValue());

    SDL_RenderFillRect(renderer,&toRender);

}

void setupTriangleRendering (Triangle* tri, renderingCoordinates* triangleCoords){

    /**
        This function sets up the coordinates of a triangle that will be used
        to render it onto the screen. They will be unchanged for the duration of
        the program unless the triangle is resized.

        When a triangle is moved ( or the position on the screen is changed by
        the user dragging the mouse, these coordinates are simply recalculated
        relative to the current reference point of the triangle- meaning this function
        doesn't need to be calculated again).
    **/

    triangleCoords->coordinates.clear();

    int x1 = convertIntDouble (tri->xcoordPoint1());
    int y1 = convertIntDouble (tri->ycoordPoint1());
    int x2 = convertIntDouble (tri->xcoordPoint2());
    int y2 = convertIntDouble (tri->ycoordPoint2());
    int x3 = convertIntDouble (tri->xcoordPoint3());
    int y3 = convertIntDouble (tri->ycoordPoint3());
    int referenceX = convertIntDouble(tri->xPosition());
    int referenceY = convertIntDouble(tri->yPosition());

    /**
        To get all of the required coordinates, we loop from
        the smallest x coordinate and largest y coordinate of the triangle,
        to the largest x coordinate and smallest y coordinate of the triangle.
        (Essentially a rectangle)

        Then we check which points are in the triangle by calling
        the pointInTriangle function
    **/

    int smallestX = x1, largestX = x1, smallestY = y1, largestY = y1;

    triangleCoords->startReferencex = referenceX;
    triangleCoords->startReferencey = referenceY;
    triangleCoords->currentReferencex = referenceX;
    triangleCoords->currentReferencey = referenceY;

    if (x2 < smallestX)
        smallestX = x2;

    if (x3 < smallestX)
        smallestX = x3;

    if (x2 > largestX)
        largestX = x2;

    if (x3 > largestX)
        largestX = x3;

    if (y2 < smallestY)
        smallestY = y2;

    if (y3 < smallestY)
        smallestY = y3;

    if (y2 > largestY)
        largestY = y2;

    if (y3 > largestY)
        largestY = y3;


    for (int x  = smallestX; x <= largestX; x++){

        shortRendering singleLine;
        singleLine.x = x;

        bool initialised = false;

        for (int y = largestY; y >= smallestY; y--){

            if (!pointInTriangle(x1,y1,x2,y2,x3,y3,x,y)){
                if (initialised)
                    break;

                continue;
            }

            singleLine.yCoordinates.push_back(y);
            initialised = true;


        }

        if (initialised)
            triangleCoords->coordinates.push_back(singleLine);

    }
}


void renderTriangle (SDL_Renderer* renderer, Triangle* tri,
                     Rectangle* simulationRect,
                     renderingCoordinates* triangleCoords,
                     int mapWidth, int mapHeight, int dx, int dy){



    int referenceX = convertIntDouble(tri->xPosition());
    int referenceY = convertIntDouble(tri->yPosition());

    triangleCoords->currentReferencex = referenceX;
    triangleCoords->currentReferencey = referenceY;

    //simulationRect does not need to be supplied...
    //...if caller knows triangle to be rendered will be valid
    if (simulationRect != nullptr){
        int x1 = convertIntDouble (tri->xcoordPoint1());
        int y1 = convertIntDouble (tri->ycoordPoint1());
        int x2 = convertIntDouble (tri->xcoordPoint2());
        int y2 = convertIntDouble (tri->ycoordPoint2());
        int x3 = convertIntDouble (tri->xcoordPoint3());
        int y3 = convertIntDouble (tri->ycoordPoint3());

        int simXPos = convertIntDouble(simulationRect->xPosition());
        int simYPos = convertIntDouble(simulationRect->yPosition());
        int simW = convertIntDouble(simulationRect->getWidth());
        int simH = convertIntDouble(simulationRect->getHeight());

        SDL_Point points[3] = {{x1,y1},
                               {x2,y2},
                               {x3,y3}};

        SDL_Point simPoints[4] = {{simXPos,simYPos},
                                  {simXPos + simW, simYPos},
                                  {simXPos, simYPos - simH},
                                  {simXPos + simW,simYPos -simH}};


        bool validToRender = false;

        int i;

        for (i = 0; i<3; i++){
            if (pointInRect(simXPos,simYPos,simW,simH,
                            points[i].x,points[i].y)){
                validToRender = true;
                break;
            }
        }

        /**
            One reason I used Cartesian coordinates to check if a shape is valid to render
            as opposed to screen coordinates, is because the pointInTriangle function
            uses normal Cartesian coordinates. So I kept this consistent with the renderRectangle
            function as well
        **/
        if (!validToRender){
            for (i = 0; i<4;i++){
                if (pointInTriangle(x1,y1,x2,y2,x3,y3,simPoints[i].x,simPoints[i].y)){
                    validToRender = true;
                    break;
                }
            }
        }

        if (!validToRender){
            if (!isRectangleTriangleCollided(*simulationRect,*tri))
                return;
        }
    }

    SDL_SetRenderDrawColor(renderer,tri->rValue(), tri->gValue(),tri->bValue(),tri->aValue());

    SDL_Point* renderingPoints = new SDL_Point[maxPixels];
    int numPixels = 0;

    int numCoordinates = triangleCoords->coordinates.size();

    for (int i  = 0; i < numCoordinates; i++){

        /**
           Here, x and y coordinate get realigned in case triangle was moved
           Saves having to set them all up again

           As you can see, if triangle hasn't been moved, referenceX will be
           equal to triangleCoordinates[i].referencePointx, so they both cancel out,
           leaving triangleCoordinates[i].x

        **/
        int x = referenceX + (triangleCoords->coordinates[i].x- triangleCoords->startReferencex);

        int convertedX = convertX (x,mapWidth,dx);

        if ( convertedX < 0 || convertedX >= mapWidth)
            continue;


        int numYCoordinates = triangleCoords->coordinates[i].yCoordinates.size();

        for (int j = 0; j<numYCoordinates; j++){
            int y = referenceY + (triangleCoords->coordinates[i].yCoordinates[j]-
                                  triangleCoords->startReferencey);


            int convertedY = convertY (y,mapHeight,dy);

            if (convertedY < 0 || convertedY >= mapHeight )
                continue;

           renderingPoints[numPixels].x = convertedX;
           renderingPoints[numPixels++].y = convertedY;

        }

        //It's faster to render all of the points currently determined and then repeat,
        //as opposed to rendering them all at the end
        SDL_RenderDrawPoints(renderer,renderingPoints,numPixels);
        numPixels = 0;

    }


    delete [] renderingPoints;
}

void setupCircleRendering (Circle* circle, renderingCoordinates* circleCoords){

    circleCoords->coordinates.clear();

    /**
        Like the setupCircleRendering function, this function
        sets up the coordinates for rendering a circle onto the screen.

        Note that only half of the coordinates are required.
        Due to the symmetry of a circle, the other half can be calculated upon rendering.
    **/

    int x = convertIntDouble(circle->xPosition());
    int y = convertIntDouble(circle->yPosition());
    int r = circle->getRadius();

    int startx = x - r;
    int starty = y-r;

    int endx = x + r;
    int endy = y;

    circleCoords->startReferencex = x;
    circleCoords->startReferencey = y;
    circleCoords->currentReferencex = x;
    circleCoords->currentReferencey = y;

    for (int pointx = startx; pointx <= endx; pointx++){

        shortRendering singleLine;
        singleLine.x = pointx;

        bool initialised = false;

        for (int pointy = starty; pointy <= endy; pointy++){

            if (pointInCircle(pointx,pointy,r,x,y)){
                singleLine.yCoordinates.push_back(pointy);
                initialised = true;
            }
        }

        if (initialised)
            circleCoords->coordinates.push_back(singleLine);
    }
}

void renderCircle (SDL_Renderer* renderer, Circle* circle,
                   renderingCoordinates* circleCoords,
                   int mapWidth, int mapHeight, int dx, int dy){

    int referenceX = convertIntDouble(circle->xPosition());
    int referenceY = convertIntDouble(circle->yPosition());
    int startReferenceX = circleCoords->startReferencex;
    int startReferenceY = circleCoords->startReferencey;
    circleCoords->currentReferencex = referenceX;
    circleCoords->currentReferencey = referenceY;
    int r = convertIntDouble(circle->getRadius());


    int numCoordinates = circleCoords->coordinates.size();

    SDL_Point* renderingPoints = new SDL_Point[maxPixels];
    int numPixels = 0;

    SDL_SetRenderDrawColor (renderer,circle->rValue(),circle->gValue(),circle->bValue(),circle->aValue());

    for (int i = 0; i < numCoordinates; i++){
        int pointx =  referenceX + (circleCoords->coordinates[i].x- startReferenceX);

        int convertedX = convertX (pointx,mapWidth,dx);

        if (convertedX <0 || convertedX > mapWidth)
            continue;

        int numYCoords = circleCoords->coordinates[i].yCoordinates.size();

        for (int j = 0; j < numYCoords; j++){

            int pointy = referenceY + (circleCoords->coordinates[i].yCoordinates[j]- startReferenceY);
            int convertedY = convertY (pointy,mapHeight,dy);


            if (convertedY >= 0 && convertedY <= mapHeight){

				/*I was using this for debugging purposes*/
                /*
                if ( (pointx-referenceX)*(pointx-referenceX)+
                      (pointy-referenceY)*(pointy-referenceY)>= r*r)
                    printf("broken: %d %d. Ref x: %d. ref y: %d\n", circleCoords->coordinates[i].x,
                           circleCoords->coordinates[i].yCoordinates[j],
                           startReferenceX,startReferenceY );*/

                renderingPoints[numPixels].x = convertedX;
                renderingPoints[numPixels++].y = convertedY;

            }

            int diffy = referenceY - pointy;

            //Same y coordinate as radius, so point does not have a corresponding...
            //...reflected point
            if (diffy == 0)
                continue;

            int reflectedY =  pointy+diffy*2;

            int convertedreflectedY = convertY(reflectedY,mapHeight,dy);

            if (convertedreflectedY >= 0 && convertedreflectedY <= mapHeight){
                renderingPoints[numPixels].x = convertedX;
                renderingPoints[numPixels++].y = convertedreflectedY;
            }
        }

        SDL_RenderDrawPoints(renderer,renderingPoints, numPixels);
        numPixels = 0;
    }

    delete [] renderingPoints;
}

//Defunct, didn't work in every case as well
/*
bool polygonValidToRender (int numPoints, SDL_Point points[], int mapWidth, int mapHeight){
*/
    /**
        Function checks if a polygon is valid to render.
        The most obvious method to check if a polygon is valid to render,
        is if one of the points lies within the simulation map.

        However, all points on a polygon can lie outside the simulation map
        and the polygon can still have a portion of itself within the simulation map,
        and the function also checks for this
         by looking at each of its points
        and seeing if one of them lies within the simulation map
    **/
/*
    int xlessThan0 = 0;
    int xmoreThanWidth = 0;
    int ylessThan0 = 0;
    int yMoreThanHeight = 0;

    bool atLeastOnexInRange = false;
    bool atLeastOneyInRange = false;

    for (int i = 0; i<numPoints; i++){

        if ((points[i].x >= 0 && points[i].x < mapWidth)&&
            (points[i].y >= 0 && points[i].y < mapHeight)){
             return true;
        }

        if (points[i].x < 0)
            xlessThan0++;

        else if (points[i].x >= mapWidth)
            xmoreThanWidth++;

        else
            atLeastOnexInRange = true;

        if (points[i].y < 0)
            ylessThan0++;

        else if (points[i].y >= mapHeight)
            yMoreThanHeight++;

        else
            atLeastOneyInRange = true;
    }


    if (xlessThan0 > 0 && xmoreThanWidth >0 && atLeastOneyInRange)
        return true;

    if (ylessThan0 > 0 && yMoreThanHeight > 0 && atLeastOnexInRange)
        return true;

    if (xlessThan0 > 0 && xmoreThanWidth >0 &&
        ylessThan0 > 0 && yMoreThanHeight > 0 )
        return true;

    return false;

}
*/

/**
//Used to check if a triangle is a right angle or not (defunct)
bool isRightAngle (Triangle& tri, int* x1hyp, int* y1hyp, int* x2hyp, int* y2hyp,
                   int* rightAnglex, int* rightAngley){


    bool recordLongestSide = true;
    bool recordRightangle = true;


    if (x1hyp == nullptr || y1hyp == nullptr || x2hyp == nullptr || y2hyp == nullptr)
        recordLongestSide = false;

    if (rightAnglex == nullptr || rightAngley == nullptr)
        recordRightangle = false;


    int x1 = convertIntDouble (tri.xcoordPoint1());
    int y1 = convertIntDouble (tri.ycoordPoint1());
    int x2 = convertIntDouble (tri.xcoordPoint2());
    int y2 = convertIntDouble (tri.ycoordPoint2());
    int x3 = convertIntDouble (tri.xcoordPoint3());
    int y3 = convertIntDouble (tri.ycoordPoint3());

    //We add 0.5 to avoid truncation errors when converting to an integer type
    int distance1and2 = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + 0.5;
    int distance1and3 = (x1-x3)*(x1-x3) + (y1-y3)*(y1-y3) + 0.5;
    int distance2and3 = (x2-x3)*(x2-x3) + (y2-y3)*(y2-y3) + 0.5;

    if (distance1and2 > distance1and3 && distance1and2 > distance2and3){


        //Function will record coordinates of longest side even if it's not a right angle
        if (recordLongestSide){
            *x1hyp = x1;
            *y1hyp = y1;
            *x2hyp = x2;
            *y2hyp = y2;


        }

        if (recordRightangle){

            *rightAnglex = x3;
            *rightAngley = y3;
        }


        if (distance2and3 + distance1and3 != distance1and2)
            return false;

        return true;
    }

    if (distance1and3 > distance1and2 && distance1and3 > distance2and3){


        if (recordLongestSide){
            *x1hyp = x1;
            *y1hyp = y1;
            *x2hyp = x3;
            *y2hyp = y3;


        }

        if (recordRightangle){
            *rightAnglex = x2;
            *rightAngley = y2;
        }

         if (distance2and3 + distance1and2 != distance1and3)
            return false;

        return true;
    }

    if (distance2and3 > distance1and2 && distance2and3 > distance1and3){


        if (recordLongestSide){

            *x1hyp = x2;
            *y1hyp = y2;
            *x2hyp = x3;
            *y2hyp = y3;

        }

        if (recordRightangle){
            *rightAnglex = x1;
            *rightAngley = y1;
        }

        if (distance1and3 + distance1and2 != distance2and3)
            return false;

        return true;
    }

    return false;



}

**/

/**
void renderTriangleDefunct (SDL_Renderer* renderer, Triangle& tri,
                            int mapWidth, int mapHeight, int dx, int dy, bool bypass){


    int x1 = convertIntDouble (tri.xcoordPoint1());
    int y1 = convertIntDouble (tri.ycoordPoint1());
    int x2 = convertIntDouble (tri.xcoordPoint2());
    int y2 = convertIntDouble (tri.ycoordPoint2());
    int x3 = convertIntDouble (tri.xcoordPoint3());
    int y3 = convertIntDouble (tri.ycoordPoint3());

    if (x1 == x2 && x1 == x3 &&
        y1 == y2 && y1 == y3){

            renderPoint (renderer,x1,y1, mapWidth,mapHeight,dx,dy);


            printf ("Returning Point: (%d,%d), (%d,%d), (%d,%d)\n", x1,y1,x2,y2,x3,y3);
            return;

    }


     if (convertIntDouble(abs (x1-x2)) == 1|| convertIntDouble(abs (x1-x3)) == 1 ||
        convertIntDouble(abs (x2-x3)) == 1 || convertIntDouble(abs (y1-y2)) == 1 ||
        convertIntDouble(abs (y1-y3)) == 1 || convertIntDouble(abs (y2-y3)) == 1){
                 printf ("Returning Line: (%d,%d), (%d,%d), (%d,%d)\n", x1,y1,x2,y2,x3,y3);
       return;

    }



    renderLine (renderer,x1,y1,x2,y2,mapWidth,mapHeight,dx,dy);
    renderLine (renderer,x1,y1,x3,y3,mapWidth,mapHeight,dx,dy);
    renderLine (renderer,x3,y3,x2,y2,mapWidth,mapHeight,dx,dy);




    //hyp = hypotenuse
    int x1hyp, y1hyp, x2hyp, y2hyp, rightAnglex, rightAngley;

    /**
        bypass is used if we know the triangle is close enough to be a right angle
        like when the function is called recursively with two smaller right angles, that may have
        a slight precision error if the midpoint of the hypotenuse are not integers
    **/


    /**

    if (isRightAngle(tri, &x1hyp, &y1hyp, &x2hyp, &y2hyp, &rightAnglex,&rightAngley)
                     || bypass){


        int midpointHypx = (x1hyp+x2hyp)/2;
        int midpointHypy = (y1hyp+y2hyp)/2;


        int corner1x = rightAnglex;
        int corner1y = midpointHypy;

        int corner2x = midpointHypx;
        int corner2y = rightAngley;
        int width,height;

        int topLeftx, topLefty;

        if (midpointHypx < rightAnglex){
            topLeftx = midpointHypx;
            width = rightAnglex - midpointHypx;

        }

        else{
            topLeftx = rightAnglex;
            width = midpointHypx - rightAnglex;
        }

        if (rightAngley > midpointHypy){
            topLefty = rightAngley;
            height = rightAngley - midpointHypy;

        }

        else{
            topLefty = midpointHypy;
            height = midpointHypy - rightAngley;

        }


        Rectangle rect (topLeftx,topLefty,DEFAULT_FRAME_RATE,width,height);

        renderRectangle(renderer,rect,mapWidth,mapHeight,dx,dy);


        if (corner1x == x1hyp || corner1y == y1hyp){
            Triangle tri1 (midpointHypx,midpointHypy,corner1x,corner1y,x1hyp,y1hyp,DEFAULT_FRAME_RATE);
            Triangle tri2 (midpointHypx,midpointHypy,corner2x,corner2y,x2hyp,y2hyp,DEFAULT_FRAME_RATE);

            renderTriangleDefunct(renderer,tri1,mapWidth,mapHeight,dx,dy,false);
            renderTriangleDefunct(renderer,tri2,mapWidth,mapHeight,dx,dy,false);
        }

        else{

            Triangle tri1 (midpointHypx,midpointHypy,corner1x,corner1y,x2hyp,y2hyp,DEFAULT_FRAME_RATE);
            Triangle tri2 (midpointHypx,midpointHypy,corner2x,corner2y,x1hyp,y1hyp,DEFAULT_FRAME_RATE);

            renderTriangleDefunct(renderer,tri1,mapWidth,mapHeight,dx,dy,false);
            renderTriangleDefunct(renderer,tri2,mapWidth,mapHeight,dx,dy,false);

        }



    }


}

**/
