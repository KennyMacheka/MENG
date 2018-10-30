#ifndef BINARY_SEARCH_SHAPES_H_INCLUDED
#define BINARY_SEARCH_SHAPES_H_INCLUDED

#include <vector>
#include <stdio.h>
#include "physics_engine_global_vars.h"

namespace physicsEngine{
/**This function is used to check if a shape with a certain instantiation number already exists in the physics engine
    This is done by using a binary search method, that looks at the instantiation number of a shape.
    When a shape object is created, its instantiation number will be bigger than all other shape objects
    that have already been created
    So a shape list is guaranteed to have the instantiation numbers in order.
    Hence, a binary search can be used to locate a shape.
**/
template <class T>
bool locateShape ( std::vector<T*> shapes, int instNum, int numShapes, int* position){

    if (numShapes == 0)
        return false;

    int startPoint = 0;
    int endPoint = numShapes;
    int checkPoint = numShapes/2;

    while (true){
        int cmp = shapes[checkPoint]->Id();

        if (cmp == instNum){
            if (position != nullptr)
                *position = checkPoint;

            return true;
        }

        if (cmp > instNum){
            endPoint = checkPoint-1;
        }

        else if (cmp < instNum){

            startPoint = checkPoint +1;
        }

        //Shape not found
        if (startPoint > endPoint || startPoint == numShapes || endPoint < 0)
            return false;
        checkPoint = (startPoint+endPoint)/2;
    }
}
}
#endif // BINARY_SEARCH_SHAPES_H_INCLUDED
