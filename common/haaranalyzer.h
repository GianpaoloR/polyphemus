#ifndef haarAnalyzer_H
#define haarAnalyzer_H


#include <iostream>

#include "common/cvInclusions.h"
#include "frameHandler/guihandler.h"
#include "constants.h"

#define DARK 200





struct eyesStruct
{
    //Notice that the "left eye" is the one that the CAMERA sees to the right, but it is the user's left eye. Idem for the other eye.
    int eye_region_width;
    int eye_region_height;
    int eye_region_top;

    cv::Rect* leftEyeRegion;
    cv::Rect* rightEyeRegion;



    cv::Point* leftPupil;
    cv::Point* rightPupil;





    cv::Rect* leftEyeReducedRegion;
    cv::Rect* rightEyeReducedRegion;


#ifdef HAVING_EYE_CORNERS
    cv::Rect* leftRightCornerRegion;
    cv::Rect* leftLeftCornerRegion;
    cv::Rect* rightRightCornerRegion;
    cv::Rect* rightLeftCornerRegion;
    cv::Mat *leftCornerKernel;
    cv::Mat *rightCornerKernel;
#else

    cv::Point leftPupilAfterRotation;
    cv::Point rightPupilAfterRotation;

    //Following points have coords in eyes rotated ROI
    cv::Point leftEyeLeftExt;
    cv::Point leftEyeRightExt;
    cv::Point rightEyeLeftExt;
    cv::Point rightEyeRightExt;

#ifdef HAVING_GRADIENT_WINDOWS
    cv::Mat leftXGradient, leftYGradient;
    cv::Mat rightXGradient, rightYGradient;
#endif
#endif

};

struct faceStruct
{
    cv::Mat faceROI;
    cv::Point leftPupil;
    cv::Point rightPupil;
    eyesStruct eyes;
};

struct peakAnalysis {
    int peakFound;
    int peakType[100];  //type of peaks in this line +1, -1
    int peakVal[100];   //uchar values for peaks in this line
    int peakX[100];     //X values for peaks in this line
    bool valid;
    int maxDelta;
};

/*static int leftEyeLeftSup = 0;
static int leftEyeRightSup = 0;
static int rightEyeLeftSup = 0;
static int rightEyeRightSup = 0;
static int leftEyeEqual = 0;
static int rightEyeEqual = 0;
static int leftSupAvg = 0;
static int rightSupAvg = 0;
static int equalAvg = 0;*/

class haarAnalyzer
{

public:
    enum cascadeType
    {
        FACE,
        EYEPAIR,
        SINGLEEYE,
        LEFTEYE,
        RIGHTEYE,
        NOSE,
        MOUTH,
        UPPERBODY
    };

    haarAnalyzer();

    #ifdef WITH_GUI
    void setDebugGui(guiHandler *gui);
    #endif

    bool loadCascade(cascadeType type, std::string path);
    bool hasDetectFace(cv::Mat& img);
    void detectUpperBody(cv::Mat& img);
    bool hasDetectFaceWithSize(cv::Mat& img, cv::Size &roundSize);
    void detectFaces(cv::Mat& img);
    bool redetectFace(cv::Mat& img);
    std::vector<cv::Rect> getFaces();
    std::vector<cv::Rect> getEyeZones();
    std::vector<cv::Rect> getUpperBodies();


    //-------------------------------------------

private:
    //Classifiers
    cv::CascadeClassifier face_cascade;
    cv::CascadeClassifier eyePair_cascade;
    cv::CascadeClassifier singleEye_cascade;
    cv::CascadeClassifier leftEye_cascade;
    cv::CascadeClassifier rightEye_cascade;
    cv::CascadeClassifier nose_cascade;
    cv::CascadeClassifier mouth_cascade;
    cv::CascadeClassifier upper_body_cascade;

    //Face rectangle vector
    std::vector<cv::Rect> faces;
    std::vector<cv::Rect> upper_bodies;


#ifdef WITH_GUI
    //Debugging gui link
    guiHandler* gui;
#endif

    //-----------------------------------------------

};

#endif // haarAnalyzer_H
