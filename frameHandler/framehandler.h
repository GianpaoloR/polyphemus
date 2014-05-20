#ifndef FRAMEHANDLER_H
#define FRAMEHANDLER_H

//UO
#ifdef WEBSERVICE
#include "../clientServer/server.h"
#endif
//UO

#include "common/cvInclusions.h"
#include "frameHandler/guihandler.h"
#include "common/helpers.h"

#include <iostream>
#include <stdio.h>

#define MAX_TRY 5
#define PATH_SIZE 200
#define PHOTO_DIGITS 10

#define PHOTO_ID_START 0
//#define PHOTO_ID_END 12

#define JP_SET_START 0
#define JP_SET_SIZE 12
#define UO_SET_START 12
#define UO_SET_SIZE 12

class frameHandler
{
public:
    struct eye
    {
        cv::Mat roi;
    };

    frameHandler();
    int getHZone();
    void prepareNextReading();

#ifdef WITH_GUI
    void setDebugGui(guiHandler *gui);
#endif

    bool initCapture();
    bool read();
    void mirror();
    void setExternalFrame(cv::Mat extFrame);
    cv::Mat& getExternalFrame();
    cv::Mat& getFrame();
    void preProcessFrame();
    void showEyesRotationY(double rotation);
    void showNoseRotationY(double rotation);

    void showMouthRotationY(double rotation);
    void setFaceROI(std::vector<cv::Rect> fVett);
    void rotateFaceROI(double angle);
    //void setReducedFaceROI(cv::Mat& reducedFaceFrame);
    void setReducedFaceROI(cv::Rect& reducedRect);
    bool hasFace();
    cv::Mat& getFaceROI();
    void showHeadRotationY(double rotation);
    void setEyeROI(std::vector<cv::Rect> eyeVect);
    void setReducedROIs(std::vector<cv::Rect> regions);
    cv::Mat& getReducedROI(bool left);
    cv::Mat& getRotatedROI(bool left);
    cv::Mat rotate(float alpha);
    float getAvgLight(bool left);
    void release();
    void highlightSkin(cv::Mat frameHighlightSkin);



    //bool init;


    #ifdef TEST_MODE
    //For static test purpose
    void setTestPath(std::string path);
    int getTotalRead();
    std::vector<int> getRealCoords();
    bool trainingSetFinished();
    #endif




private:
    CvCapture* capture;
    void handleROIs();

    //External frame
    cv::Mat externalFrame;

    //Initial frame
    cv::Mat frame;

    //Restriction to eye zones (Empiric)
    cv::Mat leftEyeEmpiricFrame;
    cv::Mat rightEyeEmpiricFrame;
    cv::Mat leftEmpiricRotatedFrame;
    cv::Mat rightEmpiricRotatedFrame;

    //Second eyes restriction (incremental)


    //Average light in reduced eye zones
    float avgLightLeft;
    float avgLightRight;

    //For profiling purpose
    char videoPath[PATH_SIZE];

    #ifdef TEST_MODE
    //For static test purpose
    char testPath[PATH_SIZE];
    char finalPath [PATH_SIZE];
    int zoneCounter;
    int zoneNumber;
    int frameCounter;
    int trainingSetSize;
    int currentImageId;
    int totalRead;

    FILE* fCoords;
    std::vector<int> pupilCoords;
    bool neverOpened;
    #endif

#ifdef WITH_GUI
    //For debugging purpose
    guiHandler* gui;
#endif

    //---------------------------------------------------------


};

#endif // FRAMEHANDLER_H
