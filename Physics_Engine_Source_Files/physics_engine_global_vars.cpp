#include "physics_engine_global_vars.h"
namespace physicsEngine{
//The size of the simulation map constant
const int simulationWidth = 3600;
const int simulationHeight = 3600;

//The mass of the walls (objects on the screen will not have a mass anywhere near this)
const double wallMass = 10000000.0;

//The coefficients of restitution between the shapes and the boundaries
double gravitationalAcceleration = 0.0;
double circleToCircleRestitution = 1.0;
double circleToRectangleRestitution = 1.0;
double circleToTriangleRestitution = 1.0;
double circleToBoundaryRestitution = 1.0;

double rectangleToTriangleRestitution = 1.0;
double rectangleTorectangleRestitution = 1.0;
double rectangleToBoundaryRestitution = 1.0;

double triangleToTriangleRestitution = 1.0;
double triangleToBoundaryRestitution = 1.0;


//The default mass of an object is 5kg
const double DEFAULT_MASS = 5.0;
//The maximum mass allowed for any objects
const double MAX_MASS = 10000.0;

const double DEFAULT_RECT_WIDTH = 20;
const double DEFAULT_RECT_HEIGHT = 20;

//Used to give a unique ID to a shape object
int ShapeInstantiationNumber = 0;

int DEFAULT_FRAME_RATE = 200;

//Thickness of the four walls
const int WALL_THICKNESS = 50;

}


