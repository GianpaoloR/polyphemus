#include "polyphemus.h"

static const int maskSize = 3;

//----------------------------------
//      CONSTRUCTORS
//----------------------------------

polyphemus::polyphemus(int params, cv::Mat externalFrame) {
    constructor(params);
    fH->setExternalFrame(externalFrame);
}


polyphemus::polyphemus(int params) {
    constructor(params);
}

#ifdef TEST_MODE
polyphemus::polyphemus(int params, std::string testPath)
{
    constructor(params);
    fH->setTestPath(testPath);

    //hGazeRight = 0;
    //hGazeWrong = 0;
}
#endif


void polyphemus::constructor(int params) {

    #ifndef WEBSERVICE
    #ifdef WITH_GUI
    this->gui = NULL;
    #endif //WITH_GUI2.00
    #endif //WEBSERVICE

    #ifdef TEST_MODE
    #ifdef AUTOMATIC_TEST
    realLeftInFace = NULL;
    realRightInFace = NULL;
    leftAvgError = 0;
    rightAvgError = 0;
    leftNotFound = 0;
    rightNotFound = 0;
    foundLeftPupils = 0;
    foundRightPupils = 0;

    goodRefinementLeft = 0;
    goodRefinementRight = 0;
    falseRefinementLeft = 0;
    falseRefinementRight = 0;
    leftRefAvgError = 0;
    rightRefAvgError = 0;
    #endif
    #endif

    /*
    if(params & PROFILE_WANTED) profiling = true;
    else profiling = false;
    */

    if(params & TWINKLE_WANTED) twinkleFlag = true;
    else twinkleFlag = false;

    //Frame handler: captures and dispatches frames. Selects rois and transforms channels.
    fH = new frameHandler();

    //Haar analyzer: finds faces in a frame.
    haar = new haarAnalyzer();

    //Binary Analyzer: applies morphology algorithms.
    bin = new binaryAnalyzer();

    //Roi Handler: collects different rois
    rH = new roiHandler();

    //Pupil Analyzer: estimates pupil position
    pD = new pupilDetection(bin);

    //Gaze Estimation: estimates gaze
    gE = new gazeEstimation();

    leftInFace = NULL;
    rightInFace = NULL;
    leftInFaceRefined = NULL;
    rightInFaceRefined = NULL;

    refined = true;
    alreadyFace = false;

    //HeadRotation: find the head rotations using the nose, the mouth and the ears
    headRotation = new HeadRotation();
    headRotation->setRoiHandler(rH);

    #ifdef GREATCATCH_TEST
    correctPlace = 0;
    wrongPlace = 0;
    #endif
}


//---------------------------------------
//          WEBSERVICE METHODS
//---------------------------------------
#ifdef WEBSERVICE
//UO
void polyphemus::setScreenSize(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    headRotation->setScreenSize(screenWidth, screenHeight);
    gE->setScreenResolution(screenWidth, screenHeight);
}
//UO

cv::Point polyphemus::getWatchingPoint()
{
    return watchingPoint;
}

bool polyphemus::hasFace()
{
    return rH->hasFace();
}

cv::Rect& polyphemus::getFace()
{
    return rH->getFace();
}

bool polyphemus::hasLeftEye()
{
    return rH->hasLeftEye();
}

cv::Rect& polyphemus::getLeftEye()
{
    return rH->getLeftEye();
}

bool polyphemus::hasRightEye()
{
    return rH->hasRightEye();
}

cv::Rect& polyphemus::getRightEye()
{
    return rH->getRightEye();
}

bool polyphemus::hasNose()
{
    return rH->hasNose();
}

cv::Rect& polyphemus::getNose()
{
    return rH->getNose();
}

bool polyphemus::hasMouth()
{
    return rH->hasMouth();
}

cv::Rect& polyphemus::getMouth()
{
    return rH->getMouth();
}

#endif

//-----------------------------------------
//          SCREEN METHODS
//-----------------------------------------
void polyphemus::retrieveDesktop()
{
#ifdef __linux__
    Display* pdsp = NULL;
    Screen* pscr = NULL;

    pdsp = XOpenDisplay(NULL);
    if (!pdsp) {
        screenWidth = 0;
        screenHeight = 0;
    }
    else {
        pscr = DefaultScreenOfDisplay(pdsp);
        if (!pscr) {
            screenWidth = 0;
            screenHeight = 0;
        }
        else {
            screenWidth = pscr->width;
            screenHeight = pscr->height;

            XCloseDisplay(pdsp);
        }
    }
#else
    QApplication app(argc, argv);
    QDesktopWidget* desktop = QApplication::desktop();
    vRes = desktop->height();
    hRes = desktop->width();
#endif

    //Debug print
    if(retrieveDesktopDebug) std::cout<<"Desktop SIZE: ["<<screenWidth<<" x "<<screenHeight<<"]"<<std::endl;

    headRotation->setScreenSize(screenWidth, screenHeight);
    gE->setScreenResolution(screenWidth, screenHeight);
}

//--------------------------------------
//          DEBUG GUI METHODS
//--------------------------------------
#ifndef WEBSERVICE
#ifdef WITH_GUI
//EnableDebugGui: only for debugging purpose, it starts the user interface and sets gui field for every component.It is passive: it only shows possible outputs, it has debugging purposes.
void polyphemus::enableDebugGui()
{
    this->gui = new guiHandler(screenWidth, screenHeight);
    //gui->setHeadRotation(headRotation);
    fH->setDebugGui(this->gui);
    haar->setDebugGui(this->gui);
    headRotation->setDebugGui(this->gui);
    bin->setDebugGui(this->gui);
    std::cout<<"Debug GUI enabled."<<std::endl;
    return;
}
#endif //WITH_GUI
#endif

//----------------------------
//    INITIALIZATION
//----------------------------
void polyphemus::init(std::string face, std::string eyePair, std::string singleEye, std::string leftEye, std::string rightEye, std::string nose, std::string mouth, std::string upperBody)
{
    //Haar initialization
    if(face.compare("")!=0)
    {
        haar->loadCascade(haarAnalyzer::FACE, face);
    }

    if(upperBody.compare("")!=0)
    {
        haar->loadCascade(haarAnalyzer::UPPERBODY, upperBody);
    }

    if (eyePair.compare("") != 0) {
        haar->loadCascade(haarAnalyzer::EYEPAIR, eyePair);
    }

    if (singleEye.compare("") != 0) {
        haar->loadCascade(haarAnalyzer::SINGLEEYE, singleEye);
    }

    if (leftEye.compare("") != 0) {
        haar->loadCascade(haarAnalyzer::LEFTEYE, leftEye);
    }

    if (rightEye.compare("") != 0) {
        haar->loadCascade(haarAnalyzer::RIGHTEYE, rightEye);
    }

    if(nose.compare("")!=0)
    {
        haar->loadCascade(haarAnalyzer::NOSE, nose);
    }

    if(mouth.compare("")!=0)
    {
        haar->loadCascade(haarAnalyzer::MOUTH, mouth);
    }

#ifndef WEBSERVICE
#ifdef WITH_GUI
    //Gui initialization
    if(gui!=NULL)
    {
        gui->initWindow(MAIN_WINDOW);
        gui->initWindow(FACE_WINDOW);
        gui->initWindow(GAZE_WINDOW);
    }
#endif //WITH_GUI

    //Frame Handler Initialization
    bool goodSettings = fH->initCapture();

    if(!goodSettings)
    {
        std::cout<<"No capture"<<std::endl;
        exit(1);
    }
    cv::waitKey(0);
#endif //WEBSERVICE
    return;
}

void polyphemus::preProcessFrame()
{
    if(preprocessDebug) std::cout<<"PREPROCESSFRAME: cols = "<<fH->getFrame().cols<<", rows = "<<fH->getFrame().rows<<std::endl;

    fH->mirror();

    if(preprocessDebug) std::cout<<"PREPROCESSFRAME: cols = "<<fH->getFrame().cols<<", rows = "<<fH->getFrame().rows<<std::endl;

    fH->preProcessFrame(); //QUESTA FUNZIONE E' PESANTISSIMA! PERCHE'?

    if(preprocessDebug) std::cout<<"PREPROCESSFRAME: cols = "<<fH->getFrame().cols<<", rows = "<<fH->getFrame().rows<<std::endl;

    rH->setGrayFrame(toGrayScale(fH->getFrame()));
}

void polyphemus::clearPreviousFaces()
{
    faces.clear();
    rH->clearFace();
}

void polyphemus::findNewFaces()
{
    if(findNewFacesDebug) std::cout<<"FIND_NEW_FACES: ENTERED "<<std::endl;

    haar->detectFaces(rH->getGrayFrame());
    if(findNewFacesDebug) std::cout<<"FIND_NEW_FACES: DETECTION DONE "<<std::endl;

    faces = haar->getFaces();
    if(findNewFacesDebug) std::cout<<"FIND_NEW_FACES: FACE RECEIVED "<<std::endl;

    if(faces.size()>0)
    {
        facesFound = true;
        if(!alreadyFace)
        {
            alreadyFace = true;
            newFace = true;
            cout<<"THIS IS A NEW FACE!!"<<endl;
        }
        else newFace = false;
    }
    else
    {
        facesFound = false;
        alreadyFace = false;
    }
}

void polyphemus::reduceFace()
{
    cv::Rect reducedRect = headRotation->thresholdImage(rH->getFaceROI());
    rH->setReducedFaceROI(reducedRect);       //TODO: evaluate this

#ifndef WEBSERVICE
#ifdef WITH_GUI
    if (gui != NULL)
    {
        gui->setReducedFaceFrame(rH->getReducedFaceROI());
    }
#endif //WITH_GUI
#endif //WEBSERVICE
}

void polyphemus::processFaceData()
{
    if(processFaceDataDebug) std::cout<<"PROCESSFACEDATA: "<<faces[0]<<std::endl;

    rH->setFaceROI(faces);
    if(processFaceDataDebug) std::cout<<"PROCESSFACEDATA: cols = "<<rH->getFaceROI().cols << ", rows = "<< rH->getFaceROI().rows<<std::endl;

    #ifndef WEBSERVICE
    #ifdef WITH_GUI
    if(gui!=NULL)
    {
        gui->setFaceFrame(rH->getFaceROI());
        gui->faceReceived = true;
    }
    #endif //WITH_GUI
    #endif //WEBSERVICE

}








void polyphemus::detectEyesEmpiric()
{
    rH->makeEmpiricEyes();
    std::vector<cv::Rect> eyes = rH->getEmpiricEyes();

    #ifndef WEBSERVICE
    #ifdef WITH_GUI
    if(gui!=NULL)
    {
        //For face frame
        gui->eyes = eyes;
        gui->showEyeZones();
    }
    #endif //WITH_GUI
    #endif //WEBSERVICE
}

void polyphemus::reduceEmpiricEyes()
{
    //Restrict to reduced empiric eye ROIs
    rH->setReducedEmpiricEyes();

    #ifndef WEBSERVICE
    #ifdef WITH_GUI
    if(gui!=NULL)
    {
        //For reduced frames
        gui->eyesReduced = rH->getReducedEmpiricEyes();
        gui->setLeftEmpiricReducedFrame(rH->getLeftEmpiricReducedROI());
        gui->setRightEmpiricReducedFrame(rH->getRightEmpiricReducedROI());
    }
    #endif //WITH_GUI
    #endif //WEBSERVICE
}





#ifndef WEBSERVICE
#ifdef WITH_GUI
void polyphemus::updateFace()
{

    if(gui!=NULL)
    {
        gui->updateWindow(FACE_WINDOW);
    }
}
#endif //WITH_GUI
#endif //WEBSERVICE


#ifndef WEBSERVICE
#ifdef WITH_GUI
void polyphemus::updateMain()
{
    if(gui!=NULL)
    {
        gui->updateWindow(MAIN_WINDOW);
    }
}
#endif //WITH_GUI
#endif

#ifndef WEBSERVICE
#ifdef WITH_GUI
void polyphemus::updateGaze()
{
    if(gui!=NULL)

    {
        gui->updateWindow(GAZE_WINDOW);
    }

}
#endif //WITH_GUI
#endif




#ifdef KETROD_DEBUG
void polyphemus::printFeatures()
{
            if (rH->hasFace())
            {
                cv::Rect n = rH->getFace();
                std::cout << "Faccia" << std::endl;
                std::cout << "Posizione (x,y): (" << n.x << "," << n.y << "). Dimensione (w,h): (" << n.width << "," << n.height << ")" << std::endl;
            }

            if (rH->hasNose())
            {
                cv::Rect n = rH->getNose();
                cv::Rect f = rH->getFace();
                std::cout << "Naso" << std::endl;
                std::cout << "Posizione (x,y): (" << n.x << "," << n.y << "). Dimensione (w,h): (" << n.width << "," << n.height << ")" << std::endl;
                                std::cout << "Dimensione rispetto a face: (" << (n.width*n.height) << ")" << std::endl;
                //std::cout << "Dimensione rispetto a mouth:" <<
            }

            if (rH->hasMouth())
            {
                cv::Rect n = rH->getMouth();
                cv::Rect f = rH->getFace();
                std::cout << "Bocca" << std::endl;
                std::cout << "Posizione (x,y): (" << n.x << "," << n.y << "). Dimensione (w,h): (" << n.width << "," << n.height << ")" << std::endl;
                std::cout << "Dimensione rispetto a face: (" << (n.width*n.height) << ")" << std::endl;
            }
            if (rH->hasLeftEye())
            {
                cv::Rect n = rH->getLeftEye();
                std::cout << "Left eye" << std::endl;
                std::cout << "Posizione (x,y): (" << n.x << "," << n.y << "). Dimensione (w,h): (" << n.width << "," << n.height << ")" << std::endl;
            }
            if (rH->hasRightEye())
            {
                cv::Rect n = rH->getRightEye();
                std::cout << "Right eye" << std::endl;
                std::cout << "Posizione (x,y): (" << n.x << "," << n.y << "). Dimensione (w,h): (" << n.width << "," << n.height << ")" << std::endl;
            }
}
#endif

#ifdef TEST_MODE
#ifdef AUTOMATIC_TEST
void polyphemus::updateErrorData()
{
    //get pupil coords in main frame
    double localLeftError, localRightError, localLeftRefError, localRightRefError;

    if(pD->leftFound)
    {
        localLeftError = sqrt((this->realLeftInFace->x - this->leftInFace->x)*(this->realLeftInFace->x - this->leftInFace->x) +
                              (this->realLeftInFace->y - this->leftInFace->y)*(this->realLeftInFace->y - this->leftInFace->y));

        if(localLeftError >= 7)
        {
            std::cout<<"False positive LEFT"<<std::endl;
            pD->leftFound = false;
            leftNotFound++;
        }
        else
        {
            leftAvgError += localLeftError;
            foundLeftPupils++;
        }

        if(pD->leftFound && refined)
        {
            localLeftRefError = sqrt((this->realLeftInFace->x - this->leftInFaceRefined->x)*(this->realLeftInFace->x - this->leftInFaceRefined->x) +
                                     (this->realLeftInFace->y - this->leftInFaceRefined->y)*(this->realLeftInFace->y - this->leftInFaceRefined->y));

            if(localLeftRefError >= 7)
            {
                std::cout<<"False positive REFINEMENT LEFT"<<std::endl;
                falseRefinementLeft++;
            }
            else
            {
                leftRefAvgError += localLeftRefError;
                goodRefinementLeft++;
            }
        }
    }
    else
    {
        leftNotFound++;
    }

    if(pD->rightFound)
    {
        localRightError = sqrt((this->realRightInFace->x - this->rightInFace->x)*(this->realRightInFace->x - this->rightInFace->x) +
                              (this->realRightInFace->y - this->rightInFace->y)*(this->realRightInFace->y - this->rightInFace->y));

        if(localRightError >= 7)
        {
            std::cout<<"False positive RIGHT"<<std::endl;
            pD->rightFound = false;
            rightNotFound++;
        }
        else
        {
            rightAvgError += localRightError;
            foundRightPupils++;
        }

        if(pD->rightFound && refined)
        {
            localRightRefError = sqrt((this->realRightInFace->x - this->rightInFaceRefined->x)*(this->realRightInFace->x - this->rightInFaceRefined->x) +
                                     (this->realRightInFace->y - this->rightInFaceRefined->y)*(this->realRightInFace->y - this->rightInFaceRefined->y));


            if(localRightRefError >= 7)
            {
                std::cout<<"False positive REFINEMENT RIGHT"<<std::endl;
                falseRefinementRight++;
            }
            else
            {
                rightRefAvgError += localRightRefError;
                goodRefinementRight++;
            }
        }
    }
    else
    {
        rightNotFound++;
    }



    return;
}

//Distance data update
void polyphemus::computeRealPupilDistances()
{
    realCoords = fH->getRealCoords();
    std::cout<<"RealCoords = "<<realCoords[0]<<","<<realCoords[1]<<" - "<<realCoords[2]<<","<<realCoords[3]<<std::endl;
    cv::Rect faceRect = rH->getFace();

    std::cout<<"faceRect.width = "<<faceRect.width<<std::endl;
    //std::cout<<"faceRect.center = "<<double(faceRect.width)/2 -0.5<<std::endl;
    std::cout<<"faceRect.tl = "<<faceRect.tl()<<std::endl;


    //Getting coords in face frame
    for(uint i=0; i<realCoords.size();i++)
    {
        if(i%2)
        {
            realCoords[i] -= faceRect.y;
            std::cout<<"y in face = "<<realCoords[i]<<std::endl;
        }
        else
        {
            realCoords[i] -= faceRect.x;
            std::cout<<"x in face = "<<realCoords[i]<<std::endl;
        }
    }

    if(realLeftInFace == NULL) realLeftInFace = new cv::Point();
    if(realRightInFace == NULL) realRightInFace = new cv::Point();

    realLeftInFace->x = realCoords[0];
    realLeftInFace->y = realCoords[1];
    realRightInFace->x = realCoords[2];
    realRightInFace->y = realCoords[3];

    gE->setRealDistances(realCoords[0],realCoords[2], faceRect);
}
#endif //AUTOMATIC_TEST
#endif //TEST_MODE

void polyphemus::setPupilData()
{
    if(leftInFace != NULL)
    {
        clearOldLeftPupil();
    }

    if(rightInFace != NULL)
    {
        clearOldRightPupil();
    }

    std::vector<cv::Rect> eyesReduced = rH->getReducedEmpiricEyes();


    if(pD->leftFound)
    {
        setLeftPupil(eyesReduced[0]);
        if(refined)
        {
            setLeftPupilRefined(eyesReduced[0]);
        }
    }
    if(pD->rightFound)
    {
        setRightPupil(eyesReduced[1]);
        if(refined)
        {
            setRightPupilRefined(eyesReduced[1]);
        }
    }
}


//Main function called to track gaze.
void polyphemus::trackGaze()
{
    #ifdef TEST_MODE
    char interruptChar;
    #endif

    bool stasmCheckWanted = false;


    #ifdef TRACKGAZE_DEBUG
    int h=0;
    std::cout<<"TRACKGAZE: ENTERED"<<std::endl;
    #endif

    while(fH->read())
    {
#ifdef TEST_MODE
        std::cout<<"TEST MODE: Working on image: "<<fH->getTotalRead()<<std::endl;


        //------------------------------------------------
        //  Initial frame elaboration
        preProcessFrame();

        //------------------------------------------------
        //  face recognition
        clearPreviousFaces();
        findNewFaces();

        //-------------------------------------------------
        //  face processing

        if(facesFound)
        {            
            processFaceData();

            #ifdef AUTOMATIC_TEST
            computeRealPupilDistances();
            #endif

            /****STASM PART*******/
            stasm();
            gE->setLM(landmarks, rH->getFaceROI());//, newFace);
            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": STASM"<<std::endl;
            #endif


            /****KETROD PART******/
            headRotation->positPoint(landmarks);


            /****JP PART**********/
            detectEyesEmpiric();
            reduceEmpiricEyes();

            //---------------------------------------------
            //  Find and set pupil data

            #ifndef ONLY_REAL_PUPIL
            //Detection
            //pD->findPupils(rH->getLeftEyeROI(), rH->getRightEyeROI());
            pD->findPupils(rH->getLeftEmpiricReducedROI(), rH->getRightEmpiricReducedROI());

            //Refinement
            if(refined)     pD->refinePupils();

            setPupilData();

            gE->predictHorizontalZone(G_CATCH);
            gE->predictVerticalZone(this->newFace);

            #ifdef AUTOMATIC_TEST
            updateErrorData();
            #endif

            #else  //ONLY_REAL_PUPILS is defined
            pD->bypassPupils(rH->getLeftEmpiricReducedROI(), rH->getRightEmpiricReducedROI());
            setPupilData();
            pD->setRealPupils(leftInEye, rightInEye);

            #ifdef WITH_GUI
            pD->updateReduced();
            #endif //WITH_GUI

            #endif //ONLY_REAL_PUPILS


            //----------------------------------------------
            //  Collect user opinion

            #ifdef GREATCATCH_TEST
            std::cout<<"Is this the correct place? [y,n,t]"<<std::endl;
            char userResponse = cv::waitKey(0);
            switch(userResponse)
            {
            case 'y':
                correctPlace++;
                break;
            case 'n':
                wrongPlace++;
                break;
            default:
                break;
            }
            #endif

            #ifdef AUTOMATIC_TEST
            hZoneEst = gE->getHorizontalResponse();
            hZoneTrue = fH->getHZone();
            std::cout<<"Zona VERA: "<<hZoneTrue<<"; zona STIMATA: "<<hZoneEst<<std::endl;
            if(hZoneEst == hZoneTrue) hGazeRight++;
            else hGazeWrong++;
            #endif

            //waitKey(0);
        }
        else
        {
            std::cout<<"No face found!"<<std::cout;

            #ifdef WITH_GUI
            updateMain();
            #endif //WITH_GUI
        }


        interruptChar = waitKey(1000);
        if(interruptChar == 'c') break;

        fH->prepareNextReading();
#endif

#if defined ACTIVE_CAM || defined WEBSERVICE

        #ifdef TRACKGAZE_DEBUG
        std::cout<<"TRACKGAZE: OK "<<h++<<": READ"<<std::endl;
        #endif

        preProcessFrame();
        #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": PREPROCESSFRAME"<<std::endl;
        #endif

        clearPreviousFaces();
        #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": CLEARPREVIOUSFACES"<<std::endl;
        #endif

        findNewFaces();
        #ifdef TRACKGAZE_DEBUG
        std::cout<<"TRACKGAZE: OK "<<h++<<": FINDNEWFACES"<<std::endl;
        #endif

        if(facesFound)
        {
            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": FACESFOUND IS TRUE"<<std::endl;
            #endif

            processFaceData();
            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": PROCESSFACEDATA"<<std::endl;
            #endif

            /****STASM PART*******/
            stasm();

            #ifndef WEBSERVICE
            #ifdef WITH_GUI
            if(stasmCheckWanted) drawSingleLandmark();
            #endif //WITH_GUI
            #endif //WEBSERVICE

            gE->setLM(landmarks, rH->getFaceROI());

            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": STASM"<<std::endl;
            #endif
            /*****END OF STASM***/

            #ifndef WEBSERVICE
            #ifdef WITH_GUI
            //Prepare gaze window
            if(gui!=NULL) gui->redrawGazeFrame();
            #endif //WITH_GUI
            #endif //WEBSERVICE

            /***** KETROD PART *****/
            //headRotation->processDistances(); --> TODO: remove?
            //#ifdef TRACKGAZE_DEBUG
            //std::cout<<"TRACKGAZE: OK "<<h++<<": PROCESSDISTANCES"<<std::endl;
            //#endif

			headRotation->positPoint(landmarks);
            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": POSITPOINT"<<std::endl;
            #endif

            //TODO --> remove?
            /*
            estimateAndShowGazeStartingPoint();
            #ifndef WEBSERVICE
                #ifdef TRACKGAZE_DEBUG
                std::cout<<"TRACKGAZE: OK "<<h++<<": EVALUATEANDSHOWGAZESTARTINGPOINT"<<std::endl;
                #endif
            #endif
            */

            #ifndef WEBSERVICE
            #ifdef WITH_GUI
            if(gui!=NULL)
            {
                gui->turnOnHeadZone(headRotation->getAngleX(), headRotation->getAngleY(), headRotation->getDistanceHead(), rH->getFace());
            }
            #endif //WITH_GUI
            #endif //WEBSERVICE

            /***** END OF KETROD PART *****/

            #ifndef WEBSERVICE
            #ifdef WITH_GUI
                updateMain();
                #ifdef TRACKGAZE_DEBUG
                std::cout<<"TRACKGAZE: OK "<<h++<<": UPDATEMAIN"<<std::endl;
                #endif
            #endif //WITH_GUI
            #endif //WEBSERVICE


            /***** JP PART *****/

            detectEyesEmpiric();
            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": DETECTEYESEMPIRIC"<<std::endl;
            #endif

            reduceEmpiricEyes();
            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": REDUCEEMPIRICEYES"<<std::endl;
            #endif


            pD->findPupils(rH->getLeftEmpiricReducedROI(), rH->getRightEmpiricReducedROI());
            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": FINDPUPILS"<<std::endl;
            #endif

            if(refined)
            {
                pD->refinePupils();
                #ifdef TRACKGAZE_DEBUG
                std::cout<<"TRACKGAZE: OK "<<h++<<": REFINEMENT DONE."<<std::endl;
                #endif
            }

            setPupilData();
            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": PUPIL DATA SET."<<std::endl;
            #endif

            gE->predictHorizontalZone(G_CATCH);
            gE->predictVerticalZone(this->newFace);

            #ifndef WEBSERVICE
            #ifdef WITH_GUI
            pD->updateReduced();
            #ifdef TRACKGAZE_DEBUG
            std::cout<<"TRACKGAZE: OK "<<h++<<": PUPILS SHOWED IN REDUCED WINDOWS."<<std::endl;
            #endif //TRACKGAZE_DEBUG

            if(gui!=NULL)
            {
                if(pD->leftFound)
                {
                    //gui->turnOnZone(gE->getHorizontalResponse(), gE->getVerticalResponse(), headRotation->getObsPointX(), headRotation->getObsPointY());
                    //estimatePupilGazeDisplacement(false);
                    #ifdef TRACKGAZE_DEBUG
                    std::cout<<"TRACKGAZE: OK "<<h++<<": ESTIMATEPUPILGAZEDISPLACEMENT"<<std::endl;
                    #endif
                    //gui->showFinalGaze(watchingPoint);

                    gui->turnOnEyeZone(gE->getHorizontalResponse(), gE->getVerticalResponse());
                    #ifdef GREATCATCH_TEST
                    updateGaze();
                    std::cout<<"Is this the correct place? [y,n,t]"<<std::endl;
                    char userResponse = cv::waitKey(0);
                    switch(userResponse)
                    {
                    case 'y':
                        correctPlace++;
                        break;
                    case 'n':
                        wrongPlace++;
                        break;
                    default:
                        break;
                    }
                    #endif //GREATCATCH_TEST
                }
                else
                {
                    #ifdef TRACKGAZE_DEBUG
                        #ifndef WEBSERVICE
                        std::cout<<"NO PUPIL"<<std::endl;
                        #endif
                    #endif
                }
            }
            #endif //WITH_GUI
            #endif //WEBSERVICE

            /***** END OF JP PART *****/

            /***** FINAL PREDICTION ******/

            #ifndef WEBSERVICE
            #ifdef WITH_GUI

            if(gui!=NULL)
            {
                updateGaze();
                    #ifdef TRACKGAZE_DEBUG
                    std::cout<<"TRACKGAZE: OK "<<h++<<": UPDATEGAZE"<<std::endl;
                    #endif
                updateFace();
                    #ifdef TRACKGAZE_DEBUG
                    std::cout<<"TRACKGAZE: OK "<<h++<<": UPDATEFACE"<<std::endl;
                    #endif
            }

            #endif //WITH_GUI
            #endif //WEBSERVICE
        }
        else {
            #ifndef WEBSERVICE
            std::cout<<"No face found!"<<std::endl;
            #ifdef WITH_GUI
            updateMain();
            #endif //WITH_GUI
            #endif //WEBSERVICE
        }
#endif
    }

    #ifndef WEBSERVICE
    std::cout<<"********* END OF CAPTURE **********"<<std::endl;
    #endif


    return;
}

void polyphemus::estimatePupilGazeDisplacement(bool refined)
{
    if(!refined)    gE->followGaze(leftInFace, rightInFace, rH->getFace());
    else            gE->followGaze(leftInFaceRefined, rightInFaceRefined, rH->getFace());

    return;
}


void polyphemus::setLeftPupilRefined(Rect eyeRect)
{
    leftInEyeRefined = new cv::Point();
    leftInEyeRefined = pD->getLeftPupilRefined();

    leftInFaceRefined = new cv::Point();
    leftInFaceRefined->x = leftInEyeRefined->x + eyeRect.x;
    leftInFaceRefined->y = leftInEyeRefined->y + eyeRect.y;

    gE->setPupils(leftInFaceRefined, NULL);
}

void polyphemus::setRightPupilRefined(Rect eyeRect)
{
    rightInEyeRefined = new cv::Point();
    rightInEyeRefined = pD->getRightPupilRefined();

    rightInFaceRefined = new cv::Point();
    rightInFaceRefined->x = rightInEyeRefined->x + eyeRect.x;
    rightInFaceRefined->y = rightInEyeRefined->y + eyeRect.y;

    gE->setPupils(NULL, rightInFaceRefined);
}

void polyphemus::setRightPupil(cv::Rect eyeRect)
{

    #ifdef SETRIGHTPUPIL_DEBUG
    int h=0;
    std::cout<<"SETRIGHTPUPIL: OK "<<h++<<": ENTERED"<<std::endl;
    #endif


#ifdef ONLY_REAL_PUPIL
    std::vector<int> realPupils = fH->getRealCoords();
    cv::Rect face = rH->getFace();
    rightInEye = new cv::Point(realPupils[2]-face.x-eyeRect.x, realPupils[3]-face.y-eyeRect.y);
    rightInEyeRefined = new cv::Point(*rightInEye);
    rightInFace = new cv::Point(realPupils[2]-face.x, realPupils[3]-face.y);
    rightInFaceRefined = new cv::Point(*rightInFace);
    std::cout<<"Right pupil was ("<<realCoords[2]<<","<<realCoords[3]<<") FROM FILE.";
    std::cout<<"    Is NOW ("<<rightInEye->x<<","<<rightInEye->y<<")"<<std::endl;
#else
    rightInEye = new cv::Point();
    rightInEye = pD->getRightPupil();

    rightInFace = new cv::Point();
    rightInFace->x = rightInEye->x + eyeRect.x;
    rightInFace->y = rightInEye->y + eyeRect.y;

    gE->setPupils(NULL, rightInFace);
#endif

    #ifdef SETRIGHTPUPIL_DEBUG
    std::cout<<"SETRIGHTPUPIL: OK "<<h++<<": RightInFace SET."<<std::endl;
    #endif
}



void polyphemus::setLeftPupil(cv::Rect eyeRect)
{
    #ifdef SETLEFTPUPIL_DEBUG
    int h=0;
    std::cout<<"SETLEFTPUPIL: OK "<<h++<<": ENTERED"<<std::endl;
    #endif

#ifdef ONLY_REAL_PUPIL
    std::vector<int> realPupils = fH->getRealCoords();
    cv::Rect face = rH->getFace();

    leftInEye = new cv::Point(realPupils[0]-face.x-eyeRect.x, realPupils[1]-face.y-eyeRect.y);
    leftInEyeRefined = new cv::Point(*leftInEye);
    leftInFace = new cv::Point(realPupils[0]-face.x, realPupils[1]-face.y);
    leftInFaceRefined = new cv::Point(*leftInFace);

    std::cout<<"Left pupil was ("<<realCoords[0]<<","<<realCoords[1]<<") FROM FILE.";
    std::cout<<"    Is NOW ("<<leftInEye->x<<","<<leftInEye->y<<")"<<std::endl;
#else
    leftInEye = new cv::Point();
    leftInEye = pD->getLeftPupil();

    leftInFace = new cv::Point();
    leftInFace->x = leftInEye->x + eyeRect.x;
    leftInFace->y = leftInEye->y + eyeRect.y;

    gE->setPupils(leftInFace, NULL);

#endif

    #ifdef SETLEFTPUPIL_DEBUG
    std::cout<<"SETLEFTPUPIL: OK "<<h++<<": LeftInFace SET."<<std::endl;
    #endif
}

void polyphemus::clearOldLeftPupil()
{
    #ifdef CLEARPUPILS_DEBUG
        int h=0;
    std::cout<<"CLEAROLDLEFTPUPIL: OK "<<h++<<": ENTERED"<<std::endl;
    #endif


    free(leftInFace);
    leftInFace = NULL;


    #ifdef CLEARPUPILS_DEBUG
    std::cout<<"CLEAROLDLEFTPUPIL: OK "<<h++<<": LEFTINFACE FREED"<<std::endl;
    #endif

    if(refined)
    {
        free(leftInFaceRefined);
        leftInFaceRefined = NULL;
    }

    #ifdef CLEARPUPILS_DEBUG
    std::cout<<"CLEAROLDLEFTPUPIL: OK "<<h++<<": LEFTINFACEREFINED FREED"<<std::endl;
    #endif
}

void polyphemus::clearOldRightPupil()
{

    #ifdef CLEARPUPILS_DEBUG
    int h=0;
    std::cout<<"CLEAROLDRIGHTPUPIL: OK "<<h++<<": ENTERED"<<std::endl;
    #endif

    free(rightInFace);
    rightInFace = NULL;

    #ifdef CLEARPUPILS_DEBUG
    std::cout<<"CLEAROLDRIGHTPUPIL: OK "<<h++<<": RIGHTINFACE FREED"<<std::endl;
    #endif

    if(refined)
    {
        free(rightInFaceRefined);
        rightInFaceRefined = NULL;
    }

    #ifdef CLEARPUPILS_DEBUG
    std::cout<<"CLEAROLDRIGHTPUPIL: OK "<<h++<<": RIGHTINFACEREFINED FREED"<<std::endl;
    #endif
}


//Release: call this method before closing program.
void polyphemus::release()
{
    delete haar;
    delete bin;

#ifdef TEST_MODE
#ifdef AUTOMATIC_TEST
    int totalReadImages = fH->getTotalRead();
    if(fH->trainingSetFinished())
    {
        totalReadImages--;
    }
#endif
#endif

    fH->release();
    delete fH;

    if(gE->isCollecting())
    {
        gE->printCollectedData();
    }

#ifndef WEBSERVICE
#ifdef WITH_GUI
    if(gui!=NULL)
    {
        gui->destroy(MAIN_WINDOW);
        gui->destroy(FACE_WINDOW);
        gui->destroy(GAZE_WINDOW);
        delete gui;
    }
#endif //WITH_GUI

    #ifdef FINDPUPILS_TEST
    pD->printTestResults();
    #endif

    #ifdef GREATCATCH_TEST
    std::cout<<"Gaze tracking was CORRECT: "<< ((double)correctPlace/(correctPlace+wrongPlace))*100 << "%"<<std::endl;
    std::cout<<"Gaze tracking was WRONG: "<< ((double)wrongPlace/(correctPlace+wrongPlace))*100 << "%"<<std::endl;
    #endif

#ifdef AUTOMATIC_TEST
    std::cout<<std::endl<<"+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"<<std::endl;
    std::cout<<"Image READ: "<<totalReadImages<<std::endl;
    std::cout<<"Left found: "<< foundLeftPupils<<std::endl;
    std::cout<<"Left NOT found: "<<leftNotFound<<std::endl;
    std::cout<<"Right found: "<< foundRightPupils<<std::endl;
    std::cout<<"Right NOT found: "<<rightNotFound<<std::endl;


    assert(leftNotFound+foundLeftPupils == totalReadImages);
    assert(rightNotFound+foundRightPupils == totalReadImages);
    leftAvgError/=foundLeftPupils;
    rightAvgError/=foundRightPupils;

    if(refined)
    {
        assert(goodRefinementLeft+falseRefinementLeft == foundLeftPupils);
        assert(goodRefinementRight+falseRefinementRight == foundRightPupils);
        leftRefAvgError/=goodRefinementLeft;
        rightRefAvgError/=goodRefinementRight;
    }

    std::cout<<"Left pupil was found in : "<<foundLeftPupils<<" images ("<<(double)foundLeftPupils/totalReadImages*100<<"%)"<<std::endl;
    if(foundLeftPupils > 0)    std::cout << "  Left avg error was: "<<leftAvgError<<" pixels."<<std::endl;
    std::cout<<"Right pupil was found in : "<<foundRightPupils<<" images ("<<(double)foundRightPupils/totalReadImages*100<<"%)"<<std::endl;
    if(foundRightPupils > 0)   std::cout<<"  Right avg error was: "<<rightAvgError<<" pixels."<<std::endl;
    if(foundLeftPupils != 0 && foundRightPupils != 0)
    {
        std::cout<<"MEAN ERROR IS: "<<(leftAvgError+rightAvgError)/2<<" pixels."<<std::endl;
    }
    if(refined)
    {
        std::cout<<"Left pupil was refined correctly in: "<<goodRefinementLeft<<" images ("<<(double)goodRefinementLeft/foundLeftPupils*100<<"%)"<<std::endl;
        if(goodRefinementLeft > 0) std::cout << "  Left REF avg error was: "<<leftRefAvgError<<" pixels."<<std::endl;
        std::cout<<"Right pupil was refined correctly in: "<<goodRefinementRight<<" images ("<<(double)goodRefinementRight/foundRightPupils*100<<"%)"<<std::endl;
        if(goodRefinementRight > 0) std::cout << "  Right REF avg error was: "<<rightRefAvgError<<" pixels."<<std::endl;
        if(goodRefinementLeft != 0 && goodRefinementRight != 0)
        {
            std::cout<<"MEAN REFINEMENT ERROR IS: "<<(leftRefAvgError+rightRefAvgError)/2<<" pixels."<<std::endl;
        }
    }
    std::cout<<"RIGHT PREDICTION = "<<hGazeRight << ", WRONG PREDICTION = "<<hGazeWrong<<std::endl<<"Gaze estimation was CORRECT: "<< ((double)hGazeRight/(hGazeRight+hGazeWrong))*100 << "%"<<std::endl;
#endif

    delete pD;

#endif


    return;
}

roiHandler* polyphemus::getRoiHandler() {
    return rH;
}


#ifndef WEBSERVICE
#ifdef WITH_GUI
void polyphemus::drawSingleLandmark()
{
    Mat img;
    bool debugPrint = false;

    rH->getFaceROI().copyTo(img);
    namedWindow("SLM", CV_WINDOW_NORMAL);

    for (int i = 0; i < stasm_NLANDMARKS; i++)
    {
        img.at<uchar>(cvRound(landmarks[i*2+1]), cvRound(landmarks[i*2])) = 255;
        imshow("SLM", img);
        if(debugPrint) cout<<"LM #"<< i<< endl;
    }
}
#endif //WITH_GUI
#endif //WEBSERVICE

void polyphemus::stasm()
{
    bool printLandmarks = false;
    int foundface;
    static const char* path = "../polyphemus/stasm4_1/data/testface.jpg";
    cv::Mat_<unsigned char> img;

    rH->getFaceROI().copyTo(img); //face is already found, use it; stasm detect faces will simply return the whole img rectangle

    if (!stasm_search_single(&foundface, landmarks,
            (char*)img.data, img.cols, img.rows, path, "../polyphemus/stasm4_1/data"))

    {
        #ifndef WEBSERVICE
        printf("Error in stasm_search_single: %s\n", stasm_lasterr());
        exit(1);
        #endif
    }

    if (!foundface) {
        #ifndef WEBSERVICE
        printf("No face found in %s\n", path);
        #endif
    }
    else
    {
        if(printLandmarks)
        {
            for (int i = 0; i < stasm_NLANDMARKS; i++)
            {
                cout<<"LM "<< i << ": ("<<landmarks[i*2] <<" , "<<landmarks[i*2+1]<<endl;
            }
        }

        stasm_force_points_into_image(landmarks, img.cols, img.rows);

        #ifndef WEBSERVICE
        #ifdef WITH_GUI
        // draw the landmarks on the image as white dots
        for (int i = 0; i < stasm_NLANDMARKS; i++)
            img(cvRound(landmarks[i*2+1]), cvRound(landmarks[i*2])) = 255;
        #endif //WITH_GUI
        #endif //WEBSERVICE

        if(printLandmarks)
        {
            for (int i = 0; i < stasm_NLANDMARKS; i++)
            {
                cout<<"LM "<< i << ": ("<<landmarks[i*2] <<" , "<<landmarks[i*2+1]<<")"<<endl;
            }
        }
    }

    #ifndef WEBSERVICE
    #ifdef WITH_GUI
    if(this->gui != NULL) cv::imshow("stasm minimal", img);
    #endif //WITH_GUI
    #endif //WEBSERVICE
}


float* polyphemus::getStasmLandmark(){
    return landmarks;
}

//-----------------------------------------
//              PRIVATE
//-----------------------------------------




//cv::Point* polyphemus::refinePupil(bool left)
//{
//    std::vector<double> distances;
//    int addedPoints = 1;
//    int radius = 1;
//    cv::Point* lastPupil, *newPupil;
//    cv::Mat roi = rH->getEmpiricReducedROI(left);
//    binaryAnalyzer::setInformation* set;
//    if(left)
//    {
//        newPupil = leftPupil;
//        set = leftRefinedSet;
//    }
//    else
//    {
//        newPupil = rightPupil;
//        set = rightRefinedSet;
//    }
//    set->elem.clear();
//    //Init boolean image
//    bool added[roi.rows][roi.cols];
//    for(int i=0; i<roi.rows; i++)
//    {
//        for(int j=0; j<roi.cols; j++)
//        {
//            added[i][j] = false;
//        }
//    }
//    //std::cout<<"INIT OK" << std::endl;
//    //Expand pupil circle
//    while(addedPoints > 0 && abs(set->width-set->height)<=3) //weak version
//    {
//        //std::cout << "NEW TOUR"<<std::endl;
//        lastPupil = newPupil;
//        addedPoints = 0;
//        int baseVal = roi.at<uchar>(*lastPupil);
//        //Move along next external circle and include new pupil candidates
//        for(int i=lastPupil->y-radius; i<=lastPupil->y+radius; i++)
//        {
//            for(int j=lastPupil->x-radius; j<=lastPupil->x+radius; j++)
//            {
//                int pVal = roi.at<uchar>(i,j);
//                if( pVal >= (float)0.5*baseVal && pVal < (float)1.5*baseVal && added[i][j] == false)
//                {
//                    (set->elem).push_back(cv::Point(j,i));
//                    added[i][j] = true;
//                    addedPoints++;
//                }
//            }
//        }
//        //std::cout<<"ADDED " << addedPoints << " POINTS."<< std::endl;
//        //Update set size
//        int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
//        for(int k=0; k<set->elem.size();k++)
//        {
//            int i = set->elem[k].y;
//            int j = set->elem[k].x;
//            if(j < minX) minX = j;
//            if(j > maxX) maxX = j;
//            if(i < minY) minY = i;
//            if(i > maxY) maxY = i;
//        }
//        set->width = maxX - minX +1;
//        set->height = maxY - minY +1;
//        /*
//                std::cout<<"X = (" << minX << "->" << maxX <<")" << std::endl;
//                std::cout<<"Y = (" << minY << "->" << maxY <<")" << std::endl;
//                std::cout<<"Width = " << set->width << ", height = " << set->height << std::endl;
//                */
//        //Compute coords for the next center
//        newPupil = new cv::Point(round(((double)(maxX+minX))/2), round(((double)(maxY+minY))/2));
//        if(newPupil->x != lastPupil-> x || newPupil->y != lastPupil->y)
//        {
//            radius--;
//        }
//        radius++;
//    }
//    cv::Mat pupilCircle(roi.rows, roi.cols, CV_8U);
//    for(int i=0; i<roi.rows; i++)
//    {
//        for(int j=0; j<roi.cols; j++)
//        {
//            if(added[i][j] == false)
//            {
//                pupilCircle.at<uchar>(i,j) = 255;
//            }
//            else
//            {
//                pupilCircle.at<uchar>(i,j) = 0;
//            }
//        }
//    }
//    /*
//                cv::String s;
//                s = left?"Left Circle":"Right Circle";
//                cv::namedWindow(s, CV_WINDOW_NORMAL);
//                imshow(s, pupilCircle);
//                cv::waitKey(0);
//                cv::destroyWindow(s);
//                */
//    return newPupil;
//}








//void polyphemus::twinkle2(bool left)
//{
//    if(left && leftPupil == NULL) return;
//    if(!left && rightPupil == NULL) return;
//    cv::Point pupil;
//    cv::Mat eyeROIRotated;
//    if(eyeLineFound)
//    {
//        if(left)
//        {
//            pupil = leftPupilAfterRotation;
//            eyeROIRotated = fH->getRotatedROI(true);
//            leftCorners.clear();
//        }
//        else
//        {
//            pupil = rightPupilAfterRotation;
//            eyeROIRotated = fH->getRotatedROI(false);
//            rightCorners.clear();
//        }
//    }
//    else
//    {
//        if(left)
//        {
//            pupil = *leftPupil;
//            eyeROIRotated = rH->getEmpiricReducedROI(true);
//            leftCorners.clear();
//        }
//        else
//        {
//            pupil = *rightPupil;
//            eyeROIRotated = rH->getEmpiricReducedROI(false);
//            rightCorners.clear();
//        }
//    }
//    cv::GaussianBlur(eyeROIRotated, eyeROIRotated, cv::Size(5,5), 10);
//    //Vai lungo la linea a sx della pupilla e memorizza i candidati
//    cv::Point temp;
//    uchar locVal, leftVal, rightVal;
//    std::vector<cv::Point> candPoint;
//    std::vector<uchar*> candVal;
//    uchar* vVal;
//    int color;
//    float avgLight = fH->getAvgLight(left);
//    if(avgLight < 128) color = 255;
//    else color = 0;
//    for(temp.x = pupil.x-3, temp.y = pupil.y; temp.x > 0; temp.x--)
//    {
//        locVal = eyeROIRotated.at<uchar>(temp);
//        leftVal = eyeROIRotated.at<uchar>(temp.y, temp.x-1);
//        rightVal = eyeROIRotated.at<uchar>(temp.y, temp.x+1);
//        if(locVal <= leftVal && locVal <= rightVal && locVal < avgLight)
//        {
//            candPoint.push_back(temp);
//            vVal = new uchar[3];
//            vVal[1] = locVal;
//            vVal[0] = leftVal;
//            vVal[2] = rightVal;
//            candVal.push_back(vVal);
//        }
//    }
//    //Left side
//    if(left) std::cout << "Elenco candidati P->L occhio sinistro:\n";
//    else std::cout << "Elenco candidati P->L occhio destro:\n";
//    //Select best candidate
//    int deltaMax = INT_MIN;
//    int deltaLoc;
//    cv::Point bestPoint(-1,-1);
//    for(int i =0; i<candPoint.size(); i++)
//    {
//        std::cout << "Punto (" << candPoint[i].x << ", " << candPoint[i].y << "):\n";
//        std::cout << "\t" << (int)(candVal[i][0]) << " - " << (int)(candVal[i][1]) << " - " << (int)(candVal[i][2]) << "\n";
//        deltaLoc = ((int)(candVal[i][0]) - (int)(candVal[i][1])) + ((int)(candVal[i][2]) - (int)(candVal[i][1]));
//        if(deltaLoc > deltaMax)
//        {
//            deltaMax = deltaLoc;
//            bestPoint = candPoint[i];
//        }
//    }
//    if(bestPoint.x != -1)
//    {
//        std::cout << "\n\tBEST POINT P->L = (" << bestPoint.x <<", " <<bestPoint.y <<")\n";
//        circle(eyeROIRotated, bestPoint, 2, color);
//        if(left)
//        {
//            leftCorners.push_back(bestPoint);
//        }
//        else
//        {
//            rightCorners.push_back(bestPoint);
//        }
//    }
//    else
//    {
//        std::cout << "\n\tNO CANDIDATES P->L FOUND!!!\n";
//    }
//    std::cout.flush();
//    //Right Side
//    //Vai lungo la linea a dx della pupilla e memorizza i candidati
//    candVal.clear();
//    candPoint.clear();
//    for(temp.x = pupil.x+3, temp.y = pupil.y; temp.x < eyeROIRotated.cols; temp.x++)
//    {
//        locVal = eyeROIRotated.at<uchar>(temp);
//        leftVal = eyeROIRotated.at<uchar>(temp.y, temp.x-1);
//        rightVal = eyeROIRotated.at<uchar>(temp.y, temp.x+1);
//        if(locVal <= leftVal && locVal <= rightVal && locVal < avgLight)
//        {
//            candPoint.push_back(temp);
//            vVal = new uchar[3];
//            vVal[1] = locVal;
//            vVal[0] = leftVal;
//            vVal[2] = rightVal;
//            candVal.push_back(vVal);
//        }
//    }
//    if(left) std::cout << "Elenco candidati P->R occhio sinistro:\n";
//    else std::cout << "Elenco candidati P->R occhio destro:\n";
//    //Select best candidate
//    deltaMax = INT_MIN;
//    bestPoint.x = -1;
//    bestPoint.y = -1;
//    for(int i =0; i<candPoint.size(); i++)
//    {
//        std::cout << "Punto (" << candPoint[i].x << ", " << candPoint[i].y << "):\n";
//        std::cout << "\t" << (int)(candVal[i][0]) << " - " << (int)(candVal[i][1]) << " - " << (int)(candVal[i][2]) << "\n";
//        deltaLoc = ((int)(candVal[i][0]) - (int)(candVal[i][1])) + ((int)(candVal[i][2]) - (int)(candVal[i][1]));
//        if(deltaLoc > deltaMax)
//        {
//            deltaMax = deltaLoc;
//            bestPoint = candPoint[i];
//        }
//    }
//    if(bestPoint.x != -1)
//    {
//        std::cout << "\n\tBEST POINT P->R = (" << bestPoint.x <<", " <<bestPoint.y <<")\n";
//        circle(eyeROIRotated, bestPoint, 2, color);
//        if(left)
//        {
//            leftCorners.push_back(bestPoint);
//        }
//        else
//        {
//            rightCorners.push_back(bestPoint);
//        }
//    }
//    else
//    {
//        std::cout << "\n\tNO CANDIDATES P->R FOUND!!!\n";
//    }
//    std::cout.flush();
//}

//void polyphemus::twinkle(bool left)
//{
//    if(left && leftPupil == NULL) return;
//    if(!left && rightPupil == NULL) return;
//    bool showPath = true;
//    cv::Point pupil;
//    cv::Mat eyeROIRotated;
//    binaryAnalyzer::setInformation pupilSet;
//    if(eyeLineFound)
//    {
//        if(left)
//        {
//            pupil = leftPupilAfterRotation;
//            eyeROIRotated = fH->getRotatedROI(true);
//            pupilSet = *leftPupilSet;
//        }
//        else
//        {
//            pupil = rightPupilAfterRotation;
//            eyeROIRotated = fH->getRotatedROI(false);
//            pupilSet = *rightPupilSet;
//        }
//    }
//    else
//    {
//        if(left)
//        {
//            pupil = *leftPupil;
//            eyeROIRotated = rH->getEmpiricReducedROI(true);
//            pupilSet = *leftPupilSet;
//        }
//        else
//        {
//            pupil = *rightPupil;
//            eyeROIRotated = rH->getEmpiricReducedROI(false);
//            pupilSet = *rightPupilSet;
//        }
//    }
//    //std::cout<<"8.1 - TWINKLE ASSIGNATION OK"<<std::endl;
//    // equalizeHist(face.eyes.leftEyeROIRotated, face.eyes.leftEyeROIRotated);
//    int maxY, minY, maxX, minX;
//    maxY = eyeROIRotated.rows-1;
//    minY = 2;
//    maxX = eyeROIRotated.cols-1;
//    minX = 2;
//    int verticalDelta = 10*eyeROIRotated.rows/100; //pupilSet.height/2;
//    int horizontalDelta = 15*eyeROIRotated.cols/100; //pupilSet.width/2;
//    if(pupil.x - horizontalDelta < minX ||
//            pupil.x + horizontalDelta >= maxX ||
//            pupil.y - 1.2*verticalDelta < minY ||
//            pupil.y + 1.2*verticalDelta >= maxY )
//    {
//        //std::cout<<"CRASHED!"<<std::endl; --> NECESSARY CHECK
//        return;
//    }
//    //UP LEFT
//    cv::Point ulPoints[3];
//    uchar ulValues[3];
//    cv::Point upLeft = pupil;
//    upLeft.y = upLeft.y - verticalDelta;
//    upLeft.x = upLeft.x - horizontalDelta;
//    cv::circle(eyeROIRotated, upLeft, 0, 255);
//    int startingHeightUp = upLeft.y;
//    int dxUL, dyUL;
//    bool firstUL = true;
//    cv::Point lastUL;
//    std::vector<cv::Point> ulVect;
//    //DOWN LEFT
//    cv::Point dlPoints[3];
//    uchar dlValues[3];
//    cv::Point downLeft = pupil;
//    downLeft.y = downLeft.y + 1.2*verticalDelta;
//    downLeft.x = downLeft.x - horizontalDelta;
//    cv::circle(eyeROIRotated, downLeft, 0, 255);
//    int startingHeightDown = downLeft.y;
//    int dxDL, dyDL;
//    bool firstDL = true;
//    cv::Point lastDL;
//    std::vector<cv::Point> dlVect;
//    //DOWN RIGHT
//    cv::Point drPoints[3];
//    uchar drValues[3];
//    cv::Point downRight = pupil;
//    downRight.y = startingHeightDown;
//    downRight.x = downRight.x + horizontalDelta;
//    cv::circle(eyeROIRotated, downRight, 0, 255);
//    int dxDR, dyDR;
//    bool firstDR = true;
//    cv::Point lastDR;
//    std::vector<cv::Point> drVect;
//    //UP RIGHT
//    cv::Point urPoints[3];
//    uchar urValues[3];
//    cv::Point upRight = pupil;
//    upRight.y = startingHeightUp;
//    upRight.x = upRight.x + horizontalDelta;
//    cv::circle(eyeROIRotated, upRight, 0, 255);
//    int dxUR, dyUR;
//    bool firstUR = true;
//    cv::Point lastUR;
//    std::vector<cv::Point> urVect;
//    //std::cout<<"CRASHED1!"<<std::endl;
//    bool endUL = false, endUR = false, endDL = false, endDR = false;
//    int ctr = 0;
//    int inPath[eyeROIRotated.rows][eyeROIRotated.cols];
//    int i, j;
//    for(i=0; i<eyeROIRotated.rows; i++)
//    {
//        for(j=0;j<eyeROIRotated.cols; j++)
//        {
//            inPath[i][j] = 0;
//        }
//    }
//    while(!endUL || !endUR || !endDR || !endDL)
//    {
//#ifdef HAVING_DEBUG
//        printf("Stage %d:\n", ctr++);
//#endif
//        //UP LEFT CORNER
//        if(!endUL)
//        {
//            if(upLeft.x > minX && upLeft.y > minY && upLeft.y < maxY)
//            {
//                //std::cout<<"UL ENTERED "<<std::endl;
//                dyUL = abs(upLeft.y - pupil.y);//startingHeightUp);
//                dxUL = abs(upLeft.x - pupil.x);
//                if(dyUL > 0.75*dxUL) upLeft.x--;
//                else
//                {
//                    ulPoints[0] = upLeft;
//                    ulPoints[0].x--;
//                    ulPoints[0].y--;
//                    ulPoints[1] = upLeft;
//                    ulPoints[1].x--;
//                    ulPoints[2] = upLeft;
//                    ulPoints[2].x--;
//                    ulPoints[2].y++;
//                    ulValues[0] = eyeROIRotated.at<uchar>(ulPoints[0]);
//                    ulValues[1] = eyeROIRotated.at<uchar>(ulPoints[1]);
//                    ulValues[2] = eyeROIRotated.at<uchar>(ulPoints[2]);
//#ifdef HAVING_DEBUG
//                    printf("ul[0].x = %d, ul[0].y = %d\n", ulPoints[0].x, ulPoints[0].y);
//                    printf("ul[1].x = %d, ul[1].y = %d\n", ulPoints[1].x, ulPoints[1].y);
//                    printf("ul[2].x = %d, ul[2].y = %d\n", ulPoints[2].x, ulPoints[2].y);
//                    printf("Val[0] = %d, Val[1] = %d, Val[2] = %d\n", (int)(ulValues[0]), (int)(ulValues[1]), (int)(ulValues[2]));
//#endif
//                    upLeft = ulPoints[darkest(ulValues, 3, true)];
//                }
//                (inPath[upLeft.y][upLeft.x])++;
//                if(gui && showPath) gui->showTwinklePath(upLeft, left, eyeLineFound);
//                if(firstUL)
//                {
//                    lastUL = upLeft;
//                    firstUL = false;
//                }
//                else
//                {
//                    ulVect.push_back(lastUL);
//                    if(inPath[upLeft.y][upLeft.x] == 2 ||
//                            inPath[upLeft.y+1][upLeft.x-1] == 1 ||
//                            inPath[upLeft.y+1][upLeft.x] == 1 ||
//                            (inPath[upLeft.y+1][upLeft.x+1] == 1 && upLeft.y+1 != lastUL.y))
//                    {
//                        endUL = true;
//                        endDL = true;
//                        ulVect.push_back(upLeft);
//                    }
//                    lastUL = upLeft;
//                }
//            }
//            else
//            {
//                endUL = true;
//            }
//        }
//        //std::cout<<"CRASHED 2 UL!"<<std::endl;
//        //UP RIGHT CORNER
//        if(!endUR)
//        {
//            if(upRight.x < maxX && upRight.y > minY && upRight.y < maxY)
//            {
//                dyUR = abs(upRight.y - pupil.y);//startingHeightUp);
//                dxUR = abs(upRight.x - pupil.x);
//                if(dyUR > 0.75*dxUR) upRight.x++;
//                else
//                {
//                    urPoints[0] = upRight;
//                    urPoints[0].x++;
//                    urPoints[0].y--;
//                    urPoints[1] = upRight;
//                    urPoints[1].x++;
//                    urPoints[2] = upRight;
//                    urPoints[2].x++;
//                    urPoints[2].y++;
//                    urValues[0] = eyeROIRotated.at<uchar>(urPoints[0]);
//                    urValues[1] = eyeROIRotated.at<uchar>(urPoints[1]);
//                    urValues[2] = eyeROIRotated.at<uchar>(urPoints[2]);
//#ifdef HAVING_DEBUG
//                    printf("ur[0].x = %d, ur[0].y = %d\n", urPoints[0].x, urPoints[0].y);
//                    printf("ur[1].x = %d, ur[1].y = %d\n", urPoints[1].x, urPoints[1].y);
//                    printf("ur[2].x = %d, ur[2].y = %d\n", urPoints[2].x, urPoints[2].y);
//                    printf("Val[0] = %d, Val[1] = %d, Val[2] = %d\n", (int)(urValues[0]), (int)(urValues[1]), (int)(urValues[2]));
//#endif
//                    upRight= urPoints[darkest(urValues, 3, true)];
//                }
//                (inPath[upRight.y][upRight.x])++;
//                if(gui && showPath) gui->showTwinklePath(upRight, left, eyeLineFound);
//                if(firstUR)
//                {
//                    lastUR = upRight;
//                    firstUR = false;
//                }
//                else
//                {
//                    urVect.push_back(lastUR);
//                    if(inPath[upRight.y][upRight.x] == 2 ||
//                            (inPath[upRight.y+1][upRight.x-1] == 1 && upRight.y+1 != lastUR.y )||
//                            inPath[upRight.y+1][upRight.x] == 1 ||
//                            inPath[upRight.y+1][upRight.x+1] == 1 )
//                    {
//                        endUR = true;
//                        endDR = true;
//                        urVect.push_back(upRight);
//                    }
//                    lastUR = upRight;
//                }
//            }
//            else
//            {
//                endUR = true;
//            }
//        }
//        //std::cout<<"CRASHED 2 UR!"<<std::endl;
//        //DOWNLEFT CORNER
//        if(!endDL)
//        {
//            if(downLeft.x > minX && downLeft.y > minY && downLeft.y < maxY)
//            {
//                dyDL = abs(downLeft.y - pupil.y);
//                dxDL = abs(downLeft.x - pupil.x);
//                if(dyDL > 1.75*dxDL) downLeft.x--;
//                else
//                {
//                    dlPoints[0] = downLeft;
//                    dlPoints[0].x--;
//                    dlPoints[0].y--;
//                    dlPoints[1] = downLeft;
//                    dlPoints[1].x--;
//                    dlPoints[2] = downLeft;
//                    dlPoints[2].x--;
//                    dlPoints[2].y++;
//                    dlValues[0] = eyeROIRotated.at<uchar>(dlPoints[0]);
//                    dlValues[1] = eyeROIRotated.at<uchar>(dlPoints[1]);
//                    dlValues[2] = eyeROIRotated.at<uchar>(dlPoints[2]);
//#ifdef HAVING_DEBUG
//                    printf("dl[0].x = %d, dl[0].y = %d\n", dlPoints[0].x, dlPoints[0].y);
//                    printf("dl[1].x = %d, dl[1].y = %d\n", dlPoints[1].x, dlPoints[1].y);
//                    printf("dl[2].x = %d, dl[2].y = %d\n", dlPoints[2].x, dlPoints[2].y);
//                    printf("Val[0] = %d, Val[1] = %d, Val[2] = %d\n", (int)(dlValues[0]), (int)(dlValues[1]), (int)(dlValues[2]));
//#endif
//                    downLeft = dlPoints[darkest(dlValues, 3, false)];
//                }
//                (inPath[downLeft.y][downLeft.x])++;
//                if(gui && showPath) gui->showTwinklePath(downLeft, left, eyeLineFound);
//                if(firstDL)
//                {
//                    lastDL = downLeft;
//                    firstDL = false;
//                }
//                else
//                {
//                    dlVect.push_back(lastDL);
//                    if(inPath[downLeft.y][downLeft.x] == 2 ||
//                            inPath[downLeft.y-1][downLeft.x-1] == 1 ||
//                            inPath[downLeft.y-1][downLeft.x] == 1 ||
//                            (inPath[downLeft.y-1][downLeft.x+1] == 1 && downLeft.y-1 != lastDL.y) ||
//                            inPath[downLeft.y-2][downLeft.x] == 1 ||
//                            inPath[downLeft.y-2][downLeft.x+1] == 1)
//                    {
//                        endUL = true;
//                        endDL = true;
//                        dlVect.push_back(downLeft);
//                    }
//                    lastDL = downLeft;
//                }
//            }
//            else
//            {
//                endDL = true;
//            }
//        }
//        //std::cout<<"CRASHED 2 DL!"<<std::endl;
//        //DOWNRIGHT CORNER
//        if(!endDR)
//        {
//            if(downRight.x < maxX && downRight.y > minY && downRight.y < maxY)
//            {
//                dyDR = abs(downRight.y - pupil.y); //startingHeightDown);
//                dxDR = abs(downRight.x - pupil.x);
//                if(dyDR > 1.75*dxDR) downRight.x++;
//                else
//                {
//                    drPoints[0] = downRight;
//                    drPoints[0].x++;
//                    drPoints[0].y--;
//                    drPoints[1] = downRight;
//                    drPoints[1].x++;
//                    drPoints[2] = downRight;
//                    drPoints[2].x++;
//                    drPoints[2].y++;
//                    drValues[0] = eyeROIRotated.at<uchar>(drPoints[0]);
//                    drValues[1] = eyeROIRotated.at<uchar>(drPoints[1]);
//                    drValues[2] = eyeROIRotated.at<uchar>(drPoints[2]);
//#ifdef HAVING_DEBUG
//                    printf("dr[0].x = %d, dr[0].y = %d\n", drPoints[0].x, drPoints[0].y);
//                    printf("dr[1].x = %d, dr[1].y = %d\n", drPoints[1].x, drPoints[1].y);
//                    printf("dr[2].x = %d, dr[2].y = %d\n", drPoints[2].x, drPoints[2].y);
//                    printf("Val[0] = %d, Val[1] = %d, Val[2] = %d\n", (int)(drValues[0]), (int)(drValues[1]), (int)(drValues[2]));
//#endif
//                    downRight = drPoints[darkest(drValues, 3, false)];
//                }
//                (inPath[downRight.y][downRight.x])++;
//                if(gui && showPath) gui->showTwinklePath(downRight, left, eyeLineFound);
//                if(firstDR)
//                {
//                    lastDR = downRight;
//                    firstDR = false;
//                }
//                else
//                {
//                    drVect.push_back(lastDR);
//                    if(inPath[downRight.y][downRight.x] == 2 ||
//                            (inPath[downRight.y-1][downRight.x-1] == 1 && downRight.y-1 != lastDR.y) ||
//                            inPath[downRight.y-1][downRight.x] == 1 ||
//                            inPath[downRight.y-1][downRight.x+1] == 1 ||
//                            inPath[downRight.y-2][downRight.x] == 1 ||
//                            inPath[downRight.y-2][downRight.x-1] == 1)
//                    {
//                        endDR = true;
//                        endUR = true;
//                        drVect.push_back(downRight);
//                    }
//                    lastDR = downRight;
//                }
//            }
//            else
//            {
//                endDR = true;
//            }
//        }
//        //std::cout<<"CRASHED 2 DR!"<<std::endl;
//        /*
//                //Set colors (debug purpose)
//                eyeROIRotated.at<uchar>(upLeft) = 0;
//                eyeROIRotated.at<uchar>(downLeft) = 50;
//                eyeROIRotated.at<uchar>(downRight) = 0;
//                eyeROIRotated.at<uchar>(upRight) = 50;
//                printf("upLeft.x = %d, upLeft.y = %d\n", upLeft.x, upLeft.y);
//                printf("upRight.x = %d, upRight.y = %d\n", upRight.x, upRight.y);
//                printf("downLeft.x = %d, downLeft.y = %d\n", downLeft.x, downLeft.y);
//                printf("downRight.x = %d, downRight.y = %d\n", downRight.x, downRight.y);
//                */
//    }
//    //END WHILE
//    //std::cout<<"CRASHED BIG WHILE 1!"<<std::endl;
//    /*
//                std::cout<<"UL SIZE: "<<ulVect.size()<<std::endl;
//                std::cout<<"UR SIZE: "<<urVect.size()<<std::endl;
//                std::cout<<"DL SIZE: "<<dlVect.size()<<std::endl;
//                std::cout<<"DR SIZE: "<<drVect.size()<<std::endl;
//                */
//    cv::Point lext1;
//    cv::Point lext2;
//    cv::Point rext1;
//    cv::Point rext2;
//    if(ulVect.size() == 0 || dlVect.size() == 0)
//    {
//        lext1.x = upLeft.x;
//        lext1.y = (upLeft.y+downLeft.y)/2;
//        lext2.x = downLeft.x;
//        lext2.y = (upLeft.y+downLeft.y)/2;
//    }
//    else
//    {
//        lext1 = ulVect[ulVect.size()-1];
//        lext2 = dlVect[dlVect.size()-1];
//    }
//    if(urVect.size() == 0 || drVect.size() == 0)
//    {
//        rext1.x = upRight.x;
//        rext1.y = (upRight.y+downRight.y)/2;
//        rext2.x = downRight.x;
//        rext2.y = (upRight.y+downRight.y)/2;
//    }
//    else
//    {
//        rext1 = urVect[urVect.size()-1];
//        rext2 = drVect[drVect.size()-1];
//    }
//    //std::cout<<"CRASHED BIG WHILE 2!"<<std::endl;
//    //std::cout<<"L1 = "<<lext1<<", L2 = "<<lext2<<std::endl;
//    //Assign ranges for the eye --> notice that coords are for rotated ROI!
//    if(left)
//    {
//        leftCorners.push_back(lext1.x < lext2.x ? lext1 : lext2);
//        leftCorners.push_back(rext1.x > rext2.x ? rext1 : rext2);
//        //std::cout<<"CRASHED 3 L1!"<<std::endl;
//        /*
//                std::cout<<"Range for left eye: (ROW " << leftCorners[0].y << ",COL " <<leftCorners[0].x <<
//                ") -> (ROW " << leftCorners[1].y << ",COL " << leftCorners[1].x << ")\n";
//                std::cout.flush();
//                */
//        //std::cout<<"CRASHED 3 L2!"<<std::endl;
//    }
//    else
//    {
//        rightCorners.push_back(lext1.x < lext2.x ? lext1 : lext2);
//        rightCorners.push_back(rext1.x > rext2.x ? rext1 : rext2);
//        //std::cout<<"CRASHED 3 R1!"<<std::endl;
//        /*
//                std::cout<<"Range for right eye: (ROW " << rightCorners[0].y << ",COL " <<rightCorners[0].x <<
//                ") -> (ROW " << rightCorners[1].y << ",COL " << rightCorners[1].x << ")\n";
//                std::cout.flush();
//                */
//        //std::cout<<"CRASHED 3 R2!"<<std::endl;
//    }
//    return;
//}




//void polyphemus::twinkleMask(bool left)
//{
//    if(left && leftPupil == NULL) return;
//    if(!left && rightPupil == NULL) return;
//    bool showPath = false;
//    cv::Point pupil;
//    cv::Mat eyeROIRotated;
//    binaryAnalyzer::setInformation pupilSet;
//    if(eyeLineFound)
//    {
//        if(left)
//        {
//            pupil = leftPupilAfterRotation;
//            eyeROIRotated = fH->getRotatedROI(true);
//            pupilSet = *leftPupilSet;
//        }
//        else
//        {
//            pupil = rightPupilAfterRotation;
//            eyeROIRotated = fH->getRotatedROI(false);
//            pupilSet = *rightPupilSet;
//        }
//    }
//    else
//    {
//        if(left)
//        {
//            pupil = *leftPupil;
//            eyeROIRotated = rH->getEmpiricReducedROI(true);
//            pupilSet = *leftPupilSet;
//        }
//        else
//        {
//            pupil = *rightPupil;
//            eyeROIRotated = rH->getEmpiricReducedROI(false);
//            pupilSet = *rightPupilSet;
//        }
//    }
//    //std::cout<<"8.1 - TWINKLE ASSIGNATION OK"<<std::endl;
//    // equalizeHist(face.eyes.leftEyeROIRotated, face.eyes.leftEyeROIRotated);
//    int maxY, minY, maxX, minX;
//    maxY = eyeROIRotated.rows-2;
//    minY = 3;
//    maxX = eyeROIRotated.cols-2;
//    minX = 3;
//    int verticalDelta = 15*eyeROIRotated.rows/100;
//    int horizontalDelta = 15*eyeROIRotated.cols/100;
//    if(pupil.x - horizontalDelta < minX ||
//            pupil.x + horizontalDelta >= maxX ||
//            pupil.y - 1.2*verticalDelta < minY ||
//            pupil.y + 1.2*verticalDelta >= maxY )
//    {
//        //std::cout<<"CRASHED!"<<std::endl; // --> NECESSARY CHECK
//        return;
//    }
//    std::vector<cv::Point> candPoints;
//    //UP LEFT
//    cv::Point upLeft = pupil;
//    upLeft.y = upLeft.y - verticalDelta;
//    upLeft.x = upLeft.x - horizontalDelta;
//    cv::circle(eyeROIRotated, upLeft, 0, 255);
//    int startingHeightUp = upLeft.y;
//    int dxUL, dyUL;
//    bool firstUL = true;
//    cv::Point lastUL;
//    std::vector<cv::Point> ulVect;
//    //DOWN LEFT
//    cv::Point downLeft = pupil;
//    downLeft.y = downLeft.y + 1.2*verticalDelta;
//    downLeft.x = downLeft.x - horizontalDelta;
//    cv::circle(eyeROIRotated, downLeft, 0, 255);
//    int startingHeightDown = downLeft.y;
//    int dxDL, dyDL;
//    bool firstDL = true;
//    cv::Point lastDL;
//    std::vector<cv::Point> dlVect;
//    //DOWN RIGHT
//    cv::Point downRight = pupil;
//    downRight.y = startingHeightDown;
//    downRight.x = downRight.x + horizontalDelta;
//    cv::circle(eyeROIRotated, downRight, 0, 255);
//    int dxDR, dyDR;
//    bool firstDR = true;
//    cv::Point lastDR;
//    std::vector<cv::Point> drVect;
//    //UP RIGHT
//    cv::Point upRight = pupil;
//    upRight.y = startingHeightUp;
//    upRight.x = upRight.x + horizontalDelta;
//    cv::circle(eyeROIRotated, upRight, 0, 255);
//    int dxUR, dyUR;
//    bool firstUR = true;
//    cv::Point lastUR;
//    std::vector<cv::Point> urVect;
//    cv::Point* temPoint;
//    std::vector<const bool(*)[3][3]> m;
//    bool endUL = false, endUR = false, endDL = false, endDR = false;
//    int inPath[eyeROIRotated.rows][eyeROIRotated.cols];
//    int i, j;
//    for(i=0; i<eyeROIRotated.rows; i++)
//    {
//        for(j=0;j<eyeROIRotated.cols; j++)
//        {
//            inPath[i][j] = 0;
//        }
//    }
//    while(!endUL || !endUR || !endDR || !endDL)
//    {
//        //UP LEFT CORNER
//        if(!endUL)
//        {
//            if(upLeft.x > minX && upLeft.y > minY && upLeft.y < maxY)
//            {
//                //std::cout<<"UL ENTERED "<<std::endl;
//                dyUL = abs(upLeft.y - pupil.y);
//                dxUL = abs(upLeft.x - pupil.x);
//                if(dyUL > 0.75*dxUL) upLeft.x--;
//                else
//                {
//                    candPoints.clear();
//                    m.clear();
//                    temPoint = new cv::Point(upLeft);
//                    temPoint->x--;
//                    temPoint->y--;
//                    candPoints.push_back(*temPoint);
//                    temPoint = new cv::Point(upLeft);
//                    temPoint->x--;
//                    candPoints.push_back(*temPoint);
//                    temPoint = new cv::Point(upLeft);
//                    temPoint->x--;
//                    temPoint->y++;
//                    candPoints.push_back(*temPoint);
//                    m.push_back(&MASKGENERAL);
//                    m.push_back(&MASKGENERAL);
//                    m.push_back(&MASKGENERAL);
//                    /*
//                m.push_back(&MASKUL);
//                m.push_back(&MASKLEFT);
//                m.push_back(&MASKDL);
//                */
//                    upLeft = darkestMask(eyeROIRotated, candPoints, m);
//                }
//                (inPath[upLeft.y][upLeft.x])++;
//                if(gui && showPath) gui->showTwinklePath(upLeft, left, eyeLineFound);
//                if(firstUL)
//                {
//                    lastUL = upLeft;
//                    firstUL = false;
//                }
//                else
//                {
//                    ulVect.push_back(lastUL);
//                    if(inPath[upLeft.y][upLeft.x] == 2 ||
//                            inPath[upLeft.y+1][upLeft.x-1] == 1 ||
//                            inPath[upLeft.y+1][upLeft.x] == 1 ||
//                            (inPath[upLeft.y+1][upLeft.x+1] == 1 && upLeft.y+1 != lastUL.y))
//                    {
//                        endUL = true;
//                        endDL = true;
//                        ulVect.push_back(upLeft);
//                    }
//                    lastUL = upLeft;
//                }
//            }
//            else
//            {
//                endUL = true;
//            }
//        }
//        // std::cout<<"PASSED UL!"<<std::endl;
//        //UP RIGHT CORNER
//        if(!endUR)
//        {
//            if(upRight.x < maxX && upRight.y > minY && upRight.y < maxY)
//            {
//                //std::cout<<"Entered UR"<<std::endl;
//                dyUR = abs(upRight.y - pupil.y);
//                dxUR = abs(upRight.x - pupil.x);
//                if(dyUR > 0.75*dxUR) upRight.x++;
//                else
//                {
//                    candPoints.clear();
//                    m.clear();
//                    temPoint = new cv::Point(upRight);
//                    temPoint->x++;
//                    temPoint->y--;
//                    candPoints.push_back(*temPoint);
//                    temPoint = new cv::Point(upRight);
//                    temPoint->x++;
//                    candPoints.push_back(*temPoint);
//                    temPoint = new cv::Point(upRight);
//                    temPoint->x++;
//                    temPoint->y++;
//                    candPoints.push_back(*temPoint);
//                    m.push_back(&MASKGENERAL);
//                    m.push_back(&MASKGENERAL);
//                    m.push_back(&MASKGENERAL);
//                    /*
//                m.push_back(&MASKUR);
//                m.push_back(&MASKRIGHT);
//                m.push_back(&MASKDR);
//                */
//                    upRight = darkestMask(eyeROIRotated, candPoints, m);
//                }
//                (inPath[upRight.y][upRight.x])++;
//                if(gui && showPath) gui->showTwinklePath(upRight, left, eyeLineFound);
//                if(firstUR)
//                {
//                    lastUR = upRight;
//                    firstUR = false;
//                }
//                else
//                {
//                    urVect.push_back(lastUR);
//                    if(inPath[upRight.y][upRight.x] == 2 ||
//                            (inPath[upRight.y+1][upRight.x-1] == 1 && upRight.y+1 != lastUR.y )||
//                            inPath[upRight.y+1][upRight.x] == 1 ||
//                            inPath[upRight.y+1][upRight.x+1] == 1 )
//                    {
//                        endUR = true;
//                        endDR = true;
//                        urVect.push_back(upRight);
//                    }
//                    lastUR = upRight;
//                }
//            }
//            else
//            {
//                endUR = true;
//            }
//        }
//        //std::cout<<"PASSED UR!"<<std::endl;
//        //DOWNLEFT CORNER
//        if(!endDL)
//        {
//            if(downLeft.x > minX && downLeft.y > minY && downLeft.y < maxY)
//            {
//                //std::cout<<"Entered DL"<<std::endl;
//                dyDL = abs(downLeft.y - pupil.y);
//                dxDL = abs(downLeft.x - pupil.x);
//                if(dyDL > 1.75*dxDL) downLeft.x--;
//                else
//                {
//                    candPoints.clear();
//                    m.clear();
//                    temPoint = new cv::Point(downLeft);
//                    temPoint->x--;
//                    temPoint->y--;
//                    candPoints.push_back(*temPoint);
//                    temPoint = new cv::Point(downLeft);
//                    temPoint->x--;
//                    candPoints.push_back(*temPoint);
//                    temPoint = new cv::Point(downLeft);
//                    temPoint->x--;
//                    temPoint->y++;
//                    candPoints.push_back(*temPoint);
//                    m.push_back(&MASKGENERAL);
//                    m.push_back(&MASKGENERAL);
//                    m.push_back(&MASKGENERAL);
//                    /*
//                m.push_back(&MASKUL);
//                m.push_back(&MASKLEFT);
//                m.push_back(&MASKDL);
//                */
//                    downLeft = darkestMask(eyeROIRotated, candPoints, m);
//                }
//                (inPath[downLeft.y][downLeft.x])++;
//                if(gui && showPath) gui->showTwinklePath(downLeft, left, eyeLineFound);
//                if(firstDL)
//                {
//                    lastDL = downLeft;
//                    firstDL = false;
//                }
//                else
//                {
//                    dlVect.push_back(lastDL);
//                    if(inPath[downLeft.y][downLeft.x] == 2 ||
//                            inPath[downLeft.y-1][downLeft.x-1] == 1 ||
//                            inPath[downLeft.y-1][downLeft.x] == 1 ||
//                            (inPath[downLeft.y-1][downLeft.x+1] == 1 && downLeft.y-1 != lastDL.y) ||
//                            inPath[downLeft.y-2][downLeft.x] == 1 ||
//                            inPath[downLeft.y-2][downLeft.x+1] == 1)
//                    {
//                        endUL = true;
//                        endDL = true;
//                        dlVect.push_back(downLeft);
//                    }
//                    lastDL = downLeft;
//                }
//            }
//            else
//            {
//                endDL = true;
//            }
//        }
//        //std::cout<<"PASSED DL!"<<std::endl;
//        //DOWNRIGHT CORNER
//        if(!endDR)
//        {
//            if(downRight.x < maxX && downRight.y > minY && downRight.y < maxY)
//            {
//                //std::cout<<"Entered DR"<<std::endl;
//                dyDR = abs(downRight.y - pupil.y);
//                dxDR = abs(downRight.x - pupil.x);
//                if(dyDR > 1.75*dxDR) downRight.x++;
//                else
//                {
//                    candPoints.clear();
//                    m.clear();
//                    temPoint = new cv::Point(downRight);
//                    temPoint->x++;
//                    temPoint->y--;
//                    candPoints.push_back(*temPoint);
//                    temPoint = new cv::Point(downRight);
//                    temPoint->x++;
//                    candPoints.push_back(*temPoint);
//                    temPoint = new cv::Point(downRight);
//                    temPoint->x++;
//                    temPoint->y++;
//                    candPoints.push_back(*temPoint);
//                    m.push_back(&MASKGENERAL);
//                    m.push_back(&MASKGENERAL);
//                    m.push_back(&MASKGENERAL);
//                    /*
//                m.push_back(&MASKUR);
//                m.push_back(&MASKRIGHT);
//                m.push_back(&MASKDR);
//                */
//                    downRight = darkestMask(eyeROIRotated, candPoints, m);
//                }
//                (inPath[downRight.y][downRight.x])++;
//                if(gui && showPath) gui->showTwinklePath(downRight, left, eyeLineFound);
//                if(firstDR)
//                {
//                    lastDR = downRight;
//                    firstDR = false;
//                }
//                else
//                {
//                    drVect.push_back(lastDR);
//                    if(inPath[downRight.y][downRight.x] == 2 ||
//                            (inPath[downRight.y-1][downRight.x-1] == 1 && downRight.y-1 != lastDR.y) ||
//                            inPath[downRight.y-1][downRight.x] == 1 ||
//                            inPath[downRight.y-1][downRight.x+1] == 1 ||
//                            inPath[downRight.y-2][downRight.x] == 1 ||
//                            inPath[downRight.y-2][downRight.x-1] == 1)
//                    {
//                        endDR = true;
//                        endUR = true;
//                        drVect.push_back(downRight);
//                    }
//                    lastDR = downRight;
//                }
//            }
//            else
//            {
//                endDR = true;
//            }
//        }
//        //std::cout<<"PASSED DR!"<<std::endl;
//    }
//    //END WHILE
//    //std::cout<<"CRASHED BIG WHILE 1!"<<std::endl;
//    cv::Point lext1;
//    cv::Point lext2;
//    cv::Point rext1;
//    cv::Point rext2;
//    if(ulVect.size() == 0 || dlVect.size() == 0)
//    {
//        lext1.x = upLeft.x;
//        lext1.y = (upLeft.y+downLeft.y)/2;
//        lext2.x = downLeft.x;
//        lext2.y = (upLeft.y+downLeft.y)/2;
//    }
//    else
//    {
//        lext1 = ulVect[ulVect.size()-1];
//        lext2 = dlVect[dlVect.size()-1];
//    }
//    if(urVect.size() == 0 || drVect.size() == 0)
//    {
//        rext1.x = upRight.x;
//        rext1.y = (upRight.y+downRight.y)/2;
//        rext2.x = downRight.x;
//        rext2.y = (upRight.y+downRight.y)/2;
//    }
//    else
//    {
//        rext1 = urVect[urVect.size()-1];
//        rext2 = drVect[drVect.size()-1];
//    }
//    //std::cout<<"CRASHED BIG WHILE 2!"<<std::endl;
//    //std::cout<<"L1 = "<<lext1<<", L2 = "<<lext2<<std::endl;
//    //Assign ranges for the eye --> notice that coords are for rotated ROI!
//    if(left)
//    {
//        leftCorners.push_back(lext1.x < lext2.x ? lext1 : lext2);
//        leftCorners.push_back(rext1.x > rext2.x ? rext1 : rext2);
//        //std::cout<<"CRASHED 3 L1!"<<std::endl;
//        /*
//                std::cout<<"Range for left eye: (ROW " << leftCorners[0].y << ",COL " <<leftCorners[0].x <<
//                ") -> (ROW " << leftCorners[1].y << ",COL " << leftCorners[1].x << ")\n";
//                std::cout.flush();
//                */
//        //std::cout<<"CRASHED 3 L2!"<<std::endl;
//    }
//    else
//    {
//        rightCorners.push_back(lext1.x < lext2.x ? lext1 : lext2);
//        rightCorners.push_back(rext1.x > rext2.x ? rext1 : rext2);
//        //std::cout<<"CRASHED 3 R1!"<<std::endl;
//        /*
//                std::cout<<"Range for right eye: (ROW " << rightCorners[0].y << ",COL " <<rightCorners[0].x <<
//                ") -> (ROW " << rightCorners[1].y << ",COL " << rightCorners[1].x << ")\n";
//                std::cout.flush();
//                */
//        //std::cout<<"CRASHED 3 R2!"<<std::endl;
//    }
//    return;
//}








