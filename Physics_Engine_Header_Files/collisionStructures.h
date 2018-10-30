#ifndef COLLISIONSTRUCTURES_H_INCLUDED
#define COLLISIONSTRUCTURES_H_INCLUDED

namespace physicsEngine{
struct Graph;
struct CollisionInfo;

/**
    I put these structures in one file as they need to be accessed by both
    collisionDetection.h and collisionHandlin.h
**/

//Used to create a graph of connections
struct Graph{
    long int hashNum;
    int numObjects;
    bool shouldHandleCollision;
    bool sameRestitution;
    double restitutionValue;

    Graph();
};

struct CollisionInfo{

    Graph connection;
    int counter;
    CollisionInfo(){counter = 0;}

};

}
#endif // COLLISIONSTRUCTURES_H_INCLUDED
