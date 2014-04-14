#ifndef HEADROTATION_H
#define HEADROTATION_H

#include "common/roiHandler.h"
#include "common/haaranalyzer.h"
#include "constants.h"
#include "common/helpers.h"

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
    double evaluateRotationY();
    double evaluateRotationX();
    double evaluateRotationWithNoseY();
    double evaluateRotationWithMouthY();
    double evaluateDistance(cv::Point a, cv::Point b);
    double evaluateRotationWithEyesY();
    cv::Point evaluatePosition(double rotation, cv::Rect face, cv::Mat frame);
    cv::Rect thresholdImage(cv::Mat& img);
    int searchWhiteAtL(int k, cv::Mat drawing);
    int searchWhiteAtR(int k, cv::Mat drawing);
    void processDistances();
    double evaluateRotationZ(haarAnalyzer *haar);

private:
    roiHandler* rH;
    int vRes;
    int hRes;
};

#endif // HEADROTATION_H
