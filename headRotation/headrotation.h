#ifndef HEADROTATION_H
#define HEADROTATION_H

#include "common/roiHandler.h"
#include "common/haaranalyzer.h"
#include "constants.h"
#include "common/helpers.h"
#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>


#ifdef WEBSERVICE
#include "clientServer/server.h"
#endif

class HeadRotation
{
public:
    HeadRotation();
    void setRoiHandler(roiHandler* rH);
    void setScreenSize(int w, int h);
    roiHandler* getRoiHandler();
    double evaluateRotationY(float* landmarks);
    double evaluateRotationX(float* landmarks);
    double evaluateRotationWithNoseY(float* landmarks);
    double evaluateRotationWithMouthY(float* landmarks);
    double evaluateDistance(cv::Point a, cv::Point b);
    double evaluateRotationWithEyesY();
    cv::Point evaluatePosition(double rotation, cv::Rect face, cv::Mat frame, float* landmarks);
    cv::Rect thresholdImage(cv::Mat& img);
    int searchWhiteAtL(int k, cv::Mat drawing);
    int searchWhiteAtR(int k, cv::Mat drawing);
    double evaluateRotationZ(haarAnalyzer *haar);
    void positPoint(float * landmarks);
    void posit(std::vector<CvPoint3D32f> modelPoints, std::vector<CvPoint2D32f> srcImagePoints);
    void setGazeFromPosit();
    double getObsPointX();
    double getObsPointY();
    double getObsPointXKalmanCorrection();
    double getObsPointYKalmanCorrection();
    void kalmanFilter();
    double radianti_gradi(double radiante);
    void setDistanceHead();
    double getDistanceHead();
    double getAngleZ();
    double getAngleX();
    double getAngleY();
    #ifdef WITH_GUI
    void setDebugGui(guiHandler *gui);
    #endif


private:
    roiHandler* rH;
    #ifdef WITH_GUI
    guiHandler* gui;
    #endif
    int vRes;
    int hRes;
    double angle_x;
    double angle_y;
    double angle_z;
    double distance;
    double eyeDim;
    double obsPointX;
    double obsPointY;
    double obsPointXKalmanCorrection;
    double obsPointYKalmanCorrection;
    Mat_<float> measurement;
    bool initKalman;
    cv::Mat prediction;
    cv::Mat estimated;
    KalmanFilter KF;
    vector<Point> obsv,kalmanv;
};

#endif // HEADROTATION_H
