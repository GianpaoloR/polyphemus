#ifndef CONSTANTS_H
#define CONSTANTS_H

//#define WEBSERVICE

const int nLM = 77;
const int eyeLM = 8;

// Debugging
const bool kPlotVectorField = false;

// Size constants
const int kEyePercentTop = 25;
const int kEyePercentSide = 13;
const int kEyePercentHeight = 30;
const int kEyePercentWidth = 35;

//Test constants
#ifndef WEBSERVICE
const bool canny_test=false;
const bool eye_test=true;
const bool face_test=true;
#else
const bool canny_test=false;
const bool eye_test=false;
const bool face_test=false;
#endif

// Preprocessing
const bool kSmoothFaceImage = false;
const float kSmoothFaceFactor = 0.005;

// Algorithm Parameters
const int kFastEyeWidth = 50;
const int kWeightBlurSize = 5;
const bool kEnableWeight = true;
const float kWeightDivisor = 150.0;
const double kGradientThreshold = 50.0;

// Postprocessing
const bool kEnablePostProcess = true;
const float kPostProcessThreshold = 0.97;

// Eye Corner
const bool kEnableEyeCorner = true;


//Position evaluation
#define DISTANZA_MEDIA_SCHERMO 70
#define CM_TO_PIXEL 37.795275591    //1cm -> 37.795275591 pixel
#define DIMENSIONE_SCHERMO 1600
#define DIMENSIONE_SCHERMO_Y 900
#define FOCAL_LENGTH 1000

//extern int screenWidthPixel;
//extern int screenHeightPixel;

#endif
