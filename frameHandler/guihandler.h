#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#ifdef __linux__
#include <X11/Xlib.h>
#else
//#include <qapplication.h>
//#include <QtGui/QApplication>
#include <QApplication>
//#include <qdesktopwidget.h>
//#include <QtGui/QDesktopWidget>
#include <QDesktopWidget>
#endif

#ifdef WITH_GUI

#include <iostream>
#include <queue>
#include <stdio.h>


#include "common/cvInclusions.h"
#include "constants.h"
//#include "wtypes.h"


enum WINDOW_TYPE
{
    MAIN_WINDOW,
    FACE_WINDOW,
    LEFT_REDUCED,
    RIGHT_REDUCED,
    LEFT_ROTATED,
    RIGHT_ROTATED,
    GAZE_WINDOW,
    PROFILE
};
/*
    LEFT_EYE_T,
    RIGHT_EYE_T,
    LEFT_EYE_R,
    RIGHT_EYE_R,
    LEFT_EYE_GRADIENT,
    RIGHT_EYE_GRADIENT
};*/

class guiHandler
{
public:

    int hRes, vRes;

    cv::Mat mainFrame;
    cv::Mat faceFrame;
    cv::Mat reducedFaceFrame;
    cv::Mat leftFrame;
    cv::Mat rightFrame;
    cv::Mat leftFrameRotated;
    cv::Mat rightFrameRotated;
    cv::Mat gazeFrame;

    std::vector<cv::Rect> eyes;
    std::vector<cv::Rect> eyesReduced;

    cv::Point leftPupilFaceCoords;
    cv::Point leftPupilFaceCoordsRefined;

    cv::Point rightPupilFaceCoords;
    cv::Point rightPupilFaceCoordsRefined;


    cv::Point leftPupilAfterRotation;
    cv::Point rightPupilAfterRotation;
    cv::Point gaze;
    cv::Point globalGaze;
    cv::Point square;
    int squareX;
    int squareY;

    bool faceReceived;
    bool eyesReceived;

    guiHandler(int w, int h);
    void initWindow(WINDOW_TYPE wt);
    void updateWindow(WINDOW_TYPE wt);
    void destroy(WINDOW_TYPE wt);

    //Image setters
    void setFaceFrame(cv::Mat frame);
    void setReducedFaceFrame(cv::Mat frame);
    void setLeftEmpiricReducedFrame(cv::Mat frame);
    void setRightEmpiricReducedFrame(cv::Mat frame);

    //Main showers
    void showFaceElements(std::vector<cv::Rect> elements, cv::Rect& face);
    void showEyesRotationY(cv::Rect& face, cv::Rect& leftEye, cv::Rect& rightEye, double rotation);
    void showNoseRotationY(cv::Rect& face, cv::Rect& nose, double rotation);
    void showMouthRotationY(cv::Rect& face, cv::Rect& mouth, double rotation);
    void showFaces(std::vector<cv::Rect> faces);
    void showUpperBody(std::vector<cv::Rect> upperBodies);
    void showHeadRotationY(cv::Rect& face, double rotation);

    //Face showers
    void showEyeZones();
    void showPupilsLine();
    void showPupils(Point* left, Point *right, bool refined, bool reduced);


    //Gaze showers
    void redrawGazeFrame();
    void turnOnEyeZone(int hZone, int vZone);
    void showStartingEyeGaze(cv::Point gazeStartingPoint);
    void turnOnHeadZone(double X, double Y, double KX, double KY);
    void showStartingHeadGaze(cv::Point headOrientation, cv::Scalar color);

    void showRoi(const cv::Mat& roi, std::string name, bool maintain);
    void showEyesRotated(cv::Point l, cv::Point r);
    void showTwinklePath(cv::Point setCenter, bool left, bool both);
    void showCorners(std::vector<cv::Point> cornerVector, bool left, bool both);
    //cv::Size getGazeFrameSize();
    void setGazeFromHeadData(double rotation, cv::Rect face, int difference_y);
    void setSquareFromGazeData();
    void adjustXComponent(double ratio);
    //void setHeadRotation(headRotation);
//  void testSquare();  --> NEVER USED!!!

private:
    std::string mainWindowName, faceWindowName, lEyeReducedWindowName, rEyeReducedWindowName, profileWindowName, lRotatedWindowName, rRotatedWindowName, gazeWindowName, gazeKeWindowName;
    //HeadRotation headRotation;
};

#endif // WITH_GUI

#endif // GUIHANDLER_H

