#include "fileHandling.h"
#include <windows.h>
#include <stdio.h>

using std::vector;

char fileExtension[] = ".meng";

bool attemptDatabaseOpen (sqlite3** database,char* path, vector<physicsEngine::Triangle>*triangles,
                          vector<physicsEngine::Rectangle>*rectangles,
                          vector<physicsEngine::Circle>*circles){



    if (sqlite3_open_v2(path,database,SQLITE_OPEN_READWRITE,NULL) != SQLITE_OK)
        return false;

    try{
        loadData(*database,rectangles,circles,triangles);
    }

    catch (loadMechanicsFail& e){ return false; }

    catch(loadTableFail& e){ return false; }

    return true;

}


bool openFileExplorer(sqlite3** database,vector<physicsEngine::Triangle>* triangles, vector<physicsEngine::Rectangle>* rectangles,
              vector<physicsEngine::Circle>* circles, bool* fileLoaded){

    //OPENFILENAME is the structure used to handle file opening
    OPENFILENAME openFileStruct;

    //This is where the path name of the MENG file the user chooses to open will be stored
    //MAX_PATH is 260, and is the max number of characters a file path can have on Windows
    char fileName[MAX_PATH] = "";

    //This function fills a given memory block with 0s
    //Saves having to initialise every element in openFileStruct to 0 if we aren't going to use it
    ZeroMemory(&openFileStruct,sizeof(openFileStruct));

    //Length of the structure must be stored
    openFileStruct.lStructSize = sizeof(openFileStruct);

    //Pointer to the filename (passing arrays passes the pointer to the first element of an array)
    openFileStruct.lpstrFile = fileName;
    openFileStruct.nMaxFile = MAX_PATH;

    //This is to allow the files to be filtered through
    openFileStruct.lpstrFilter = "MENG Files(*.meng)\0*.meng\0Database File(*.db;*.meng)\0*.db;*.meng\0All Files(*.*)\0*.*\0\0";

    //A set of bit flags that will be abided when file explorer opens
    //Here, two flags are ORed using the logical bitwise OR operator
    //They ensure that any file or path typed in the search box must exist
    openFileStruct.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    //The extension added if the user types a file but doesn't add an extension
    //The '.' isn't mentioned, so I provide a pointer to the 'm' in ".meng"
    openFileStruct.lpstrDefExt = &fileExtension[1];

    bool loopedOnce = false;
    while (GetOpenFileName(&openFileStruct)){

        /**Task: make a general save function as below must be repeated for save
           and save as**/
        int rectSize = rectangles->size();
        int triSize = triangles->size();
        int circSize = circles->size();

        if (!loopedOnce){
            if (*fileLoaded){
                int saveCurrentFile = MessageBox(NULL,"Do you want to save current file?","Save File?",
                                                 MB_ICONEXCLAMATION|MB_YESNO);

                if (saveCurrentFile == IDYES){

                    bool failedSave = false;

                    try{

                        saveData(*database,rectangles,circles,triangles);

                    }
                    catch (deletionFail& e){failedSave = true;}

                    catch (updateFail& e) { failedSave = true;}

                    if (failedSave){
                        MessageBox(NULL,"Could not save file. Try Again.",
                                   NULL,MB_OK);
                        return false;
                    }

                    /**
                        If the opening of a file succeeds, then the original contents
                        of the shape vectors need to be removed. So we need to store
                        their current size so we know which portions of the vectors
                        to delete later.
                        They cannot be deleted at this stage in case the opening of a file fails
                    **/
                    if (sqlite3_close_v2(*database) != SQLITE_OK){
                        MessageBox (NULL,"Could not close current file. Try again.",
                                    NULL,MB_OK);
                        return false;
                    }
                    *database = nullptr;
                }
            }

            loopedOnce = true;

        }

       if (attemptDatabaseOpen(database,fileName,triangles,rectangles,circles)){


            triangles->erase(triangles->begin(),triangles->begin()+triSize);
            rectangles->erase(rectangles->begin(),rectangles->begin()+rectSize);
            circles->erase(circles->begin(),circles->begin()+circSize);

           *fileLoaded = true;

           MessageBox (NULL,"File Loaded ","Loaded",MB_OK);
           return true;
       }

       else{
            MessageBox(NULL,"File invalid",NULL,MB_OK);

            return false;

       }

    }
}

bool saveAsFileExplorer(sqlite3** database,vector<physicsEngine::Triangle>* triangles,
						vector<physicsEngine::Rectangle>* rectangles,
                        vector<physicsEngine::Circle>* circles, bool* fileLoaded){

    char fileName[MAX_PATH] = "";
    OPENFILENAME saveAsFileStruct;
    ZeroMemory(&saveAsFileStruct,sizeof(saveAsFileStruct));

    saveAsFileStruct.lStructSize = sizeof(saveAsFileStruct);

    saveAsFileStruct.lpstrFile = fileName;
    saveAsFileStruct.nMaxFile = MAX_PATH;

    saveAsFileStruct.lpstrFilter = "MENG Files(*.meng)\0*.meng\0Database File(*.db;*.meng)\0*.db;*.meng\0All Files(*.*)\0*.*\0\0";

    saveAsFileStruct.Flags = OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;
    saveAsFileStruct.lpstrDefExt = &fileExtension[1];

    //If saving fails, I need to delete the file.
    //Further, I need to create a new database that is this saved file
    //If a file has not been loaded, then this will become the database in question of the program
    //If a file is already loaded, we close the database, and free up the memory
    while (GetSaveFileName(&saveAsFileStruct)){

        bool shouldDeleteFile = false;

        try{

            //If a file is already loaded, save As will just save file
            if (*fileLoaded){
                sqlite3* tempDb = setupDatabase(fileName);
                saveData(tempDb,rectangles,circles,triangles);
                sqlite3_close(tempDb);
            }
            else{
                *database = setupDatabase(fileName);
                saveData(*database,rectangles,circles,triangles);
                *fileLoaded = true;
            }

        }

        catch (dbCreationFail &e) { shouldDeleteFile = true;}

        catch (tableCreationFail &e) { shouldDeleteFile = true;}

        catch(deletionFail &e) {shouldDeleteFile = true;}

        catch(updateFail &e ){ shouldDeleteFile = true;}

        if (shouldDeleteFile){
            if (!fileLoaded && database != nullptr)
                sqlite3_close_v2(*database);

            remove(fileName);

            MessageBox (NULL,"Save as failed. Try again.",NULL,MB_OK);
            continue;

        }

        return true;

    }

    return false;

}
