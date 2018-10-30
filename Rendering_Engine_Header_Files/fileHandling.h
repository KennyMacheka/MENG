#ifndef FILEHANDLING_H_INCLUDED
#define FILEHANDLING_H_INCLUDED

#include "database.h"
#include "shape.h"
#include <vector>


extern char fileExtension[];
bool attemptDatabaseOpen (sqlite3** database,char* path, std::vector<physicsEngine::Triangle>*triangles,
                          std::vector<physicsEngine::Rectangle>*rectangles,
                          std::vector<physicsEngine::Circle>*circles);


bool openFileExplorer(sqlite3** database,std::vector<physicsEngine::Triangle>* triangles,
                      std::vector<physicsEngine::Rectangle>* rectangles,
                      std::vector<physicsEngine::Circle>* circles, bool* fileLoaded);


bool saveAsFileExplorer(sqlite3** database,std::vector<physicsEngine::Triangle>* triangles,
                        std::vector<physicsEngine::Rectangle>* rectangles,
                        std::vector<physicsEngine::Circle>* circles, bool* fileLoaded);

#endif // FILEHANDLING_H_INCLUDED
