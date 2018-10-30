#include <iostream>
#include <windows.h>
#include <string>
#include <cmath>
#include "utility.h"
#include "database.h"
#include "sdl_utilities.h"
#include "texture.h"
#include "utility.h"
#include "loadSDL.h"
#include "rendering_engine_global_vars.h"
#include "physics_engine_global_vars.h"
#include "renderingShapes.h"
#include "fileHandling.h"
#include "clicking_dragging_resizing.h"


/**
    VERY IMPORTANT:
        If I'm going to use pointer references referring to other elements from other vectors,
        only add those pointer references when I am 100% sure that the vector with the elements won't be changed anymore.
        I.e. don't start adding elements to the physics engine until the user actually presses the button to start physics engine,
        otherwise memory addresses aren't guaranteed to remain the same (rephrase this)
**/
#define TESTING 1

using namespace std;


#if TESTING == 1

/**Question: How do we check user hasn't dragged a shape into another.
Collision detection may not detect this if shape fits perfectly inside another shape(plan is to allow this)

MAKE SURE TO CHECK IF A TRAIANGLE DRAWN BY USER IS COLINEAR

DO a check to ensure shape instantiations are always in order

add merge sort in binary search function later on

remember to call resetMotionHandler on all shapes if simulation is restarted
**/

void colourWalls();

bool attemptDatabaseOpen(sqlite3 **database, char *path, vector<physicsEngine::Triangle> *triangles,
                         vector<physicsEngine::Rectangle> *rectangles,
                         vector<physicsEngine::Circle> *circles);

void setupTriangleCoordinates(vector<physicsEngine::Triangle> *triangles,
                              vector<vector<shortRendering> > *trianglesCoordinates);

void setupCircleCoordinates(vector<physicsEngine::Circle> *circles,
                            vector<vector<shortRendering> > *circlesCoordinates);

bool rectValidOnMap(int x, int y, int w, int h);

bool triValidOnMap(int x1, int y1, int x2, int y2, int x3, int y3);

bool circleValidOnMap(int x, int y, int radius);


//A quick reference to the four walls so they don't have to be explicitly written out every ti,e
physicsEngine::Rectangle *fourWalls[4] = {&physicsEngine::topWall,
                                          &physicsEngine::bottomWall,
                                          &physicsEngine::rightWall,
                                          &physicsEngine::leftWall};

void colourWalls() {
    physicsEngine::topWall.changeColour(0, 0, 0, 0);
    physicsEngine::bottomWall.changeColour(0, 0, 0, 0);
    physicsEngine::leftWall.changeColour(0, 0, 0, 0);
    physicsEngine::rightWall.changeColour(0, 0, 0, 0);
}

/**
When shapes are loaded, the following functions are used to set up rendering
coordinates for triangles and circles.
This is used when a file has been loaded

**/
void setupTriangleCoordinates(vector<physicsEngine::Triangle> *triangles,
                              vector<renderingCoordinates> *trianglesCoordinates) {

    //Erase old contents if any
    trianglesCoordinates->erase(trianglesCoordinates->begin(), trianglesCoordinates->end());

    for (int i = 0; i < triangles->size(); i++) {
        renderingCoordinates coordinates;
        setupTriangleRendering(&(*triangles)[i], &coordinates);
        trianglesCoordinates->push_back(coordinates);
    }

}

void setupCircleCoordinates(vector<physicsEngine::Circle> *circles,
                            vector<renderingCoordinates> *circlesCoordinates) {

    //Erase old contents if any
    circlesCoordinates->erase(circlesCoordinates->begin(), circlesCoordinates->end());

    for (int i = 0; i < circles->size(); i++) {
        renderingCoordinates coordinates;
        setupCircleRendering(&(*circles)[i], &coordinates);
        circlesCoordinates->push_back(coordinates);
    }

}

void copyRect(vector<physicsEngine::Rectangle> &copyTo,
              vector<physicsEngine::Rectangle> &copyFrom) {

    if (copyFrom.size() == 0)
        return;

    copyTo.clear();

    for (int i = 0; i < copyFrom.size(); i++) {
        physicsEngine::Rectangle rect(convertIntDouble(copyFrom[i].xPosition()),
                                      convertIntDouble(copyFrom[i].yPosition()),
                                      physicsEngine::DEFAULT_FRAME_RATE,
                                      copyFrom[i].getWidth(),
                                      copyFrom[i].getHeight(),
                                      copyFrom[i].getMass());


        rect.changeSpeedx(copyFrom[i].xSpeed());
        rect.changeSpeedy(copyFrom[i].ySpeed());
        rect.changeAccelerationx(copyFrom[i].xAcceleration());
        rect.changeAccelerationy(copyFrom[i].yAcceleration());
        rect.gravityEffect(copyFrom[i].getGravityEffect());

        bool highlighted = false;
        if (copyFrom[i].shouldHighlightShape) {
            copyFrom[i].shouldHighlightShape = false;
            highlighted = true;
        }


        rect.changeColour(copyFrom[i].rValue(), copyFrom[i].gValue(), copyFrom[i].bValue(),
                          copyFrom[i].aValue());


        if (highlighted)
            copyFrom[i].shouldHighlightShape = true;


        copyTo.push_back(rect);
    }
}

void copyTri(vector<physicsEngine::Triangle> &copyTo,
             vector<physicsEngine::Triangle> &copyFrom) {

    if (copyFrom.size() == 0)
        return;


    copyTo.clear();

    for (int i = 0; i < copyFrom.size(); i++) {
        physicsEngine::Triangle tri(convertIntDouble(copyFrom[i].xcoordPoint1()),
                                    convertIntDouble(copyFrom[i].ycoordPoint1()),
                                    convertIntDouble(copyFrom[i].xcoordPoint2()),
                                    convertIntDouble(copyFrom[i].ycoordPoint2()),
                                    convertIntDouble(copyFrom[i].xcoordPoint3()),
                                    convertIntDouble(copyFrom[i].ycoordPoint3()),
                                    physicsEngine::DEFAULT_FRAME_RATE);

        tri.changeSpeedx(copyFrom[i].xSpeed());
        tri.changeSpeedy(copyFrom[i].ySpeed());
        tri.changeAccelerationx(copyFrom[i].xAcceleration());
        tri.changeAccelerationy(copyFrom[i].yAcceleration());
        tri.changeMass(copyFrom[i].getMass());
        tri.gravityEffect(copyFrom[i].getGravityEffect());


        bool highlighted = false;
        if (copyFrom[i].shouldHighlightShape) {
            copyFrom[i].shouldHighlightShape = false;
            highlighted = true;
        }

        tri.changeColour(copyFrom[i].rValue(), copyFrom[i].gValue(), copyFrom[i].bValue(),
                         copyFrom[i].aValue());

        if (highlighted)
            copyFrom[i].shouldHighlightShape = true;


        copyTo.push_back(tri);
    }
}

void copyCircle(vector<physicsEngine::Circle> &copyTo,
                vector<physicsEngine::Circle> &copyFrom) {

    if (copyFrom.size() == 0)
        return;

    copyTo.clear();

    for (int i = 0; i < copyFrom.size(); i++) {
        physicsEngine::Circle circle(convertIntDouble(copyFrom[i].xPosition()),
                                     convertIntDouble(copyFrom[i].yPosition()),
                                     copyFrom[i].getRadius(),
                                     physicsEngine::DEFAULT_FRAME_RATE);

        circle.changeSpeedx(copyFrom[i].xSpeed());
        circle.changeSpeedy(copyFrom[i].ySpeed());
        circle.changeAccelerationx(copyFrom[i].xAcceleration());
        circle.changeAccelerationy(copyFrom[i].yAcceleration());
        circle.changeMass(copyFrom[i].getMass());
        circle.gravityEffect(copyFrom[i].getGravityEffect());

        bool highlighted = false;
        if (copyFrom[i].shouldHighlightShape) {
            copyFrom[i].shouldHighlightShape = false;
            highlighted = true;
        }

        circle.changeColour(copyFrom[i].rValue(), copyFrom[i].gValue(), copyFrom[i].bValue(),
                            copyFrom[i].aValue());

        if (highlighted)
            copyFrom[i].shouldHighlightShape = true;


        copyTo.push_back(circle);
    }
}


/**
    The following functions check a user hasn't tried to drag a shape
    into any of the four walls.

    The reason the functions in collisionDetection.cpp are not used,
    is because they only check for direct intersections of edges between shapes,
    not if one shape is wholly inside the other
**/

bool rectValidOnMap(int x, int y, int w, int h) {

    if (w < minRectWidth)
        return false;

    if (h < minRectHeight)
        return false;

    for (int i = 0; i < 4; i++) {

        if (pointInRect(fourWalls[i], x, y) ||
            pointInRect(fourWalls[i], x + w, y) ||
            pointInRect(fourWalls[i], x, y - h) ||
            pointInRect(fourWalls[i], x + w, y - h)) {

            return false;
        }

    }

    //Check if any points are outside simulation map
    if (x < -physicsEngine::simulationWidth / 2 || x + w > physicsEngine::simulationWidth / 2)
        return false;

    if (y > physicsEngine::simulationHeight / 2 || y - h < -physicsEngine::simulationHeight / 2)
        return false;

    return true;

}

bool triValidOnMap(int x1, int y1, int x2, int y2, int x3, int y3) {

    int xCoords[] = {x1, x2, x3};
    int yCoords[] = {y1, y2, y3};

    int i;
    for (i = 0; i < 4; i++) {

        if (pointInRect(fourWalls[i], x1, y1) ||
            pointInRect(fourWalls[i], x2, y2) ||
            pointInRect(fourWalls[i], x3, y3)) {
            return false;
        }
    }

    //Check points outside map
    for (i = 0; i < 3; i++) {
        if (xCoords[i] < -physicsEngine::simulationWidth / 2 || xCoords[i] > physicsEngine::simulationWidth / 2)
            return false;
    }

    for (i = 0; i < 3; i++) {
        if (yCoords[i] < -physicsEngine::simulationHeight / 2 || yCoords[i] > physicsEngine::simulationHeight / 2)
            return false;
    }


    return true;

}

bool circleValidOnMap(int x, int y, int radius) {
    
    /**
        To check if a circle is inside a wall, we check if the distance from
        the centre of the circle to each wall is less than its radius.
        We only need to check one coordinate axis.

        For the left and right wall, we use the x coordinate axis, for
        the top and bottom wall we use the y coordinate axis
    **/
    if (radius < minCircleRadius)
        return false;

    for (int i = 0; i < 4; i++) {
        int rectx = convertIntDouble(fourWalls[i]->xPosition());
        int recty = convertIntDouble(fourWalls[i]->yPosition());

        int distanceSquared;

        //Check if the centre itself lies in one of the walls
        if (pointInRect(fourWalls[i], x, y))
            return false;

        //x coordinate of rectangle is from top left
        //We want the x coordinate of edge opposite that
        if (fourWalls[i] == &physicsEngine::leftWall) {
            rectx += fourWalls[i]->getWidth();
        }

        if (fourWalls[i] == &physicsEngine::topWall) {
            recty -= fourWalls[i]->getHeight();
        }

        if (fourWalls[i] == &physicsEngine::topWall ||
            fourWalls[i] == &physicsEngine::bottomWall) {

            distanceSquared = (recty - y) * (recty - y);
        } else if (fourWalls[i] == &physicsEngine::leftWall ||
                   fourWalls[i] == &physicsEngine::rightWall) {

            distanceSquared = (rectx - x) * (rectx - x);

        }


        if (distanceSquared < radius * radius)
            return false;
    }

    //Check if coordinates outside map
    if (x < -physicsEngine::simulationWidth / 2 || x > physicsEngine::simulationWidth / 2)
        return false;

    if (y < -physicsEngine::simulationHeight / 2 || y > physicsEngine::simulationHeight / 2)
        return false;


    return true;
}

enum DATA_BOX_DATA {
    SIMULATION_CONSTANTS = 0,
    RECTANGLE_DATA,
    CIRCLE_DATA,
    TRIANGLE_DATA
};

struct textChanging {

    bool isTextChanging;

    enum TEXT_INPUT_TYPE {
        T_INTEGER = 0,
        T_DECIMAL
    } inputType;

    string preTextInput;
    string textInput;
    SDL_Colour colour;
    SDL_Rect *displayRect;
    Texture *textPtr;

    union SHAPE_PTR {
        physicsEngine::Rectangle *rect;
        physicsEngine::Circle *circle;
        physicsEngine::Triangle *tri;
    } shape;

};

enum SIMULATION_STATE {
    NOT_RUNNING = 0,
    RUNNING,
    PAUSED
};

struct simulation {

    //i.e. microseconds or milliseconds (so will either be 1000000 or 1000)
    int subseconds;
    int frameRate;
    int frequency;
    int countNum;


    SIMULATION_STATE simulationRunning;


} simInfo;

int main(int argc, char *args[]) {


    /**
        Below are a set of unions used to measure time so the frame rate
        can be properly calculated in the simulation
    **/
    LARGE_INTEGER frequency;
    LARGE_INTEGER startTicks, currentTicks;

    //Gets the frequency of the counter
    QueryPerformanceFrequency(&frequency);


    int i, j;

    sqlite3 *database = nullptr;

    vector<physicsEngine::Triangle> triangles;
    vector<physicsEngine::Rectangle> rectangles;
    vector<physicsEngine::Circle> circles;

    //These are copies of the three shapes that will be supplied to the physics engine
    //This is to store original data when a simulation begins so data can later on be reversed
    vector<physicsEngine::Triangle> trianglesCopy;
    vector<physicsEngine::Rectangle> rectanglesCopy;
    vector<physicsEngine::Circle> circlesCopy;

    /**
       Each element's index for both vectors
       will have the same index as the shape it represents in the above vectors

       To guarantee this, these vectors will only be set up when the user is ready to run
       the simulation. If the simulation is stopped (not paused, but stopped), these
       vectors will be reset. This additionally means the user cannot make any changes
       while the simulation is running
    **/

    vector<renderingCoordinates> trianglesCoordinates;
    vector<renderingCoordinates> circlesCoordinates;

    colourWalls();

    physicsEngine::physicsEngineHandler pEngine(decimalPlaces);

    simInfo.simulationRunning = NOT_RUNNING;
    bool fileLoaded = false;
    /**
        If the user opens a file using this program, the file path will be provided
        as an argument to main (which will be the second pointer of args
    **/

    if (argc > 1) {
        fileLoaded = attemptDatabaseOpen(&database, args[1], &triangles, &rectangles, &circles);
        if (!fileLoaded) {
            /**
                The windows API provides a function called MessageBox, which allows a
                message box to be displayed on the screen.

                If the user attempts to open a file with this program that is invalid,
                a message box will pop up alerting the user of this.
                They will then be given a choice of starting this program afresh or exiting.
            **/
            int exitOrContinue = MessageBox(NULL, "File is an invalid file for MENG.\r"\
                                            "Do you want to start a new program?", NULL, MB_YESNO);

            if (exitOrContinue == IDNO)
                return 0;

            triangles.clear();
            rectangles.clear();
            circles.clear();
            if (database != nullptr)
                sqlite3_close(database);
        }
    }

    //Used to index the simulation constants presented to the user
    enum CONSTANTS_INDICES {
        GRAVITY = 0,
        CIRC_RECT_RES,
        CIRC_TRI_RES,
        CIRC_BOUND_RES,
        CIRCS_RES,
        RECT_TRI_RES,
        RECTS_RES,
        RECT_BOUND_RES,
        TRIS_RES,
        TRI_BOUND_RES,
        FLOAT_CONSTANTS
    };

    //Used to index certain pieces of data present in all shapes
    enum SHAPES_VALUES_INDICES {
        MASS = 0,
        XVELOCITY,
        YVELOCITY,
        XACCELERATION,
        YACCELERATION,
        GRAVITY_EFFECT,
        RVALUE,
        GVALUE,
        BVALUE,
        AVALUE,
        TOTAL_GENERAL_VALUES,
    };

    //Used to index rectangle data
    enum RECTANGLES_VALUES_INDICES {
        TOP_LEFT_X = 0,
        TOP_LEFT_Y,
        WIDTH,
        HEIGHT,
        RECT_VALUES
    };

    //Indexing circle data
    enum CIRCLES_VALUES_INDICES {
        CENTRE_X = 0,
        CENTRE_Y,
        RADIUS,
        CIRCLE_VALUES
    };

    //Indexing triangle data
    enum TRIANGLES_VALUES_INDICES {
        P1X = 0,
        P1Y,
        P2X,
        P2Y,
        P3X,
        P3Y,
        TRI_VALUES
    };


    double *constantsArray[FLOAT_CONSTANTS] = {&physicsEngine::gravitationalAcceleration,
                                               &physicsEngine::circleToRectangleRestitution,
                                               &physicsEngine::circleToTriangleRestitution,
                                               &physicsEngine::circleToBoundaryRestitution,
                                               &physicsEngine::circleToCircleRestitution,
                                               &physicsEngine::rectangleToTriangleRestitution,
                                               &physicsEngine::rectangleTorectangleRestitution,
                                               &physicsEngine::rectangleToBoundaryRestitution,
                                               &physicsEngine::triangleToTriangleRestitution,
                                               &physicsEngine::triangleToBoundaryRestitution};

    //The other constant in the physics engine is frame rate
    const int FRAME_RATE_INDEX = FLOAT_CONSTANTS;
    const int TOTAL_CONSTANTS = FRAME_RATE_INDEX + 1;


    //A structure that will store monitor information such as the width and height of the monitor
    SDL_DisplayMode monitorInformation;
    //The SDL_Window structure
    SDL_Window *window = nullptr;
    //This program will work using texture-based rendering, so it will use SDL_Renderer instead of SDL_Surface
    SDL_Renderer *renderer = nullptr;

    //Dimensions of the window
    int windowWidth, windowHeight;

    //The position of the top left corner of the window relative to the monitor
    int window_x, window_y;

    //Window title which will be shown on the window bar
    const char *windowTitle = "MENG";

    //The name of the true type font that will be used in the program
    const char *calibri = "C:\\Windows\\Fonts\\Calibri.ttf";

    //The different font sizes that will be used
    TTF_Font *calibri_48 = nullptr;
    TTF_Font *calibri_24 = nullptr;
    TTF_Font *calibri_18 = nullptr;

    SDL_Color black{0, 0, 0, 255};
    SDL_Color white{255, 255, 255, 255};

    SDL_Rect menuBar, dataBox;
    SDL_Rect consoleBox, shapes, simulationMap, shapesBox;

    const short int numMenuBarButtons = 6;

    /**IF I AM GOING TO HAVE THIS ALL IN ARRAYS, THEN THEY MAY NOT BE NEEDED
        ALL TOGETHER**/

    Texture newDisplay, openDisplay, saveDisplay, saveAsDisplay, runDisplay;
    Texture constantsDisplay;

    Texture pauseImage, resumeImage, stopImage;

    //Used to store dimensions for menu bar texts
    SDL_Rect newDisplayBox, openDisplayBox, saveDisplayBox, saveAsDisplayBox, runDisplayBox;
    SDL_Rect constantsDisplayBox;

    SDL_Rect pauseRect, resumeRect, stopRect;


    SDL_Rect *menuBarButtons[numMenuBarButtons] = {&newDisplayBox, &openDisplayBox, &saveDisplayBox, &saveAsDisplayBox,
                                                   &constantsDisplayBox, &runDisplayBox,};

    Texture *menuBarTexts[numMenuBarButtons] = {&newDisplay, &openDisplay, &saveDisplay, &saveAsDisplay,
                                                &constantsDisplay, &runDisplay,};

    /** These are used to store the dimensions of the texts that will be
        used to provide data on shapes and simulation data**/

    SDL_Rect *constantsRects = new SDL_Rect[TOTAL_CONSTANTS];


    SDL_Rect *generalShapeValuesRects = new SDL_Rect[TOTAL_GENERAL_VALUES];

    SDL_Rect *rectValuesRects = new SDL_Rect[RECT_VALUES];

    SDL_Rect *circleValuesRects = new SDL_Rect[CIRCLE_VALUES];

    SDL_Rect *triValuesRects = new SDL_Rect[TRI_VALUES];


    /**These store the texts that will be outputted on the screen
     for providing data on shapes and simulation data**/
    Texture *constantsTexts = new Texture[TOTAL_CONSTANTS];


    Texture *generalShapeValuesTexts = new Texture[TOTAL_GENERAL_VALUES];
    Texture *rectValuesTexts = new Texture[RECT_VALUES];
    Texture *circleValuesTexts = new Texture[CIRCLE_VALUES];
    Texture *triValuesTexts = new Texture[TRI_VALUES];

    //Second element in each row will have the value being displayed on screen stored
    //This will be for the purposes of text input
    string constantsStrings[TOTAL_CONSTANTS][2] = {{"Gravity: ",                         ""},
                                                   {"Circle-Rectangle Restitution: ",    ""},
                                                   {"Circle-Triangle Restitution: ",     ""},
                                                   {"Circle-Boundary Restitution: ",     ""},
                                                   {"Circle-Circle Restitution: ",       ""},
                                                   {"Rectangle-Triangle Restitution: ",  ""},
                                                   {"Rectangle-Rectangle Restitution: ", ""},
                                                   {"Rectangle-Boundary Restitution: ",  ""},
                                                   {"Triangle-Triangle Restitution: ",   ""},
                                                   {"Triangle-Boundary Restitution: ",   ""},
                                                   {"Frame Rate: "}};


    string generalShapeValues[TOTAL_GENERAL_VALUES][2] = {{"Mass: ",                ""},
                                                          {"Velocity x-axis: ",     ""},
                                                          {"Velocity y-axis: ",     ""},
                                                          {"Acceleration x-axis: ", ""},
                                                          {"Acceleration y-axis: ", ""},
                                                          {"Affected by Gravity: ", ""},
                                                          {"Red: ",                 ""},
                                                          {"Green: ",               ""},
                                                          {"Blue: ",                ""},
                                                          {"Alpha: ",               ""}};

    string rectValues[RECT_VALUES][2] = {{"Top-left x: ", ""},
                                         {"Top-left y: ", ""},
                                         {"Width: ",      ""},
                                         {"Height: ",     ""}};

    string circleValues[CIRCLE_VALUES][2] = {{"Centre x: ", ""},
                                             {"Centre y: ", ""},
                                             {"Radius: ",   ""}};

    string triValues[TRI_VALUES][2] = {{"Point 1 x: ", ""},
                                       {"Point 1 y: ", ""},
                                       {"Point 2 x: ", ""},
                                       {"Point 2 y: ", ""},
                                       {"Point 3 x: ", ""},
                                       {"Point 3 y: ", ""}};


    bool endProgram = false;
    SDL_Event event;


    //Initialise SDL. Specifically we'll need to use its timing, video and events features
    //If SDL fails to initialise it returns -1
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        cout << "Failed to initialise SDL";
        return -1;
    }

    //Gets information on the indexed monitor,which is stored in the monitorInformation structure
    SDL_GetDesktopDisplayMode(0, &monitorInformation);

    //The following statements set up the window screen dimensions and position on the monitor
    if (monitorInformation.w > physicsEngine::simulationWidth)
        windowWidth = maxWindowWidth;
    else
        windowWidth = monitorInformation.w * widthScale;

    if (monitorInformation.h > physicsEngine::simulationHeight)
        windowHeight = maxWindowHeight;

    else
        windowHeight = monitorInformation.h * heightScale;

    window_x = windowWidth * windowPos_x_scale;
    window_y = windowHeight * windowPos_y_scale;

    SDL_Rect windowRect = {0, 0, windowWidth, windowHeight};

    if (!initWindow(&window, windowTitle, window_x, window_y, windowWidth, windowHeight, SDL_WINDOW_SHOWN)) {
        cout << "Failed to initialise SDL window";
        return -1;
    }

    if (!initRenderer(&renderer, window, -1, SDL_RENDERER_ACCELERATED)) {
        cout << "Failed to initialise the SDL renderer";
        return -1;
    }

    if (!initSDLImage(IMG_INIT_PNG)) {
        cout << "Failed to initialise SDL PNG image support";
        return -1;
    }

    if (!initialiseTTF()) {
        printf("Failed to initialise TTF: %s\n", TTF_GetError());
        return -1;
    }

    if (!loadFont(&calibri_48, calibri, 48)) {
        printf("Couldn't initialise calibri 48: %s\n", TTF_GetError());
        return -1;

    }

    if (!loadFont(&calibri_24, calibri, 24)) {
        printf("Couldn't initialise calibri 24: %s\n", TTF_GetError());
        return -1;
    }

    if (!loadFont(&calibri_18, calibri, 18)) {
        printf("Could not initialise calibri 18: %s\n", TTF_GetError());
        return -1;
    }

    //Load the texts for the menu button
    if (!newDisplay.loadText(calibri_24, "New", black, renderer) ||
        !openDisplay.loadText(calibri_24, "Open", black, renderer) ||
        !saveDisplay.loadText(calibri_24, "Save", black, renderer) ||
        !saveAsDisplay.loadText(calibri_24, "Save as", black, renderer) ||
        !runDisplay.loadText(calibri_24, "Run Simulation", black, renderer) ||
        !constantsDisplay.loadText(calibri_24, "Constants", black, renderer)) {
        printf("Failed to load menu bar texts. ");
        return -1;
    }


    //Load the images for pausing and resuming
    if (!pauseImage.loadImage(renderer, "pause.png") ||
        !resumeImage.loadImage(renderer, "resume.png") ||
        !stopImage.loadImage(renderer, "stop.png")) {

        cout << "Failed to load all of the pause, stop and resume images";
        return -1;
    }

    //Dimensions for the menu bar
    menuBar.x = 0;
    menuBar.y = 0;
    menuBar.w = windowWidth;
    menuBar.h = menuBarHeight;

    //Dimensions for the data entry box
    dataBox.x = 0;
    dataBox.y = menuBarHeight;
    dataBox.w = windowWidth * dataBoxWidthRatio;
    dataBox.h = (windowHeight - menuBarHeight) * dataBoxHeightRatio;

    //Dimensions for the console box
    consoleBox.x = 0;
    consoleBox.y = dataBox.y + dataBox.h;
    consoleBox.w = windowWidth * consoleBoxWidthRatio;
    consoleBox.h = (windowHeight - menuBarHeight) * consoleBoxHeightRatio;

    //Dimensions for the simulation map
    simulationMap.x = dataBox.w;
    simulationMap.y = menuBarHeight;
    simulationMap.w = windowWidth - dataBox.w;
    simulationMap.h = (windowHeight - menuBarHeight) * simulationMapHeightRatio;

    struct MapPanning {
        int dy, dx;
        bool panningMap;
        int prevMousex, prevMousey;
    } mapPosition;

    mapPosition.dy = 0;
    mapPosition.dx = 0;
    mapPosition.panningMap = false;

    //Dimensions for shapes box (the shapes displayed so the user can drag them on the screen)
    shapesBox.x = dataBox.w;
    shapesBox.y = simulationMap.y + simulationMap.h;
    shapesBox.w = windowWidth - dataBox.w;
    shapesBox.h = windowHeight - menuBarHeight - simulationMap.h;

    int shapesBox_x_converted = inverseConvertX(0, shapesBox.w, 0) - 1;
    int shapesBox_y_converted = inverseConvertY(0, shapesBox.h, 0) + 1;

    /**
        Strictly speaking, this is unnecessary.
        When calling the renderRectangle function, one must pass a Rectangle object
        of the simulation map, which is converted from screen coordinates to normal
        Cartesian coordinates. This is used to check the rectangle is valid to render.

        In this case, with the shapes that will be displayed, we know that they will
        be valid to render, however I used this for testing purposes, and it didn't make
        much of a difference if I removed it.

    **/

    physicsEngine::Rectangle shapesBoxRect(shapesBox_x_converted,
                                           shapesBox_y_converted,
                                           physicsEngine::DEFAULT_FRAME_RATE,
                                           shapesBox.w,
                                           shapesBox.h);

    //Dimensions for open button
    newDisplayBox.x = 5;
    newDisplayBox.y = 5;
    newDisplayBox.w = newDisplay.width_size();
    newDisplayBox.h = newDisplay.height_size();

    /*This loop sorts out the position of the menu bar buttons
      Each button will be next to the one that comes before it*/

    for (i = 1; i < numMenuBarButtons; i++) {
        menuBarButtons[i]->x = menuBarButtons[i - 1]->x + menuBarButtons[i - 1]->w + spacingMenuButtons;
        menuBarButtons[i]->y = 5;
        menuBarButtons[i]->w = menuBarTexts[i]->width_size();
        menuBarButtons[i]->h = menuBarTexts[i]->height_size();
    }

    pauseRect.x = menuBar.w / 2;
    pauseRect.y = 0;
    pauseRect.w = menuBarHeight;
    pauseRect.h = menuBarHeight;

    resumeRect.x = menuBar.w / 2;
    resumeRect.y = 0;
    resumeRect.w = menuBarHeight;
    resumeRect.h = menuBarHeight;

    stopRect.x = resumeRect.x + resumeRect.w + 5;
    stopRect.y = 0;
    stopRect.w = menuBarHeight;
    stopRect.h = menuBarHeight;


    cout << "Monitor width is : " << monitorInformation.w << " Monitor height is: " << monitorInformation.h << endl;

    //Spacing between texts in data box
    int spacingConstants = (dataBox.h * 0.4) / FLOAT_CONSTANTS;
    int spacingShapes = (dataBox.h * 0.1) / TOTAL_GENERAL_VALUES;


    MOUSE_STATE mouseClickedState = NOT_CLICKED;
    shapeClicked displayShapeClicked(false);
    shapeClicked simulationShape(true);

    bool displayShapePressed = false;

    bool menuBarRendered = false;
    bool mouseInMenuButton = false;
    bool mouseDownMenuButton = false;

    bool miniMapRendered = false;
    bool simulationMapRendered = false;
    bool displayShapesRendered = false;
    bool menuButtonsRendered = false;
    bool dataBoxRendered = false;

    //This is used to determine what to display in the databox

    DATA_BOX_DATA dataToDisplay = SIMULATION_CONSTANTS;
    textChanging textInput;
    textInput.isTextChanging = false;

    textInput.colour = black;

    /**
    This is used in case one clicked the mouse button down while not clicking a button,
    and then clicks up when they touch a button.

    This is needed, because certain buttons that are clicked should not
    instantly run a certain operation unless the user fully clicks it (so they hold the mouse
    button down as well as up while in the pointer is in the button).

    This is includes aspects like opening a file, saving, save as.

    This is a fairly common standard on most applications, so I'm just assuming my
    program doesn't veer away from commonaility
    **/
    bool shouldHandleMouseClick = false;

    SDL_Rect *rectMouseInside = nullptr;
    Texture *textMouseInside = nullptr;

    //Dimensions for shapes to display
    SDL_Rect rectDisplay{-shapesBox.w / 3, shapesBox.h / 3, shapesBox.w / 6, shapesBox.h / 1.5};

    physicsEngine::Rectangle displayRectangle(rectDisplay.x, rectDisplay.y, physicsEngine::DEFAULT_FRAME_RATE,
                                              rectDisplay.w, rectDisplay.h);

    //The display rectangle will be red
    displayRectangle.changeColour(255, 0, 0, 255);

    physicsEngine::Triangle displayTriangle(rectDisplay.x + rectDisplay.w + shapesBox.w / 10,
                                            rectDisplay.y - rectDisplay.h,
                                            rectDisplay.x + rectDisplay.w + shapesBox.w / 10,
                                            rectDisplay.y,
                                            rectDisplay.x + rectDisplay.w + shapesBox.w / 5,
                                            rectDisplay.y - rectDisplay.h,
                                            physicsEngine::DEFAULT_FRAME_RATE);

    renderingCoordinates displayTriangleCoordinates;
    setupTriangleRendering(&displayTriangle, &displayTriangleCoordinates);
    displayTriangle.changeColour(0, 255, 255, 255);

    physicsEngine::Circle displayCircle(shapesBox.w / 5, 0, shapesBox.w / 30, physicsEngine::DEFAULT_FRAME_RATE);
    renderingCoordinates displayCircleCoordinates;
    displayCircle.changeColour(255, 255, 0, 255);

    setupCircleRendering(&displayCircle, &displayCircleCoordinates);

    //Used to determine whether to update the renderer or not
    bool renderWasChanged = false;

    while (!endProgram) {
        if (!simulationMapRendered) {
            //Simulation map is a shade of green
            SDL_SetRenderDrawColor(renderer, 34, 177, 76, 255);
            SDL_RenderFillRect(renderer, &simulationMap);
            SDL_RenderSetViewport(renderer, &simulationMap);

            //Get cartesian coordinates of portion simulation map displayed on screen
            int convertedSimulationx = inverseConvertX(0, simulationMap.w, mapPosition.dx);
            int convertedSimulationy = inverseConvertY(0, simulationMap.h, mapPosition.dy);

            //A rectangle object representing the simulation map displayed on screen
            physicsEngine::Rectangle simulationRect(convertedSimulationx, convertedSimulationy,
                                                    physicsEngine::DEFAULT_FRAME_RATE,
                                                    simulationMap.w, simulationMap.h);
            //Render the four walls
            for (i = 0; i < 4; i++) {
                renderRectangle(renderer, fourWalls[i], &simulationRect,
                                simulationMap.w, simulationMap.h,
                                mapPosition.dx, mapPosition.dy);
            }

            for (i = 0; i < rectangles.size(); i++) {
                renderRectangle(renderer, &rectangles[i],
                                &simulationRect,
                                simulationMap.w, simulationMap.h,
                                mapPosition.dx, mapPosition.dy);
            }

            for (i = 0; i < triangles.size(); i++) {
                renderTriangle(renderer, &triangles[i], &simulationRect,
                               &trianglesCoordinates[i],
                               simulationMap.w, simulationMap.h,
                               mapPosition.dx, mapPosition.dy);
            }

            for (i = 0; i < circles.size(); i++) {
                renderCircle(renderer, &circles[i], &circlesCoordinates[i],
                             simulationMap.w, simulationMap.h,
                             mapPosition.dx, mapPosition.dy);
            }

            //Render circles for resizing if a rectangle or triangle is being clicked
            //And the simulation is not running
            if (simulationShape.mouseState == CLICKED && !simInfo.simulationRunning) {

                if (simulationShape.shapeNum == shapeClicked::RECTANGLE) {

                    for (i = 0; i < 8; i++) {
                        renderCircle(renderer, *simulationShape.resizeShape.rect.allRectCircles[i],
                                     simulationShape.coords.circleRect.allRectCirclesCoords[i],
                                     simulationMap.w, simulationMap.h, mapPosition.dx, mapPosition.dy);
                    }
                } else if (simulationShape.shapeNum == shapeClicked::TRIANGLE) {

                    for (i = 0; i < 3; i++) {
                        renderCircle(renderer, *simulationShape.resizeShape.tri.allTriCircles[i],
                                     simulationShape.coords.circleTri.allTriCirclesCoords[i],
                                     simulationMap.w, simulationMap.h, mapPosition.dx, mapPosition.dy);
                    }
                }
            }

            SDL_RenderSetViewport(renderer, &windowRect);

            simulationMapRendered = true;
            renderWasChanged = true;
        }

        //Menu bar is set as a shade of white
        if (!menuBarRendered) {
            SDL_SetRenderDrawColor(renderer, 224, 224, 224, 255);
            SDL_RenderFillRect(renderer, &menuBar);

            //Render all of the menu buttons or simulation video buttons
            if (!simInfo.simulationRunning) {
                for (i = 0; i < numMenuBarButtons; i++)
                    menuBarTexts[i]->render(renderer, menuBarButtons[i], nullptr);

            } else {
                if (simInfo.simulationRunning == RUNNING)
                    pauseImage.render(renderer, &pauseRect, nullptr);

                else if (simInfo.simulationRunning == PAUSED)
                    resumeImage.render(renderer, &resumeRect, nullptr);

                stopImage.render(renderer, &stopRect, nullptr);
            }

            menuBarRendered = true;
            renderWasChanged = true;
        }

        if (!miniMapRendered) {
            //Console box is black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &consoleBox);

            miniMapRendered = true;
            renderWasChanged = true;
        }

        //Here, the data box will be rendered
        if (!dataBoxRendered) {

            //Databox is a shade of grey
            SDL_SetRenderDrawColor(renderer, 96, 96, 96, 1);
            SDL_RenderFillRect(renderer, &dataBox);

            //If the constants of the simulation are to be displayed
            if (dataToDisplay == SIMULATION_CONSTANTS) {
                //Here, we loop through each number that represents one of the simulation constants
                for (i = 0; i < TOTAL_CONSTANTS; i++) {
                    //Converts float to string
                    string val;

                    //Frame rate is an integer
                    if (i == FRAME_RATE_INDEX)
                        val = convert_str(physicsEngine::DEFAULT_FRAME_RATE);

                        //Gravity is 1 dp
                    else if (i == GRAVITY)
                        val = convert_str_d(*constantsArray[i]);

                        //Restitution is 2 dp, hence for the two separate function calls
                    else
                        val = convert_str_d(*constantsArray[i], restitutionDecimalPlaces);

                    constantsTexts[i].loadText(calibri_18,
                                               (constantsStrings[i][0] + val).c_str(),
                                               white, renderer);
                    //The value is stored so it can be referenced if the user tries to modify the data
                    constantsStrings[i][1] = val;
                }

                //The SDL_Rect for the gravity text is set up for the purposes of rendering the text...
                //..."Gravitational Acceleration ", and the value of the gravitational acceleration
                constantsRects[GRAVITY].x = dataBox.x,
                        constantsRects[GRAVITY].y = dataBox.y;
                constantsRects[GRAVITY].w = constantsTexts[GRAVITY].width_size();
                constantsRects[GRAVITY].h = constantsTexts[GRAVITY].height_size();
                constantsTexts[GRAVITY].render(renderer, &constantsRects[GRAVITY], nullptr);

                /**
                    The same thing is now done for the rest of the constants

                    On screen, they'll be displayed one under the other, hence why
                    it's easier to place them in a for loop. As the gravity text comes first,
                    it wasn't a part of the loop, as each time the loop iterates,
                    the previous text is accessed
                **/

                for (i = 1; i < TOTAL_CONSTANTS; i++) {
                    //The y coordinate of the top left corner of the previous text and its height
                    int prevy = constantsRects[i - 1].y;
                    int prevh = constantsRects[i - 1].h;

                    constantsRects[i].x = dataBox.x;
                    constantsRects[i].y = prevy + prevh + spacingConstants;
                    constantsRects[i].w = constantsTexts[i].width_size();
                    constantsRects[i].h = constantsTexts[i].height_size();

                    constantsTexts[i].render(renderer, &constantsRects[i], nullptr);

                }
            }

                /**
                    Simulation constants is the default display of the data box. So if this isn't
                    being displayed, it can only mean the user clicked a shape or dragged a shape onto the screen,
                    and now data on that specific shape will be shown
                **/
            else {
                /**
                    These are the general data items that all three shapes will have.
                    The following statements concatenate the captioned text (e.g. "Mass: "),
                    along with the current value for  shape
                **/
                string mass = generalShapeValues[MASS][0] + convert_str_d(simulationShape.getMass());
                string xVel = generalShapeValues[XVELOCITY][0] + convert_str_d(simulationShape.getXVelocity());
                string yVel = generalShapeValues[YVELOCITY][0] + convert_str_d(simulationShape.getYVelocity());
                string xAccl = generalShapeValues[XACCELERATION][0] + convert_str_d(simulationShape.getXAcceleration());
                string yAccl = generalShapeValues[YACCELERATION][0] + convert_str_d(simulationShape.getYAcceleration());

                //If shape is affected by  gravity, "True" will be displayed, otherwise "False"
                string affectedByGravity = generalShapeValues[GRAVITY_EFFECT][0] +
                                           (simulationShape.getGravityEffect() == true ? "True" : "False");

                string rValue = generalShapeValues[RVALUE][0] + convert_str(simulationShape.getRValue());
                string gValue = generalShapeValues[GVALUE][0] + convert_str(simulationShape.getGValue());
                string bValue = generalShapeValues[BVALUE][0] + convert_str(simulationShape.getBValue());
                string aValue = generalShapeValues[AVALUE][0] + convert_str(simulationShape.getAValue());

                string values[] = {mass, xVel, yVel, xAccl, yAccl, affectedByGravity, rValue, gValue, bValue, aValue};


                for (i = 0; i < TOTAL_GENERAL_VALUES; i++) {
                    generalShapeValuesTexts[i].loadText(calibri_18, values[i].c_str(), white, renderer);

                    /**
                        Here, we store the value in the second element of whatever the actual
                        piece of data is. For example, "Mass" is already stored in the first element,
                        so we do not need to store it again. So the substr function is called, which slices
                        the string one past the position of the captioned text, so the actual record of data
                        is left for whatever attribute. E.g. if we have "Mass: 5.0", the result will be "5.0"
                    **/
                    generalShapeValues[i][1] = values[i].substr(generalShapeValues[i][0].length());

                }

                //The same that was done with gravity is done with mass
                generalShapeValuesRects[MASS].x = dataBox.x;
                generalShapeValuesRects[MASS].y = dataBox.y;
                generalShapeValuesRects[MASS].w = generalShapeValuesTexts[MASS].width_size();
                generalShapeValuesRects[MASS].h = generalShapeValuesTexts[MASS].height_size();

                generalShapeValuesTexts[MASS].render(renderer, &generalShapeValuesRects[MASS], nullptr);

                for (i = 1; i < TOTAL_GENERAL_VALUES; i++) {
                    int prevy = generalShapeValuesRects[i - 1].y;
                    int prevh = generalShapeValuesRects[i - 1].h;

                    generalShapeValuesRects[i].x = dataBox.x;
                    generalShapeValuesRects[i].y = prevy + prevh + spacingShapes;
                    generalShapeValuesRects[i].w = generalShapeValuesTexts[i].width_size();
                    generalShapeValuesRects[i].h = generalShapeValuesTexts[i].height_size();

                    generalShapeValuesTexts[i].render(renderer, &generalShapeValuesRects[i], nullptr);
                }


                /**
                    One of rectangle, circle or triangle data will be displayed.
                    The above was all the data that was common to all, now here
                    we actually look at the unique data of each shape
                **/
                if (dataToDisplay == RECTANGLE_DATA) {

                    string topLeftx = rectValues[TOP_LEFT_X][0] +
                                      convert_str(convertIntDouble(simulationShape.shape.rect->xPosition()));

                    string topLefty = rectValues[TOP_LEFT_Y][0] +
                                      convert_str(convertIntDouble(simulationShape.shape.rect->yPosition()));

                    string width = rectValues[WIDTH][0] + convert_str(simulationShape.shape.rect->getWidth());
                    string height = rectValues[HEIGHT][0] + convert_str(simulationShape.shape.rect->getHeight());

                    string rectStringValues[] = {topLeftx, topLefty, width, height};

                    for (i = 0; i < RECT_VALUES; i++) {
                        rectValuesTexts[i].loadText(calibri_18, rectStringValues[i].c_str(), white, renderer);
                        rectValues[i][1] = rectStringValues[i].substr(rectValues[i][0].length());
                    }

                    rectValuesRects[TOP_LEFT_X].x = dataBox.x;
                    rectValuesRects[TOP_LEFT_X].y = generalShapeValuesRects[TOTAL_GENERAL_VALUES - 1].y +
                                                    generalShapeValuesRects[TOTAL_GENERAL_VALUES - 1].h + spacingShapes;

                    rectValuesRects[TOP_LEFT_X].w = rectValuesTexts[TOP_LEFT_X].width_size();
                    rectValuesRects[TOP_LEFT_X].h = rectValuesTexts[TOP_LEFT_X].height_size();

                    rectValuesTexts[TOP_LEFT_X].render(renderer, &rectValuesRects[TOP_LEFT_X], nullptr);

                    for (i = 1; i < RECT_VALUES; i++) {
                        int prevy = rectValuesRects[i - 1].y;
                        int prevh = rectValuesRects[i - 1].h;

                        rectValuesRects[i].x = dataBox.x;
                        rectValuesRects[i].y = prevy + prevh + spacingShapes;
                        rectValuesRects[i].w = rectValuesTexts[i].width_size();
                        rectValuesRects[i].h = rectValuesTexts[i].height_size();

                        rectValuesTexts[i].render(renderer, &rectValuesRects[i], nullptr);

                    }
                } else if (dataToDisplay == CIRCLE_DATA) {

                    string centreX = circleValues[CENTRE_X][0] +
                                     convert_str(convertIntDouble(simulationShape.shape.circle->xPosition()));

                    string centreY = circleValues[CENTRE_Y][0] +
                                     convert_str(convertIntDouble(simulationShape.shape.circle->yPosition()));

                    string radius = circleValues[RADIUS][0] + convert_str(simulationShape.shape.circle->getRadius());

                    string circleStrings[] = {centreX, centreY, radius};

                    for (i = 0; i < CIRCLE_VALUES; i++) {
                        circleValuesTexts[i].loadText(calibri_18, circleStrings[i].c_str(), white, renderer);
                        circleValues[i][1] = circleStrings[i].substr(circleValues[i][0].length());
                    }

                    circleValuesRects[0].x = dataBox.x;
                    circleValuesRects[0].y = generalShapeValuesRects[TOTAL_GENERAL_VALUES - 1].y +
                                             generalShapeValuesRects[TOTAL_GENERAL_VALUES - 1].h + spacingShapes;

                    circleValuesRects[0].w = circleValuesTexts[0].width_size();
                    circleValuesRects[0].h = circleValuesTexts[0].height_size();

                    circleValuesTexts[0].render(renderer, &circleValuesRects[0], nullptr);

                    for (i = 1; i < CIRCLE_VALUES; i++) {
                        int prevy = circleValuesRects[i - 1].y;
                        int prevh = circleValuesRects[i - 1].h;

                        circleValuesRects[i].x = dataBox.x;
                        circleValuesRects[i].y = prevy + prevh + spacingShapes;
                        circleValuesRects[i].w = circleValuesTexts[i].width_size();
                        circleValuesRects[i].h = circleValuesTexts[i].height_size();

                        circleValuesTexts[i].render(renderer, &circleValuesRects[i], nullptr);
                    }
                } else if (dataToDisplay == TRIANGLE_DATA) {

                    string p1x = triValues[P1X][0] +
                                 convert_str(convertIntDouble(simulationShape.shape.tri->xcoordPoint1()));

                    string p1y = triValues[P1Y][0] +
                                 convert_str(convertIntDouble(simulationShape.shape.tri->ycoordPoint1()));

                    string p2x = triValues[P2X][0] +
                                 convert_str(convertIntDouble(simulationShape.shape.tri->xcoordPoint2()));

                    string p2y = triValues[P2Y][0] +
                                 convert_str(convertIntDouble(simulationShape.shape.tri->ycoordPoint2()));


                    string p3x = triValues[P3X][0] +
                                 convert_str(convertIntDouble(simulationShape.shape.tri->xcoordPoint3()));

                    string p3y = triValues[P3Y][0] +
                                 convert_str(convertIntDouble(simulationShape.shape.tri->ycoordPoint3()));


                    string triStrings[] = {p1x, p1y, p2x, p2y, p3x, p3y};

                    for (i = 0; i < TRI_VALUES; i++) {
                        triValuesTexts[i].loadText(calibri_18, triStrings[i].c_str(), white, renderer);
                        triValues[i][1] = triStrings[i].substr(triValues[i][0].length());
                    }

                    triValuesRects[P1X].x = dataBox.x;
                    triValuesRects[P1X].y = generalShapeValuesRects[TOTAL_GENERAL_VALUES - 1].y +
                                            generalShapeValuesRects[TOTAL_GENERAL_VALUES - 1].h + spacingShapes;;
                    triValuesRects[P1X].w = triValuesTexts[P1X].width_size();
                    triValuesRects[P1X].h = triValuesTexts[P1X].height_size();

                    triValuesTexts[P1X].render(renderer, &triValuesRects[P1X], nullptr);

                    for (i = 1; i < TRI_VALUES; i++) {
                        int prevy = triValuesRects[i - 1].y;
                        int prevh = triValuesRects[i - 1].h;

                        triValuesRects[i].x = dataBox.x;
                        triValuesRects[i].y = prevy + prevh + spacingShapes;
                        triValuesRects[i].w = triValuesTexts[i].width_size();
                        triValuesRects[i].h = triValuesTexts[i].height_size();
                        triValuesTexts[i].render(renderer, &triValuesRects[i], nullptr);
                    }
                }
            }

            //Change text of anything undergoing change by user
            if (textInput.isTextChanging && !simInfo.simulationRunning) {
                //SDL_RenderSetViewport(renderer,textInput.displayRect);
                SDL_SetRenderDrawColor(renderer, 96, 96, 96, 1);
                SDL_RenderFillRect(renderer, textInput.displayRect);
                //SDL_RenderSetViewport(renderer,&windowRect);
                textInput.textPtr->loadText(calibri_18,
                                            (textInput.preTextInput + textInput.textInput).c_str(),
                                            textInput.colour, renderer);

                textInput.displayRect->w = textInput.textPtr->width_size();
                textInput.displayRect->h = textInput.textPtr->height_size();
                textInput.textPtr->render(renderer, textInput.displayRect, nullptr);
            }

            dataBoxRendered = true;
            renderWasChanged = true;
        }

        if (!displayShapesRendered) {

            //Shape box is dark blue
            SDL_SetRenderDrawColor(renderer, 0, 0, 51, 255);
            SDL_RenderFillRect(renderer, &shapesBox);

            SDL_RenderSetViewport(renderer, &shapesBox);

            renderRectangle(renderer, &displayRectangle, nullptr,
                            shapesBox.w, shapesBox.h, 0, 0);

            renderTriangle(renderer, &displayTriangle, nullptr,
                           &displayTriangleCoordinates,
                           shapesBox.w, shapesBox.h, 0, 0);

            renderCircle(renderer, &displayCircle, &displayCircleCoordinates,
                         shapesBox.w, shapesBox.h, 0, 0);

            SDL_RenderSetViewport(renderer, &windowRect);

            displayShapesRendered = true;
            renderWasChanged = true;
        }

        if (simInfo.simulationRunning) {
            QueryPerformanceCounter(&currentTicks);
            LARGE_INTEGER elapsedTime;

            elapsedTime.QuadPart = (currentTicks.QuadPart - startTicks.QuadPart) * 1000000;
            elapsedTime.QuadPart /= frequency.QuadPart;

            if (elapsedTime.QuadPart >= simInfo.frequency) {
                //printf("%lld, %d elap\n", elapsedTime.QuadPart, simInfo.frequency);
                simInfo.countNum += 1;
                startTicks.QuadPart = currentTicks.QuadPart;

                //Object will not move if user has pressed paused

                if (simInfo.simulationRunning == RUNNING) {
                    pEngine.moveObjects();

                    simulationMapRendered = false;
                    dataBoxRendered = false;

                }
            }

        }

        //Coordinates of mouse pointer
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);

        for (i = 0; i < numMenuBarButtons && !mouseInMenuButton && !simInfo.simulationRunning; i++) {
            //Check if user has hovered mouse over one of the buttons on the menu bar
            //If so, button is highlighted a shade of blue, with RGB value 204-229-255
            if (mouse_inside(mousex, mousey, *menuBarButtons[i])) {
                mouseInMenuButton = true;

                SDL_SetRenderDrawColor(renderer, 204, 229, 255, 255);
                SDL_RenderFillRect(renderer, menuBarButtons[i]);
                menuBarTexts[i]->render(renderer, menuBarButtons[i], nullptr);
                rectMouseInside = menuBarButtons[i];
                textMouseInside = menuBarTexts[i];

                //User may be currently holding mouse down, in which case they've clicked elsewhere
                //We should not handle a mouse up click in button as a result
                //Also, we don't want to handle a mouse click if the simulation is running
                //They'd have to press the stop button to resume normal functionality
                if (mouseClickedState == NOT_CLICKED && simulationMapRendered)
                    shouldHandleMouseClick = true;

                renderWasChanged = true;
                textInput.isTextChanging = false;
            }
        }

        while (SDL_PollEvent(&event) != 0) {
            //In case coordinates of mouse change, get latest one
            SDL_GetMouseState(&mousex, &mousey);

            //User has pressed the X button, program will end
            if (event.type == SDL_QUIT) {
                endProgram = true;

                simulationShape.unhilightShape();

                if (fileLoaded) {
                    try {

                        if (!simInfo.frequency)
                            saveData(database, &rectangles, &circles, &triangles);

                        else
                            saveData(database, &rectanglesCopy, &circlesCopy, &trianglesCopy);
                        /**This is an end of file operation, so if the program cannot save file,
                           it is no longer its responsibility at this stage, hence the generic catch**/
                    } catch (...) {

                        break;
                    }
                } else {

                    int saveAs = MessageBox(NULL, "Do you want to save file?", "Save?",
                                            MB_YESNOCANCEL | MB_ICONEXCLAMATION);

                    if (saveAs == IDYES) {
                        if (!simInfo.simulationRunning)
                            saveAsFileExplorer(&database, &triangles, &rectangles, &circles, &fileLoaded);
                        else
                            saveAsFileExplorer(&database, &trianglesCopy, &rectanglesCopy, &circlesCopy, &fileLoaded);

                        break;
                    } else if (saveAs == IDNO)
                        break;

                    else if (saveAs == IDCANCEL) {
                        endProgram = false;
                    }


                }

            } else if (event.type == SDL_MOUSEBUTTONUP) {
                mouseClickedState = NOT_CLICKED;
                simulationShape.mouseState = NOT_CLICKED;

                //Check if user has fully clicked on a menu button
                if (mouseInMenuButton && shouldHandleMouseClick) {
                    if (rectMouseInside == &openDisplayBox) {
                        if (openFileExplorer(&database, &triangles, &rectangles, &circles, &fileLoaded)) {
                            setupCircleCoordinates(&circles, &circlesCoordinates);
                            setupTriangleCoordinates(&triangles, &trianglesCoordinates);
                            simulationMapRendered = false;
                            textInput.isTextChanging = false;

                            if (simulationShape.isSetup())
                                simulationShape.removeShape();
                        }
                    }

                    //Save a file if one has been loaded if save button is clicked or run simulation button is clicked...
                    //...Or the new button is clicked
                    if ((rectMouseInside == &saveDisplayBox && fileLoaded) ||
                        (rectMouseInside == &runDisplayBox && fileLoaded ||
                         rectMouseInside == &newDisplayBox && fileLoaded)) {

                        textInput.isTextChanging = false;
                        bool failed = false;
                        try {
                            bool shapeHighlighted = false;
                            //To stop a highlighted shape being saved that way
                            if (simulationShape.isShapeHighlighted()) {
                                simulationShape.unhilightShape();
                                shapeHighlighted = true;

                            }
                            saveData(database, &rectangles, &circles, &triangles);

                            if (shapeHighlighted)
                                simulationShape.highlightShape();
                        }

                        catch (updateFail &e) { failed = true; }

                        catch (deletionFail &e) { failed = true; }

                        if (failed) {
                            MessageBox(NULL, "Could not save file. Try Again.",
                                       NULL, MB_OK);
                        }
                    }

                    //Save as if file not yet loaded, or user presses save as button
                    if (rectMouseInside == &saveAsDisplayBox ||
                        (rectMouseInside == &saveDisplayBox && !fileLoaded)) {
                        textInput.isTextChanging = false;
                        //To stop a highlighted shape being saved that way
                        bool shapeHighlighted = false;

                        if (simulationShape.isShapeHighlighted()) {
                            simulationShape.unhilightShape();
                            shapeHighlighted = true;
                        }

                        saveAsFileExplorer(&database, &triangles, &rectangles, &circles, &fileLoaded);

                        if (shapeHighlighted)
                            simulationShape.highlightShape();
                    }

                    if (rectMouseInside == &runDisplayBox) {

                        textInput.isTextChanging = false;
                        shouldHandleMouseClick = false;
                        simInfo.simulationRunning = RUNNING;
                        //Physics engine will run on microsecond level scale
                        simInfo.subseconds = 1000000;
                        simInfo.frameRate = physicsEngine::DEFAULT_FRAME_RATE;
                        simInfo.frequency = simInfo.subseconds / simInfo.frameRate;
                        simInfo.countNum = 0;

                        copyRect(rectanglesCopy, rectangles);
                        copyTri(trianglesCopy, triangles);
                        copyCircle(circlesCopy, circles);


                        for (i = 0; i < rectangles.size(); i++) {
                            rectangles[i].resetMotionHandler();
                            pEngine.addRectangle(&rectangles[i]);

                        }

                        for (i = 0; i < circles.size(); i++) {
                            circles[i].resetMotionHandler();
                            pEngine.addCircle(&circles[i]);

                        }

                        for (i = 0; i < triangles.size(); i++) {
                            triangles[i].resetMotionHandler();
                            pEngine.addTriangle(&triangles[i]);

                        }

                        pEngine.moveObjects();
                        QueryPerformanceCounter(&startTicks);

                        simulationMapRendered = false;
                        dataBoxRendered = false;
                        menuBarRendered = false;

                    }

                    if (rectMouseInside == &constantsDisplayBox) {
                        dataToDisplay = SIMULATION_CONSTANTS;
                        simulationShape.removeShape();

                        textInput.isTextChanging = false;
                        dataBoxRendered = false;
                        simulationMapRendered = false;
                    }

                    if (rectMouseInside == &newDisplayBox) {
                        if (!fileLoaded) {

                            int saveAs = MessageBox(NULL, "Do you want to save file?", "Save?",
                                                    MB_YESNO | MB_ICONEXCLAMATION);

                            if (saveAs == IDYES) {
                                saveAsFileExplorer(&database, &triangles, &rectangles, &circles, &fileLoaded);

                            }

                        }

                        circles.clear();
                        circlesCoordinates.clear();

                        triangles.clear();
                        trianglesCoordinates.clear();

                        rectangles.clear();
                        rectanglesCopy.clear();
                        circlesCopy.clear();
                        trianglesCopy.clear();
                        pEngine.clearShapes();

                        simulationShape.removeShape();
                        textInput.isTextChanging = false;
                        dataToDisplay = SIMULATION_CONSTANTS;
                        mapPosition.panningMap = false;
                        mapPosition.dx = 0;
                        mapPosition.dy = 0;


                        physicsEngine::gravitationalAcceleration = 0.0;



                        //All coefficients of restitution are 1 by default
                        for (i = 1; i < FLOAT_CONSTANTS; i++)
                            *constantsArray[i] = 1.0;

                        simulationMapRendered = false;
                        dataBoxRendered = false;

                        if (fileLoaded) {
                            fileLoaded = false;
                            sqlite3_close(database);
                        }

                    }

                    if (mapPosition.panningMap)
                        mapPosition.panningMap = false;

                    shouldHandleMouseClick = false;
                }


                //If user clicked a display shape and has now released mouse, mouse state is not clicked
                //This ensures the shape they clicked won't be dragged onto the screen
                if (displayShapeClicked.mouseState == CLICKED) {
                    displayShapeClicked.mouseState = NOT_CLICKED;
                    displayShapeClicked.unhilightShape();
                    dataToDisplay = SIMULATION_CONSTANTS;
                    displayShapesRendered = false;
                    textInput.isTextChanging = false;
                }
            }

                //The cursor has been moved
            else if (event.type == SDL_MOUSEMOTION) {

                //Check if user WAS hovering over a menu button, and now isn't
                if (mouseInMenuButton) {
                    if (!mouse_inside(mousex, mousey, *rectMouseInside)) {
                        //Reset the background colour of the menu button back to grey
                        SDL_SetRenderDrawColor(renderer, 224, 224, 224, 255);
                        SDL_RenderFillRect(renderer, rectMouseInside);

                        //Render the text again, as resetting background colour would have filled previous text
                        //But don't render if simulation is running
                        if (!simInfo.simulationRunning)
                            textMouseInside->render(renderer, rectMouseInside, nullptr);

                        mouseInMenuButton = false;
                        shouldHandleMouseClick = false;
                        rectMouseInside = nullptr;
                        textMouseInside = nullptr;

                        renderWasChanged = true;
                    }
                }


                if (mapPosition.panningMap) {
                    //User cannot pan map if they are clicking on a shape on the map
                    //This is why simulationShape.mouseState must be NOT_CLICKED
                    if (mouseClickedState == CLICKED && simulationShape.mouseState == NOT_CLICKED) {
                        //Limits of simulation map
                        int upperBoundWidth = physicsEngine::simulationWidth * 0.5 - simulationMap.w * 0.5;
                        int lowerBoundWidth = simulationMap.w * 0.5 - physicsEngine::simulationWidth * 0.5;
                        int upperBoundHeight = physicsEngine::simulationHeight * 0.5 - simulationMap.h * 0.5;
                        int lowerBoundHeight = simulationMap.h * 0.5 - physicsEngine::simulationHeight * 0.5;

                        if (mouse_inside(mousex, mousey, simulationMap)) {
                            if (mousex < mapPosition.prevMousex && mapPosition.dx < upperBoundWidth) {
                                mapPosition.dx += panningdx;

                                if (mapPosition.dx > upperBoundWidth)
                                    mapPosition.dx = upperBoundWidth;
                            } else if (mousex > mapPosition.prevMousex && mapPosition.dx > lowerBoundWidth) {

                                mapPosition.dx -= panningdx;

                                if (mapPosition.dx < lowerBoundWidth)
                                    mapPosition.dx = lowerBoundWidth;
                            }

                            if (mousey < mapPosition.prevMousey && mapPosition.dy < upperBoundHeight) {

                                mapPosition.dy += panningdy;

                                if (mapPosition.dy > upperBoundHeight)
                                    mapPosition.dy = upperBoundHeight;
                            } else if (mousey > mapPosition.prevMousey && mapPosition.dy > lowerBoundHeight) {
                                mapPosition.dy -= panningdy;

                                if (mapPosition.dy < lowerBoundHeight)
                                    mapPosition.dy = lowerBoundHeight;
                            }

                            mapPosition.prevMousex = mousex;
                            mapPosition.prevMousey = mousey;

                            simulationMapRendered = false;
                            textInput.isTextChanging = false;
                        } else
                            mapPosition.panningMap = false;

                    }
                }

                //User has clicked on a display shape and is still pressing it
                if (displayShapeClicked.mouseState == CLICKED && !simInfo.simulationRunning) {
                    //User has dragged shape into simulation map
                    if (mouse_inside(mousex, mousey, simulationMap)) {
                        //Unhilight display shape
                        //displayShapeClicked.unhilightShape();

                        //The Cartesian coordinates of the mouse on the simulation map
                        int converted_mousex = inverseConvertX(mousex - simulationMap.x, simulationMap.w,
                                                               mapPosition.dx);
                        int converted_mousey = inverseConvertY(mousey - simulationMap.y, simulationMap.h,
                                                               mapPosition.dy);

                        int differenceMousex;
                        int differenceMousey;

                        displayShapeClicked.getDifferenceMousex(&differenceMousex);
                        displayShapeClicked.getDifferenceMousey(&differenceMousey);

                        bool simulationMapChanged = false;

                        if (displayShapeClicked.shapeNum == shapeClicked::RECTANGLE) {

                            int rectx = convertIntDouble(displayRectangle.xPosition());
                            int recty = convertIntDouble(displayRectangle.yPosition());

                            int w = convertIntDouble(displayRectangle.getWidth());
                            int h = convertIntDouble(displayRectangle.getHeight());

                            int newx = converted_mousex + differenceMousex;
                            int newy = converted_mousey + differenceMousey;

                            if (rectValidOnMap(newx, newy, w, h)) {

                                physicsEngine::Rectangle rect(converted_mousex + differenceMousex,
                                                              converted_mousey + differenceMousey,
                                                              physicsEngine::DEFAULT_FRAME_RATE, w, h);

                                rectangles.push_back(rect);

                                simulationShape.addRect(&rectangles[rectangles.size() - 1],
                                                        converted_mousex, converted_mousey);

                                simulationMapChanged = true;
                                dataToDisplay = RECTANGLE_DATA;
                            }
                        } else if (displayShapeClicked.shapeNum == shapeClicked::CIRCLE) {

                            int r = displayShapeClicked.shape.circle->getRadius();
                            int newx = converted_mousex + differenceMousex;
                            int newy = converted_mousey + differenceMousey;

                            if (circleValidOnMap(newx, newy, r)) {


                                physicsEngine::Circle circle(newx, newy, r,
                                                             physicsEngine::DEFAULT_FRAME_RATE);

                                renderingCoordinates circleCoordinates;
                                setupCircleRendering(&circle, &circleCoordinates);
                                circles.push_back(circle);

                                circlesCoordinates.push_back(circleCoordinates);

                                displayShapeClicked.mouseState = NOT_CLICKED;

                                simulationShape.addCircle(&circles[circles.size() - 1],
                                                          &circlesCoordinates[circlesCoordinates.size() - 1],
                                                          converted_mousex, converted_mousey);
                                simulationMapChanged = true;
                                dataToDisplay = CIRCLE_DATA;
                            }
                        } else if (displayShapeClicked.shapeNum == shapeClicked::TRIANGLE) {

                            int x1 = convertIntDouble(displayShapeClicked.shape.tri->xcoordPoint1());
                            int y1 = convertIntDouble(displayShapeClicked.shape.tri->ycoordPoint1());
                            int x2 = convertIntDouble(displayShapeClicked.shape.tri->xcoordPoint2());
                            int y2 = convertIntDouble(displayShapeClicked.shape.tri->ycoordPoint2());
                            int x3 = convertIntDouble(displayShapeClicked.shape.tri->xcoordPoint3());
                            int y3 = convertIntDouble(displayShapeClicked.shape.tri->ycoordPoint3());

                            int new_x1 = converted_mousex + differenceMousex;
                            int new_y1 = converted_mousey + differenceMousey;

                            int new_x2 = new_x1 + (x2 - x1);
                            int new_y2 = new_y1 + (y2 - y1);

                            int new_x3 = new_x1 + (x3 - x1);
                            int new_y3 = new_y1 + (y3 - y1);

                            if (triValidOnMap(new_x1, new_y1, new_x2, new_y2, new_x3, new_y3)) {

                                physicsEngine::Triangle tri(new_x1, new_y1,
                                                            new_x2, new_y2,
                                                            new_x3, new_y3,
                                                            physicsEngine::DEFAULT_FRAME_RATE);

                                renderingCoordinates triCoordinates;

                                setupTriangleRendering(&tri, &triCoordinates);

                                triangles.push_back(tri);
                                trianglesCoordinates.push_back(triCoordinates);


                                simulationShape.addTri(&triangles[triangles.size() - 1],
                                                       &trianglesCoordinates[trianglesCoordinates.size() - 1],
                                                       converted_mousex, converted_mousey);

                                simulationMapChanged = true;
                                dataToDisplay = TRIANGLE_DATA;
                            }
                        }


                        if (simulationMapChanged) {
                            simulationShape.mouseState = CLICKED;
                            displayShapeClicked.mouseState = NOT_CLICKED;
                            simulationShape.highlightShape();
                            displayShapeClicked.unhilightShape();
                            simulationMapRendered = false;
                            displayShapesRendered = false;
                            dataBoxRendered = false;
                            textInput.isTextChanging = false;

                        }
                    }
                }

                //If a shape on the map is being pressed
                if (simulationShape.mouseState == CLICKED && !simInfo.simulationRunning) {
                    if (mouse_inside(mousex, mousey, simulationMap)) {

                        //The Cartesian coordinates of the mouse on the simulation map
                        int converted_mousex = inverseConvertX(mousex - simulationMap.x, simulationMap.w,
                                                               mapPosition.dx);
                        int converted_mousey = inverseConvertY(mousey - simulationMap.y, simulationMap.h,
                                                               mapPosition.dy);

                        int differenceMousex;
                        int differenceMousey;

                        simulationShape.getDifferenceMousex(&differenceMousex);
                        simulationShape.getDifferenceMousey(&differenceMousey);

                        //Used so simulation map and data box can be modified if shape changes
                        bool shapeChanged = false;

                        //Rectangle being clicked
                        if (simulationShape.shapeNum == shapeClicked::RECTANGLE) {

                            physicsEngine::Rectangle *simRect = simulationShape.shape.rect;

                            int rectx = convertIntDouble(simRect->xPosition());
                            int recty = convertIntDouble(simRect->yPosition());

                            int w = convertIntDouble(simRect->getWidth());
                            int h = convertIntDouble(simRect->getHeight());


                            int newx, newy;
                            int newWidth, newHeight;

                            int resizeNum = simulationShape.resizeShape.rect.resizeNum;


                            if (resizeNum == shapeClicked::NO_RECT_RESIZE) {
                                bool newPointValid = true;

                                newx = converted_mousex + differenceMousex;
                                newy = converted_mousey + differenceMousey;

                                if (rectValidOnMap(newx, newy, w, h)) {
                                    simRect->changePosition(newx, newy);
                                    simulationShape.updateResizeCircles();
                                    shapeChanged = true;

                                }
                            } else {

                                newx = converted_mousex + differenceMousex;
                                newy = converted_mousey + differenceMousey;

                                if (resizeNum == shapeClicked::TOP_LEFT) {
                                    newWidth = w + (rectx - newx);
                                    newHeight = h - (recty - newy);

                                    if (rectValidOnMap(newx, newy, newWidth, newHeight)) {

                                        simRect->changePosition(newx, newy);
                                        simRect->changeWidth(newWidth);
                                        simRect->changeHeight(newHeight);

                                        simulationShape.removeShape();
                                        simulationShape.addRect(simRect, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.rect.resizeNum = shapeClicked::TOP_LEFT;
                                        shapeChanged = true;
                                    }

                                } else if (resizeNum == shapeClicked::TOP_MIDDLE) {

                                    newHeight = h - (recty - newy);

                                    if (rectValidOnMap(rectx, newy, w, newHeight)) {
                                        simRect->changePosition(rectx, newy);
                                        simRect->changeHeight(newHeight);

                                        simulationShape.removeShape();
                                        simulationShape.addRect(simRect, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.rect.resizeNum = shapeClicked::TOP_MIDDLE;
                                        shapeChanged = true;

                                    }
                                } else if (resizeNum == shapeClicked::TOP_RIGHT) {
                                    //newx is coordinate for top left corner as...
                                    //...differenceMousex is given by topleftx - cartesian mouse x
                                    //To get difference with top right corner, we add by the width...
                                    //...to give differenceMousex as top left x + w - cartesian mouse x
                                    newx += w;

                                    //New width is x position of top right corner - top left corner
                                    newWidth = newx - rectx;
                                    newHeight = h - (recty - newy);

                                    if (rectValidOnMap(rectx, newy, newWidth, newHeight)) {
                                        simRect->changePosition(rectx, newy);
                                        simRect->changeHeight(newHeight);
                                        simRect->changeWidth(newWidth);

                                        simulationShape.removeShape();
                                        simulationShape.addRect(simRect, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.rect.resizeNum = shapeClicked::TOP_RIGHT;
                                        shapeChanged = true;

                                    }
                                } else if (resizeNum == shapeClicked::RIGHT_MIDDLE) {

                                    newx += w;

                                    newWidth = newx - rectx;

                                    if (rectValidOnMap(rectx, recty, newWidth, h)) {
                                        simRect->changeWidth(newWidth);

                                        simulationShape.removeShape();
                                        simulationShape.addRect(simRect, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.rect.resizeNum = shapeClicked::RIGHT_MIDDLE;
                                        shapeChanged = true;
                                    }
                                } else if (resizeNum == shapeClicked::BOTTOM_RIGHT) {
                                    newx += w;
                                    newy -= h;

                                    newWidth = newx - rectx;
                                    newHeight = recty - newy;

                                    if (rectValidOnMap(rectx, recty, newWidth, newHeight)) {

                                        simRect->changeWidth(newWidth);
                                        simRect->changeHeight(newHeight);

                                        simulationShape.removeShape();
                                        simulationShape.addRect(simRect, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.rect.resizeNum = shapeClicked::BOTTOM_RIGHT;
                                        shapeChanged = true;
                                    }
                                } else if (resizeNum == shapeClicked::BOTTOM_LEFT) {

                                    newy -= h;
                                    newWidth = w + (rectx - newx);
                                    newHeight = recty - newy;

                                    if (rectValidOnMap(newx, recty, newWidth, newHeight)) {

                                        simRect->changePosition(newx, recty);
                                        simRect->changeWidth(newWidth);
                                        simRect->changeHeight(newHeight);

                                        simulationShape.removeShape();
                                        simulationShape.addRect(simRect, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.rect.resizeNum = shapeClicked::BOTTOM_LEFT;
                                        shapeChanged = true;
                                    }
                                } else if (resizeNum == shapeClicked::BOTTOM_MIDDLE) {
                                    newy -= h;

                                    newHeight = recty - newy;

                                    if (rectValidOnMap(rectx, recty, w, newHeight)) {
                                        simRect->changeHeight(newHeight);

                                        simulationShape.removeShape();
                                        simulationShape.addRect(simRect, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.rect.resizeNum = shapeClicked::BOTTOM_MIDDLE;
                                        shapeChanged = true;
                                    }
                                } else if (resizeNum == shapeClicked::LEFT_MIDDLE) {

                                    int newWidth = w + (rectx - newx);


                                    if (rectValidOnMap(newx, recty, newWidth, h)) {

                                        simRect->changePosition(newx, recty);
                                        simRect->changeWidth(newWidth);

                                        simulationShape.removeShape();
                                        simulationShape.addRect(simRect, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.rect.resizeNum = shapeClicked::LEFT_MIDDLE;
                                        shapeChanged = true;

                                    }
                                }
                            }
                        } else if (simulationShape.shapeNum == shapeClicked::CIRCLE) {

                            int newx = converted_mousex + differenceMousex;
                            int newy = converted_mousey + differenceMousey;
                            int r = simulationShape.shape.circle->getRadius();

                            if (circleValidOnMap(newx, newy, r)) {
                                simulationShape.shape.circle->changePosition(newx, newy);
                                simulationShape.updateResizeCircles();
                                shapeChanged = true;
                            }
                        } else if (simulationShape.shapeNum == shapeClicked::TRIANGLE) {

                            int resizeNum = simulationShape.resizeShape.tri.resizeNum;

                            physicsEngine::Triangle *simTri = simulationShape.shape.tri;
                            renderingCoordinates *triCoords = simulationShape.shapeRenderCoords.tri;

                            int x1 = convertIntDouble(simTri->xcoordPoint1());
                            int y1 = convertIntDouble(simTri->ycoordPoint1());
                            int x2 = convertIntDouble(simTri->xcoordPoint2());
                            int y2 = convertIntDouble(simTri->ycoordPoint2());
                            int x3 = convertIntDouble(simTri->xcoordPoint3());
                            int y3 = convertIntDouble(simTri->ycoordPoint3());

                            int new_x1 = converted_mousex + differenceMousex;
                            int new_y1 = converted_mousey + differenceMousey;

                            int new_x2 = new_x1 + (x2 - x1);
                            int new_y2 = new_y1 + (y2 - y1);

                            int new_x3 = new_x1 + (x3 - x1);
                            int new_y3 = new_y1 + (y3 - y1);

                            if (resizeNum == shapeClicked::NO_TRI_RESIZE) {

                                if (triValidOnMap(new_x1, new_y1, new_x2, new_y2, new_x3, new_y3)) {
                                    simTri->changePoint1(new_x1, new_y1);
                                    simTri->changePoint2(new_x2, new_y2);
                                    simTri->changePoint3(new_x3, new_y3);
                                    simulationShape.updateResizeCircles();
                                    shapeChanged = true;
                                }
                            } else {

                                if (resizeNum == shapeClicked::POINT1) {

                                    if (triValidOnMap(new_x1, new_y1, x2, y2, x3, y3)) {
                                        simTri->changePoint1(new_x1, new_y1);
                                        setupTriangleRendering(simTri, triCoords);

                                        simulationShape.removeShape();
                                        simulationShape.addTri(simTri, triCoords, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.tri.resizeNum = shapeClicked::POINT1;
                                        shapeChanged = true;

                                    }
                                } else if (resizeNum == shapeClicked::POINT2) {
                                    if (triValidOnMap(x1, y1, new_x2, new_y2, x3, y3)) {
                                        simTri->changePoint2(new_x2, new_y2);
                                        setupTriangleRendering(simTri, triCoords);

                                        simulationShape.removeShape();
                                        simulationShape.addTri(simTri, triCoords, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.tri.resizeNum = shapeClicked::POINT2;
                                        shapeChanged = true;

                                    }
                                } else if (resizeNum == shapeClicked::POINT3) {
                                    if (triValidOnMap(x1, y1, x2, y2, new_x3, new_y3)) {
                                        simTri->changePoint3(new_x3, new_y3);
                                        setupTriangleRendering(simTri, triCoords);

                                        simulationShape.removeShape();
                                        simulationShape.addTri(simTri, triCoords, converted_mousex, converted_mousey);
                                        simulationShape.highlightShape();
                                        simulationShape.resizeShape.tri.resizeNum = shapeClicked::POINT3;
                                        shapeChanged = true;

                                    }
                                }
                            }
                        }

                        if (shapeChanged) {
                            simulationMapRendered = false;
                            dataBoxRendered = false;
                            textInput.isTextChanging = false;
                        }
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                //User has clicked simulation map
                //Note, this will need to be changed, to ensure the map is only panned if they are also not...
                //...clicking a shape. If statement must check if a shape is being clicked
                if (mouse_inside(mousex, mousey, simulationMap) && mouseClickedState == NOT_CLICKED) {

                    //If the user clicks on simulation map, then mouse panning will be initiated
                    //However, this will only happen if they click on a portion of the map that doesn't have a shape on it
                    //Or on a shape they have already clicked
                    int converted_mousex = inverseConvertX(mousex - simulationMap.x, simulationMap.w, mapPosition.dx);
                    int converted_mousey = inverseConvertY(mousey - simulationMap.y, simulationMap.h, mapPosition.dy);

                    mapPosition.panningMap = true;
                    mapPosition.prevMousex = mousex;
                    mapPosition.prevMousey = mousey;

                    bool clickingOnShape = false;
                    bool shapeAlreadyMoving = false;
                    textInput.isTextChanging = false;

                    for (i = 0; i < triangles.size(); i++) {

                        if (pointInTriangle(&triangles[i], converted_mousex, converted_mousey)) {

                            if (simulationShape.isSetup()) {
                                if (triangles[i].Id() == simulationShape.instNum &&
                                    simulationShape.mouseState == CLICKED) {
                                    shapeAlreadyMoving = true;
                                    break;
                                }
                            }

                            simulationShape.addTri(&triangles[i], &trianglesCoordinates[i],
                                                   converted_mousex, converted_mousey);
                            simulationShape.highlightShape();
                            simulationShape.resizeShape.tri.resizeNum = shapeClicked::NO_TRI_RESIZE;
                            simulationShape.mouseState = CLICKED;
                            dataToDisplay = TRIANGLE_DATA;
                            clickingOnShape = true;
                            break;
                        }
                    }


                    if (!clickingOnShape && !shapeAlreadyMoving) {
                        for (i = 0; i < rectangles.size(); i++) {
                            if (pointInRect(&rectangles[i], converted_mousex, converted_mousey)) {

                                if (simulationShape.isSetup()) {
                                    if (rectangles[i].Id() == simulationShape.instNum &&
                                        simulationShape.mouseState == CLICKED) {

                                        shapeAlreadyMoving = true;
                                        break;
                                    }
                                }

                                simulationShape.addRect(&rectangles[i], converted_mousex, converted_mousey);
                                simulationShape.highlightShape();
                                simulationShape.resizeShape.rect.resizeNum = shapeClicked::NO_RECT_RESIZE;
                                simulationShape.mouseState = CLICKED;
                                dataToDisplay = RECTANGLE_DATA;
                                clickingOnShape = true;
                                break;
                            }
                        }
                    }

                    if (!clickingOnShape && !shapeAlreadyMoving) {
                        for (i = 0; i < circles.size(); i++) {
                            if (pointInCircle(&circles[i], converted_mousex, converted_mousey)) {

                                if (simulationShape.isSetup()) {
                                    if (circles[i].Id() == simulationShape.instNum &&
                                        simulationShape.mouseState == CLICKED) {
                                        shapeAlreadyMoving = true;
                                        break;
                                    }
                                }

                                simulationShape.addCircle(&circles[i], &circlesCoordinates[i],
                                                          converted_mousex, converted_mousey);

                                simulationShape.highlightShape();
                                simulationShape.mouseState = CLICKED;
                                dataToDisplay = CIRCLE_DATA;
                                clickingOnShape = true;
                                break;
                            }
                        }
                    }

                    //Here, we check if one of the resize circles is being pressed
                    //The circles may overlap the shape it represents
                    //This is fine, and clicking a resize circle will takes precedence over clicking a shape

                    if (simulationShape.shapeNum == shapeClicked::RECTANGLE && !simInfo.simulationRunning) {
                        for (i = 0; i < 8; i++) {
                            if (pointInCircle(*simulationShape.resizeShape.rect.allRectCircles[i],
                                              converted_mousex, converted_mousey)) {
                                /**Updates cartesian coords of mouse
                                   in simulationShape, because the mouse has clicked a
                                   circle on the rectangle**/
                                simulationShape.addRect(simulationShape.shape.rect,
                                                        converted_mousex, converted_mousey);

                                //The value of i will correspond to one of the enum values
                                simulationShape.resizeShape.rect.resizeNum = i;
                                simulationShape.mouseState = CLICKED;

                                clickingOnShape = true;
                                break;
                            }
                        }
                    } else if (simulationShape.shapeNum == shapeClicked::TRIANGLE && !simInfo.simulationRunning) {
                        for (i = 0; i < 3; i++) {
                            if (pointInCircle(*simulationShape.resizeShape.tri.allTriCircles[i],
                                              converted_mousex, converted_mousey)) {
                                simulationShape.addTri(simulationShape.shape.tri,
                                                       simulationShape.shapeRenderCoords.tri,
                                                       converted_mousex, converted_mousey);
                                simulationShape.resizeShape.tri.resizeNum = i;
                                simulationShape.mouseState = CLICKED;
                                clickingOnShape = true;
                                break;
                            }
                        }
                    }

                    if (clickingOnShape)
                        dataBoxRendered = false;

                    else {

                        if (simulationShape.isShapeHighlighted()) {
                            //Change this only if shape has not been clicked
                            simulationShape.mouseState = NOT_CLICKED;

                            //In case a shape had been clicked on, we know unhilight it
                            simulationShape.unhilightShape();
                        }

                        dataToDisplay = SIMULATION_CONSTANTS;
                        dataBoxRendered = false;
                    }

                    simulationMapRendered = false;
                }

                    //Checking if user has clicked one of the display shapes
                else if (mouse_inside(mousex, mousey, shapesBox) && mouseClickedState == NOT_CLICKED
                         && !simInfo.simulationRunning) {
                    //Convert mouse coordinates to relative Cartesian coordinates of shape box
                    //We need to get the relative coordinates of the mouse to the shape box
                    //Hence we subtract each of the coordinates
                    int converted_mousex = inverseConvertX(mousex - shapesBox.x, shapesBox.w, 0);
                    int converted_mousey = inverseConvertY(mousey - shapesBox.y, shapesBox.h, 0);

                    //Triangle is being clicked
                    if (pointInTriangle(&displayTriangle, converted_mousex, converted_mousey)) {
                        //If the user clicks a triangle, it will be highlighted
                        displayShapesRendered = false;


                        displayShapeClicked.mouseState = CLICKED;
                        displayShapeClicked.addTri(&displayTriangle, &displayTriangleCoordinates,
                                                   converted_mousex, converted_mousey);

                        //In case shape is being pressed on simulation map
                        displayShapeClicked.highlightShape();
                        simulationShape.unhilightShape();
                    }

                    if (pointInRect(&displayRectangle, converted_mousex, converted_mousey)) {
                        //Rectangle highlighted
                        displayShapesRendered = false;

                        displayShapeClicked.mouseState = CLICKED;
                        displayShapeClicked.addRect(&displayRectangle, converted_mousex, converted_mousey);
                        displayShapeClicked.highlightShape();
                        simulationShape.unhilightShape();

                    }

                    if (pointInCircle(&displayCircle, converted_mousex, converted_mousey)) {

                        displayShapesRendered = false;

                        displayShapeClicked.mouseState = CLICKED;
                        displayShapeClicked.addCircle(&displayCircle, &displayCircleCoordinates,
                                                      converted_mousex, converted_mousey);
                        displayShapeClicked.highlightShape();
                        simulationShape.unhilightShape();
                    }
                }

                    //User potentially wants to alter data
                else if (mouse_inside(mousex, mousey, dataBox) && !simInfo.simulationRunning) {

                    //User is clicking a shape attribute, as opposed to a simulation constant
                    if (dataToDisplay != SIMULATION_CONSTANTS) {

                        if (mouse_inside(mousex, mousey, generalShapeValuesRects[GRAVITY_EFFECT])) {

                            simulationShape.changeGravityEffect();
                            dataBoxRendered = false;
                            textInput.isTextChanging = false;
                        }

                        for (j = 0; j < 4; j++) {

                            SDL_Rect *shapeRect;
                            string (*text)[2];
                            Texture *textPtr;

                            int numValues;

                            //General constants
                            if (j == 0) {
                                shapeRect = generalShapeValuesRects;
                                textPtr = generalShapeValuesTexts;
                                text = generalShapeValues;
                                numValues = TOTAL_GENERAL_VALUES;

                            }

                                //Rectangles
                            else if (j == 1) {

                                if (dataToDisplay != RECTANGLE_DATA)
                                    continue;

                                shapeRect = rectValuesRects;
                                textPtr = rectValuesTexts;
                                text = rectValues;
                                numValues = RECT_VALUES;
                            }

                                //Circles
                            else if (j == 2) {
                                if (dataToDisplay != CIRCLE_DATA)
                                    continue;

                                shapeRect = circleValuesRects;
                                textPtr = circleValuesTexts;
                                text = circleValues;
                                numValues = CIRCLE_VALUES;
                            }

                                //Triangles
                            else if (j == 3) {
                                if (dataToDisplay != TRIANGLE_DATA)
                                    continue;

                                shapeRect = triValuesRects;
                                textPtr = triValuesTexts;
                                text = triValues;
                                numValues = TRI_VALUES;
                            }

                            for (i = 0; i < numValues; i++) {

                                //This is true or false, only clicking it will change its value
                                //No text input needed
                                if (i == GRAVITY_EFFECT && j == 0)
                                    continue;

                                if (mouse_inside(mousex, mousey, shapeRect[i])) {
                                    textInput.isTextChanging = true;

                                    //Some simulation constants are integers or decimals
                                    //All other UNIQUE shape attributes are integers
                                    if (j == 0) {
                                        switch (i) {

                                            //Only RGBA inputs for shape data are strictly integers
                                            case RVALUE:
                                            case GVALUE:
                                            case BVALUE:
                                            case AVALUE:
                                                textInput.inputType = textChanging::T_INTEGER;
                                                break;

                                                //Everything else can be a decimal
                                            default:
                                                textInput.inputType = textChanging::T_DECIMAL;
                                                break;
                                        }
                                    } else {
                                        textInput.inputType = textChanging::T_INTEGER;

                                    }

                                    //Get current data
                                    textInput.preTextInput = text[i][0];
                                    textInput.textInput = text[i][1];
                                    textInput.displayRect = &shapeRect[i];
                                    textInput.textPtr = &textPtr[i];

                                    if (dataToDisplay == RECTANGLE_DATA)
                                        textInput.shape.rect = simulationShape.shape.rect;

                                    else if (dataToDisplay == TRIANGLE_DATA)
                                        textInput.shape.tri = simulationShape.shape.tri;

                                    else if (dataToDisplay == CIRCLE_DATA)
                                        textInput.shape.circle = simulationShape.shape.circle;

                                    dataBoxRendered = false;
                                }
                            }
                        }
                    } else {

                        for (i = 0; i < TOTAL_CONSTANTS; i++) {
                            if (mouse_inside(mousex, mousey, constantsRects[i])) {
                                textInput.isTextChanging = true;
                                textInput.preTextInput = constantsStrings[i][0];
                                textInput.textInput = constantsStrings[i][1];
                                textInput.displayRect = &constantsRects[i];
                                textInput.textPtr = &constantsTexts[i];

                                //Frame rate input is an integer input
                                if (i == FRAME_RATE_INDEX)
                                    textInput.inputType = textChanging::T_INTEGER;

                                else
                                    textInput.inputType = textChanging::T_DECIMAL;

                                dataBoxRendered = false;
                                break;
                            }
                        }
                    }
                }
                    /**Here, we check if one of pause, stop or resume buttons was pressed if simulation is running

                       We don't explicitly check if a mouse down was carried out on the other main menu buttons,
                       because the only time clicking one of the menu buttons should have an effect, is if
                       they a mouse down and mouse up are both carried out.

                    **/
                else if (mouse_inside(mousex, mousey, menuBar) && simInfo.simulationRunning) {
                    if (simInfo.simulationRunning == RUNNING) {
                        if (mouse_inside(mousex, mousey, pauseRect)) {
                            simInfo.simulationRunning = PAUSED;
                            menuBarRendered = false;
                        }
                    } else if (simInfo.simulationRunning == PAUSED) {
                        if (mouse_inside(mousex, mousey, resumeRect)) {
                            simInfo.simulationRunning = RUNNING;
                            menuBarRendered = false;

                        }
                    }

                    if (mouse_inside(mousex, mousey, stopRect)) {
                        simInfo.simulationRunning = NOT_RUNNING;
                        menuBarRendered = false;
                        simulationMapRendered = false;
                        dataBoxRendered = false;

                        pEngine.clearShapes();
                        simulationShape.removeShape();
                        dataToDisplay = SIMULATION_CONSTANTS;

                        copyRect(rectangles, rectanglesCopy);
                        copyTri(triangles, trianglesCopy);
                        copyCircle(circles, circlesCopy);

                        rectanglesCopy.clear();
                        trianglesCopy.clear();
                        circlesCopy.clear();

                    }
                }

                mouseClickedState = CLICKED;
            } else if (event.type == SDL_MOUSEWHEEL && !simInfo.simulationRunning) {

                if (simulationShape.shapeNum == shapeClicked::CIRCLE) {


                    if (mouse_inside(mousex, mousey, simulationMap)) {

                        physicsEngine::Circle *circle = simulationShape.shape.circle;
                        renderingCoordinates *circleCoords = simulationShape.shapeRenderCoords.circle;

                        int x = convertIntDouble(circle->xPosition());
                        int y = convertIntDouble(circle->yPosition());
                        int r = circle->getRadius();

                        int convertedMousex = inverseConvertX(mousex - simulationMap.x, simulationMap.w,
                                                              mapPosition.dx);
                        int convertedMousey = inverseConvertY(mousey - simulationMap.y, simulationMap.h,
                                                              mapPosition.dy);

                        if (pointInCircle(simulationShape.shape.circle, convertedMousex, convertedMousey)) {
                            int scrollAmount = event.wheel.y;
                            int changeRadius;

                            if (scrollAmount < 0)
                                changeRadius = -5;

                            else if (scrollAmount > 0)
                                changeRadius = 5;

                            if (circleValidOnMap(x, y, r + changeRadius)) {
                                circle->changeRadius(r + changeRadius);
                                setupCircleRendering(circle, circleCoords);

                                simulationShape.removeShape();
                                simulationShape.addCircle(circle, circleCoords, convertedMousex, convertedMousey);
                                simulationShape.highlightShape();
                                simulationMapRendered = false;
                                dataBoxRendered = false;

                            }
                        }
                    }
                }
            } else if (event.type == SDL_KEYDOWN && !simInfo.simulationRunning) {

                //User wants to delete a shape
                //A shape can only be deleted if it is highlighted on the screen
                if (event.key.keysym.sym == SDLK_DELETE) {

                    if (simulationShape.isSetup()) {

                        int pos;

                        if (simulationShape.shapeNum == shapeClicked::RECTANGLE) {
                            if (simulationShape.shape.rect->shouldHighlightShape) {
                                pos = simulationShape.shape.rect - &rectangles[0];
                                simulationShape.removeShape();
                                rectangles.erase(rectangles.begin() + pos, rectangles.begin() + pos + 1);
                            }
                        } else if (simulationShape.shapeNum == shapeClicked::TRIANGLE) {

                            if (simulationShape.shape.tri->shouldHighlightShape) {
                                pos = simulationShape.shape.tri - &triangles[0];
                                simulationShape.removeShape();

                                triangles.erase(triangles.begin() + pos, triangles.begin() + pos + 1);
                                trianglesCoordinates.erase(trianglesCoordinates.begin() + pos,
                                                           trianglesCoordinates.begin() + pos + 1);
                            }
                        } else if (simulationShape.shapeNum == shapeClicked::CIRCLE) {

                            if (simulationShape.shape.circle->shouldHighlightShape) {
                                pos = simulationShape.shape.circle - &circles[0];
                                simulationShape.removeShape();

                                circles.erase(circles.begin() + pos, circles.begin() + pos + 1);
                                circlesCoordinates.erase(circlesCoordinates.begin() + pos,
                                                         circlesCoordinates.begin() + pos + 1);
                            }
                        }
                        dataToDisplay = SIMULATION_CONSTANTS;
                        simulationMapRendered = false;
                        dataBoxRendered = false;
                    }

                }

                //If a shape has been clicked, its colour can be changed by key presses
                if (simulationShape.isSetup()) {
                    bool colourChanged = false;
                    switch (event.key.keysym.sym) {

                        //Pressing e decreases red value
                        case SDLK_e:
                            simulationShape.unhilightShape();
                            simulationShape.changeColour(colourDecrease, 0, 0, 0);
                            colourChanged = true;
                            break;

                            //Pressing r increases red value
                        case SDLK_r:
                            simulationShape.unhilightShape();
                            simulationShape.changeColour(colourIncrease, 0, 0, 0);
                            colourChanged = true;
                            break;

                            //Pressing a decreases alpha value
                        case SDLK_a:
                            simulationShape.unhilightShape();
                            simulationShape.changeColour(0, 0, 0, colourDecrease);
                            colourChanged = true;
                            break;

                            //Pressing s increases alpha value
                        case SDLK_s:
                            simulationShape.unhilightShape();
                            simulationShape.changeColour(0, 0, 0, colourIncrease);
                            colourChanged = true;
                            break;

                            //Pressing v decreases blue value
                        case SDLK_v:
                            simulationShape.unhilightShape();
                            simulationShape.changeColour(0, 0, colourDecrease, 0);
                            colourChanged = true;
                            break;

                            //Pressing b increases blue value
                        case SDLK_b:
                            simulationShape.unhilightShape();
                            simulationShape.changeColour(0, 0, colourIncrease, 0);
                            colourChanged = true;
                            break;

                            //Pressing f decreases green value
                        case SDLK_f:
                            simulationShape.unhilightShape();
                            simulationShape.changeColour(0, colourDecrease, 0, 0);
                            colourChanged = true;
                            break;

                            //Pressing g increases green value
                        case SDLK_g:
                            simulationShape.unhilightShape();
                            simulationShape.changeColour(0, colourIncrease, 0, 0);
                            colourChanged = true;
                            break;

                    }

                    if (colourChanged) {
                        simulationMapRendered = false;
                        dataBoxRendered = false;

                        if (simulationShape.shapeNum == shapeClicked::RECTANGLE)
                            dataToDisplay = RECTANGLE_DATA;

                        else if (simulationShape.shapeNum == shapeClicked::TRIANGLE)
                            dataToDisplay = TRIANGLE_DATA;

                        else if (simulationShape.shapeNum == shapeClicked::CIRCLE)
                            dataToDisplay = CIRCLE_DATA;
                    }
                }

                if (textInput.isTextChanging) {
                    int key = event.key.keysym.sym;

                    int inputSize = textInput.textInput.size();

                    //Erase one character at a time if back space is presses
                    //However, if the text input is blanked then a back space press is ignored
                    if (key == SDLK_BACKSPACE && inputSize > 0) {
                        textInput.textInput.erase(textInput.textInput.begin() + inputSize - 1,
                                                  textInput.textInput.begin() + inputSize);

                        dataBoxRendered = false;
                    }

                        /**
                            If a minus key is pressed (implying a negative number is being entered),
                            this is allowed, as long as it is the first character in the input stream
                            and the user is not attempting to change the frame rate

                            (negative frame rate is not possible)
                        **/
                    else if (key == SDLK_MINUS && inputSize == 0 &&
                             textInput.displayRect != &constantsRects[FRAME_RATE_INDEX]) {
                        textInput.textInput += "-";
                        dataBoxRendered = false;
                    }

                        //The .find ensures user cannot enter . more than one
                    else if (key == SDLK_PERIOD && textInput.inputType == textChanging::T_DECIMAL
                             && inputSize < maxInputLength && textInput.textInput.find(".") == string::npos) {
                        textInput.textInput += ".";
                        dataBoxRendered = false;
                    } else if (inputSize < maxInputLength) {

                        switch (key) {

                            case SDLK_0:
                                textInput.textInput += "0";
                                dataBoxRendered = false;
                                break;

                            case SDLK_1:
                                textInput.textInput += "1";
                                dataBoxRendered = false;
                                break;

                            case SDLK_2:
                                textInput.textInput += "2";
                                dataBoxRendered = false;
                                break;

                            case SDLK_3:
                                textInput.textInput += "3";
                                dataBoxRendered = false;
                                break;

                            case SDLK_4:
                                textInput.textInput += "4";
                                dataBoxRendered = false;
                                break;

                            case SDLK_5:
                                textInput.textInput += "5";
                                dataBoxRendered = false;
                                break;

                            case SDLK_6:
                                textInput.textInput += "6";
                                dataBoxRendered = false;
                                break;

                            case SDLK_7:
                                textInput.textInput += "7";
                                dataBoxRendered = false;
                                break;

                            case SDLK_8:
                                textInput.textInput += "8";
                                dataBoxRendered = false;
                                break;

                            case SDLK_9:
                                textInput.textInput += "9";
                                dataBoxRendered = false;
                                break;

                        }
                    }

                    //User wants to submit text entered
                    if (key == SDLK_RETURN || key == SDLK_RETURN2) {
                        //Check if one of the simulation constants is attempting to be changed
                        //And is not the frame rate
                        if (dataToDisplay == SIMULATION_CONSTANTS &&
                            textInput.inputType == textChanging::T_DECIMAL) {

                            double floatNum;

                            //If statement checks input is a valid double
                            if (convert_str_to_double(textInput.textInput.c_str(), &floatNum)) {
                                SDL_Rect *simRect = textInput.displayRect;

                                if (simRect == &constantsRects[GRAVITY]) {

                                    if (abs(floatNum) <= physicsEngine::DEFAULT_FRAME_RATE) {

                                        physicsEngine::gravitationalAcceleration = roundNumber(floatNum, decimalPlaces);
                                        textInput.isTextChanging = false;
                                        dataBoxRendered = false;
                                    } else {
                                        MessageBox(NULL, "Absolute value for gravity cannot exceed frame rate.", NULL,
                                                   MB_OK);

                                    }

                                }

                                    //Every other simulation constant is restitution
                                else {
                                    double rounded = roundNumber(floatNum, restitutionDecimalPlaces);
                                    for (i = 1; i < FLOAT_CONSTANTS; i++) {
                                        if (simRect == &constantsRects[i]) {
                                            if (rounded > 0 && rounded <= 1) {
                                                *constantsArray[i] = rounded;
                                                textInput.isTextChanging = false;
                                                dataBoxRendered = false;
                                                break;
                                            } else {
                                                MessageBox(NULL,
                                                           "Restitution must be greater than 0 and less than or equal to 1.",
                                                           NULL, MB_OK);
                                                break;
                                            }
                                        }
                                    }
                                }

                            } else
                                MessageBox(NULL, "Invalid input for floating point number.", NULL, MB_OK);

                        }

                            //Frame Rate is being changed
                        else if (dataToDisplay == SIMULATION_CONSTANTS &&
                                 textInput.inputType == textChanging::T_INTEGER &&
                                 textInput.displayRect == &constantsRects[FRAME_RATE_INDEX]) {

                            int newFrameRate;

                            if (convert_str_to_int(textInput.textInput.c_str(), &newFrameRate)) {

                                //Frame rate cannot be 0
                                if (newFrameRate > 0) {
                                    physicsEngine::DEFAULT_FRAME_RATE = newFrameRate;

                                    for (i = 0; i < rectangles.size(); i++)
                                        rectangles[i].changeFrameRate(newFrameRate);

                                    for (i = 0; i < triangles.size(); i++)
                                        triangles[i].changeFrameRate(newFrameRate);

                                    for (i = 0; i < circles.size(); i++)
                                        circles[i].changeFrameRate(newFrameRate);

                                    pEngine.changeFrameRate(newFrameRate);

                                    dataBoxRendered = false;
                                    textInput.isTextChanging = false;


                                } else
                                    MessageBox(NULL, "Frame rate can't be 0.", NULL, MB_OK);

                            } else
                                MessageBox(NULL, "Invalid input for integer.", NULL, MB_OK);
                        }

                            //One of shapes data is being changed
                        else {
                            double floatNum;
                            double rounded;
                            int intNum;
                            bool valid = false;
                            if (textInput.inputType == textChanging::T_INTEGER) {

                                if (convert_str_to_int(textInput.textInput.c_str(), &intNum))
                                    valid = true;
                            } else if (textInput.inputType == textChanging::T_DECIMAL) {
                                if (convert_str_to_double(textInput.textInput.c_str(), &floatNum)) {
                                    valid = true;
                                    rounded = roundNumber(floatNum, decimalPlaces);

                                }
                            }

                            if (valid) {
                                SDL_Rect *simRect = textInput.displayRect;

                                if (simRect == &generalShapeValuesRects[MASS]) {

                                    //Mass must be greater than 0 and less than 10000
                                    if (rounded > 0 && rounded <= physicsEngine::MAX_MASS) {

                                        if (dataToDisplay == RECTANGLE_DATA)
                                            textInput.shape.rect->changeMass(rounded);

                                        else if (dataToDisplay == TRIANGLE_DATA)
                                            textInput.shape.tri->changeMass(rounded);

                                        else if (dataToDisplay == CIRCLE_DATA)
                                            textInput.shape.circle->changeMass(rounded);

                                        dataBoxRendered = false;
                                        textInput.isTextChanging = false;

                                    } else {
                                        MessageBox(NULL, "Mass must be greater than 0 and less than or equal to 10000.",
                                                   NULL, MB_OK);
                                    }


                                } else if (simRect == &generalShapeValuesRects[XVELOCITY] ||
                                           simRect == &generalShapeValuesRects[YVELOCITY] ||
                                           simRect == &generalShapeValuesRects[XACCELERATION] ||
                                           simRect == &generalShapeValuesRects[YACCELERATION]) {

                                    int pos = simRect - &generalShapeValuesRects[0];


                                    if (abs(rounded) <= physicsEngine::DEFAULT_FRAME_RATE) {

                                        switch (pos) {
                                            case XVELOCITY:
                                                if (dataToDisplay == RECTANGLE_DATA)
                                                    textInput.shape.rect->changeSpeedx(rounded);

                                                else if (dataToDisplay == TRIANGLE_DATA)
                                                    textInput.shape.tri->changeSpeedx(rounded);

                                                else if (dataToDisplay == CIRCLE_DATA)
                                                    textInput.shape.circle->changeSpeedx(rounded);

                                                break;

                                            case YVELOCITY:
                                                if (dataToDisplay == RECTANGLE_DATA)
                                                    textInput.shape.rect->changeSpeedy(rounded);

                                                else if (dataToDisplay == TRIANGLE_DATA)
                                                    textInput.shape.tri->changeSpeedy(rounded);

                                                else if (dataToDisplay == CIRCLE_DATA)
                                                    textInput.shape.circle->changeSpeedy(rounded);

                                                break;

                                            case XACCELERATION:
                                                if (dataToDisplay == RECTANGLE_DATA)
                                                    textInput.shape.rect->changeAccelerationx(rounded);

                                                else if (dataToDisplay == TRIANGLE_DATA)
                                                    textInput.shape.tri->changeAccelerationx(rounded);

                                                else if (dataToDisplay == CIRCLE_DATA)
                                                    textInput.shape.circle->changeAccelerationx(rounded);

                                                break;

                                            case YACCELERATION:
                                                if (dataToDisplay == RECTANGLE_DATA)
                                                    textInput.shape.rect->changeAccelerationy(rounded);

                                                else if (dataToDisplay == TRIANGLE_DATA)
                                                    textInput.shape.tri->changeAccelerationy(rounded);

                                                else if (dataToDisplay == CIRCLE_DATA)
                                                    textInput.shape.circle->changeAccelerationy(rounded);

                                                break;

                                        }


                                        dataBoxRendered = false;
                                        textInput.isTextChanging = false;
                                    } else {
                                        MessageBox(NULL,
                                                   "A motion value's absolute value must be less than or equal to the frame rate.",
                                                   NULL, MB_OK);
                                    }
                                } else if (simRect == &generalShapeValuesRects[RVALUE] ||
                                           simRect == &generalShapeValuesRects[GVALUE] ||
                                           simRect == &generalShapeValuesRects[BVALUE] ||
                                           simRect == &generalShapeValuesRects[AVALUE]) {


                                    if (simulationShape.isShapeHighlighted())
                                        simulationShape.unhilightShape();


                                    int pos = simRect - &generalShapeValuesRects[0];
                                    switch (pos) {

                                        case RVALUE:
                                            if (dataToDisplay == RECTANGLE_DATA)
                                                textInput.shape.rect->changeColour(intNum,
                                                                                   textInput.shape.rect->gValue(),
                                                                                   textInput.shape.rect->bValue(),
                                                                                   textInput.shape.rect->aValue());

                                            else if (dataToDisplay == TRIANGLE_DATA)
                                                textInput.shape.tri->changeColour(intNum, textInput.shape.tri->gValue(),
                                                                                  textInput.shape.tri->bValue(),
                                                                                  textInput.shape.tri->aValue());

                                            else if (dataToDisplay == CIRCLE_DATA)
                                                textInput.shape.circle->changeColour(intNum,
                                                                                     textInput.shape.circle->gValue(),
                                                                                     textInput.shape.circle->bValue(),
                                                                                     textInput.shape.circle->aValue());


                                            break;

                                        case GVALUE:
                                            if (dataToDisplay == RECTANGLE_DATA)
                                                textInput.shape.rect->changeColour(textInput.shape.rect->rValue(),
                                                                                   intNum,
                                                                                   textInput.shape.rect->bValue(),
                                                                                   textInput.shape.rect->aValue());

                                            else if (dataToDisplay == TRIANGLE_DATA)
                                                textInput.shape.tri->changeColour(textInput.shape.tri->rValue(), intNum,
                                                                                  textInput.shape.tri->bValue(),
                                                                                  textInput.shape.tri->aValue());


                                            else if (dataToDisplay == CIRCLE_DATA)
                                                textInput.shape.circle->changeColour(textInput.shape.circle->rValue(),
                                                                                     intNum,
                                                                                     textInput.shape.circle->bValue(),
                                                                                     textInput.shape.circle->aValue());

                                            break;

                                        case BVALUE:
                                            if (dataToDisplay == RECTANGLE_DATA)
                                                textInput.shape.rect->changeColour(textInput.shape.rect->rValue(),
                                                                                   textInput.shape.rect->gValue(),
                                                                                   intNum,
                                                                                   textInput.shape.rect->aValue());

                                            else if (dataToDisplay == TRIANGLE_DATA)
                                                textInput.shape.tri->changeColour(textInput.shape.tri->rValue(),
                                                                                  textInput.shape.tri->gValue(),
                                                                                  intNum,
                                                                                  textInput.shape.tri->aValue());

                                            else if (dataToDisplay == CIRCLE_DATA)
                                                textInput.shape.circle->changeColour(textInput.shape.circle->rValue(),
                                                                                     textInput.shape.circle->gValue(),
                                                                                     intNum,
                                                                                     textInput.shape.circle->aValue());
                                            break;

                                        case AVALUE:
                                            if (dataToDisplay == RECTANGLE_DATA)
                                                textInput.shape.rect->changeColour(textInput.shape.rect->rValue(),
                                                                                   textInput.shape.rect->gValue(),
                                                                                   textInput.shape.rect->bValue(),
                                                                                   intNum);

                                            else if (dataToDisplay == CIRCLE_DATA)
                                                textInput.shape.circle->changeColour(textInput.shape.circle->rValue(),
                                                                                     textInput.shape.circle->gValue(),
                                                                                     textInput.shape.circle->bValue(),
                                                                                     intNum);

                                            else if (dataToDisplay == TRIANGLE_DATA)
                                                textInput.shape.tri->changeColour(textInput.shape.tri->rValue(),
                                                                                  textInput.shape.tri->gValue(),
                                                                                  textInput.shape.tri->bValue(),
                                                                                  intNum);
                                            break;


                                    }

                                    dataBoxRendered = false;
                                    textInput.isTextChanging = false;
                                    simulationMapRendered = false;

                                }

                                    //Check for button submits on specific shapes
                                else if (dataToDisplay == RECTANGLE_DATA) {

                                    physicsEngine::Rectangle *rect = textInput.shape.rect;
                                    int x = convertIntDouble(rect->xPosition());
                                    int y = convertIntDouble(rect->yPosition());
                                    int w = rect->getWidth();
                                    int h = rect->getHeight();

                                    bool rectChanged = false;

                                    //User wants to change x position of rectangle
                                    if (simRect == &rectValuesRects[TOP_LEFT_X]) {

                                        if (rectValidOnMap(intNum, x, w, h)) {

                                            rect->changePosition(intNum, y);
                                            rectChanged = true;
                                        } else
                                            MessageBox(NULL, "New x position is not valid on map.", NULL, MB_OK);


                                    } else if (simRect == &rectValuesRects[TOP_LEFT_Y]) {
                                        if (rectValidOnMap(x, intNum, w, h)) {
                                            rect->changePosition(x, intNum);
                                            rectChanged = true;

                                        } else
                                            MessageBox(NULL, "New y position is not valid on map.", NULL, MB_OK);
                                    } else if (simRect == &rectValuesRects[WIDTH]) {

                                        if (rectValidOnMap(x, y, intNum, h)) {
                                            rect->changeWidth(intNum);
                                            rectChanged = true;
                                        } else
                                            MessageBox(NULL,
                                                       "New width is not valid on map given the position of the rectangle.",
                                                       NULL, MB_OK);
                                    } else if (simRect == &rectValuesRects[HEIGHT]) {

                                        if (rectValidOnMap(x, y, w, intNum)) {
                                            rect->changeHeight(intNum);
                                            rectChanged = true;
                                        } else
                                            MessageBox(NULL,
                                                       "New height is not valid on map given the position of the rectangle.",
                                                       NULL, MB_OK);
                                    }

                                    if (rectChanged) {
                                        dataBoxRendered = false;
                                        simulationMapRendered = false;
                                        textInput.isTextChanging = false;
                                    }

                                } else if (dataToDisplay == TRIANGLE_DATA) {

                                    physicsEngine::Triangle *tri = textInput.shape.tri;
                                    int x1 = convertIntDouble(tri->xcoordPoint1());
                                    int y1 = convertIntDouble(tri->ycoordPoint1());
                                    int x2 = convertIntDouble(tri->xcoordPoint2());
                                    int y2 = convertIntDouble(tri->ycoordPoint2());
                                    int x3 = convertIntDouble(tri->xcoordPoint3());
                                    int y3 = convertIntDouble(tri->ycoordPoint3());

                                    bool triChanged = false;
                                    bool changeInvalid = false;

                                    if (simRect == &triValuesRects[P1X]) {
                                        if (triValidOnMap(intNum, y1, x2, y2, x3, y3)) {
                                            tri->changePoint1(intNum, y1);
                                            triChanged = true;
                                        } else
                                            changeInvalid = true;
                                    } else if (simRect == &triValuesRects[P1Y]) {
                                        if (triValidOnMap(x1, intNum, x2, y2, x3, y3)) {
                                            tri->changePoint1(x1, intNum);
                                            triChanged = true;
                                        } else
                                            changeInvalid = true;
                                    } else if (simRect == &triValuesRects[P2X]) {
                                        if (triValidOnMap(x1, y1, intNum, y2, x3, y3)) {
                                            tri->changePoint2(intNum, y2);
                                            triChanged = true;
                                        } else
                                            changeInvalid = true;
                                    } else if (simRect == &triValuesRects[P2Y]) {
                                        if (triValidOnMap(x1, y1, x2, intNum, x3, y3)) {
                                            tri->changePoint2(x2, intNum);
                                            triChanged = true;
                                        } else
                                            changeInvalid = true;
                                    } else if (simRect == &triValuesRects[P3X]) {
                                        if (triValidOnMap(x1, y1, x2, y2, intNum, y3)) {
                                            tri->changePoint3(intNum, y3);
                                            triChanged = true;
                                        } else
                                            changeInvalid = true;
                                    } else if (simRect == &triValuesRects[P3Y]) {
                                        if (triValidOnMap(x1, y1, x2, y2, x3, intNum)) {
                                            tri->changePoint3(x3, intNum);
                                            triChanged = true;
                                        } else
                                            changeInvalid = true;
                                    }

                                    if (triChanged) {
                                        setupTriangleRendering(tri, simulationShape.shapeRenderCoords.tri);
                                        dataBoxRendered = false;
                                        simulationMapRendered = false;
                                        textInput.isTextChanging = false;
                                    } else if (changeInvalid) {
                                        MessageBox(NULL, "New Coordinate invalid.", NULL, MB_OK);
                                    }


                                } else if (dataToDisplay == CIRCLE_DATA) {

                                    physicsEngine::Circle *circle = textInput.shape.circle;

                                    int x = convertIntDouble(circle->xPosition());
                                    int y = convertIntDouble(circle->yPosition());
                                    int r = circle->getRadius();

                                    bool circleChanged = false;

                                    if (simRect == &circleValuesRects[CENTRE_X]) {
                                        if (circleValidOnMap(intNum, y, r)) {
                                            circle->changePosition(intNum, y);
                                            circleChanged = true;

                                        } else
                                            MessageBox(NULL, "New x coordinate is invalid.", NULL, MB_OK);
                                    } else if (simRect == &circleValuesRects[CENTRE_Y]) {
                                        if (circleValidOnMap(x, intNum, r)) {
                                            circle->changePosition(x, intNum);
                                            circleChanged = true;
                                        } else
                                            MessageBox(NULL, "New x coordinate is invalid.", NULL, MB_OK);
                                    } else if (simRect == &circleValuesRects[RADIUS]) {
                                        if (circleValidOnMap(x, y, intNum)) {
                                            circle->changeRadius(intNum);
                                            circleChanged = true;

                                            setupCircleRendering(circle, simulationShape.shapeRenderCoords.circle);

                                        } else
                                            MessageBox(NULL, "New radius is invalid.", NULL, MB_OK);
                                    }

                                    if (circleChanged) {
                                        simulationMapRendered = false;
                                        dataBoxRendered = false;
                                        textInput.isTextChanging = false;
                                    }
                                }

                            } else
                                MessageBox(NULL, "Invalid input.", NULL, MB_OK);

                        }
                    }

                }
            }

                //This prevents the screen from potentially turning black if other programs are executed
                //(particularly task manager)

            else if (event.type == SDL_WINDOWEVENT && !textInput.isTextChanging) {
                if (event.window.event == SDL_WINDOWEVENT_LEAVE ||
                    event.window.event == SDL_WINDOWEVENT_ENTER) {
                    simulationMapRendered = false;
                    displayShapesRendered = false;
                    dataBoxRendered = false;
                    menuBarRendered = false;
                    miniMapRendered = false;
                }
            }

        }

        if (renderWasChanged) {
            SDL_RenderPresent(renderer);
            renderWasChanged = false;

        }


    }

    /**Here, all memory is freed up**/

    delete[] constantsRects;
    delete[] generalShapeValuesRects;
    delete[] rectValuesRects;
    delete[] circleValuesRects;
    delete[] triValuesRects;

    delete[] constantsTexts;
    delete[] generalShapeValuesTexts;
    delete[] rectValuesTexts;
    delete[] circleValuesTexts;
    delete[] triValuesTexts;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_CloseFont(calibri_18);
    TTF_CloseFont(calibri_24);
    TTF_CloseFont(calibri_48);
    TTF_Quit();
    SDL_Quit();


    return 0;


}


#elif TESTING == 0
void databaseMENGTest(){

    //Using a .MENG extension automatically sets a database as .MENG
    //Later on, if user tries to open a database that isn't .MENG flag up an error
    //If .MENG file doesn't contain tables, inform user that .MENG file is invalid

    sqlite3* db = setupDatabase("templateTest.meng");

    if (!db)
        cout << "Failure";

    else
        cout <<"Database created";
}


int main (int argc, char* args[]){

#include "tests.h"
    //fraction_t();

    //matrix_t();

    //motion_frame_test();

    //collisionsTests();

    //checkMixingObjects();

    //collisionTestsPhysicsEngine();

    //collisionHandlingTestsSameRest();

    collisionHandlingTestsDifferentRest();

    /**WHEN DOING PIXEL PER FRAMES THING, ADD DECIMAL PLACE
        TO CAP NUMBER AFTER A SET AMOUNT OF TIMES IN WHILE(!isInteger)**/
   //collisionHandlingTestsDiffRestPhysicsEngineOwn();




    //collisionTestSameCoordinateAxis();

    //databaseSaveTest();

    //loadDatabaseTest();

    //databaseMENGTest();

    //randTest();

    //collideRectEmergency();

    //pixelMovementTests(60,59.9);
    return 0;

}


#endif // DEBUG


