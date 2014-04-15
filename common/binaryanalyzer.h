#ifndef BINARYANALYZER_H
#define BINARYANALYZER_H


#include <iostream>
#include <iomanip>
#include "common/cvInclusions.h"
#include "frameHandler/guihandler.h"
#include "common/helpers.h"

#define NOISE_SIZE 0
#define ERVAL 150
#define DILVAL 100
#define NOT_VALID_ID -1

class binaryAnalyzer
{
public:


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
    void fillHole(Mat &roi, Point start, int *opening, int* minY, int* maxY);
    void fillHoleIter(cv::Mat& roi, cv::Point start);

    //Sets analysis
    void clearOverallSets();
    std::vector<setInformation> findSets(const cv::Mat &, bool timeIntegration);
    std::vector<setInformation> glueSets(std::vector<setInformation> sets,int glueDist);
    std::vector<setInformation> computeSetSize(std::vector<setInformation> sets);
    std::vector<setInformation> removeBorderSets(std::vector<setInformation> sets, int r, int c);
    setInformation getBiggestSet(std::vector<setInformation> sets, int *index);

    //Utilities
    void resetId();
    std::vector<setInformation> getOverallSets();

    //---------------------------------


private:

    void findNeighbors(cv::Point p);
    void setHistoryUpdate();
    void updateSetsOverall();
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
