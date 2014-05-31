#ifndef TWINKLE_H
#define TWINKLE_H

#include "common/cvInclusions.h"
#include "common/helpers.h"
#include "common/binaryanalyzer.h"

enum direction
{
    LEFT_DIR,
    RIGHT_DIR,
    UP_DIR,
    DOWN_DIR
};

class twinkle
{
public:
    twinkle();
    cv::Mat twinkleDilate(const cv::Mat& roi);  //Work on neighborhood to define point
    setInformation twinkleMethod(Mat mat, Point *pupil);//, setInformation *pupilSet);

private:
    void findLimbusContour(cv::Mat mat, Point* pupil);
    void moveCursor(Point p, Mat m, std::vector<Point> *l, bool debug, Mat debMat, direction dir);
    void findBarrier(Point p, Mat m, std::vector<Point> *l, bool debug, Mat debMat, direction dir);
    void handleContours(cv::Mat mat);
    std::vector<double> assignThresholdProbability(int rows, setInformation eyeSet);
    std::vector<double> assignSetsProbability(std::vector<setInformation> sets, std::vector<double> pData, int totRows);
    double computeProbability(setInformation set, double center, int totRows);
    std::vector<setInformation> filterSets(std::vector<setInformation> orig, std::vector<double> prob, std::vector<double> thresholds);
    Point moveMask(int k, Mat img, Point p, int *dirChosen);
    void tryGoingDown(Mat m, Point p, std::string wName);
    std::vector<Point> labelPoints(cv::Mat mat);
    int checkNext(int k, bool next, std::vector<bool>& checked, cv::Mat mat, int i, int j);
    setInformation getNearestSet(int startingIndex, std::vector<setInformation> sets, Point* p1, Point* p2, setInformation *pupilSet);
    Point selectPathPoint(Point* tl, Point sp, Size kSize);
    Size nextSearchArea(Point* tl, Point last, Point previous, bool left);
    std::vector<setInformation> fillSetCornerData(cv::Mat mat, std::vector<Point> corners);


    std::vector<setInformation> *sets;
    binaryAnalyzer* bin;
    Mat twinkleMat;

};

#endif // TWINKLE_H
