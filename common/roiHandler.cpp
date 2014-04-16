#include "roiHandler.h"

roiHandler::roiHandler()
{
    itHasNose = false;
    itHasMouth = false;
    itHasFace = false;
    itHasLeftEye = false;
    itHasRightEye = false;

    itHasLeftEyeHighLeftInit = false;
    itHasLeftEyeCenterInit = false;
    itHasLeftEyeLowRightInit = false;

    itHasRightEyeCenterInit = false;
    itHasRightEyeHighLeftInit = false;
    itHasRightEyeLowRightInit = false;

    itHasMouthCenterInit = false;
    itHasMouthHighLeftInit = false;
    itHasMouthLowRightInit = false;

    itHasNoseCenterInit=false;
    itHasNoseHighLeftInit=false;
    itHasNoseLowRightInit=false;

    itHasDistanceNoseMouth = false;
    itHasDistanceMouthCenter = false;
    itHasDistanceEyes = false;
    itHasDistanceNoseCenter = false;

    fakeRect = Rect(0,0,0,0);
}

void roiHandler::setGrayFrame(cv::Mat m)
{
#ifdef SETGRAYFRAME_DEBUG
    std::cout<<"SETGRAYFRAME: cols = "<<m.cols<<", rows = "<<m.rows<<std::endl;
#endif
    grayFrame = m;
}


//GetGrayFrame: getter method, returns POINTER for sake of efficiency, so pay attention
cv::Mat& roiHandler::getGrayFrame()
{
    #ifdef GETGRAYFRAME_DEBUG
    std::cout<<"GETGRAYFRAME: cols = "<<grayFrame.cols<<", rows = "<<grayFrame.rows<<std::endl;
    #endif
    return grayFrame;
}

/*******************************************************************************/
/*                                   EYES                                      */
/*******************************************************************************/

/******************/
/* EYE(S) SETTERS */
/******************/

//setEyePairROI: makes the roiHandler set his eye region frame
void roiHandler::setEyePairROI(std::vector<cv::Rect> fVett) {
    if(fVett.size() == 0) return;

    itHasEyePair = true;
    eyePairHaarRect = fVett[0];
    eyePairHaarFrame = grayFrame(fVett[0]);

    return;
}

//setEyesHaarROI: makes the roiHandler set his eyes frame
void roiHandler::setEyesHaarROI(std::vector<cv::Rect> fVett)
{
    if(fVett.size() != 2) return;

    //TODO: evaluate the eyes position, because the algorithm could find an eye over another or with a big size difference
    int iLeft, iRight;
    if (fVett[0].x < fVett[1].x) {
        iLeft = 0;
        iRight = 1;
    }
    else {
        iLeft = 1;
        iRight = 0;
    }
    itHasLeftEye = true;
    leftEyeHaarRect = fVett[iLeft];
    leftEyeHaarFrame = grayFrame(fVett[iLeft]);

    itHasRightEye = true;
    rightEyeHaarRect = fVett[iRight];
    rightEyeHaarFrame = grayFrame(fVett[iRight]);

    return;
}

//SetEmpiricEyes: refreshes eyes' rects and rois
void roiHandler::setEmpiricEyes()
{
    if(eyeZone.size() == 0)
    {
        return;
    }

    leftEyeEmpiricRect = eyeZone[0];
    leftEyeEmpiricFrame = faceHaarFrame(eyeZone[0]);            //TODO: modify here!! insert the roi based on the haar search

    rightEyeEmpiricRect = eyeZone[1];
    rightEyeEmpiricFrame = faceHaarFrame(eyeZone[1]);

#ifdef SETEMPIRICEYES_DEBUG
    std::cout<<"SETEMPIRICEYES: leftEyeEmpiricFrame: cols = "<< leftEyeEmpiricFrame.cols<<", rows = "<<leftEyeEmpiricFrame.rows<<std::endl;
    std::cout<<"SETEMPIRICEYES: rightEyeEmpiricFrame: cols = "<< rightEyeEmpiricFrame.cols<<", rows = "<<rightEyeEmpiricFrame.rows<<std::endl;
#endif

    empiricEyes.clear();
    empiricEyes.push_back(leftEyeEmpiricRect);
    empiricEyes.push_back(rightEyeEmpiricRect);

    return;
}

//setLeftEyeROI: makes the roiHandler set his left eye frame
void roiHandler::setLeftEyeROI(std::vector<cv::Rect> fVett) {
    if(fVett.size() == 0) return;

    itHasLeftEye = true;
    leftEyeHaarRect = fVett[0];
    leftEyeHaarFrame = grayFrame(fVett[0]);

    return;
}

//setRightEyeROI: makes the roiHandler set his right eye frame
void roiHandler::setRightEyeROI(std::vector<cv::Rect> fVett) {
    if(fVett.size() == 0) return;

    itHasRightEye = true;
    rightEyeHaarRect = fVett[0];
    rightEyeHaarFrame = grayFrame(fVett[0]);

    return;
}


//SetReducedEmpiricEyes: refreshes reduced rects and rois.
void roiHandler::setReducedEmpiricEyes()
{
    eyesReduced.clear();

    int nEyes = empiricEyes.size();
    for(int i=0; i<nEyes; i++)
    {
        eyesReduced.push_back(empiricEyes[i]);
        eyesReduced[i].width /= 1.5;
        eyesReduced[i].height /= 1.5;

        eyesReduced[i].x += (empiricEyes[i].width - eyesReduced[i].width) / 2;
        eyesReduced[i].y += (empiricEyes[i].height - eyesReduced[i].height) / 2;
    }

    if(nEyes==2)
    {
        leftEmpiricReducedFrame = faceHaarFrame(eyesReduced[0]);
        rightEmpiricReducedFrame = faceHaarFrame(eyesReduced[1]);
    }

    return;
}

void roiHandler::setLeftEmpiricRotatedFrame(cv::Mat& img)
{
    leftEmpiricRotatedFrame = img;
}

void roiHandler::setRightEmpiricRotatedFrame(cv::Mat& img)
{
    rightEmpiricRotatedFrame = img;
}



/******************/
/* EYE(S) GETTERS */
/******************/

cv::Rect& roiHandler::getLeftEye() {
    return leftEyeHaarRect;
}

cv::Mat& roiHandler::getLeftEyeROI() {
    return leftEyeHaarFrame;
}

cv::Rect& roiHandler::getRightEye() {
    return rightEyeHaarRect;
}

cv::Mat& roiHandler::getRightEyeROI() {
    return rightEyeHaarFrame;
}

cv::Mat& roiHandler::getLeftEmpiricReducedROI() {
    return leftEmpiricReducedFrame;
}

cv::Mat& roiHandler::getRightEmpiricReducedROI() {
    return rightEmpiricReducedFrame;
}

cv::Mat& roiHandler::getOldHaarNoseROI() {
    return oldNoseHaarFrame;
}

cv::Mat& roiHandler::getOldHaarMouthROI() {
    return oldMouthHaarFrame;
}


std::vector<cv::Rect> roiHandler::getEmpiricEyes() {
    return empiricEyes;
}

std::vector<cv::Rect> roiHandler::getReducedEmpiricEyes() {
    return eyesReduced;
}


//getReducedROI: getter method. ROI is returned by reference, so pay attention!
cv::Mat& roiHandler::getEmpiricReducedROI(bool left)
{
    if(left) return leftEmpiricReducedFrame;
    else     return rightEmpiricReducedFrame;
}

cv::Mat& roiHandler::getLeftEmpiricRotatedFrame()
{
    return leftEmpiricRotatedFrame;
}

cv::Mat& roiHandler::getRightEmpiricRotatedFrame()
{
    return rightEmpiricRotatedFrame;
}


/******************/
/* EYE(S) CLEANER */
/******************/

void roiHandler::clearEmpiricEyes()
{
    leftEyeEmpiricRect = cv::Rect(0, 0, 0, 0);
    leftEyeEmpiricFrame.release();

    rightEyeEmpiricRect = cv::Rect(0, 0, 0, 0);
    rightEyeEmpiricFrame.release();
}

//clearEyes: flush the eyes frame
void roiHandler::clearHaarEyes()
{
    if (itHasLeftEye) {
        itHasLeftEye = false;
        leftEyeHaarRect = cv::Rect(0, 0, 0, 0);
        leftEyeHaarFrame.release();
    }
    if (itHasRightEye) {
        itHasRightEye = false;
        rightEyeHaarRect = cv::Rect(0, 0, 0, 0);
        rightEyeHaarFrame.release();
    }
}

/********************/
/* EYE(S) EXISTANCE */
/********************/
bool roiHandler::hasLeftEye() {
    return itHasLeftEye;
}

bool roiHandler::hasRightEye() {
    return itHasRightEye;
}

/********************/
/* EYE(S) HELPERS*/
/********************/
//MakeEmpiricEyes: set empiric rois for the eyes (not-haar rois)
void roiHandler::makeEmpiricEyes() {
#ifdef MAKEEMPIRICEYES_DEBUG
    int h=0;
    std::cout<<"MAKEEMPIRICEYES: "<<h++<<std::endl;
#endif
    clearEmpiricEyes();
#ifdef MAKEEMPIRICEYES_DEBUG
    std::cout<<"MAKEEMPIRICEYES: "<<h++<<std::endl;
#endif
    detectEyeZone();
#ifdef MAKEEMPIRICEYES_DEBUG
    std::cout<<"MAKEEMPIRICEYES: "<<h++<<std::endl;
#endif
    setEmpiricEyes();
#ifdef MAKEEMPIRICEYES_DEBUG
    std::cout<<"MAKEEMPIRICEYES: (empiricEyes[0]): cols = "<<empiricEyes[0].width<<", rows = "<<empiricEyes[0].height<<std::endl;
    std::cout<<"MAKEEMPIRICEYES: (empiricEyes[1]): cols = "<<empiricEyes[1].width<<", rows = "<<empiricEyes[1].height<<std::endl;
#endif
}

//DetectEyeZone: returns 2 cv::Rect that represents eyes limits in the first face.
void roiHandler::detectEyeZone() {
    eyeZone.clear();

    //-- Restrict to eye regions. Notice that eye regions are static, they don't depend on head position!!!
    eyeWidth = faceHaarRect.width * (kEyePercentWidth/100.0);
    eyeHeight = faceHaarRect.width * (kEyePercentHeight/100.0);
    eyeTop = faceHaarRect.height * (kEyePercentTop/100.0);
    //Rect constructor: Rect(topleft.x, topleft.y, width, height)
    eyeZone.push_back(cv::Rect(faceHaarRect.width*(kEyePercentSide/100.0),
                               eyeTop,
                               eyeWidth,   //width
                               eyeHeight)); //height


    eyeZone.push_back(cv::Rect(faceHaarRect.width - eyeWidth - faceHaarRect.width*(kEyePercentSide/100.0),
                               eyeTop,   //suppose horizontally aligned head!!!
                               eyeWidth,
                               eyeHeight));
    return;
}


/*******************************************************************************/
/*                                   FACE                                      */
/*******************************************************************************/

/********************/
/* FACE EXISTANCE */
/********************/
bool roiHandler::hasFace() {
    return itHasFace;
}

/********************/
/* FACE SETTERS     */
/********************/
//SetFaceROI: makes the roiHandler set his face frame
void roiHandler::setFaceROI(std::vector<cv::Rect> fVett)
{
    if(fVett.size() == 0) return;

    itHasFace = true;
    faceHaarRect = fVett[0];
    faceHaarFrame = grayFrame(fVett[0]);

#ifdef SETFACEROI_DEBUG
    std::cout<<"SETFACEROI: faceHaarFrame.cols = "<<faceHaarFrame.cols <<", rows = "<<faceHaarFrame.rows<<std::endl;
#endif
    return;
}

//setReducedFaceROI: makes the frameHandler set his reduced face frame
void roiHandler::setReducedFaceROI(cv::Rect& reducedRect)
{
    faceReducedRect.width=reducedRect.width;
    faceReducedRect.height=reducedRect.height;
    faceReducedRect.x+=reducedRect.x;
    faceReducedRect.y+=reducedRect.y;

    //KE
    faceReducedFrame = faceHaarFrame (faceReducedRect);
    //KE

    return;
}

/********************/
/* FACE GETTERS     */
/********************/
cv::Rect& roiHandler::getFace() {
    return faceHaarRect;
}

cv::Mat& roiHandler::getFaceROI() {
//    Mat gray;
//    cvtColor(faceHaarFrame, gray, CV_BGR2GRAY);
//    cout << "HERE " << faceHaarFrame.channels() <<endl;
//    return gray;
    return faceHaarFrame;
}

cv::Mat& roiHandler::getReducedFaceROI() {
    return faceReducedFrame;
}

/********************/
/* FACE CLEANER     */
/********************/
void roiHandler::clearFace()
{
    if (itHasFace) {
        itHasFace = false;
        faceHaarRect = cv::Rect(0, 0, 0, 0);
        faceHaarFrame.release();
        faceReducedRect = cv::Rect(0, 0, 0, 0);
        faceReducedFrame.release();
    }
}

void roiHandler::setOldNose()
{
    oldNoseHaarFrame=getFaceROI()(getNose());
    oldNoseHaarRect=getNose();
}

void roiHandler::setOldMouth()
{
    oldMouthHaarFrame=getFaceROI()(getMouth());
    oldMouthHaarRect=getMouth();
}

/*******************************************************************************/
/*                                   NOSE                                      */
/*******************************************************************************/
//SetNoseROI: makes the frameHandler set his nose frame
void roiHandler::setNoseROI(std::vector<cv::Rect> fVett)
{
    if(fVett.size() == 0) return;

    itHasNose = true;
    noseHaarRect = fVett[0];
    noseHaarFrame = grayFrame(fVett[0]);

    return;
}

bool roiHandler::hasNose() {
    return itHasNose;
}

cv::Rect& roiHandler::getNose() {
    return noseHaarRect;
}

cv::Mat& roiHandler::getNoseROI() {
    return noseHaarFrame;
}

//clearNose: flush the noseFrame
void roiHandler::clearNose()
{
    if (itHasNose) {
        itHasNose = false;
        noseHaarRect = cv::Rect(0, 0, 0, 0);
        noseHaarFrame.release();
    }
}


/*******************************************************************************/
/*                                   MOUTH                                     */
/*******************************************************************************/
//SetMouthROI: makes the frameHandler set his Mouth frame
void roiHandler::setMouthROI(std::vector<cv::Rect> fVett)
{
    if(fVett.size() == 0) return;

    itHasMouth = true;
    mouthHaarRect = fVett[0];
    mouthHaarFrame = grayFrame(fVett[0]);

    return;
}

bool roiHandler::hasMouth() {
    return itHasMouth;
}

cv::Rect& roiHandler::getMouth() {
    return mouthHaarRect;
}

cv::Mat& roiHandler::getMouthROI() {
    return mouthHaarFrame;
}

//clearMouth: flush the mouthFrame
void roiHandler::clearMouth()
{
    if (itHasMouth) {
        itHasMouth = false;
        mouthHaarRect = cv::Rect(0, 0, 0, 0);
        mouthHaarFrame.release();
    }
}

void roiHandler::release()
{
    clearNose();
    clearMouth();
    clearFace();

    grayFrame.release();
    faceHaarFrame.release();
}

//Set center point
void roiHandler::setInitMouthCenter(cv::Rect mouth)
{
    itHasMouthCenterInit=true;
    MouthHaarRectCenterInit=mouth;
}

void roiHandler::setInitNoseCenter(cv::Rect nose)
{
    itHasNoseCenterInit=true;
    NoseHaarRectCenterInit=nose;
}

void roiHandler::setInitLeftEyeCenter(cv::Rect leftEye)
{
    itHasLeftEyeCenterInit=true;
    leftEyeHaarRectCenterInit=leftEye;
}

void roiHandler::setInitRightEyeCenter(cv::Rect rightEye)
{
    itHasRightEyeCenterInit=true;
    RightEyeHaarRectCenterInit=rightEye;
}


//Set high-left point

void roiHandler::setInitMouthHighLeft(cv::Rect mouth)
{
    itHasMouthHighLeftInit=true;
    MouthHaarRectHighLeftInit=mouth;
}

void roiHandler::setInitNoseHighLeft(cv::Rect nose)
{
    itHasNoseHighLeftInit=true;
    NoseHaarRectHighLeftInit=nose;
}

void roiHandler::setInitLeftEyeHighLeft(cv::Rect leftEye)
{
    itHasLeftEyeHighLeftInit=true;
    leftEyeHaarRectHighLeftInit=leftEye;
}

void roiHandler::setInitRightEyeHighLeft(cv::Rect rightEye)
{
    itHasRightEyeHighLeftInit=true;
    RightEyeHaarRectHighLeftInit=rightEye;
}

//Set low-right point
void roiHandler::setInitMouthLowRight(cv::Rect mouth)
{
    itHasMouthLowRightInit=true;
    MouthHaarRectLowRightInit=mouth;
}

void roiHandler::setInitNoseLowRight(cv::Rect nose)
{
    itHasNoseLowRightInit=true;
    NoseHaarRectLowRightInit=nose;
}

void roiHandler::setInitLeftEyeLowRight(cv::Rect leftEye)
{
    itHasLeftEyeLowRightInit=true;
    leftEyeHaarRectLowRightInit=leftEye;
}

void roiHandler::setInitRightEyeLowRight(cv::Rect rightEye)
{
    itHasRightEyeLowRightInit=true;
    RightEyeHaarRectLowRightInit=rightEye;
}

//Get LowRight point
cv::Rect& roiHandler::getInitMouthLowRight()
{
    if (itHasMouthLowRightInit)
    {
        return MouthHaarRectLowRightInit;
    }
    else return fakeRect;
}

cv::Rect& roiHandler::getInitNoseLowRight()
{
    if (itHasNoseLowRightInit)
    {
        return NoseHaarRectLowRightInit;
    }
    else return fakeRect;
}

cv::Rect& roiHandler::getInitLeftEyeLowRight()
{
    if (itHasLeftEyeLowRightInit)
    {
        return leftEyeHaarRectLowRightInit;
    }
    else return fakeRect;
}

cv::Rect& roiHandler::getInitRightEyeLowRight()
{
    if (itHasRightEyeLowRightInit)
    {
        return RightEyeHaarRectLowRightInit;
    }
    else return fakeRect;
}


//Get center point
cv::Rect& roiHandler::getInitMouthCenter()
{
    if (itHasDistanceMouthCenter)
    {
        return MouthHaarRectCenterInit;
    }
    else return fakeRect;
}

cv::Rect& roiHandler::getInitNoseCenter()
{
    if (itHasDistanceNoseCenter)
    {
        return NoseHaarRectCenterInit;
    }
    else return fakeRect;
}

cv::Rect& roiHandler::getInitLeftEyeCenter()
{
    if (itHasLeftEyeCenterInit)
    {
        return leftEyeHaarRectCenterInit;
    }
    else return fakeRect;
}

cv::Rect& roiHandler::getInitRightEyeCenter()
{
    if (itHasRightEyeCenterInit)
    {
        return RightEyeHaarRectCenterInit;
    }
    else return fakeRect;
}

//Get HighLeft point
cv::Rect& roiHandler::getInitMouthHighLeft()
{
    if (itHasMouthHighLeftInit)
    {
        return MouthHaarRectHighLeftInit;
    }
    else return fakeRect;
}

cv::Rect& roiHandler::getInitNoseHighLeft()
{
    if (itHasNoseHighLeftInit)
    {
        return NoseHaarRectHighLeftInit;
    }
    else return fakeRect;
}

cv::Rect& roiHandler::getInitLeftEyeHighLeft()
{
    if (itHasLeftEyeHighLeftInit)
    {
        return leftEyeHaarRectHighLeftInit;
    }
    else return fakeRect;
}

cv::Rect& roiHandler::getInitRightEyeHighLeft()
{
    if (itHasRightEyeHighLeftInit)
    {
        return RightEyeHaarRectHighLeftInit;
    }    
    else return fakeRect;
}


bool roiHandler::itHasCenterPoint()
{
    if (itHasLeftEyeCenterInit && itHasRightEyeCenterInit && itHasNoseCenterInit && itHasMouthCenterInit)
    {
        return true;
    }
    return false;
}

bool roiHandler::itHasLowRightPoint()
{
    if (itHasLeftEyeLowRightInit && itHasRightEyeLowRightInit && itHasNoseLowRightInit && itHasMouthLowRightInit)
    {
        return true;
    }
    return false;
}

bool roiHandler::itHasHighLeftPoint()
{
    if (itHasLeftEyeHighLeftInit && itHasRightEyeHighLeftInit && itHasNoseHighLeftInit && itHasMouthHighLeftInit)
    {
        return true;
    }
    return false;
}
