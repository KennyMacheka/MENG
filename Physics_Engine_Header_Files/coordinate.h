#ifndef COORDINATE_H_INCLUDED
#define COORDINATE_H_INCLUDED

namespace physicsEngine{
//A generic class not just used to specify the position of an object, but also the velocity and acceleration
//If it can be used for velocity and acceleration, then technically it should already be vector...
//...however, the name vector is already defined in the C++ standard library, and I didn't want to create confusion.
class Coordinate{

public:
    double x;
    double y;
};

}
#endif // COORDINATE_H_INCLUDED
