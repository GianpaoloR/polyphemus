#include "pupilrois.h"

pupilRois::pupilRois()
{
}


//GETTERS
cv::Mat pupilRois::getLeftROI()
{
    return leftROI;
}

cv::Mat pupilRois::getLeftROIEq()
{
    return leftROIEq;
}

cv::Mat pupilRois::getRightROI()
{
    return rightROI;
}

cv::Mat pupilRois::getRightROIEq()
{
    return rightROIEq;
}




cv::Mat pupilRois::getLeftRotatedROI()
{
    return leftRotated;
}

cv::Mat pupilRois::getRightRotatedROI()
{
    return rightRotated;
}

//SETTERS
void pupilRois::setLeftROI(cv::Mat& roi)
{
    roi.copyTo(leftROI);
}

void pupilRois::setRightROI(cv::Mat& roi)
{
    roi.copyTo(rightROI);
}

void pupilRois::setLeftRotatedROI(cv::Mat& roi)
{
    roi.copyTo(leftRotated);
}

void pupilRois::setRightRotatedROI(cv::Mat& roi)
{
    roi.copyTo(rightRotated);
}



//OTHERS
void pupilRois::equalizeROIs()
{
    equalizeHist(this->leftROI, this->leftROIEq);
    equalizeHist(this->rightROI, this->rightROIEq);
}
