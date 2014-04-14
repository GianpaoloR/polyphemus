#ifndef PUPILGUI_H
#define PUPILGUI_H

#include "common/cvInclusions.h"
#include "common/helpers.h"

class pupilGui
{
public:
    pupilGui();

    //Initializers
    void initFrame(std::string name);

    //Setters
    void setLeftFrame(Mat roi);
    void setRightFrame(cv::Mat roi);
    void setLeftRotatedFrame(cv::Mat& roi);
    void setRightRotatedFrame(cv::Mat& roi);
    void setFilterFrame(cv::Mat& roi);
    void setLeftFilteredFrame(cv::Mat& roi);
    void setRightFilteredFrame(cv::Mat& roi);

    //Drawers
    void showRoi(const cv::Mat& roi, std::string name);
    void showCorners(std::vector<cv::Point> cornerVector, bool left, bool both);
    void showPupilsLine(Point leftPupil, Point rightPupil);
    void showEyesRotated(cv::Point l, cv::Point r);
    void showTwinklePath(cv::Point point, bool left, bool both);
    void showLeft();
    void showRight();
    void showFilter();
    void showLeftFiltered();
    void showRightFiltered();
    void showPupils(Point *left, Point *right, bool refined);



private:
    cv::Mat faceFrame;
    cv::Mat leftFrame;
    cv::Mat rightFrame;
    cv::Mat leftRotatedFrame;
    cv::Mat rightRotatedFrame;
    cv::Mat filterFrame;
    cv::Mat leftFilteredFrame;
    cv::Mat rightFilteredFrame;

    cv::Point leftPupil;
    cv::Point leftPupilRefined;
    cv::Point leftPupilFiltered;
    cv::Point leftPupilFilteredRefined;
    cv::Point rightPupil;
    cv::Point rightPupilRefined;
    cv::Point rightPupilFiltered;
    cv::Point rightPupilFilteredRefined;
};

#endif // PUPILGUI_H
