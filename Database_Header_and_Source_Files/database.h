#include "sqlite3//sqlite3.h"
#include <exception>
#include "collisionDetection.h"

#ifndef DATABASE_H_INCLUDED
#define  DATABASE_H_INCLUDED


/**
    For the database, I'm using SQLite3
    Below are exceptions that will be thrown in the event that database errors occur
    These were primarily designed for debugging purposes
**/

struct dbCreationFail : public std::exception{

	const char * what() const throw () { return "Failed to create database"; }
};

struct tableCreationFail : public std::exception{

	const char * what () const throw() { return "Failed to create a table"; }
};

struct deletionFail: public std::exception{

    const char * what() const throw() {return "Failed to delete contents of original database when saving.";}
};

struct updateFail: public std::exception{

    const char * what() const throw() { return "Failed to update table when saving.";}

};

struct loadMechanicsFail: public std::exception{

    const char * what() const throw() { return "Failed to load a mechanics data for a shape.";}
};

struct loadTableFail: public std::exception{

    const char * what() throw() { return "Failed to load a table.";}

};


//Function prototypes so functions in database.cpp can be accessed by different files
sqlite3* setupDatabase (char* dbName);

bool loadData(sqlite3* database,std::vector<physicsEngine::Rectangle>* rectangles,
              std::vector<physicsEngine::Circle>* circles, std::vector<physicsEngine::Triangle>* triangles);

bool saveData (sqlite3* database, std::vector<physicsEngine::Rectangle>* rectangles,
               std::vector<physicsEngine::Circle>*circles, std::vector<physicsEngine::Triangle>* triangles);


#endif
