#include "database.h"
#include "utility.h"
#include "physics_engine_global_vars.h"
#include "rendering_engine_global_vars.h"
#include "colourHashing.h"
#include <string>
#include <iostream>
#include <stdio.h>


int globalVarsCallback (void*, int , char** , char** );
int mechanicsDataCallback (void*, int,  char** , char** );
int circleCallback (void*, int, char**, char**);
int rectCallback (void*, int , char **, char** );
int triangleCallback (void* , int , char** , char**);

/**

    This file contains the functions related to the creating of a new database,
    which in turn creates a new file, as well as the updating of an existing database.

    A database of the physics engine is always created from an existing template.
    When a database/file is updated, the records are completely erased and then added again

    Note: if I ever remove the throw statements, add return appropriate returns in place of


**/

using std::string;
using std::cout;
using std::vector;

using namespace physicsEngine;

/**
    This is a structure that is used to store recently loaded records from the Mechanics table,
    so they can be added onto a shape (rectangle, circle or triangle) object
**/
struct mechanicsInfo{

        double mass, velX, velY, acclX, acclY;
        bool affectedByGravity;
        unsigned int colour;
        uint8_t r,g,b,a;
}mechanicsData;

/**
   These vectors are used to store the information of shapes that
   were loaded from the database. They are then added to the physics engine
**/
vector<Circle> loadedCircles;
vector<Triangle> loadedTriangles;
vector<Rectangle> loadedRectangles;

//This query is required by three different functions so I decided to make it a global variable
std::string mechanicsQuery = "SELECT * from Mechanics WHERE mechanicsID = ";

//This is used as a  pointer to the current database being used
//It will only be used when saving a file and is not needed for setting up a database
sqlite3* databaseGlobal = nullptr;

sqlite3* setupDatabase (char* dbName){

    /**This function is used to create a database for a file. dbName will be the
       name of the database**/

	//The database structure
	sqlite3* database = nullptr;

	//Used to store string for the setting up of the tables
	string tableCreation;
	int tableCreationResult;
	char* errorMsg = nullptr;

    //Function opens an existing database or creates a new one if it doesn't exist
	int dbCreationResult = sqlite3_open (dbName, &database);

	//Failed to create database. Return a null pointer
	if (dbCreationResult != SQLITE_OK){
		cout << "Failed to create database " << sqlite3_errmsg(database);
		//Add return nullptr if I remove this
		throw dbCreationFail();
	}


	//Create game data table
	tableCreation = "CREATE TABLE SimulationConstants("\
					"Gravity REAL DEFAULT 0.0,"\
					"CircleToRectangleRestitution REAL DEFAULT 1.0," \
					"CircleToTriangleRestitution REAL DEFAULT 1.0,"\
					"CircleToCircleRestitution REAL DEFAULT 1.0,"\
					"CircleToBoundaryRestitution REAL DEFAULT 1.0,"\
					"RectangleToTriangleRestitution REAL DEFAULT 1.0,"\
					"RectangleToRectangleRestitution REAL DEFAULT 1.0,"\
					"RectangleToBoundaryRestitution REAL DEFAULT 1.0,"\
					"TriangleToTriangleRestitution REAL DEFAULT 1.0,"\
					"TriangleToBoundaryRestitution REAL DEFAULT 1.0)";
    /**
        sqlite3_exec is the function used to carry out queries
        It takes the following arguments in this order:
        1. The database structure
        2. A query, in the form of a char * type
        3. A pointer to a function that will be called after the query is executed
        4.
        5.
        6. A char * pointer to store an error message in the event of a query failure
    **/

	tableCreationResult = sqlite3_exec (database, tableCreation.c_str(),
										nullptr, nullptr, &errorMsg);


	//Create mechanics table
	tableCreation = "CREATE TABLE Mechanics("\
                    "MechanicsID INTEGER PRIMARY KEY NOT NULL,"\
					"MASS REAL DEFAULT 5.0,"\
					"VelocityX REAL DEFAULT 0.0,"\
					"VelocityY REAL DEFAULT 0.0,"\
					"AccelerationX REAL DEFAULT 0.0,"\
					"AccelerationY REAL DEFAULT 0.0,"\
					"AffectedByGravity INTEGER DEFAULT 0,"\
					"Colour INTEGER DEFAULT 0)";


	tableCreationResult = sqlite3_exec (database, tableCreation.c_str(),
										nullptr, nullptr, &errorMsg);

    //sqlite3_exec returns value SQLITE_OK (which is 0) if the query was successful
	if (tableCreationResult != SQLITE_OK){

		cout << "Failed to create mechanics table " << sqlite3_errmsg(database);
		throw tableCreationFail();
	}

	//Note that SQLite3 does not store array datatypes.
	//So a colour array must be converted into an integer
	//This is done by converting the base-256 value into base 10, and during loading...
	//...it is converted back into its base-256 value
	tableCreation = "CREATE TABLE Triangle("\
					"X1 INTEGER NOT NULL,"\
					"Y1 INTEGER NOT NULL,"\
					"X2 INTEGER NOT NULL,"\
					"Y2 INTEGER NOT NULL,"\
					"X3 INTEGER NOT NULL,"\
					"Y3 INTEGER NOT NULL,"\
					"MechanicsID INTEGER,"\
					"FOREIGN KEY(MechanicsID) REFERENCES Mechanics(ID))";


	tableCreationResult = sqlite3_exec (database, tableCreation.c_str(), nullptr, nullptr, &errorMsg);

	if (tableCreationResult != SQLITE_OK){

		cout << "Failed to create triangle table " << sqlite3_errmsg(database);
		throw tableCreationFail();
	}

    //The Rectangle table
	tableCreation = "CREATE TABLE Rectangle("\
					"X INTEGER NOT NULL,"\
					"Y INTEGER NOT NULL,"\
					"Width INTEGER NOT NULL,"\
					"Height INTEGER NOT NULL,"\
					"MechanicsID INTEGER NOT NULL,"\
					"FOREIGN KEY(MechanicsID) REFERENCES Mechanics(ID) )";


	tableCreationResult = sqlite3_exec (database, tableCreation.c_str(), nullptr, nullptr, &errorMsg);

	if (tableCreationResult != SQLITE_OK){

		cout << "Failed to create rectangle table " << sqlite3_errmsg(database);
		throw tableCreationFail();
	}

    //The circle table
	tableCreation = "CREATE TABLE Circle("\
					"X INTEGER NOT NULL,"\
					"Y INTEGER NOT NULL,"\
					"Radius INTEGER NOT NULL,"\
					"MechanicsID INTEGER NOT NULL,"\
					"FOREIGN KEY(MechanicsID) REFERENCES Mechanics(ID) )";


	tableCreationResult = sqlite3_exec (database, tableCreation.c_str(), nullptr, nullptr, &errorMsg);

	if (tableCreationResult != SQLITE_OK){

		cout << "Failed to create circle table " << sqlite3_errmsg(database);
		throw tableCreationFail();
	}

	return database;

}

bool saveData (sqlite3* database, vector<Rectangle>* rectangles,
               vector<Circle>*circles, vector<Triangle>* triangles){

    /**
        This function is used to save data for a given physicsEngineHandler object

        It will erase the records of data in the current database, and all shape data in
        the physicsEngineHandler object, as well saving the current value of
		the global variables

        If anything does wrong, the database will be reverted back
		to its original contents

    **/


    sqlite3_exec (database,"BEGIN",nullptr,nullptr,nullptr);
    //The queries to delete all records from every table
    string deleteRecords = "DELETE from SimulationConstants;"\
                           "DELETE from Mechanics;"\
                           "DELETE from Rectangle;"\
                           "DELETE from Triangle;"\
                           "DELETE from Circle";

    //Call the sqlite3 query function to delete records
    int deletionResult = sqlite3_exec (database,deleteRecords.c_str(),
									   nullptr,nullptr,nullptr);

    if (deletionResult != SQLITE_OK){
        cout << "Failed to delete contents of original db when saving "	<<
		sqlite3_errmsg(database);

        //If I ever removed the throws, add back return false
        throw deletionFail();
        sqlite3_exec(database,"ROLLBACK",nullptr,nullptr,nullptr);

    }

    //I place the following characters in string variables to make concatenation easier...
	//...when creating queries
    //We cannot concatenate a string literal, so I put them into string variables
    string comm = ",";
    string bracket = ")";
    string semiColon = ";";

    /**
        The restitution values and gravity value are converted to strings so
        they can be added into the database
    **/
    string gravity = convert_str_d(gravitationalAcceleration, restitutionDecimalPlaces);
    string circRect= convert_str_d(circleToRectangleRestitution, restitutionDecimalPlaces);
    string circBound = convert_str_d (circleToBoundaryRestitution, restitutionDecimalPlaces);
    string circTri = convert_str_d(circleToTriangleRestitution, restitutionDecimalPlaces);
    string circRes = convert_str_d(circleToCircleRestitution, restitutionDecimalPlaces);
    string rectTri = convert_str_d (rectangleToTriangleRestitution, restitutionDecimalPlaces);
    string rectsRes = convert_str_d (rectangleTorectangleRestitution, restitutionDecimalPlaces);
    string rectBound = convert_str_d(rectangleToBoundaryRestitution, restitutionDecimalPlaces);
    string triRes = convert_str_d (triangleToTriangleRestitution, restitutionDecimalPlaces);
    string triBound = convert_str_d(triangleToBoundaryRestitution, restitutionDecimalPlaces);


    //The query to update the SimulationConstants table
    string updateGlobalVars = "INSERT INTO SimulationConstants" \
                              "(Gravity, CircleToRectangleRestitution, CircleToTriangleRestitution,"\
                              "CircleToCircleRestitution, CircleToBoundaryRestitution,"\
                              "RectangleToTriangleRestitution, RectangleToRectangleRestitution, RectangleToBoundaryRestitution,"\
                              "TriangleToTriangleRestitution,TriangleToBoundaryRestitution)"\
                              "VALUES(" + gravity + comm + circRect + comm + circTri + comm+ circRes +  comm + circBound + comm +
                              rectTri + comm + rectsRes + comm + rectBound + comm + triRes + comm + triBound + bracket + semiColon;

    int addGlobalsResult = sqlite3_exec (database,updateGlobalVars.c_str(),nullptr,nullptr,nullptr);

    //Check if SimulationConstants table was successfully updated
    if (addGlobalsResult != SQLITE_OK){
        cout << "Failed to update global variables when saving " << sqlite3_errmsg(database);
        throw updateFail();
        sqlite3_exec(database,"ROLLBACK",nullptr,nullptr,nullptr);
        return false;
    }

    //The Mechanics table uses a primary key MechanicsID, which is a foregin key in the shape tables
    //This variable will be incremented for every shape saved into the database, so each shape has a unique foreign key
    int mechanicsID = 1;
    int i;

    //The general query that will be used to insert records into the mechanics table
    string mechanicsTableInsert = "INSERT INTO Mechanics "\
                                  "(MechanicsID,Mass,VelocityX,VelocityY,AccelerationX, AccelerationY,AffectedByGravity,Colour) ";

    //Loop through every triangle
    for (i = 0; i<triangles->size();i++){
        //To make things shorter, I've created a pointer called tri, so I don't keep on having to type...
        //...*triangles[i]
        Triangle& tri = (*triangles)[i];

        //Below, all data on a shape is converted into a string
        string idStr = convert_str (mechanicsID);
        string xVel = convert_str_d (tri.xSpeed());
        string yVel = convert_str_d (tri.ySpeed());
        string xAccl = convert_str_d (tri.xAcceleration());
        string yAccl = convert_str_d (tri.yAcceleration());
        string mass = convert_str_d (tri.getMass());

        /**
           sqlite3 does not store Boolean values.
           But true is generally a 1 and false is 0, so this ternary operation
           simply adds a 1 if shape is affected by gravity, else 0
        **/
        string gravity = (tri.getGravityEffect() == true)? "1":"0";

        /**
            I use hashing to convert the RGBA values of a shape to a single number that can be stored
            in the database, which saves space.
        **/
        string colourID = convert_str (getColourHash (tri.rValue(),tri.gValue(),tri.bValue(),tri.aValue()));

        /**
            All coordinates are technically integers in the physics engine.
            However, they are stored as double values so computations such as
            collision detection can be accurately carried out on them.

            The issue is, if we try to floor a double number that is precisely meant to represent
            an integer (converting a double to an int like we do here truncates the fractional part
            which is essentially what flooring is), the number may go down 1 due to a floating point
            precision error. So if we add 0.5 to each number, flooring it will guarantee an
			integer of that number.

            That is what convertIntDouble does
        **/
        string x1 = convert_str (convertIntDouble(tri.xcoordPoint1()));
        string y1 = convert_str (convertIntDouble(tri.ycoordPoint1()));
        string x2 = convert_str (convertIntDouble(tri.xcoordPoint2()));
        string y2 = convert_str (convertIntDouble(tri.ycoordPoint2()));
        string x3 = convert_str (convertIntDouble(tri.xcoordPoint3()));
        string y3 = convert_str (convertIntDouble(tri.ycoordPoint3()));

        //The query for updating a triangle table
        string triangleInsert = "INSERT INTO Triangle (X1,Y1,X2,Y2,X3,Y3,MechanicsID)";

        triangleInsert+= "VALUES (";
        triangleInsert += x1 + comm + y1 + comm + x2 + comm + y2 + comm + x3 + comm +
                          y3 + comm + idStr + ");";

        /**
            This is the query for adding data belonging to the triangle in the mechanics table.
            The variable idStr will be the unique ID of the record that will later allow the data
            to be reconciled with the triangle
        **/
        string mechInsert = mechanicsTableInsert + "VALUES(";
        mechInsert += idStr + comm + mass + comm + xVel + comm + yVel + comm + xAccl + comm+
                      yAccl + comm + gravity + comm + colourID + ");";


        //Execute query to add record to triangle table
        int addTriangleResult = sqlite3_exec(database,triangleInsert.c_str(),nullptr,nullptr,nullptr);

        if (addTriangleResult != SQLITE_OK){

            cout << "\nFailed to update triangle table " << sqlite3_errmsg(database);
            sqlite3_exec(database,"ROLLBACK",nullptr,nullptr,nullptr);
            throw updateFail();
            return false;
        }


        //Execute query to add mechanics data to the Mechanics table
        int updateMechanicsResult = sqlite3_exec(database,mechInsert.c_str(),nullptr,nullptr,nullptr);

        if (updateMechanicsResult != SQLITE_OK){

            cout << "Failed to update mechanics table for triangle " << i << "\n" << sqlite3_errmsg(database);
            sqlite3_exec(database,"ROLLBACK",nullptr,nullptr,nullptr);
            throw updateFail();
        }

        //mechanicsID is incremented, so every shape looped through has a unique foreign key...
        //...and every record in the Mechanics table has a unique primary key
        mechanicsID += 1;

    }

    //Here we do the same that was done to triangles but for circles
    for (i=0; i < circles->size();i++){

        Circle& circ = (*circles)[i];

        string idStr = convert_str (mechanicsID);
        string xVel = convert_str_d (circ.xSpeed());
        string yVel = convert_str_d (circ.ySpeed());
        string xAccl = convert_str_d (circ.xAcceleration());
        string yAccl = convert_str_d (circ.yAcceleration());
        string mass = convert_str_d (circ.getMass());
        string gravity = (circ.getGravityEffect() == true)?"1":"0";
        string colourID = convert_str (getColourHash (circ.rValue(),circ.gValue(),circ.bValue(),circ.aValue()));
        string x = convert_str (convertIntDouble(circ.xPosition()));
        string y = convert_str (convertIntDouble(circ.yPosition()));
        string radius = convert_str (convertIntDouble(circ.getRadius()));

        string circleInsert = "INSERT INTO Circle (X,Y,Radius,MechanicsID)";

        circleInsert+= "VALUES (";
        circleInsert += x + comm + y + comm + radius + comm + idStr + ");";

        int addCircleResult = sqlite3_exec(database,circleInsert.c_str(),nullptr,nullptr,nullptr);

        if (addCircleResult != SQLITE_OK){

            cout << "Failed to update circle table " << sqlite3_errmsg(database);
            sqlite3_exec(database,"ROLLBACK",nullptr,nullptr,nullptr);
            throw updateFail();
        }


        string mechInsert = mechanicsTableInsert + "VALUES(";
        mechInsert += idStr + comm + mass + comm + xVel + comm + yVel + comm + xAccl + comm+
                      yAccl + comm + gravity + comm + colourID + ");";


        int updateMechanicsResult = sqlite3_exec(database,mechInsert.c_str(),nullptr,nullptr,nullptr);

        if (updateMechanicsResult != SQLITE_OK){

            cout << "Failed to update mechanics table for circle " << sqlite3_errmsg(database);
            sqlite3_exec(database,"ROLLBACK",nullptr,nullptr,nullptr);
            throw updateFail();
        }

        mechanicsID += 1;

    }

    //Same thing is done for all rectangles
    for (i = 0; i<rectangles->size();i++){

        Rectangle& rect = (*rectangles)[i];

        //No need to save a wall, it's built-in
        if (rect.getMass() == wallMass)
            continue;

        string idStr = convert_str (mechanicsID);
        string xVel = convert_str_d (rect.xSpeed());
        string yVel = convert_str_d (rect.ySpeed());
        string xAccl = convert_str_d (rect.xAcceleration());
        string yAccl = convert_str_d (rect.yAcceleration());
        string mass = convert_str_d (rect.getMass());

        string gravity = (rect.getGravityEffect() == true)?"1":"0";

        string colourID = convert_str (getColourHash (rect.rValue(),rect.gValue(),
						  rect.bValue(),rect.aValue()));

        string x = convert_str (convertIntDouble( rect.xPosition() ));
        string y = convert_str (convertIntDouble(rect.yPosition()));
        string width = convert_str(convertIntDouble(rect.getWidth()));
        string height = convert_str(convertIntDouble(rect.getHeight()));

        string rectInsert = "INSERT INTO Rectangle (X,Y,Width,Height,MechanicsID)";

        rectInsert+= "VALUES (";
        rectInsert += x + comm + y + comm + width + comm + height + comm + idStr + ");";

        int addRectResult = sqlite3_exec(database,rectInsert.c_str(),nullptr,nullptr,nullptr);

        if (addRectResult != SQLITE_OK){

            cout << "Failed to update rectangle table " << sqlite3_errmsg(database);
            sqlite3_exec(database,"ROLLBACK",nullptr,nullptr,nullptr);
            throw updateFail();
        }


        string mechInsert = mechanicsTableInsert + "VALUES(";
        mechInsert += idStr + comm + mass + comm + xVel + comm + yVel + comm + xAccl + comm+
                      yAccl + comm + gravity + comm + colourID + ");";



        int updateMechanicsResult = sqlite3_exec(database,mechInsert.c_str(),nullptr,nullptr,nullptr);

        if (updateMechanicsResult != SQLITE_OK){

            cout << "Failed to update mechanics table for rectangle " << sqlite3_errmsg(database);
            sqlite3_exec(database,"ROLLBACK",nullptr,nullptr,nullptr);
            throw updateFail();
        }

        mechanicsID += 1;
    }

    sqlite3_exec(database,"COMMIT",nullptr,nullptr,nullptr);
    return true;

}

bool loadData(sqlite3* database, vector<Rectangle>* rectangles, vector<Circle>* circles,
              vector<Triangle>* triangles){

    /**
        This function loads data from a database, and adds it to the physics engine,
        as well as into separate rectangle, circle and triangle vectors,
		which will be the actual locations of the shape objects
		(the physicsEngine only stores pointers to the shape objects)
    **/


    //databaseGlobal now points to the current database where data will be loaded from
    //These will be used by various functions
    databaseGlobal = database;

    int i;

    //o Stands for original
    //Current physics engine global variables need to be backed up so they...
	//...can be reverted if loading fails
    double o_gravitationalAcceleration =  gravitationalAcceleration;
    double o_circleToRectangleRestitution = circleToRectangleRestitution;
    double o_circleToBoundaryRestitution = circleToBoundaryRestitution;
    double o_circleToTriangleRestitution = circleToTriangleRestitution;
    double o_circleToCircleRestitution = circleToCircleRestitution;
    double o_rectangleToRectangleRestitution = rectangleTorectangleRestitution;
    double o_rectangleToBoundaryRestitution = rectangleToBoundaryRestitution;
    double o_triangleToTriangleRestitution = triangleToTriangleRestitution;
    double o_triangleToBoundaryRestitution = triangleToBoundaryRestitution;

    //This value is returned at the end of the function
    bool status = true;

    //The queries for loading data from each table
    std::string getGlobalVars = "SELECT * from simulationConstants";
    std::string getRectangles = "SELECT * from Rectangle";
    std::string getTriangles =  "SELECT * from Triangle";
    std::string getCircles = "SELECT * from Circle";

    /**
        Unlike for saving and creating database, here we make use of the
        third argument, which is a pointer to a function (globalVarsCallback).

        This function is called for every record that is selected from a table. In the simulationConstants table,
        there is only one record, so the function is called once. The function will
        convert the data into double variables, and update the global variables used by the physics engine acoordingly

    **/

    int globalVarsResult =  sqlite3_exec(database,getGlobalVars.c_str(),globalVarsCallback,nullptr,nullptr);

    //The rectCallback function is called for every rectangle record retrieved from the database
    //The same is done the Triangle and Circle tables
    int rectanglesLoadResult = sqlite3_exec(database,getRectangles.c_str(),rectCallback,nullptr,nullptr);
    int trianglesLoadResult = sqlite3_exec(database,getTriangles.c_str(),triangleCallback,nullptr,nullptr);
    int circlesLoadResult = sqlite3_exec(database,getCircles.c_str(),circleCallback,nullptr,nullptr);

    //Here, if anything does wrong the function will carry on with the rest anyway

    if (globalVarsResult != SQLITE_OK){
        cout << "Failed to load global variables " << sqlite3_errmsg(database);
        throw loadTableFail();
        status = false;
    }


    if (trianglesLoadResult != SQLITE_OK){
        cout << "Failed to load triangles "  << sqlite3_errmsg(database);
        throw loadTableFail();
        status = false;
    }


    if (rectanglesLoadResult != SQLITE_OK){
        cout << "Failed to load rectangles " << sqlite3_errmsg(database);
        throw loadTableFail();
        status =  false;
    }

    if (circlesLoadResult != SQLITE_OK){
        cout << "Failed to load circles " << sqlite3_errmsg(database);
        throw loadTableFail();
        status = false;
    }

    if (status){
        //Here, the loaded shapes are added to the shape vectors provided as arguments call for this function
        //Pointers to these shape vectors are then added into the physics engine
        for (i = 0; i< loadedRectangles.size();i++){
            rectangles->push_back(loadedRectangles[i]);
        }


        for (i = 0; i< loadedTriangles.size();i++)
                triangles->push_back(loadedTriangles[i]);


        for (i = 0; i< loadedCircles.size();i++)
            circles->push_back(loadedCircles[i]);


        //Reset the global variables for the loaded shapes for the next function call
        loadedCircles.erase(loadedCircles.begin(),loadedCircles.end());
        loadedRectangles.erase(loadedRectangles.begin(),loadedRectangles.end());
        loadedTriangles.erase(loadedTriangles.begin(),loadedTriangles.end());
    }

    //Reset global variables back to original ones
    else{
        //Revert global variables back to original values in case they were changed
        gravitationalAcceleration = o_gravitationalAcceleration;
        circleToRectangleRestitution = o_circleToRectangleRestitution;
        circleToBoundaryRestitution = o_circleToBoundaryRestitution;
        circleToTriangleRestitution = o_circleToTriangleRestitution;
        circleToCircleRestitution = o_circleToCircleRestitution;
        rectangleTorectangleRestitution = o_rectangleToRectangleRestitution;
        rectangleToBoundaryRestitution = o_rectangleToBoundaryRestitution;
        triangleToTriangleRestitution = o_triangleToTriangleRestitution;
        triangleToBoundaryRestitution = o_triangleToBoundaryRestitution;
    }

    return status;

}


int globalVarsCallback (void* data, int numColumns, char** fields, char** columnName){

    /**
        int globalVarsResult =  sqlite3_exec(database,getGlobalVars.c_str(),globalVarsCallback,nullptr,nullptr);

        When sqlite_exec is called in the loadData function via this statement, this function is called,
        as it is provided as the third argument. It will be given the four arguments by its caller.

        The variable fields will contain call of the data for each field for a particular record.
        In this case there'll only be one record.

    **/


    /**Order of fields in SimulationConstants:
        Gravity
        CircleToRectangleRestitution
        CircleToTriangleRestitution
        CircleToCircleRestitution
        CircleToBoundaryRestitution
        RectangleToTriangleRestitution
        RectangleToRectangleRestitution
        RectangleToBoundaryRestitution
        TriangleToTriangleRestitution
        TriangleToBoundaryRestitution

        It's important to know the order of the fields, as below they are uniquely indexed
    **/

    //Here, we modify the values of the global variables based on the values in the record
    //They are converted to doubles by the function call
    convert_str_to_double(fields[0],&gravitationalAcceleration);
    convert_str_to_double(fields[1],&circleToRectangleRestitution);
    convert_str_to_double(fields[2],&circleToTriangleRestitution);
    convert_str_to_double(fields[3],&circleToCircleRestitution);
    convert_str_to_double(fields[4],&circleToBoundaryRestitution);
    convert_str_to_double(fields[5],&rectangleToTriangleRestitution);
    convert_str_to_double(fields[6],&rectangleTorectangleRestitution);
    convert_str_to_double(fields[7],&rectangleToBoundaryRestitution);
    convert_str_to_double(fields[8],&triangleToTriangleRestitution);
    convert_str_to_double(fields[9],&triangleToBoundaryRestitution);

    return 0;

}

int mechanicsDataCallback (void* data, int numColumns, char** fields, char** columnName){

    /**
        This function operates just like globalVarsCallback

        However, because there will most likely be multiple shapes, there will be multiple records
        in the Mechanics table, so this function will be called multiple times

        The data per record will be added to the mechanicsData structure. This global data can then
        be accessed by the required function which handled a particular shape callback

        This function is called when one of the callback functions for the shapes queries the Mechanics table
        for a record with a corresponding Id to the foreign key of the shape
    **/

    /**
        Order of fields for a mechanics table is:
            MechanicsID
            MASS
            VelocityX
            VelocityY
            AccelerationX
            AccelerationY
            AffectedByGravity
            Colour
        **/

    convert_str_to_double(fields[1],&mechanicsData.mass);
    convert_str_to_double(fields[2],&mechanicsData.velX);
    convert_str_to_double(fields[3],&mechanicsData.velY);
    convert_str_to_double(fields[4],&mechanicsData.acclX);
    convert_str_to_double(fields[5],&mechanicsData.acclY);

    //0 typically means false and 1 means true when dealing with Booleans
    if (fields[6][0] == '0')
        mechanicsData.affectedByGravity = false;

    else if (fields[6][0] == '1')
        mechanicsData.affectedByGravity = true;

    convert_str_to_uint(fields[7],&mechanicsData.colour);
    convertHash(mechanicsData.colour, &mechanicsData.r, &mechanicsData.g,
                &mechanicsData.b, &mechanicsData.a);

    /**As contradicting as it may seem, when returning success or fail, if  a Boolean is not being used
       and int types are being used instead, in C/C++, 0 usually corresponds to success and 1 to failure **/
    return 0;

}

int circleCallback (void* data, int numColumns, char** fields, char** columnName){

    /**
        Order of fields for circle table is:
            X
            Y
            Radius
            MechanicsID
    **/

    static vector<Circle> circles;

    int x,y, radius;
    char* mechanicsID = fields[3];

    convert_str_to_int (fields[0],&x);
    convert_str_to_int (fields[1],&y);
    convert_str_to_int (fields[2],&radius);

    std::string query = mechanicsQuery + mechanicsID;
    query += ";";


    int mechanicsQueryResult = sqlite3_exec(databaseGlobal,query.c_str(),mechanicsDataCallback,nullptr,nullptr);

    if (mechanicsQueryResult != SQLITE_OK){

        cout << "Failed to load mechanics data for a circle\n" << sqlite3_errmsg(databaseGlobal);
        throw loadMechanicsFail();
        return 1;
    }

    Circle circ (x,y,radius,DEFAULT_FRAME_RATE,mechanicsData.mass);
    circ.changeSpeedx(mechanicsData.velX);
    circ.changeSpeedy(mechanicsData.velY);
    circ.changeAccelerationx(mechanicsData.acclX);
    circ.changeAccelerationy(mechanicsData.acclY);
    circ.gravityEffect(mechanicsData.affectedByGravity);
    circ.changeColour(mechanicsData.r,mechanicsData.g,mechanicsData.b,mechanicsData.a);

    loadedCircles.push_back (circ);

    return 0;

}

int rectCallback (void* data, int numColumns, char ** fields, char** columnName){

    /**
        Order of fields for rectangle table is:
        X
        Y
        Width
        Height
        Mechanics ID
    */

    int x,y, width,height;
    char* mechanicsID = fields[4];

    convert_str_to_int(fields[0],&x);
    convert_str_to_int(fields[1],&y);
    convert_str_to_int(fields[2],&width);
    convert_str_to_int(fields[3],&height);

    std::string query = mechanicsQuery + mechanicsID;
    query += ";";

    int mechanicsQueryResult = sqlite3_exec(databaseGlobal,query.c_str(),mechanicsDataCallback,nullptr,nullptr);

    if (mechanicsQueryResult != SQLITE_OK){
        cout << "Failed to load mechanics data for a rectangle\n" << sqlite3_errmsg(databaseGlobal);
        throw loadMechanicsFail();
        return 1;
    }


    Rectangle rect (x,y,DEFAULT_FRAME_RATE,width,height,mechanicsData.mass);
    rect.changeSpeedx(mechanicsData.velX);
    rect.changeSpeedy(mechanicsData.velY);
    rect.changeAccelerationx(mechanicsData.acclX);
    rect.changeAccelerationy(mechanicsData.acclY);
    rect.changeMass(mechanicsData.mass);
    rect.gravityEffect(mechanicsData.affectedByGravity);
    rect.changeColour(mechanicsData.r,mechanicsData.g,mechanicsData.b,mechanicsData.a);

    loadedRectangles.push_back(rect);

    return 0;

}

int triangleCallback (void* data, int numColumns, char** fields, char** columnName){
    /**
    Order of fields for triangle table is:
        X1
        Y1
        X2
        Y2
        X3
        Y3
        MechanicsID
    **/
    int x1,y1,x2,y2,x3,y3;
    char* mechanicsID = fields[6];

    convert_str_to_int(fields[0],&x1);
    convert_str_to_int(fields[1],&y1);
    convert_str_to_int(fields[2],&x2);
    convert_str_to_int(fields[3],&y2);
    convert_str_to_int(fields[4],&x3);
    convert_str_to_int(fields[5],&y3);

    std::string query = mechanicsQuery + mechanicsID;
    query += ";";


    int mechanicsQueryResult = sqlite3_exec(databaseGlobal,query.c_str(),mechanicsDataCallback,nullptr,nullptr);

    if (mechanicsQueryResult != SQLITE_OK){

        cout << "Failed to load mechanics data for a triangle\n" << sqlite3_errmsg(databaseGlobal);
        throw loadMechanicsFail();
        return 1;
    }


    Triangle tri(x1,y1,x2,y2,x3,y3,DEFAULT_FRAME_RATE,mechanicsData.mass);
    tri.changeSpeedx(mechanicsData.velX);
    tri.changeSpeedy(mechanicsData.velY);
    tri.changeAccelerationx(mechanicsData.acclX);
    tri.changeAccelerationy(mechanicsData.acclY);
    tri.changeMass(mechanicsData.mass);
    tri.gravityEffect(mechanicsData.affectedByGravity);
    tri.changeColour(mechanicsData.r,mechanicsData.g,mechanicsData.b,mechanicsData.a);

    loadedTriangles.push_back (tri);

    return 0;

}

