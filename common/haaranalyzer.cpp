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


//getEyePairs: getter method
std::vector<cv::Rect> haarAnalyzer::getEyePairs() {
    return eyePairs;
}

//getSingleEyes: getter method
std::vector<cv::Rect> haarAnalyzer::getSingleEyes() {
    return singleEyes;
}

//getLeftEyes: getter method
std::vector<cv::Rect> haarAnalyzer::getLeftEyes() {
    return leftEyes;
}

//getRightEyes: getter method
std::vector<cv::Rect> haarAnalyzer::getRightEyes() {
    return rightEyes;
}

//detectEyePairs: looks for eyes region and saves data into haarAnalyzer::eyePairs vector.
void haarAnalyzer::detectEyePairs(cv::Mat& img, cv::Rect& face) {
    cv::Mat subImg = img(cv::Rect(0, 0, face.width, face.height * 0.5));

    eyePair_cascade.detectMultiScale(subImg, eyePairs, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(0.1 * face.width, 0.05 * face.height));

    #ifdef WITH_GUI
    if (gui != NULL) {
        gui->showFaceElements(eyePairs, face);
    }
    #endif //WITH_GUI

    return;
}

/******************
 *      TODO      *
 ******************/
//detectSingleEyes: looks for eyes and saves data into haarAnalyzer::singleEyes vector.
void haarAnalyzer::detectSingleEyes(cv::Mat& img, cv::Rect& face) {
    int eyeWidth = face.width * (kEyePercentWidth/100.0);
    int eyeHeight = face.width * (kEyePercentHeight/100.0);
    int eyeTop = face.height * (kEyePercentTop/100.0);

    std::vector<cv::Rect> eyeZone;
    //Rect constructor: Rect(topleft.x, topleft.y, width, height)
    eyeZone.push_back(cv::Rect(face.width*(kEyePercentSide/100.0),
                               eyeTop,
                               eyeWidth,   //width
                               eyeHeight)); //height


    eyeZone.push_back(cv::Rect(face.width - eyeWidth - face.width*(kEyePercentSide/100.0),
                               eyeTop,   //suppose horizontally aligned head!!!
                               eyeWidth,
                               eyeHeight));

    //cv::Mat subImg = img(cv::Rect(face.width*(kEyePercentSide/100.0), eyeTop, face.width - 2*face.width*(kEyePercentSide/100.0), eyeHeight));
    cv::Mat subImg = img(cv::Rect(0, 0, face.width, face.height/2));
    //singleEye_cascade.detectMultiScale( subImg, singleEyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
    singleEye_cascade.detectMultiScale( subImg, singleEyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(0.1 * face.width, 0.05 * face.height) );

    //UO
    for( uint i = 0; i < singleEyes.size(); i++ )  //per ogni faccia
    {
        //singleEyes[i].x += face.width*(kEyePercentSide/100.0);
        singleEyes[i].x += 0;
        singleEyes[i].y += 0;
    }

    #ifdef WITH_GUI
    if(gui!=NULL)
    {
        gui->showFaceElements(singleEyes, face);
    }
    #endif //WITH_GUI

    return;
}

//detectLeftEyes: looks for left eyes and saves data into haarAnalyzer::leftEyes vector.
void haarAnalyzer::detectLeftEyes(cv::Mat& img, cv::Rect& face) {
    cv::Mat subImg = img(cv::Rect(0, face.height * 0.1, face.width * 0.66, face.height * 0.5));
    //leftEye_cascade.detectMultiScale(subImg, leftEyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(30, 30));
    leftEye_cascade.detectMultiScale(subImg, leftEyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(0.1 * face.width, 0.05 * face.height));

    for (uint i = 0; i < leftEyes.size(); i++) {
        leftEyes[i].y += face.height*0.1;
    }

    #ifdef WITH_GUI
    if (gui != NULL) {
        gui->showFaceElements(leftEyes, face);
    }
    #endif //WITH_GUI

    return;
}

//detectRightEyes: looks for right eyes and saves data into haarAnalyzer::rightEyes vector.
void haarAnalyzer::detectRightEyes(cv::Mat& img, cv::Rect& face) {
    int left = (int)face.width * 0.33;
    cv::Mat subImg = img(cv::Rect(left, face.height * 0.1, face.width - left, face.height * 0.5));
    //rightEye_cascade.detectMultiScale(subImg, rightEyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(30, 30));
    rightEye_cascade.detectMultiScale(subImg, rightEyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(0.1 * face.width, 0.05 * face.height));

    for (uint i = 0; i < rightEyes.size(); i++) {
        rightEyes[i].y += face.height * 0.1;
        rightEyes[i].x += left;
    }

    #ifdef WITH_GUI
    if (gui != NULL) {
        gui->showFaceElements(rightEyes, face);
    }
    #endif //WITH_GUI

    return;
}

//getNoses: getter method
std::vector<cv::Rect> haarAnalyzer::getNoses()
{
    return noses;
}

//detectNoses: looks for noses and saves data into haarAnalyzer::noses vector.
void haarAnalyzer::detectNoses(cv::Mat& img, cv::Rect& face)
{
    int cutImageY = (int)(img.rows/4);
    int cutImageX = (int)(img.cols/5);
    cv::Mat subImg = img(cv::Rect(cutImageX, cutImageY, img.cols - cutImageX, img.rows - cutImageY));
    //nose_cascade.detectMultiScale( subImg, noses, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(30, 30) );
    nose_cascade.detectMultiScale( subImg, noses, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(0.1 * face.width, 0.1 * face.height) );

    for( uint i = 0; i < noses.size(); i++ )  //per ogni faccia
    {
        noses[i].y += cutImageY;
        noses[i].x += cutImageX;
    }

#ifdef WITH_GUI
    if(gui!=NULL)
    {
        gui->showFaceElements(noses, face);
    }
#endif //WITH_GUI

    return;
}

//GetFaces: getter method
std::vector<cv::Rect> haarAnalyzer::getMouths()
{
    return mouths;
}

//detectMouths: looks for faces and saves data into haarAnalyzer::faces vector.
void haarAnalyzer::detectMouths(cv::Mat& img, cv::Rect& face)
{
    int cutImageY = (int)(img.rows/2);
    int cutImageX = (int)(img.cols/5);
    cv::Mat subImg = img(cv::Rect(cutImageX, cutImageY, img.cols - cutImageX, img.rows - cutImageY));
    mouth_cascade.detectMultiScale( subImg, mouths, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(30, 30) );

    //UO
    for( uint i = 0; i < mouths.size(); i++ )  //per ogni faccia
    {
        mouths[i].y += cutImageY;
        mouths[i].x += cutImageX;
    }

#ifdef WITH_GUI
    if(gui!=NULL)
    {
        gui->showFaceElements(mouths, face);
    }
#endif //WITH_GUI

    return;
}



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

/*******************************************
 *   TODO!!! GETEYEZONES & DETECTSINGLEEYES
 ******************************************/
//GetEyeZones: getter method
//std::vector<cv::Rect> haarAnalyzer::getEyeZones()
//{
//    return eyeZone;
//}


//--------------------------------------------------
