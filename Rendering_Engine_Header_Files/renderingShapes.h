#ifndef RENDERINGSHAPES_H_INCLUDED
#define RENDERINGSHAPES_H_INCLUDED

#include <SDL.h>
#include <vector>

#include "rendering_engine_global_vars.h"
#include "shape.h"


//This is used to store all of the coordinates of a circle or triangle before rendering
//That way, rendering is made faster;
struct shortRendering{
    /**
        This is used to store the y coordinates of a circle or triangle before rendering
        with a particular x coordinate (Cartesian)
    **/
    int x;

    std::vector<int> yCoordinates;

    //shortRendering(){yCoordinates.reserve(1000);}
};

struct renderingCoordinates{

    /**
        This structure is used to store all of the x and y coordinates from instances
        of shortRendering all in one place. One instance of renderingCoordinates
        will have a reference of all of the coordinates of a circle or triangle
    **/

    /**
        These  following coordinates are to ensure we don't have to get all of the coordinates
        again when determining the rendering coordinates of a shape if the shape is moved-
        unless it is resized
    **/
    int startReferencex;
    int startReferencey;

    int currentReferencex;
    int currentReferencey;

    std::vector<shortRendering> coordinates;

    //renderingCoordinates(){coordinates.reserve(10000);}

};

void setupTriangleRendering (physicsEngine::Triangle* tri, renderingCoordinates* triangleCoordinates);
void setupCircleRendering (physicsEngine::Circle* circle,  renderingCoordinates* circleCoordinates);


bool pointInTriangle (int x1, int y1, int x2, int y2,
                      int x3, int y3, int x, int y);

//If a triangle object is provided instead of direct coordinates
bool pointInTriangle (physicsEngine::Triangle* tri, int x, int y);

bool pointInRect (int topLeftx, int topLefty, int w ,int h, int x, int y);
//If rectangle object is provided instead of direct coordinates
bool pointInRect (physicsEngine::Rectangle* rect, int x, int y);

bool pointInCircle (physicsEngine::Circle* circle, int x, int y);
bool pointInCircle (int centrex, int centrey, int radius, int x, int y);


void renderTriangle (SDL_Renderer* renderer, physicsEngine::Triangle* tri,
                     physicsEngine::Rectangle* simulationRect,
                     renderingCoordinates* triangleCoordinates,
                     int mapWidth, int mapHeight, int dx, int dy);

void renderCircle (SDL_Renderer* renderer, physicsEngine::Circle* circle,
                   renderingCoordinates* circleCoordinates,
                   int mapWidth, int mapHeight, int dx, int dy);

void renderRectangle(SDL_Renderer* renderer, physicsEngine::Rectangle* rect,
                     physicsEngine::Rectangle* simulationRect,
                     int mapWidth, int mapHeight, int dx, int dy);

int convertX (int cartesianX, int mapWidth, int dx);
int convertY (int cartesianY, int mapHeight, int dy);
int inverseConvertX (int screenX, int mapWidth, int dx);
int inverseConvertY (int screenY, int mapHeight, int dy);



#endif // RENDERINGSHAPES_H_INCLUDED
