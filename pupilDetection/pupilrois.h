#ifndef PUPILROIS_H
#define PUPILROIS_H

#include "common/cvInclusions.h"

class pupilRois
{
public:
    pupilRois();

    //Setters
    void setLeftROI(cv::Mat& roi);
    void setRightROI(cv::Mat& roi);
    void setLeftRotatedROI(cv::Mat& roi);
    void setRightRotatedROI(cv::Mat& roi);
    void setFilter(Mat &roi);
    void setLeftFilteredROI(cv::Mat& roi);
    void setRightFilteredROI(cv::Mat& roi);



    //Getters
    cv::Mat getLeftROI();
    cv::Mat getRightROI();
    cv::Mat getLeftROIEq();
    cv::Mat getRightROIEq();
    cv::Mat getLeftRotatedROI();
    cv::Mat getRightRotatedROI();

    //Others
    void equalizeROIs();

private:
    cv::Mat leftROI;
    cv::Mat rightROI;
    cv::Mat leftROIEq;
    cv::Mat rightROIEq;
    cv::Mat leftRotated;
    cv::Mat rightRotated;

};

#endif // PUPILROIS_H
