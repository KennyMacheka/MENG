#ifndef SHAPE_CLICKING_DRAGGING_H_INCLUDED
#define SHAPE_CLICKING_DRAGGING_H_INCLUDED

#include <SDL.h>
#include "shape.h"
#include "renderingShapes.h"
#include <vector>


//This is used to determine whether clicking on a button should be handled or not
enum MOUSE_STATE{
    CLICKED = 0,
    NOT_CLICKED = 1,
};



//If a shape is clicked, this structure will store information about it
struct shapeClicked{

    shapeClicked(bool isResizing);
    ~shapeClicked();

    int shapeNum;
    int instNum;


    MOUSE_STATE mouseState;

    enum SHAPE_NUMBER{
        NO_SHAPE = 0,
        RECTANGLE,
        TRIANGLE,
        CIRCLE,
    };

    enum RECT_RESIZE{
        NO_RECT_RESIZE = -1,
        TOP_LEFT,
        TOP_MIDDLE,
        TOP_RIGHT,
        RIGHT_MIDDLE,
        BOTTOM_RIGHT,
        BOTTOM_MIDDLE,
        BOTTOM_LEFT,
        LEFT_MIDDLE
    };

    enum TRI_RESIZE{
        NO_TRI_RESIZE = -1,
        POINT1,
        POINT2,
        POINT3
    };


    //Only one of these will be initialised at a time, based on which shape is clicked
    union Shapes{
        physicsEngine::Rectangle* rect;
        physicsEngine::Triangle* tri;
        physicsEngine::Circle* circle;
    }shape;

    union ShapesCoordinates{
        renderingCoordinates* tri;
        renderingCoordinates* circle;
    }shapeRenderCoords;

    struct Resizing{

        struct Rect{
            physicsEngine::Circle* topLeftCorner;
            physicsEngine::Circle* topMiddle;
            physicsEngine::Circle* topRightCorner;
            physicsEngine::Circle* rightMiddle;
            physicsEngine::Circle* bottomRightCorner;
            physicsEngine::Circle* bottomMiddle;
            physicsEngine::Circle* bottomLeftCorner;
            physicsEngine::Circle* leftMiddle;

            physicsEngine::Circle** allRectCircles[8] = {&topLeftCorner,&topMiddle,&topRightCorner,
                                                        &rightMiddle,&bottomRightCorner,&bottomMiddle,&bottomLeftCorner,
                                                        &leftMiddle};
            int resizeNum;

        }rect;


        struct Tri{
            physicsEngine::Circle* point1Circle;
            physicsEngine::Circle* point2Circle;
            physicsEngine::Circle* point3Circle;

            physicsEngine::Circle** allTriCircles[3] = {&point1Circle,&point2Circle,&point3Circle};
            int resizeNum;
        }tri;

    }resizeShape;

    struct renderCoords{

        struct Rect{
            renderingCoordinates topLeftCorner;
            renderingCoordinates topMiddle;
            renderingCoordinates topRightCorner;
            renderingCoordinates rightMiddle;
            renderingCoordinates bottomRightCorner;
            renderingCoordinates bottomMiddle;
            renderingCoordinates bottomLeftCorner;
            renderingCoordinates leftMiddle;

            renderingCoordinates* allRectCirclesCoords[8] = {&topLeftCorner,&topMiddle,&topRightCorner,
                                                             &rightMiddle,&bottomRightCorner,&bottomMiddle,&bottomLeftCorner,
                                                             &leftMiddle};
        }circleRect;

        struct Tri{
            renderingCoordinates point1Circle;
            renderingCoordinates point2Circle;
            renderingCoordinates point3Circle;

            renderingCoordinates* allTriCirclesCoords[3] = {&point1Circle,&point2Circle,&point3Circle};
        }circleTri;

    }coords;


    void addRect(physicsEngine::Rectangle* rect, int mousex, int mousey);

    void addCircle(physicsEngine::Circle* circle, renderingCoordinates* circleCoords,
                   int cartesianMousex, int cartesianMousey);

    void addTri(physicsEngine::Triangle* tri, renderingCoordinates* triCoords,
                int cartesianMousex, int cartesianMousey);

    void highlightShape();
    void unhilightShape();
    void removeShape();

    void getDifferenceMousex (int* placeHolder);
    void getDifferenceMousey (int* placeHolder);

    bool isSetup();
    bool isShapeHighlighted();
    void updateResizeCircles();

    //When we need to get the information of a shape being clicked, this class will provide functions for this
    //Saves having to check the type of shape being clicked every time
    double getMass();
    double getXVelocity();
    double getYVelocity();
    double getXAcceleration();
    double getYAcceleration();
    bool getGravityEffect();
    uint8_t getRValue();
    uint8_t getGValue();
    uint8_t getBValue();
    uint8_t getAValue();

    void changeColour(int dr, int dg, int db, int da);
    void changeGravityEffect();




    private:
        bool resizingNeeded;
        bool shapeSetup;
        //m for mouse
        //This is the difference between one of the x & y coordinates of the shape and the mouse
        int differenceMousex;
        int differenceMousey;

        void setupRectCircles();
        void updateRectCircles();


        void setupTriCircles();
        void updateTriCircles();

};

#endif // SHAPE_CLICKING_DRAGGING_H_INCLUDED
