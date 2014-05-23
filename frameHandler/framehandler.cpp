#include "frameHandler/framehandler.h"



frameHandler::frameHandler()
{
    #ifdef WITH_GUI
    gui = NULL;
    #endif //WITH_GUI

    capture = NULL;

    #ifdef TEST_MODE
    trainingSetSize = 3;
    zoneCounter = 1;
    zoneNumber = 9;
    frameCounter = 0;
    currentImageId = 0;
    totalRead = 1;
    pupilCoords.resize(4);
    neverOpened = true;
    #endif

    //init=true;
}

#ifdef TEST_MODE
#ifdef AUTOMATIC_TEST
int frameHandler::getHZone()
{
    return ((zoneCounter-1)%3) +1;
}
#endif
#endif

#ifdef WITH_GUI
//SetDebugGui: enables gui for debugger
void frameHandler::setDebugGui(guiHandler *gui)
{
    this->gui = gui;
    return;
}
#endif //WITH_GUI

//InitCapture: initializes the frame handler, activating the webcam
bool frameHandler::initCapture()
{
    #ifdef ACTIVE_CAM
    int i=0;
    while(capture==NULL && i<MAX_TRY)
    {
        cv::waitKey(50);
        capture = cvCaptureFromCAM( -1 );       //here generate an HIGHGUI ERROR: V4L/V4L2: VIDIOC_S_CROP, but all is done fine anyhow
        i++;
    }
    if(capture == NULL) return false;
    #endif

    return true;

}

#ifdef TEST_MODE
#ifdef AUTOMATIC_TEST
std::vector<int> frameHandler::getRealCoords()
{
    return pupilCoords;
}
#endif
#endif


//Read: reads one frame from the video stream
bool frameHandler::read()
{
    #ifdef ACTIVE_CAM
    int nOldFrames = 5;
    int i = 0;
    while(i<nOldFrames)
    {
        frame = cvQueryFrame(capture);
        i++;
    }
    #ifdef DEBUG
    std::cout<<"**********************************************************"<<std::endl;
    std::cout<< "READ: cols = "<<frame.cols<<", rows = "<<frame.rows<<std::endl;
    #endif

    //externalFrame.copyTo(frame);
    if (frame.empty()) return false;
    std::cout<<"Press 'c' to exit"<<std::endl;
    char c = waitKey(30);
    if(c == 'c')
    {
        std::cout<<"\nNow exiting..."<<std::endl;
        return false;
    }
    return true;
    #endif //ACTIVE_CAM

    #ifdef TEST_MODE
    bool debugPrint = false;

    if(debugPrint) std::cout<<"**********************************************************"<<std::endl;

    //Read frames from testPath subfolders.
    char localPath[PATH_SIZE/4] = "";
    char finalPath[PATH_SIZE] = "";
    #ifdef AUTOMATIC_TEST
    char fileLine[50];
    char filePath[PATH_SIZE] = "";
    #endif //AUTOMATIC_TEST

    if(zoneCounter > zoneNumber)
    {
        std::cout<<"END OF SCREEN ZONES"<<std::endl;
        return false;
    }

    strcat(finalPath, testPath);

    sprintf(localPath, "Zona%d/", zoneCounter);
    strcat(finalPath, localPath);

    #ifdef AUTOMATIC_TEST
    //Handle coords file
    if(neverOpened)
    {
        strcpy(filePath, finalPath);
        strcat(filePath, "PupilCoords");
        std::cout<<"File path is: "<<filePath<<std::endl;

        fCoords = fopen(filePath, "r");
        if(fCoords == NULL)
        {
            std::cout<<"File not found."<<std::endl;
            return false;
            //cv::waitKey(0);
        }
        else
        {
            std::cout<<"First time a file is opened."<<std::endl;
        }

        neverOpened = false;

        for(int i=0; i<currentImageId; i++)
        {
            if(fgets(fileLine, 50, fCoords)!=NULL)
            {
                std::cout<<"LINEA DA FILE: "<<fileLine;
            }
        }
    }

    if(currentImageId == PHOTO_ID_START)
    {
        strcpy(filePath, finalPath);
        strcat(filePath, "PupilCoords");
        std::cout<<"File path is: "<<filePath<<std::endl;

        //Close old file
        if(fCoords!=NULL)
        {
            fclose(fCoords);
            fCoords = NULL;
            std::cout<<"File closed."<<std::endl;
        }

        //Open this zone file
        fCoords = fopen(filePath, "r");
        if(fCoords == NULL)
        {
            std::cout<<"File not found."<<std::endl;
            return false;
            //cv::waitKey(0);
        }
        else
        {
            std::cout<<"File opened."<<std::endl;
        }
    }
    #endif //AUTOMATIC_TEST

    switch(zoneCounter)
    {
    case 1:
        strcpy(localPath, "tl");
        break;
    case 2:
        strcpy(localPath, "tc");
        break;
    case 3:
        strcpy(localPath,"tr");
        break;
    case 4:
        strcpy(localPath, "cl");
        break;
    case 5:
        strcpy(localPath, "cc");
        break;
    case 6:
        strcpy(localPath, "cr");
        break;
    case 7:
        strcpy(localPath, "bl");
        break;
    case 8:
        strcpy(localPath, "bc");
        break;
    case 9:
        strcpy(localPath, "br");
        break;
    default:
        std::cout<<"ERROR IN TEST PATH! NOW EXITING..."<<std::endl;
        exit(-1);
        break;

    }
    strcat(finalPath, localPath);

    sprintf(localPath, "%d", currentImageId);
    strcat(localPath, ".jpg");

    strcat(finalPath, localPath);

    if(debugPrint) std::cout<<"Final path is: \""<< finalPath <<"\""<<std::endl;


    //Read frame
    this->frame = imread(finalPath, CV_LOAD_IMAGE_COLOR);
    if(!frame.data)  // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl;
        return false;
    }

    #ifdef AUTOMATIC_TEST
    //Read pupil coords
    if(fCoords!=NULL)
    {
        if(fgets(fileLine, 50, fCoords) != NULL)
        {
            std::cout<<"LINEA DA FILE: "<<fileLine;

            sscanf(fileLine, "%*s %*c %d %d %*c %d %d", &(pupilCoords[0]), &(pupilCoords[1]), &(pupilCoords[2]), &(pupilCoords[3]));
            std::cout<<"PupilCoords: L=("<<pupilCoords[0]<<","<<pupilCoords[1]<<"), R=("<<pupilCoords[2]<<","<<pupilCoords[3]<<")"<<std::endl;
        }
    }
    #endif //AUTOMATIC_TEST

    #endif //TEST_MODE

    #ifdef WEBSERVICE
    //UO
    externalFrame.copyTo(frame);

    return Server::getServer()->continueReading();
    //UO
    #endif

    return true;
}

#ifdef TEST_MODE
void frameHandler::prepareNextReading()
{
    //Prepare for next reading
    currentImageId++;
    totalRead++;
    if(currentImageId == trainingSetSize)
    {
        currentImageId = 0;
        std::cout<<"END OF ZONE "<< zoneCounter <<" TRAINING SET."<<std::endl;

        zoneCounter++;
    }
}
#endif

#ifdef TEST_MODE
int frameHandler::getTotalRead()
{
    return totalRead;
}

bool frameHandler::trainingSetFinished()
{
    if(this->zoneCounter > this->zoneNumber) return true;
    else return false;
}

#endif


//Mirror: necessary if capturing from webcam
void frameHandler::mirror()
{
    cv::flip(frame, frame, 1);

    /*cv::Mat tmp;
    cv::GaussianBlur(frame, tmp, cv::Size(5,5), 5);
    //cv::addWeighted(img, 1.5, tmp, -0.5, 0, img);
    cv::addWeighted(frame, 2.5, tmp, -1.5, 0, frame);*/

    #ifdef WITH_GUI
    if(gui!=NULL)
    {
        frame.copyTo(gui->mainFrame);
    }
    #endif //WITH_GUI

    return;
}

//SetExternalFrame: setter method, sets a frame coming from an external source
void frameHandler::setExternalFrame(cv::Mat extFrame) {
    extFrame.copyTo(externalFrame);
}

//GetExternalFrame: getter method, returns the frame coming from an external source
cv::Mat& frameHandler::getExternalFrame() {
    return externalFrame;
}

//GetFrame: getter method, returns POINTER for sake of efficiency, so pay attention
cv::Mat& frameHandler::getFrame() {
    return frame;
}


/**
 * @brief frameHandler::preProcessFrame: makes all the frame correction.
 */

void frameHandler::preProcessFrame() {
//    namedWindow("originalframe", CV_WINDOW_NORMAL);
//    imshow("originalframe", frame);
//    frame.convertTo(frame, -1, 2, 0);
//    namedWindow("convertToframe", CV_WINDOW_NORMAL);
//    imshow("convertToframe", frame);
    flatField(frame);
//    namedWindow("flatFieldframe", CV_WINDOW_NORMAL);
//    imshow("flatFieldframe", frame);
}







////showNoseRotation: call the gui to show the head rotation based on nose
//void frameHandler::showEyesRotationY(double rotation)
//{
//    if ((gui == NULL) || (rotation == NULL)) return;

//    gui->showEyesRotationY(getFace(), getLeftEye(), getRightEye(), rotation);

//    return;
//}







////showNoseRotation: call the gui to show the head rotation based on nose
//void frameHandler::showNoseRotationY(double rotation)
//{
//    if ((gui == NULL) || (rotation == NULL)) return;

//    gui->showNoseRotationY(getFace(), getNose(), rotation);

//    return;
//}




////showMouthRotation: call the gui to show the head rotation based on mouth
//void frameHandler::showMouthRotationY(double rotation)
//{
//    if ((gui == NULL) || (rotation == NULL)) return;

//    gui->showMouthRotationY(getFace(), getMouth(), rotation);

//    return;
//}







//setReducedFaceROI: makes the frameHandler set his reduced face frame
/*void frameHandler::setReducedFaceROI(cv::Mat& reducedFaceFrame)
{
    face = cv::Rect(0, 0, reducedFaceFrame.cols, reducedFaceFrame.rows);
    faceFrame = reducedFaceFrame;


    if (gui != NULL) {
        faceFrame.copyTo(gui->faceFrame);
    }

    return;

}*/


//GetAvgLight: getter method
float frameHandler::getAvgLight(bool left)
{
    if(left) return avgLightLeft;
    else return avgLightRight;
}



//getRotatedROI: getter method. ROI is returned by reference, so pay attention!
cv::Mat& frameHandler::getRotatedROI(bool left)
{
    if(left) return leftEmpiricRotatedFrame;
    else     return rightEmpiricRotatedFrame;
}



//Release: deactivates the webcam and frees cv::Mats
void frameHandler::release()
{
#ifndef WEBSERVICE
    cvReleaseCapture( &capture );
#endif

    frame.release();

    return;
}


/* HIGHILIGHTS THE SKIN */
void frameHandler::highlightSkin(cv::Mat frameHighlightSkin) {
    cv::Mat im_ycrcb, skin_ycrcb;
    cv::cvtColor(frameHighlightSkin, im_ycrcb, cv::COLOR_BGR2YCrCb);

//    cv::Mat sat;
//    frameHighlightSkin.copyTo(sat);
//    for (int i = 0; i < sat.rows; i++) {
//        for (int j = 0; j < sat.cols; j++) {
//            //            sat.at<double>(i, j, 0) = 255;
//            //            sat.at<double>(i, j, 1) = 255;
//            //            sat.at<double>(i, j, 2) = 255;
//            //            std::cout << sat.at<int>(i, j, 0) << std::endl;
//            for (int z = 0; z < 3; z++) {
//                sat.at<cv::Vec3b>(i, j)[z] = 255;
//            }
//            //            std::cout << (int)sat.at<cv::Vec3b>(i, j)[0] << std::endl;
//            //            break;
//        }
//    }
//    cv::namedWindow("sat", CV_WINDOW_NORMAL);
//    imshow("sat", sat);

    //    std::vector<cv::Mat> planes;
    //    cv::split(im_ycrcb, planes);
    //    cv::namedWindow("planes[0]", CV_WINDOW_NORMAL);
    //    imshow("planes[0]", planes[0]);
    //    cv::namedWindow("planes[1]", CV_WINDOW_NORMAL);
    //    imshow("planes[1]", planes[1]);
    //    cv::namedWindow("planes[2]", CV_WINDOW_NORMAL);
    //    imshow("planes[2]", planes[2]);

    cv::Scalar skin_ycrcb_mint = cv::Scalar(0, 133, 77);
    cv::Scalar skin_ycrcb_maxt = cv::Scalar(255, 173, 127);
    cv::inRange(im_ycrcb, skin_ycrcb_mint, skin_ycrcb_maxt, skin_ycrcb);

    cv::namedWindow("skin_ycrcb", CV_WINDOW_NORMAL);
    imshow("skin_ycrcb", skin_ycrcb);
    //cv2.imwrite(sys.argv[2], skin_ycrcb) # Second image

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(skin_ycrcb, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat frameWithContours;
    frameHighlightSkin.copyTo(frameWithContours);
    for (uint i = 0; i< contours.size(); i++) {
        double area = cv::contourArea(contours.at(i));
        if (area > 1000)
            cv::drawContours(frameWithContours, contours, i, Scalar(255, 0, 0), 3);
    }
    cv::namedWindow("Highlight skin", CV_WINDOW_NORMAL);
    imshow("Highlight skin", frameWithContours);
}

#ifdef TEST_MODE
void frameHandler::setTestPath(std::string path)
{
    assert(path.size() < PATH_SIZE);
    path.copy(testPath, path.size(), 0);
    std::cout<<"Main test path is: \""<<path<<"\""<<std::endl;
    return;
}
#endif

//------------------------------------------------------------------------------
