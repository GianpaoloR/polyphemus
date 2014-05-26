#include "haaranalyzer.h"
//#include <limits>

#include <queue>
#include <iostream>
#include <limits>
#include <string>
//#include <math.h>

#include "constants.h"
#include "helpers.h"

#define TOLERANCE 1


haarAnalyzer::haarAnalyzer()
{
#ifdef WITH_GUI
    gui = NULL;
#endif //WITH_GUI
}

#ifdef WITH_GUI
//SetDebugGui: enables gui for debugger
void haarAnalyzer::setDebugGui(guiHandler *gui)
{
    this->gui = gui;
    return;
}
#endif //WITH_GUI

//LoadCascade: initializes cascade classifiers
bool haarAnalyzer::loadCascade(cascadeType type, std::string path)
{
    bool ok = false;
    bool debugPrint = false;

    if(debugPrint) cout<<"Trying to load cascade at " + path <<"...";

    // Load the cascade
    switch(type)
    {
    case FACE:
        if(face_cascade.load( path ) ) ok = true;
        break;
    case EYEPAIR:
        if(eyePair_cascade.load( path ) ) ok = true;
        break;
    case SINGLEEYE:
        if(singleEye_cascade.load( path ) ) ok = true;
        break;
    case LEFTEYE:
        if(leftEye_cascade.load( path ) ) ok = true;
        break;
    case RIGHTEYE:
        if(rightEye_cascade.load( path ) ) ok = true;
        break;
    case NOSE:
        if(nose_cascade.load( path ) ) ok = true;
        break;
    case MOUTH:
        if(mouth_cascade.load( path ) ) ok = true;
        break;
    case UPPERBODY:
        if(upper_body_cascade.load ( path ) ) ok = true;
        break;
    default:
        break;
    }

#ifndef WEBSERVICE
    if(!ok)
    {
        if(debugPrint) std::cout<<"(!)Error loading cascade\n";
    }
    else
    {
        if(debugPrint) cout<<" done."<<endl;
    }
#endif

    return ok;
}




/******************
 *      TODO      *
 ******************/



//GetFaces: getter method
std::vector<cv::Rect> haarAnalyzer::getFaces()
{
    return faces;
}

//hasDetectFace: looks for faces and returns true if it find one face.
bool haarAnalyzer::hasDetectFace(cv::Mat& img) {
    std::vector<cv::Rect> faces;
    face_cascade.detectMultiScale(img, faces,
                                  1.1,
                                  2,
                                  //0,
                                  0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT,
                                  //0,
                                  //cv::Size(150, 150));
                                  cv::Size(0.2 * img.cols, 0.2 * img.rows));
    if (faces.size() > 0)
        return true;
    else
        return false;
}

//GetUpperBodies: getter method
std::vector<cv::Rect> haarAnalyzer::getUpperBodies()
{
    return upper_bodies;
}


//hasDetectFaceWithSize: looks for faces (using a maximum/minimum size) and returns true if it find one face.
bool haarAnalyzer::hasDetectFaceWithSize(cv::Mat& img, cv::Size& roundSize) {
    std::vector<cv::Rect> faces;
    cv::Size maximumSize, minimumSize;
    if ((roundSize.width == 0) && (roundSize.height == 0)) {
        maximumSize = cv::Size(img.cols, img.rows);
        minimumSize = cv::Size(0.2 * img.cols, 0.2 * img.rows);
    }
    else {
        int width10Perc = (roundSize.width / 100 * 10);
        int height10Perc = (roundSize.height / 100 * 10);
        maximumSize = cv::Size(roundSize.width + width10Perc, roundSize.height + height10Perc);
        minimumSize = cv::Size(roundSize.width - width10Perc, roundSize.height - height10Perc);
    }
    face_cascade.detectMultiScale(img, faces,
                                  1.1,
                                  2,
                                  //0,
                                  0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT,
                                  //0,
                                  //cv::Size(150, 150));
                                  //cv::Size(0.2 * img.cols, 0.2 * img.rows)
                                  minimumSize, maximumSize
                                  );
    if (faces.size() > 0) {
        if ((roundSize.width == 0) && (roundSize.height == 0)) {
            roundSize = cv::Size(faces[0].width, faces[0].height);
            std::cout << "roundSize: " << roundSize.height << " " << roundSize.width << std::endl;
        }
        return true;
    }
    else
        return false;
}

//DetectFaces: looks for faces and saves data into haarAnalyzer::faces vector.
void haarAnalyzer::detectFaces(cv::Mat& img)
{
#ifdef DETECTFACES_DEBUG
    std::cout<<"DETECTFACES: cols = "<<img.cols<<", rows = "<<img.rows<<std::endl;
#endif
    face_cascade.detectMultiScale( img, faces,
                                   1.1,
                                   2,
                                   //0,
                                   0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT,
                                   //0,
                                   //cv::Size(150, 150));
                                   cv::Size(0.2 * img.cols, 0.2 * img.rows));

#ifdef WITH_GUI
    if(gui!=NULL)
    {
        gui->showFaces(faces);
    }
#endif //WITH_GUI

    return;
}

//DetectFaces: looks for faces and saves data into haarAnalyzer::faces vector.
void haarAnalyzer::detectUpperBody(cv::Mat& img)
{
#ifdef DETECTUPPERBODY_DEBUG
    std::cout<<"DETECT_UPPER_BODY: cols = "<<img.cols<<", rows = "<<img.rows<<std::endl;
#endif
    upper_body_cascade.detectMultiScale( img, upper_bodies,
                                   1.1,
                                   2,
                                   //0,
                                   0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT,
                                   //0,
                                   //cv::Size(150, 150));
                                   cv::Size(0.35 * img.cols, 0.35 * img.rows));

#ifdef WITH_GUI
    if(gui!=NULL)
    {
        gui->showUpperBody(upper_bodies);
    }
#endif //WITH_GUI

    return;
}

//redetectFace: looks for faces and saves data into haarAnalyzer::faces vector.
bool haarAnalyzer::redetectFace(cv::Mat& img)
{
    std::vector<cv::Rect> newFaces;
    face_cascade.detectMultiScale( img, newFaces,
                                   1.1,
                                   2,
                                   0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT,
                                   //cv::Size(150, 150));
                                   cv::Size(0.2 * img.cols, 0.2 * img.rows));

    std::cout<<"Inside Faces size: "<<newFaces.size()<<std::endl;
    /*if(gui!=NULL)
    {
        gui->showFaces(newFaces);
    }*/
    if (newFaces.size() == 1)
        return true;
    else
        return false;
}


