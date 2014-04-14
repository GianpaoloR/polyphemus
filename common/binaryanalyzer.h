#ifndef BINARYANALYZER_H
#define BINARYANALYZER_H


#include <iostream>
#include <iomanip>
#include "common/cvInclusions.h"
#include "frameHandler/guihandler.h"

#define NOISE_SIZE 0
#define ERVAL 150
#define DILVAL 100
#define NOT_VALID_ID -1

class binaryAnalyzer
{
public:
    struct setInformation
    {
        int id;
        int nPoints;
        int width;
        int height;
        cv::Point center;
        cv::Point centroid;
        std::vector<cv::Point> elem;
        std::vector<int> sons;
        std::vector<Point> corners;
    };

    enum direction
    {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    enum filterMode
    {
        AVG,
        MAX
    };


    binaryAnalyzer();
    void setDebugGui(guiHandler *gui);
    cv::Mat newThresholded(int r, int c);
    cv::Mat thresholdImg(const cv::Mat &roi, int tVal);
    cv::Mat invert(const cv::Mat& roi);

    //Morphology
    cv::Mat close(const cv::Mat& roi);
    cv::Mat dilate(const cv::Mat& roi);
    cv::Mat erode(const cv::Mat& roi);
    cv::Mat open(const cv::Mat& roi);
    cv::Mat twinkleDilate(const cv::Mat& roi);  //Work on neighborhood to define point
    void fillHole(Mat &roi, Point start, int *opening, int* minY, int* maxY);
    void fillHoleIter(cv::Mat& roi, cv::Point start);

    //Sets analysis
    void clearOverallSets();
    std::vector<setInformation> findSets(const cv::Mat &, bool timeIntegration);
    std::vector<setInformation> glueSets(std::vector<setInformation> sets,int glueDist);
    std::vector<setInformation> computeSetSize(std::vector<setInformation> sets);
    std::vector<setInformation> removeBorderSets(std::vector<setInformation> sets, int r, int c);
    void handleContours(cv::Mat mat);
    std::vector<Point> labelPoints(cv::Mat mat);
    std::vector<setInformation> fillSetCornerData(cv::Mat mat, std::vector<Point> corners);
    setInformation getBiggestSet(std::vector<setInformation> sets, int *index);
    setInformation getNearestSet(int startingIndex, std::vector<setInformation> sets, Point* p1, Point* p2, setInformation *pupilSet);
    std::vector<double> assignThresholdProbability(int rows, setInformation eyeSet);
    std::vector<double> assignSetsProbability(std::vector<setInformation> sets, std::vector<double> pData, int totRows);
    double computeProbability(setInformation set, double center, int totRows);
    std::vector<setInformation> filterSets(std::vector<setInformation> orig, std::vector<double> prob, std::vector<double> thresholds);

    //Path finders
    setInformation twinkle(cv::Mat mat, Point *pupil, setInformation *pupilSet);
    Point moveMask(int k, Mat img, Point p, int *dirChosen);

    //Utilities
    void resetId();
    std::vector<setInformation> getOverallSets();
    void findLimbusContour(cv::Mat mat, Point* pupil);
    void moveCursor(Point p, Mat m, std::vector<Point> *l, bool debug, Mat debMat, direction dir);
    void findBarrier(Point p, Mat m, std::vector<Point> *l, bool debug, Mat debMat, direction dir);


    //---------------------------------


private:

    void tryGoingDown(Mat m, Point p, std::string wName);

    void findNeighbors(cv::Point p);
    void setHistoryUpdate();
    void updateSetsOverall();
    Point selectPathPoint(Point* tl, Point sp, Size kSize);
    Mat twinkleMat;
    Size nextSearchArea(Point* tl, Point last, Point previous, bool left);
    //--------------------------------------------------
    guiHandler* gui;
    std::vector<cv::Mat> frames;
    int mat_id;
    int setId, lastFatherId;
    cv::Mat img1, img2;
    cv::Mat *lastImg, *previousImg;
    cv::Mat assigned;
    setInformation* lastSet;
    std::vector<setInformation> sets1, sets2, overallSets;
    std::vector<setInformation> *sets, *previousSets;
    std::vector<int> born, alive, dead;
    bool oneIsLast;


};

#endif // BINARYANALYZER_H
