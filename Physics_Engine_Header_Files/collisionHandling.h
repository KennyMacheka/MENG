#ifndef COLLISIONHANDLING_H_INCLUDED
#define COLLISIONHANDLING_H_INCLUDED

#include "collisionStructures.h"

namespace physicsEngine{
//This function is used to check for collisions between two types of shapes
//It takes in generic functions that will be common to all triangles, rectangles and circles
template <class shape1, class shape2>
void checkCollision (std::vector<shape1*>& shapes1, std::vector<shape2*>& shapes2,
                     int numShapes1, int numShapes2,
                     bool (shape2::*locateShape1) (int,int*),
                     bool (shape1::*locateShape2) (int,int*),
                     bool (*collisionFunction)(shape1&,shape2&),
                     bool (shape1::*addShape2)(shape2*),
                     bool (shape2::*addShape1) (shape1*),
                     bool (shape2::*removeShape1)(shape1*),
                     bool (shape1::*removeShape2)(shape2*),
                     bool shapesIdentical){

        //A nested loop to loop through each combination of both types of shapes in consideration
        for (int i = 0; i<numShapes1;i++){
            int start = 0;

            //If we're dealing with the same shapes, we don't want to check for a collision with itself
            //So the inner loop must start 1 past i
            if (shapesIdentical)
                start = i+1;


            for (int j = start; j<numShapes2;j++){

                int location1, location2;
                bool alreadyConnected = false;
                bool collisionDetected = false;


                //Connections to the wall should behave as an undirected edge
                bool shape1IsWall = false;
                bool shape2IsWall = false;

                if (shapes1[i]->getMass() == wallMass)
                    shape1IsWall = true;

                if (shapes2[j]->getMass() == wallMass)
                    shape2IsWall = true;

                //No need to check for collisions between two boundary walls
                if (shape1IsWall && shape2IsWall)
                    continue;

                //Check if shapes have already previously collided
                if ((shapes1[i]->*locateShape2)(shapes2[j]->Id(),&location1) ) {

                    if (!shape2IsWall)
                        (shapes2[j]->*locateShape1)(shapes1[i]->Id(),&location2);

                    alreadyConnected = true;

                }

                else{
                    //Walls don't directly have objects connected to them
                    if (shape1IsWall){
                        //shape2IsWall is guaranteed to be false...
                        //...as the loop would have stopped by this stage
                        if ((shapes2[j]->*locateShape1)(shapes1[i]->Id(),&location2))
                            alreadyConnected = true;
                    }
                }

                //Check if shapes have collided
                if ( (*collisionFunction)( *shapes1[i], *shapes2[j]) ){
                    collisionDetected = true;

                    //Add shapes to each others connection lists if they are not already connected
                    if (!alreadyConnected){

                        //printf("Collision detected %.2f, %.2f\n",shapes1[i]->xSpeed(), shapes2[j]->xSpeed());
                        if (!shape1IsWall)
                            (shapes1[i]->*addShape2)(shapes2[j]);

                        if (!shape2IsWall)
                            (shapes2[j]->*addShape1)(shapes1[i]);


                        //We don't want this to be true if two shapes were already under collision...
                        //...As collisions only need to be handled once
                        if (!shape1IsWall)
                            shapes1[i] -> recentlyDetectedCollision = true;

                        if (!shape2IsWall)
                            shapes2[j] -> recentlyDetectedCollision = true;

                    }

                    else{
                        //printf("Already connected %.2f, %.2f\n",shapes1[i]->xSpeed(), shapes2[j]->xSpeed());
                    }

                }

                else{
                    if (alreadyConnected){
                        //Remove shapes from each others connection lists, as they are no longer under collision
                        if (!shape1IsWall)
                            (shapes1[i]->*removeShape2)(shapes2[j]);

                        if (!shape2IsWall)
                            (shapes2[j]->*removeShape1)(shapes1[i]);
                    }
                }

            }


        }


}

template <class shape>
void connectionsTraversal (Graph* graph, shape* object, int shapesLoopedThrough[]){

	   /**

                At this stage, we'll definitely be handling collisions for this graph.

                We need to determine whether the coefficient of restitution
                between the objects are the same.

                If they are, then the centre of momentum method can be used to calculate the resultant velocities
                of the objects.

         **/

    //This function traverses the connections of an object, and the connections of each of those connections
    //Refer to the establishConnections function for a better understanding, as it works in conjunction with that

    /**
        The function additionally works to check if all connected bodies have the same coefficient of restitution or not

        If they do, then the centre of momentum method can be used to calculate the resultant velocities
        of the objects.

    **/

    //If object has already been visited, nothing more needs to be done
    if (shapesLoopedThrough[object->Id()] == 1)
        return;

    graph->numObjects += 1;

    //Determines whether a collision should be handled in the current frame for this connection
    //Objects that have already collided will have recentlyDetectedCollision set to false
    if (!graph->shouldHandleCollision && object->recentlyDetectedCollision){
        graph->shouldHandleCollision = true;

    }


    shapesLoopedThrough[object->Id()] = 1;
    object->recentlyDetectedCollision = false;


    int i;
    //Repeat procedure with other objects collided with current objects
    for (i = 0; i< object->numTriangles;i++){

         //The default value of graph->restitutionValue is 1
         //We take the first available element and set this as the coefficient of restitution
         //Then we check if all connections after that have the came coefficient of restitution
         //If they don't graph->sameRestitution is set to false
         if (i == 0 && graph->restitutionValue == -1){
            graph->sameRestitution = true;
            graph->restitutionValue = *object->restitutionTriangle;
        }

        else if (i == 0 && graph->sameRestitution){
            if (*object->restitutionTriangle != graph->restitutionValue)
                graph->sameRestitution = false;

        }


        connectionsTraversal(graph,object->triangles[i],shapesLoopedThrough);
    }

    for (i = 0; i< object->numCircles;i++){

        if (i == 0 && graph->restitutionValue == -1){
            graph->sameRestitution = true;
            graph->restitutionValue = *object->restitutionCircle;
        }

        else if (i == 0 && graph->sameRestitution){
            if (*object->restitutionCircle != graph->restitutionValue)
                graph->sameRestitution = false;

        }

        connectionsTraversal(graph,object->circles[i],shapesLoopedThrough);

    }

    for (i = 0; i< object->numRects;i++){

        bool isWall = false;

        if (object->rectangles[i]->getMass() == wallMass)
            isWall = true;

        if (i == 0 && graph->restitutionValue == -1){
            graph->sameRestitution = true;


            if (isWall){
                graph->restitutionValue = *object->restitutionWall;
                continue;
            }

            else
                graph->restitutionValue = *object->restitutionRect;
        }

        else if (i == 0 && graph->sameRestitution){

            if (isWall){

                if (*object->restitutionWall != graph->restitutionValue)
                    graph->sameRestitution = false;

                continue;

            }

            else{

                if (*object->restitutionRect != graph->restitutionValue)
                    graph->sameRestitution = false;
            }

        }
        connectionsTraversal(graph,object->rectangles[i],shapesLoopedThrough);

    }
}

template <class T>
void sameResitutionTraversal (T* shape, Graph connection, int objectsLoopedThrough[],
                              fraction *totalMomentumx, fraction*totalMomentumy,
                              fraction *totalMass){

    if (objectsLoopedThrough[shape->Id()] == 1)
        return;

    *totalMomentumx += shape->getMass_f() * shape->xSpeed_f();
    *totalMomentumy += shape->getMass_f() * shape->ySpeed_f();

    *totalMass += shape->getMass_f();
    objectsLoopedThrough[shape->Id()] = 1;

    if (shape->getMass() == wallMass){
        return;
    }


      //Here, we loop through all shapes that have direct
    for (int j = 0; j<3;j++){
        int numShapes;

        if (j == 0)
            numShapes = shape->numTriangles;

        else if (j == 1)
            numShapes = shape->numCircles;

        else if (j == 2)
            numShapes = shape->numRects;

        for (int i = 0; i<numShapes; i++){


            if (j == 0)
                sameResitutionTraversal(shape->triangles[i], connection, objectsLoopedThrough,
                                        totalMomentumx,totalMomentumy,totalMass);

            else if (j == 1)
                sameResitutionTraversal(shape->circles[i], connection, objectsLoopedThrough,
                                        totalMomentumx,totalMomentumy,totalMass);

            else if (j == 2)
                sameResitutionTraversal(shape->rectangles[i], connection, objectsLoopedThrough,
                                        totalMomentumx,totalMomentumy,totalMass);
        }
    }
}

template <class T>
void changeSpeedSameRestitution (T* shape, Graph connection, fraction centreMomentumx,
                                fraction centreMomentumy, int decimalPlaces,int objectsLoopedThrough[]){


    //We may have already considered objects
    //And we don't need to change the speed of a wall/boundary
    if (objectsLoopedThrough[shape->Id()] == 1 || shape->getMass() == wallMass)
        return;

    int i,j;

    fraction finalVelx_f = centreMomentumx - (shape->xSpeed_f()-centreMomentumx)*connection.restitutionValue;
    fraction finalVely_f = centreMomentumy - (shape->ySpeed_f()-centreMomentumy)*connection.restitutionValue;

    double finalVelx = finalVelx_f.decimalVersion();
    double finalVely = finalVely_f.decimalVersion();

    finalVelx = roundNumber(finalVelx,decimalPlaces);
    finalVely = roundNumber(finalVely,decimalPlaces);

    shape->changeSpeedx(finalVelx,true);
    shape->changeSpeedy(finalVely,true);

    objectsLoopedThrough[shape->Id()] = 1;

    for (j = 0; j<3;j++){
        int numShapes;
        if (j == 0)
            numShapes = shape->numTriangles;

        else if (j == 1)
            numShapes = shape->numCircles;

        else if (j == 2)
            numShapes = shape->numRects;


        for (i = 0; i<numShapes;i++){

            if (j == 0)
                changeSpeedSameRestitution(shape->triangles[i], connection,centreMomentumx,
                                           centreMomentumy, decimalPlaces,objectsLoopedThrough);

            else if (j == 1)
                 changeSpeedSameRestitution(shape->circles[i], connection,centreMomentumx,
                                            centreMomentumy, decimalPlaces,objectsLoopedThrough);

            else if (j == 2)
                 changeSpeedSameRestitution(shape->rectangles[i], connection,centreMomentumx,
                                            centreMomentumy, decimalPlaces, objectsLoopedThrough);
        }

    }
}

template <class T>
void changeSpeedDifferentRestitution (T* shape, int objectsLoopedThrough[], int* columnPosition, int numObjects,
                                      std::vector <fraction> finalVelocitiesX, std::vector<fraction> finalVelocitiesY,
                                      int decimalPlaces, bool changeXspeeds, bool changeYspeeds){

    if (!changeXspeeds && !changeYspeeds)
        return;


    if (changeXspeeds){

        double velocityX = finalVelocitiesX[*columnPosition].decimalVersion();
        shape -> changeSpeedx (roundNumber (velocityX, decimalPlaces),true);
    }

    if (changeYspeeds){
        double velocityY = finalVelocitiesY[*columnPosition].decimalVersion();
        shape -> changeSpeedy (roundNumber (velocityY, decimalPlaces),true);
    }

    objectsLoopedThrough[shape->Id()] = 1;

    for (int j = 0; j < 3 && *columnPosition != numObjects; j++){

        int numShapes;

        if (j == 0)
            numShapes = shape->numTriangles;

        else if (j == 1)
            numShapes = shape->numCircles;

        else if (j == 2)
            numShapes = shape->numRects;


        for (int i = 0; i<numShapes; i++){
            int instNum;
            switch (j){

                case 0:
                    instNum = shape->triangles[i]->Id();
                    break;

                case 1:
                    instNum = shape->circles[i]->Id();
                    break;

                case 2:
                    instNum = shape->rectangles[i]->Id();
                    break;
            }

            if (objectsLoopedThrough[instNum] == 1)
                continue;

            *columnPosition += 1;

            if (*columnPosition == numObjects)
                break;

            if (j == 0)
                changeSpeedDifferentRestitution(shape->triangles[i], objectsLoopedThrough,
                                                columnPosition, numObjects, finalVelocitiesX,
                                                finalVelocitiesY,decimalPlaces,changeXspeeds,changeYspeeds);
            else if (j == 1)
                changeSpeedDifferentRestitution(shape->circles[i], objectsLoopedThrough,
                                                columnPosition, numObjects, finalVelocitiesX,
                                                finalVelocitiesY,decimalPlaces,changeXspeeds,changeYspeeds);
            else
                changeSpeedDifferentRestitution(shape->rectangles[i], objectsLoopedThrough,
                                                columnPosition, numObjects, finalVelocitiesX,
                                                finalVelocitiesY,decimalPlaces,changeXspeeds,changeYspeeds);
        }
    }

}


template <class T>
void differentRestitutionTraversal (T* shape,
                                    int objectsLoopedThrough[],
                                    std::vector < std::vector<fraction> >& matrixX,
                                    std::vector < std::vector<fraction> >& matrixY,
                                    std::vector <fraction>& columnMatrixX,
                                    std::vector <fraction>& columnMatrixY,
                                    int* positionInMatrixX,
                                    int* positionInMatrixY,
                                    fraction* totalMomentumx,
                                    fraction* totalMomentumy,
                                    int *rowPosition,
                                    int numObjects){

    /**
        This function will loop through all the collided objects with different coefficients of restitution between each other
        It will get N-1 combinations with each combination being two objects that have directly collided
        The matrices are then filled up, either with a 1 or -1 in the square matrices for each object combination
        and the difference in speed between two objects*the coefficient of restitution will be placed in the column matrices
    **/

    //Two stop two shapes that already have at least one connection having a connection together
    objectsLoopedThrough[shape->Id()] = 1;
    Coordinate com = shape->centreOfMass();
    fraction shapeVelocityX = shape->xSpeed_f();
    fraction shapeVelocityY = shape->ySpeed_f();


    //At the bottom of both square matrices is the masses of all collided bodies
    matrixX[numObjects-1][*positionInMatrixX] = shape->getMass_f();
    matrixY[numObjects-1][*positionInMatrixY] = shape->getMass_f();

    //The position of the current shape in the matrix
    int shapePositionX = *positionInMatrixX;
    int shapePositionY = *positionInMatrixY;

    *positionInMatrixX += 1;
    *positionInMatrixY += 1;


    //We meed to get the total momentum in both components
    *totalMomentumx += (shape->getMass_f())*shapeVelocityX;
    *totalMomentumy += (shape->getMass_f())*shapeVelocityY;

    for (int j = 0; j < 3 && *rowPosition != numObjects; j++){

        int numShapes;

        //Triangle
        if (j == 0)
            numShapes = shape->numTriangles;

        //Circle
        else if (j == 1)
            numShapes = shape->numCircles;

        //Rectangle
        else if (j == 2)
            numShapes = shape -> numRects;

        for (int i = 0; i<numShapes;i++){

            fraction speedx;
            fraction speedy;
            Coordinate com2;
            int instNum;
            double restitution;

            switch (j){

                case 0:
                    instNum = shape->triangles[i]->Id();
                    com2 = shape->triangles[i]->centreOfMass();
                    speedx = shape->triangles[i]->xSpeed_f();
                    speedy = shape->triangles[i]->ySpeed_f();
                    restitution = *shape->restitutionTriangle;
                    break;

                case 1:
                    com2 = shape->circles[i]->centreOfMass();
                    speedx = shape->circles[i]->xSpeed_f();
                    speedy = shape->circles[i]->ySpeed_f();
                    instNum = shape->circles[i]->Id();
                    restitution = *shape->restitutionCircle;
                    break;


                case 2:
                    com2 = shape->rectangles[i]->centreOfMass();
                    speedx = shape->rectangles[i]->xSpeed_f();
                    speedy = shape->rectangles[i]->ySpeed_f();
                    instNum = shape->rectangles[i]->Id();
                    if (shape->rectangles[i]->getMass() == wallMass)
                        restitution = *shape->restitutionWall;
                    else
                        restitution = *shape->restitutionRect;
                    break;
            }

                if (objectsLoopedThrough[instNum] == 1)
                    continue;

                //Both x and y components. When k is 0, x component, when k is 1, y component
                for (int k = 0; k < 2; k++){

                    double* coord1;
                    double* coord2;

                    fraction* oneOrMinus1;
                    fraction* oneOrMinus2;

                    fraction* speed1;
                    fraction* speed2;

                    if (k == 0){
                        coord1 = &com.x;
                        coord2 = &com2.x;

                        oneOrMinus1 = &matrixX[*rowPosition][shapePositionX];
                        oneOrMinus2 = &matrixX[*rowPosition][*positionInMatrixX];

                        speed1 = &shapeVelocityX;
                        speed2 = &speedx;

                        if (*speed2 > *speed1)
                            columnMatrixX[*rowPosition] = (*speed2-*speed1)*restitution;

                        else
                            columnMatrixX[*rowPosition] = (*speed1-*speed2)*restitution;
                    }

                    else{

                        coord1 = &com.y;
                        coord2 = &com2.y;

                        oneOrMinus1 = &matrixY[*rowPosition][shapePositionY];
                        oneOrMinus2 = &matrixY[*rowPosition][*positionInMatrixY];

                        speed1 = &shapeVelocityY;
                        speed2 = &speedy;

                        if (*speed2 > *speed1)
                            columnMatrixY[*rowPosition] = (*speed2-*speed1)*restitution;

                        else
                            columnMatrixY[*rowPosition] = (*speed1-*speed2)*restitution;

                    }


                    if (*coord1 < *coord2){
                        *oneOrMinus1 = -1;
                        *oneOrMinus2 = 1;
                    }

                    else if (*coord1 > *coord2){
                        *oneOrMinus1 = 1;
                        *oneOrMinus2 = -1;
                    }

                    else{

                        /**
                            If both objects have the same coordinate axis, we must look at
                            their speeds to see how they could possibly have collided in the first place.

                            If both are negative, the one with the bigger absolute value is given a 1 and the other a -1
                            If both are positive, the one with the smaller speed is given a 1 and the other a -1
                            If one is positive and the other is negative, the negative one is given a 1 and the positive one a -1
                            If they are both 0, it doesn't matter.
                        **/

                        if (*speed1 <= 0 && *speed2 <= 0){
                            if (speed1->absoluteValue() > speed2->absoluteValue()){
                                *oneOrMinus1 = 1;
                                *oneOrMinus2 = -1;
                            }
                            else{
                                *oneOrMinus1 = -1;
                                *oneOrMinus2 = 1;
                            }
                        }

                        else if (*speed1 >=0 && *speed2 >= 0){

                            if (*speed1 < *speed2){
                                *oneOrMinus1 = 1;
                                *oneOrMinus2 = -1;
                            }
                            else{
                                *oneOrMinus1 = -1;
                                *oneOrMinus2 = 1;
                            }
                        }

                        else if (*speed1 < 0 && *speed2>0){
                            *oneOrMinus1 = 1;
                            *oneOrMinus2 = -1;
                        }

                        else if (*speed1 > 0 && *speed2<0){
                            *oneOrMinus1 = -1;
                            *oneOrMinus2 = 1;
                        }

                    }
                }

                *rowPosition += 1;

                if (*rowPosition == numObjects)
                    break;

                if (j == 0)
                    differentRestitutionTraversal(shape->triangles[i],objectsLoopedThrough,
                                                  matrixX,matrixY,
                                                  columnMatrixX,columnMatrixY,
                                                  positionInMatrixX,positionInMatrixY,
                                                  totalMomentumx,
                                                  totalMomentumy,
                                                  rowPosition,
                                                  numObjects);

                else if (j == 1)
                    differentRestitutionTraversal(shape->circles[i],objectsLoopedThrough,
                                                  matrixX,matrixY,
                                                  columnMatrixX,columnMatrixY,
                                                  positionInMatrixX,positionInMatrixY,
                                                  totalMomentumx,
                                                  totalMomentumy,
                                                  rowPosition,
                                                  numObjects);

                else
                    differentRestitutionTraversal(shape->rectangles[i],objectsLoopedThrough,
                                                  matrixX,matrixY,
                                                  columnMatrixX,columnMatrixY,
                                                  positionInMatrixX,positionInMatrixY,
                                                  totalMomentumx,
                                                  totalMomentumy,
                                                  rowPosition,
                                                  numObjects);
        }
    }
}

//May not need Graph connection
template <class T>
void handleCollisionDifferentRestitution(T* shape, Graph connection, int decimalPlaces,
                                         int objectsLoopedThrough[], int maxObjects){

    /**
        This function is used to handle collisions between objects which do not all
        have the same coefficient of restitution between each other

        It starts by forming N-1 connections, where N is the number of objects,
        in order to later form an N-dimensional matrix
    **/

    int i,j;
    bool changeXspeeds = true;
    bool changeYspeeds = true;

    std::vector< std::vector<fraction> > matrixX;
    std::vector< std::vector<fraction> > matrixY;
    std::vector< std::vector<fraction> > inverseMatrixX;
    std::vector< std::vector<fraction> > inverseMatrixY;

    std::vector<fraction> columnMatrixX;
    std::vector<fraction> columnMatrixY;
    std::vector<fraction> finalXVelocities;
    std::vector<fraction> finalYVelocities;

    fraction totalMomentumx(0,1);
    fraction totalMomentumy(0,1);

    for (i = 0; i < connection.numObjects;i++){
        fraction zero(0,1);
        std::vector<fraction> row;

        for (j = 0; j<connection.numObjects;j++){
            row.push_back(zero);

        }

        matrixX.push_back(row);
        matrixY.push_back(row);
        inverseMatrixX.push_back(row);
        inverseMatrixY.push_back(row);

        columnMatrixX.push_back(zero);
        columnMatrixY.push_back(zero);
        finalXVelocities.push_back(zero);
        finalYVelocities.push_back(zero);
    }


    int posMatrixX = 0;
    int posMatrixY = 0;
    int rowPosition = 0;
    differentRestitutionTraversal (shape,objectsLoopedThrough,
                                   matrixX, matrixY,
                                   columnMatrixX, columnMatrixY,
                                   &posMatrixX,&posMatrixY,
                                   &totalMomentumx,&totalMomentumy,
                                   &rowPosition,
                                   connection.numObjects);


    columnMatrixX[connection.numObjects-1] = totalMomentumx;
    columnMatrixY[connection.numObjects-1] = totalMomentumy;

    for (i =0; i<maxObjects;i++)
        objectsLoopedThrough[i] = 0;


    if (inverse (connection.numObjects,matrixX,inverseMatrixX))
        squareColumnMultiplication(connection.numObjects,inverseMatrixX,columnMatrixX,finalXVelocities);

    /**If the square matrix is uninvertible then by the logic of the physics engine,
       all objects involved weren't moving in that axis
    **/

    else
        changeXspeeds = false;

    if (inverse (connection.numObjects,matrixY,inverseMatrixY))
        squareColumnMultiplication(connection.numObjects,inverseMatrixY,columnMatrixY,finalYVelocities);

    else
        changeYspeeds = false;



    int columnPosition = 0;
    changeSpeedDifferentRestitution(shape,objectsLoopedThrough, &columnPosition,connection.numObjects,
                                    finalXVelocities,finalYVelocities,decimalPlaces, changeXspeeds,changeYspeeds);

}

template <class T>
void handleCollisionSameResitution (T* shape, Graph connection, int decimalPlaces,
                                    int objectsLoopedThrough[], int maxObjects){
    /**
        This function handles a collision between objects with the same coefficient of restitution

        As specified in the design, the general formula for working out the resultant velocity
        of an object after colliding with other objects of the same coefficient of restitution is

        Vfinal = Centre of Momentum - (Initial Speed - Centre of Momentum)*coefficient of restitution

        The centre of momentum is calculated by adding up the momenta of all of the objects
        and dividing by their total mass

        This must be done in both x and y components

    **/

    int i;
    fraction totalMomentumx(0,1);
    fraction totalMomentumy(0,1);
    fraction totalMass (0,1);

    fraction centreMomentumx;
    fraction centreMomentumy;

    sameResitutionTraversal(shape,connection,objectsLoopedThrough,&totalMomentumx,
                            &totalMomentumy,&totalMass);

    for (i = 0; i < maxObjects;i++)
        objectsLoopedThrough[i] = 0;


    centreMomentumx = totalMomentumx/totalMass;
    centreMomentumy = totalMomentumy/totalMass;

    changeSpeedSameRestitution(shape,connection,centreMomentumx,centreMomentumy,
                               decimalPlaces,objectsLoopedThrough);

}

template <class T>
void reverseCollisions (T* shape, int objectsLoopedThrough[]){

    int i,j;

    if (objectsLoopedThrough[shape->Id()] == 1)
        return;

    objectsLoopedThrough[shape->Id()] = 1;

    shape->changeXSpeedPreCollision();
    shape->changeYSpeedPreCollision();

    for (j = 0; j<3;j++){
        int numShapes;

        //Triangle
        if (j == 0)
            numShapes = shape->numTriangles;

        //Circle
        else if (j == 1)
            numShapes = shape->numCircles;

        //Rectangle
        else if (j == 2)
            numShapes = shape -> numRects;

        for (i = 0; i<numShapes;i++){
            if (j == 0){
                if (objectsLoopedThrough[shape->triangles[i]->Id()] == 0)
                    reverseCollisions(shape->triangles[i],objectsLoopedThrough);
            }

            else if (j == 1){
                if (objectsLoopedThrough[shape->circles[i]->Id()] == 0)
                    reverseCollisions(shape->circles[i],objectsLoopedThrough);
            }

            else if (j == 2){
                if (objectsLoopedThrough[shape->rectangles[i]->Id()] == 0)
                    reverseCollisions(shape->rectangles[i], objectsLoopedThrough);
            }
        }
    }

}
}
#endif // COLLISIONHANDLING_H_INCLUDED
