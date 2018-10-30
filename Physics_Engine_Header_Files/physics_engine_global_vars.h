#ifndef __PHYSICS_GLOBAL_VARS_H
#define __PHYSICS_GLOBAL_VARS_H

namespace physicsEngine{
//The size of the simulation map
extern const int simulationWidth;
extern const int simulationHeight;


extern const double wallMass;


extern double gravitationalAcceleration;
extern double circleToCircleRestitution;
extern double circleToRectangleRestitution;
extern double circleToTriangleRestitution;
extern double circleToBoundaryRestitution;

extern double rectangleToTriangleRestitution;
extern double rectangleTorectangleRestitution;
extern double rectangleToBoundaryRestitution;

extern double  triangleToTriangleRestitution;
extern double  triangleToBoundaryRestitution;

extern const double DEFAULT_MASS;
extern const double MAX_MASS;
extern const double DEFAULT_RECT_HEIGHT;
extern const double DEFAULT_RECT_WIDTH;

extern int ShapeInstantiationNumber;

extern int DEFAULT_FRAME_RATE;
extern const int WALL_THICKNESS;
}
#endif // __PHYSICS_GLOBAL_VARS_H

