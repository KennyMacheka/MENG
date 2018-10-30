#include "rendering_engine_global_vars.h"
#include "physics_engine_global_vars.h"

const double widthScale = 0.98;
const double heightScale = 0.90;
const double windowPos_x_scale = 0.01;
const double windowPos_y_scale = 0.05;
const short int menuBarHeight = 35;
const short int spacingMenuButtons = 35;
const short int decimalPlaces = 1;
const short int restitutionDecimalPlaces = 2;
const short int panningdx = 10;
const short int panningdy = 10;
const short int resizeCirclesRadius = 5;
const short int minCircleRadius = 5;
const short int minRectWidth = 20;
const short int minRectHeight = 20;
const short int colourIncrease = 1;
const short int colourDecrease = -1;
const short int maxInputLength = 6;

//The maximum number of pixels a circle or triangle may possibly take up
//This is a very large overestimate, but guarantees program won't crash if a triangle or circle was large enough
const int maxPixels = 10000000;

const double dataBoxWidthRatio = 0.3;
const double dataBoxHeightRatio = 0.7;
const double simulationMapWidthRatio = 0.7;
const double simulationMapHeightRatio = 0.9;
const double consoleBoxWidthRatio = 0.3;
const double consoleBoxHeightRatio = 0.3;
const double shapesBoxWidthRatio = 0.7;
const double shapesBoxHeightRatio = 0.1;

//Not all of the simulation map will take up the screen
//So this maximum window dimensions will still mean not all of the simulation map will be shown at any one time
const int maxWindowWidth = physicsEngine::simulationWidth;
const int maxWindowHeight = physicsEngine::simulationHeight;
