#include "pupilgui.h"

pupilGui::pupilGui()
{
    initFrame("Left Eye");
    initFrame("Right Eye");
}

//Initializers
void pupilGui::initFrame(std::string name)
{
    namedWindow(name, CV_WINDOW_NORMAL);
}


//Frame setters
void pupilGui::setLeftFrame(Mat roi)
{
    roi.copyTo(leftFrame);
}

void pupilGui::setRightFrame(Mat roi)
{
    roi.copyTo(rightFrame);
}

void pupilGui::setLeftRotatedFrame(cv::Mat& roi)
{
    roi.copyTo(leftRotatedFrame);
}

void pupilGui::setRightRotatedFrame(cv::Mat& roi)
{
    roi.copyTo(rightRotatedFrame);
}

void pupilGui::setFilterFrame(cv::Mat& roi)
{
    roi.copyTo(filterFrame);
}

void pupilGui::setLeftFilteredFrame(cv::Mat& roi)
{
    roi.copyTo(leftFilteredFrame);
}

void pupilGui::setRightFilteredFrame(cv::Mat& roi)
{
    roi.copyTo(rightFilteredFrame);
}


//Drawers
void pupilGui::showRoi(const cv::Mat& roi, std::string name)
{
    //std::cout<<"SHOWROI: entered"<<std::endl;
    namedWindow(name, CV_WINDOW_NORMAL);
    imshow(name, roi);
    waitKey(0);
}

void pupilGui::showLeft()
{
    imshow("Left Eye", leftFrame);
}

void pupilGui::showRight()
{
    imshow("Right Eye", rightFrame);
}

void pupilGui::showFilter()
{
    imshow("Filter", filterFrame);
}

void pupilGui::showLeftFiltered()
{
    imshow("Left filtered", leftFilteredFrame);
}

void pupilGui::showRightFiltered()
{
    imshow("Right filtered", rightFilteredFrame);
}

void pupilGui::showPupils(Point *left, Point *right, bool refined)
{
    if(left!=NULL)
    {
        if(refined)
        {
            leftPupilRefined = *left;
            circle(leftFrame, leftPupilRefined, leftFrame.rows/15, 200);
        }
        else
        {
            leftPupil = *left;
            circle(leftFrame, leftPupil, leftFrame.rows/10, 255);
        }
    }
    if(right != NULL)
    {
        if(refined)
        {
            rightPupilRefined = *right;
            circle(rightFrame, rightPupilRefined, rightFrame.rows/15, 200);
        }
        else
        {
            rightPupil = *right;
            circle(rightFrame, rightPupil, rightFrame.rows/10, 255);
        }
    }
}


void pupilGui::showEyesRotated(cv::Point l, cv::Point r)
{
    if(l.x>=0 && l.x<leftRotatedFrame.cols && l.y>=0 && l.y<leftRotatedFrame.rows)
    {
        circle(leftRotatedFrame, l, 1, 200);
    }
    if(r.x>=0 && r.x<rightRotatedFrame.cols && r.y>=0 && r.y<rightRotatedFrame.rows)
    {
        circle(rightRotatedFrame, r, 1, 200);
    }
    // OK, new coordinates are good. TESTED.
}

void pupilGui::showTwinklePath(cv::Point point, bool left, bool both)
{
    cv::Mat roi;
    if(left)
    {
        if(both)
        {
            roi = leftRotatedFrame;
        }
        else
        {
            roi = leftFrame;
        }
    }
    else
    {
        if(both)
        {
            roi = rightRotatedFrame;
        }
        else
        {
            roi = rightFrame;
        }
    }
    if(point.x>=0 && point.x<roi.cols && point.y>=0 && point.y<roi.rows)
    {
        circle(roi, point, 0, 200);
    }
}

void pupilGui::showCorners(std::vector<cv::Point> cornerVector, bool left, bool both)
{
    cv::Mat roi;
    if(left)
    {
        if(both)
        {
            roi = leftRotatedFrame;
        }
        else
        {
            roi = leftFrame;
        }
    }
    else
    {
        if(both)
        {
            roi = rightRotatedFrame;
        }
        else
        {
            roi = rightFrame;
        }
    }
    if(cornerVector[0].x>=0 && cornerVector[0].x<roi.cols && cornerVector[0].y>=0 && cornerVector[0].y<roi.rows)
    {
        circle(roi, cornerVector[0], 1, 200);
    }
    if(cornerVector[1].x>=0 && cornerVector[1].x<roi.cols && cornerVector[1].y>=0 && cornerVector[1].y<roi.rows)
    {
        circle(roi, cornerVector[1], 1, 200);
    }
}



/** showPupilsLine: draw a line connecting pupils in faceFrame.
*/
void pupilGui::showPupilsLine(cv::Point leftPupil, cv::Point rightPupil)
{
    line(faceFrame, leftPupil, rightPupil, 200);
}
