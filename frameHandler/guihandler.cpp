 #include "guihandler.h"

guiHandler::guiHandler(int w, int h)
{
    //getDesktop();

    mainWindowName = "Frame";
    faceWindowName = "Face";
    lEyeReducedWindowName = "Left Eye ROI";
    rEyeReducedWindowName = "Right Eye ROI";
    lRotatedWindowName = "Left Eye Rotated";
    rRotatedWindowName = "Right Eye Rotated";
    gazeWindowName = "Gaze Tracker";
    profileWindowName = "Profiling video...";

    faceReceived = false;
    eyesReceived = false;

    hRes = w;
    vRes = h;
}

void guiHandler::setGazeFromHeadData(double rotation, cv::Rect face, int difference_y)
{
    //cv::Point obs = evaluatePosition(yAngle, face, mainFrame);
    int x=0,y=0; //Coordinate punto osservato
    double distance_head; //Scostamento punto osservato posizione testa
    double eye_position, eye_position_y; //Proiezione occhi sullo schermo
    //rotation:180=x:pi -> 180*x = pi*rotation ->
    rotation = rotation*M_PI/180;
    //Calcoliamo con il teorema dei seni la coordinata x
    distance_head = DISTANZA_MEDIA_SCHERMO * sin(rotation) / sin((M_PI/2) - rotation);
    distance_head = -distance_head * CM_TO_PIXEL; //1cm -> 37.795275591 pixel
    // x:hRes=position_face:frame.cols
    eye_position = hRes*(face.x+(face.width/2))/mainFrame.cols;
    eye_position_y = vRes*(face.y+(face.height/2))/mainFrame.rows;
    //Calcolare coord x;
    x=eye_position + distance_head;
    y=eye_position_y +difference_y;
    if (x<0)
    {
        x=0;
    }
    else if (x > hRes)
    {
        x = hRes;
    }
    globalGaze.x = x;
    globalGaze.y = y;
    return;
}
void guiHandler::setSquareFromGazeData()
{
    if(globalGaze.x <= hRes/3)
    {
        if (globalGaze.y <= vRes/3)
        {
            square = cv::Point(0,0);
        }
        else if (globalGaze.y <= vRes*2/3)
        {
            square=cv::Point(0,vRes/3);
        }
        else
        {
            square=cv::Point(0,vRes*2/3);
        }
    }
    else if (globalGaze.x <= hRes*2/3)
    {
        if (globalGaze.y <= vRes/3)
        {
            square=cv::Point(hRes/3,0);
        }
        else if (globalGaze.y <= vRes*2/3)
        {
            square=cv::Point(hRes/3,vRes/3);
        }
        else
        {
            square=cv::Point(hRes/3,vRes*2/3);
        }
    }
    else
    {
        if (globalGaze.y <= vRes/3)
        {
            square=cv::Point(hRes*2/3,0);
        }
        else if (globalGaze.y <= vRes*2/3)
        {
            square=cv::Point(hRes*2/3,vRes/3);
        }
        else
        {
            square=cv::Point(hRes*2/3,vRes*2/3);
        }
    }
}

/* NEVER USED!!!
 *
void guiHandler::testSquare()
{
#ifndef WEBSERVICE
    cv::Mat schermo = cv::Mat(vRes, hRes, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::line(schermo, cv::Point(0,vRes/3), cv::Point(hRes,vRes/3), cv::Scalar(0,255,255), 3, 1, 0);
    cv::line(schermo, cv::Point(0,vRes*2/3), cv::Point(hRes,vRes*2/3), cv::Scalar(0,255,255), 3, 1, 0);
    cv::line(schermo, cv::Point(hRes/3, 0), cv::Point(hRes/3, vRes), cv::Scalar(0,255,255), 3, 1, 0);
    cv::line(schermo, cv::Point(hRes*2/3, 0), cv::Point(hRes*2/3, vRes), cv::Scalar(0,255,255), 3, 1, 0);
    cv::rectangle(schermo,cv::Rect(square.x, square.y, hRes/3, vRes/3),cv::Scalar(255,0,0),3,1,0);
    cv::namedWindow("TEST", CV_WINDOW_NORMAL);
    imshow("TEST", schermo);
#endif
}
*/





//InitWindow: creates cv windows
void guiHandler::initWindow(WINDOW_TYPE wt)
{
    switch(wt)
    {
    case MAIN_WINDOW:
        if (face_test)
        {
            cv::namedWindow(mainWindowName,CV_WINDOW_NORMAL);
        }
        break;
    case FACE_WINDOW:
        if (eye_test)
        {
            cv::namedWindow(faceWindowName,CV_WINDOW_NORMAL);
        }
        break;
    case LEFT_REDUCED:
        if (eye_test)
            cv::namedWindow(lEyeReducedWindowName,CV_WINDOW_NORMAL);
        break;
    case RIGHT_REDUCED:
        if (eye_test)
            cv::namedWindow(rEyeReducedWindowName,CV_WINDOW_NORMAL);
        break;
    case LEFT_ROTATED:
        if (eye_test)
            cv::namedWindow(lRotatedWindowName, CV_WINDOW_NORMAL);
        break;
    case RIGHT_ROTATED:
        if (eye_test)
            cv::namedWindow(rRotatedWindowName, CV_WINDOW_NORMAL);
        break;
    case GAZE_WINDOW:
        if (eye_test)
            cv::namedWindow(gazeWindowName, CV_WINDOW_NORMAL);
        break;
    case PROFILE:
        if (eye_test)
            cv::namedWindow(profileWindowName, CV_WINDOW_AUTOSIZE);
        break;
    }


    return;
}

//UpdateWindow: refresh windows content
void guiHandler::updateWindow(WINDOW_TYPE wt)
{
    switch (wt)
    {
    case MAIN_WINDOW:
        if (face_test)
        {
            imshow(mainWindowName, mainFrame);
        }
        break;
    case FACE_WINDOW:
        if(faceReceived)
        {
            if (eye_test)
            {
                imshow(faceWindowName, faceFrame);
            }
            faceReceived=false;
        }
        break;
    case LEFT_REDUCED:
        if (eye_test)
        {
            imshow(lEyeReducedWindowName, leftFrame);
        }
        break;
    case RIGHT_REDUCED:
        if (eye_test)
        {
            imshow(rEyeReducedWindowName, rightFrame);
        }
        break;
    case LEFT_ROTATED:
        if (eye_test)
        {
            imshow(lRotatedWindowName, leftFrameRotated);
        }
        break;
    case RIGHT_ROTATED:
        if (eye_test)
        {
            imshow(rRotatedWindowName, rightFrameRotated);
        }
        break;
    case GAZE_WINDOW:
        if (eye_test)
        {
            imshow(gazeWindowName, gazeFrame);
        }
        break;
    case PROFILE:
        if (eye_test)
        {
            imshow(profileWindowName, mainFrame);
        }
        break;
    default:
        break;
    }
    return;
}

//DestroyWindow: releases cv windows
void guiHandler::destroy(WINDOW_TYPE wt)
{
    if (face_test || eye_test)//TODO piu' corretto separare i casi
    {
        switch(wt)
        {
        case MAIN_WINDOW:
            cv::destroyWindow(mainWindowName);
            break;
        case FACE_WINDOW:
            cv::destroyWindow(faceWindowName);
            break;
        case LEFT_REDUCED:
            cv::destroyWindow(lEyeReducedWindowName);
            break;
        case RIGHT_REDUCED:
            cv::destroyWindow(rEyeReducedWindowName);
            break;
        case LEFT_ROTATED:
            cv::destroyWindow(lRotatedWindowName);
            break;
        case RIGHT_ROTATED:
            cv::destroyWindow(rRotatedWindowName);
            break;
        case GAZE_WINDOW:
            cv::destroyWindow(gazeWindowName);
            break;
        case PROFILE:
            cv::destroyWindow(profileWindowName);
            break;
        }
        return;

    }
}

//showFaceElements: draws an ellipse around single eyes in MAIN_WINDOW
void guiHandler::showFaceElements(std::vector<cv::Rect> elements, cv::Rect& face) {
    for (uint i = 0; i < elements.size(); i++) {
        //find center
        cv::Point center(face.x + elements[i].x + elements[i].width*0.5, face.y + elements[i].y + elements[i].height * 0.5);
        //design an ellipse around it
        ellipse(mainFrame, center, cv::Size(elements[i].width * 0.5, elements[i].height * 0.5), 0, 0, 360, cv::Scalar(0, 255, 0), 4, 8, 0);
    }
    return;
}

//showEyesRotation: draws a line in the rotation direction in MAIN_WINDOW
void guiHandler::showEyesRotationY(cv::Rect& face, cv::Rect& leftEye, cv::Rect& rightEye, double rotation) {
    cv::Point center( face.x + (leftEye.x + leftEye.width*0.5 + rightEye.x + rightEye.width*0.5) / 2, face.y + (leftEye.y + leftEye.height*0.5 + rightEye.y + rightEye.height*0.5) / 2 );
    double x = rotation;

    cv::Point normal(center.x-(10*x), center.y);
    line(mainFrame, center, normal , cv::Scalar(0, 255, 0), 3, 8, 0);
}

//showNoseRotation: draws a line in the rotation direction in MAIN_WINDOW
void guiHandler::showNoseRotationY(cv::Rect& face, cv::Rect& nose, double rotation) {
    cv::Point center( face.x + nose.x + nose.width*0.5, face.y + nose.y + nose.height*0.5 );
    double x = rotation;

    cv::Point normal(center.x-(10*x), center.y);
    line(mainFrame, center, normal, cv::Scalar(0, 255, 255), 3, 8, 0);
}

//showMouthRotation: draws a line in the rotation direction in MAIN_WINDOW
void guiHandler::showMouthRotationY(cv::Rect& face, cv::Rect& mouth, double rotation) {
    cv::Point center( face.x + mouth.x + mouth.width*0.5, face.y + mouth.y + mouth.height*0.5 );
    double x = rotation;

    cv::Point normal(center.x-(10*x), center.y);
    line(mainFrame, center, normal , cv::Scalar(0, 0, 255), 3, 8, 0);
}

//ShowFaces: draws an ellipse around faces in MAIN_WINDOW
void guiHandler::showFaces(std::vector<cv::Rect> faces)
{
    for( uint i = 0; i < faces.size(); i++ )  //per ogni faccia
    {
        //find face center
        cv::Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        //design an ellipse around it
        ellipse( mainFrame, center, cv::Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar(255, 0, 0), 4, 8, 0 );
    }
    return;
}

void guiHandler::showUpperBody(std::vector<cv::Rect> upperBodies)
{
    for( uint i = 0; i < upperBodies.size(); i++ )  //per ogni faccia
    {
        //find face center
        cv::Point center( upperBodies[i].x + upperBodies[i].width*0.5, upperBodies[i].y + upperBodies[i].height*0.5 );
        //design an ellipse around it
        ellipse( mainFrame, center, cv::Size( upperBodies[i].width*0.5, upperBodies[i].height*0.5), 0, 0, 360, cv::Scalar(10, 75, 255), 4, 8, 0 );
    }
    return;
}

void guiHandler::turnOnZone(int hZone, int vZone)
{
    Point p;
    switch(hZone)
    {
    case 0:
        p.x = hRes/4;
        break;
    case 1:
        p.x = hRes/2;
        break;
    case 2:
        p.x = 3*hRes/4;
        break;
    default:
        p.x = hRes/2;
        break;
    }

    switch(vZone)
    {
    case 0:
        p.y = vRes/4;
        break;
    case 1:
        p.y = vRes/2;
        break;
    case 2:
        p.y = 3*vRes/4;
        break;
    default:
        p.y = vRes/2;
        break;
    }

    showStartingGaze(p);
}

void guiHandler::showFinalGaze(cv::Point gazePoint)
{
    if(gazeFrame.empty()) return;
    int radius = gazeFrame.cols/20;
    cv::circle(gazeFrame, gazePoint, radius, cv::Scalar(255, 0, 0), 3, 1, 0);
    return;
}

void guiHandler::showStartingGaze(cv::Point gazeStartingPoint){
    //int position;
    cv::Point start;
    int x = gazeStartingPoint.x;
    int y = gazeStartingPoint.y;
    //int radius = gazeFrame.cols/14;
    gazeFrame.release();
    gazeFrame = cv::Mat(vRes, hRes, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::line(gazeFrame, cv::Point(0,vRes/3), cv::Point(hRes,vRes/3), cv::Scalar(0,255,255), 3, 1, 0);
    cv::line(gazeFrame, cv::Point(0,vRes*2/3), cv::Point(hRes,vRes*2/3), cv::Scalar(0,255,255), 3, 1, 0);
    cv::line(gazeFrame, cv::Point(hRes/3, 0), cv::Point(hRes/3, vRes), cv::Scalar(0,255,255), 3, 1, 0);
    cv::line(gazeFrame, cv::Point(hRes*2/3, 0), cv::Point(hRes*2/3, vRes), cv::Scalar(0,255,255), 3, 1, 0);
    //cv::circle(gazeFrame, gazeStartingPoint, radius, cv::Scalar(0,255,255), 5, 1, 0);
    if(x <= hRes/3)
    {
        if (y <= vRes/3)
        {
            start=cv::Point(0,0);
        }
        else if (y <= vRes*2/3)
        {
            start=cv::Point(0,vRes/3);
        }
        else
        {
            start=cv::Point(0,vRes*2/3);
        }
    }
    else if (x <= hRes*2/3)
    {
        if (y <= vRes/3)
        {
            start=cv::Point(hRes/3,0);
        }
        else if (y <= vRes*2/3)
        {
            start=cv::Point(hRes/3,vRes/3);
        }
        else
        {
            start=cv::Point(hRes/3,vRes*2/3);
        }
    }
    else
    {
        if (y <= vRes/3)
        {
            start=cv::Point(hRes*2/3,0);
        }
        else if (y <= vRes*2/3)
        {
            start=cv::Point(hRes*2/3,vRes/3);
        }
        else
        {
            start=cv::Point(hRes*2/3,vRes*2/3);
        }
    }

    cv::rectangle(gazeFrame,cv::Rect(start.x, start.y, hRes/3, vRes/3),cv::Scalar(255,0,0),3,1,0);

    return;
}


//showHeadRotation: draws a line in the rotation direction in MAIN_WINDOW
void guiHandler::showHeadRotationY(cv::Rect& face, double rotation) {
    cv::Point center( face.x + face.width*0.5, face.y + face.height*0.5 );
    double x = rotation;
    //double c = 75;
    //double b = c * sin(x) / sin(180 - 90 - x);
    //double distanza_pixel = b * 72 * 2.54;
    //double distanza_pixel = b * 37.795275591;       //1cm -> 37.795275591 pixel
    //std::cout<<"Pixel distance: "<< distanza_pixel <<std::endl;

    cv::Point normal(center.x-(10*x), center.y);
    line(mainFrame, center, normal , cv::Scalar(255, 0, 0), 3, 8, 0);
}

//ShowEyeZone: draws a rectangle around eyes in FACE_WINDOW
void guiHandler::showEyeZones()
{

    // ------------- FACE ROI -------------
    // draw left eye big region
    rectangle(faceFrame,eyes[0],1234);  //Design a rectangle on the face ROI for this eye. 1234 is a good color
    // draw right eye big region
    rectangle(faceFrame,eyes[1],1234);
    return;
}

//ShowRoi: shows an arbitrary roi and waits for a key pressed, then destroy the image if !maintain.
void guiHandler::showRoi(const cv::Mat& roi, std::string name, bool maintain)
{
    std::cout<<"SHOWROI: ENTERED"<<std::endl;
    cv::namedWindow(name, CV_WINDOW_NORMAL);
    imshow(name, roi);
    cv::waitKey(0);
    if(!maintain) cv::destroyWindow(name);
    return;
}

/** ShowPupils: adapt pupils coordinates and draw a little circle around pupils in faceFrame.
 */
void guiHandler::showPupils(Point* left, Point *right, bool refined, bool reduced)
{
    //Check for correctness drawing perfect ROI center
    //circle(faceFrame, cv::Point((int)hA->face.eyes.leftEyeROIReduced.cols/2 + hA->face.eyes.leftEyeReducedRegion->x, (int)hA->face.eyes.leftEyeROIReduced.rows/2 + hA->face.eyes.leftEyeReducedRegion->y), 7, 255);
    if(left != NULL)
    {
        #ifdef SHOWPUPILS_2_DEBUG
        std::cout<<"SHOWPUPILS: Refined = " << refined << ", reduced = " << reduced<< "; Left Coords: {" << left->x << "," << left->y << "}" <<std::endl;
        #endif
        if(!refined)
        {
            if(!reduced)
            {
                leftPupilFaceCoords = *left;
                circle(faceFrame, leftPupilFaceCoords, faceFrame.rows/32, 255);
            }
            /*
            else
            {
                leftPupilRedCoords = *left;
                circle(leftFrame, leftPupilRedCoords, leftFrame.rows/8, 255);
            }
            */
        }
        else
        {
            if(!reduced)
            {
                leftPupilFaceCoordsRefined = *left;
                cv::circle(this->faceFrame, leftPupilFaceCoordsRefined, faceFrame.rows/40, 200);
            }
            /*
            else
            {
                leftPupilRedCoordsRefined = *left;
                cv::circle(this->leftFrame, leftPupilRedCoordsRefined, leftFrame.rows/10, 200);
            }
            */
        }
    }
    if(right != NULL)
    {
        #ifdef SHOWPUPILS_2_DEBUG
        std::cout<<"SHOWPUPILS: Refined = " << refined << ", reduced = " << reduced<< "; Right Coords: {" << right->x << "," << right->y << "}" <<std::endl;
        #endif
        if(!refined)
        {
            if(!reduced)
            {
                rightPupilFaceCoords = *right;
                circle(faceFrame, rightPupilFaceCoords, faceFrame.rows/32, 255);
            }
            /*
            else
            {
                rightPupilRedCoords = *right;
                circle(rightFrame, rightPupilRedCoords, rightFrame.rows/8, 255);
            }
            */
        }
        else
        {
            if(!reduced)
            {
                rightPupilFaceCoordsRefined = *right;
                cv::circle(this->faceFrame, rightPupilFaceCoordsRefined, faceFrame.rows/40, 200);
            }
            /*
            else
            {
                rightPupilRedCoordsRefined = *right;
                cv::circle(this->rightFrame, rightPupilRedCoordsRefined, rightFrame.rows/10, 200);
            }
            */
        }
    }
}





//cv::Size guiHandler::getGazeFrameSize()
//{
// cv::Size size;
// if(gazeFrame.empty())
// {
// gazeFrame = cv::imread("D:\\polyphemus\\Debug\\res\\whitePaper.png");//whitePaper.png"); //SWITCH HERE
// }
// size.width = hRes;//gazeFrame.cols;
// size.height = vRes; //gazeFrame.rows;
// return size;
//}

//void guiHandler::showGaze(double ratio)
//{
//    if(gazeFrame.empty()) return;
//    /*
//   gaze.x = (int)(ratio*gazeFrame.cols);
//   if(gaze.x == gazeFrame.cols) gaze.x--;
//   */

//    cv::circle(gazeFrame, gaze, 15, CV_RGB(0,0,0), 3);
//}

void guiHandler::setFaceFrame(cv::Mat frame)
{
    frame.copyTo(faceFrame);
}

void guiHandler::setReducedFaceFrame(cv::Mat frame)
{
    frame.copyTo(reducedFaceFrame);
}

void guiHandler::setLeftEmpiricReducedFrame(cv::Mat frame)
{
    frame.copyTo(leftFrame);
}

void guiHandler::setRightEmpiricReducedFrame(cv::Mat frame)
{
    frame.copyTo(rightFrame);
}
