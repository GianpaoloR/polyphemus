#ifndef GAZEESTIMATION_H
#define GAZEESTIMATION_H

#include "common/cvInclusions.h"
#include "common/helpers.h"

class gazeEstimation
{
public:
    gazeEstimation();
    void setScreenResolution(int width, int height);
    void followGaze(cv::Point* leftPupil, cv::Point* rightPupil, Rect face);
    double getFinalHorizontalGaze();
    int getFinalXDisplacement();
    double getAvgHorizGaze();
    double prepareApprox(double dist, bool left);
    double linearApprox(double dist, bool left);
    double exponentialApprox(double dist, bool left);
    void collectDistData(double a, bool left);
    void avgGaze();
    bool isCollecting();
    void printCollectedData();
    int getHorizontalResponse();
    void setRealDistances(int realLeft, int realRight, Rect face);


private:
    bool collectingData;
    double realLeftToC;
    double realRightToC;

    int screenWidth;
    int screenHeight;

    double finalHorizGaze;
    float avgHorizGaze;

    double dEyeMax;
    bool rightGaze;
    double screenCenter;
    double dScreenMax;

//    double maxEyeDispAlongX;
//    double minEyeDispAlongX;
//    double eyeDeltaTotAlongX;

    double leftMinDisp;
    double leftMaxDisp;
    double leftEyeRange;
    double rightMinDisp;
    double rightMaxDisp;
    double rightEyeRange;

    double minHorizontalThreshold;
    double maxHorizontalThreshold;


    double minLeftDist;
    double maxLeftDist;

    double minRightDist;
    double maxRightDist;

    int xGazeDisplacement;

    std::vector<float> gazeSample;
    int nextGazeIndex;

    int horizontalZone;


    void evaluateGaze(Point *leftPupil, Point *rightPupil, Rect face);
    void normalizeRatio();

};

#endif // GAZEESTIMATION_H
