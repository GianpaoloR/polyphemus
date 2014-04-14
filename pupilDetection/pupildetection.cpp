#include "pupildetection.h"

pupilDetection::pupilDetection(binaryAnalyzer *bin)
{
    detectionMethod = GREAT_CATCH;
    refinementMethod = DARKCROSS;

    leftPupil = NULL;
    rightPupil = NULL;
    leftPupilRefined = NULL;
    rightPupilRefined = NULL;


    leftPupilSet = NULL;
    rightPupilSet = NULL;
    leftRefinedSet = new binaryAnalyzer::setInformation();
    rightRefinedSet = new binaryAnalyzer::setInformation();

    leftFound = false;
    rightFound = false;

    this->bin = bin;

    this->pupilrH = new pupilRois();

    //UO
#ifndef WEBSERVICE
    this->eyeGui = new pupilGui();
#else
    this->eyeGui = NULL;
#endif
    //UO

#ifdef FINDPUPILS_WINDOWS
    eyeGui->initFrame("Left Eye");
    eyeGui->initFrame("Right Eye");
    //eyeGui->initFrame("Left filtered");
    //eyeGui->initFrame("Right filtered");
    //eyeGui->initFrame("Filter");

    #ifdef FINDPUPILS_TEST
        fLeft_Better = 0;
        fLeft_Equal = 0;
        fLeft_Total = 0;
        fRight_Better = 0;
        fRight_Equal = 0;
        fRight_Total = 0;

        avgLeftLight = 0;
        avgRightLight = 0;

        nFrame = 0;
    #endif

#endif
}

/************************
 *   PUPIL FINDERS
 ***********************/

//DetectEyes: finds pupil inside reduced eye regions
void pupilDetection::findPupils(cv::Mat leftROI, cv::Mat rightROI)//std::vector<cv::Rect> eyesReduced)
{
#ifdef FINDPUPILS_DEBUG
    int h = 0;
    std::cout<<"FINDPUPILS: OK " << h++ <<": left: "<<leftROI.cols<<"x"<<leftROI.rows<<", right: "<<rightROI.cols<<"x"<<rightROI.rows<<std::endl;
#endif

    bool equalizing = true;

    pupilrH->setLeftROI(leftROI);
    pupilrH->setRightROI(rightROI);
    if(equalizing)
    {
        pupilrH->equalizeROIs();
        //UO
        if (eyeGui != NULL) {
            eyeGui->setLeftFrame(pupilrH->getLeftROIEq());
            eyeGui->setRightFrame(pupilrH->getRightROIEq());
        }
        //UO
    }
    else
    {
        //UO
        if (eyeGui != NULL) {
            eyeGui->setLeftFrame(pupilrH->getLeftROI());
            eyeGui->setRightFrame(pupilrH->getRightROI());
        }
        //UO
    }

    //updateReduced();

    //Compute avg light left
    leftAvgLight = computeAvgLight(leftROI);

    //Compute avg light right
    rightAvgLight = computeAvgLight(rightROI);

#ifdef FINDPUPILS_TEST
    avgRightLight += rightAvgLight;
    avgLeftLight += leftAvgLight;
#endif


#ifdef FINDPUPILS_DEBUG
//cv::Mat left = pupilrH->getLeftROI();
//cv::Mat right = pupilrH->getRightROI();
    std::cout<<"FINDPUPILS: OK " << h++ <<": left: "<<leftROI.cols<<"x"<<leftROI.rows<<", right: "<<rightROI.cols<<"x"<<rightROI.rows<<std::endl;
    std::cout<<"FINDPUPILS: OK " << h++ << ": avgLightLeft = "<<leftAvgLight<<"; avgLightRight = "<<rightAvgLight<<std::endl;
#endif

    //DETECT PUPIL
    switch(detectionMethod)
    {
        case GREAT_CATCH:
        leftPupil = greatCatch(true);
        rightPupil = greatCatch(false);
        break;
        case BLACK_HOLE:
        leftPupil = blackHole(true);
        rightPupil = blackHole(false);
        break;
        case BLACK_HOLE_v2:
        leftPupil = blackHoleV2(true);
        rightPupil = blackHoleV2(false);
        break;
        default:
        leftPupil = NULL;
        rightPupil = NULL;
        break;
    }

    if(leftPupil!=NULL)
    {
        leftFound = true;
    }
    else
    {
        leftFound = false;
    }
    if(rightPupil != NULL)
    {
        rightFound = true;
    }
    else
    {
        rightFound = false;
    }


#ifdef FINDPUPILS_DEBUG
{
    if(leftPupil != NULL)
    {
        std::cout<<"FINDPUPILS: OK " << h++ << ": leftPupil = {"<<leftPupil->x <<"," << leftPupil->y << "}" << std::endl;
    }
    else
    {
        std::cout<<"FINDPUPILS: OK " << h++ << ": leftPupil NOT FOUND." << std::endl;
    }


    if(rightPupil != NULL)
    {
        std::cout<<"FINDPUPILS: OK " << h++ << ": rightPupil = {"<<rightPupil->x <<"," << rightPupil->y << "}" << std::endl;
    }
    else
    {
        std::cout<<"FINDPUPILS: OK " << h++ << ": rightPupil NOT FOUND." << std::endl;
    }
}
#endif

#ifdef FINDPUPILS_WINDOWS
    if(eyeGui!=NULL)
    {
        showPupils();
        #ifdef FINDPUPILS_TEST
            std::cout<<"Insert LEFT test result:"<<std::endl;
            collectTestInfo(F_LEFT);
            std::cout<<"Insert RIGHT test result:"<<std::endl;
            collectTestInfo(F_RIGHT);
            std::cout<<"Frame test " << ++nFrame <<" OK."<<std::endl;
        #else
        //waitKey(0);
        #endif
    }
#endif

    return;
}

void pupilDetection::refinePupils()
{
    //UO
    #ifdef REFINEPUPILS_DEBUG
    std::cout<<"REFINEPUPILS: ENTERED"<<std::endl;
    #endif
    //UO

    leftPupilRefined = new cv::Point();
    rightPupilRefined = new cv::Point();

    //Original pupils
    if(leftFound)
    {
        *leftPupilRefined = *leftPupil;
    }
    if(rightFound)
    {
        *rightPupilRefined = *rightPupil;
    }

    //UO
    #ifdef REFINEPUPILS_DEBUG
    std::cout<<"REFINEPUPILS: ASSIGNMENT DONE"<<std::endl;
    #endif
    //UO

    switch(refinementMethod)
    {
    case UPHOLSTER:
        if(leftFound) leftPupilRefined = upholster(true);
        if(rightFound) rightPupilRefined = upholster(false);
        break;
    case DARKCROSS:
        if(leftFound) leftPupilRefined = darkCross(true, false);
        if(rightFound) rightPupilRefined = darkCross(false, false);
        break;
    case DARKCROSS_EQ:
        if(leftFound) leftPupilRefined = darkCross(true, true);
        if(rightFound) rightPupilRefined = darkCross(false, true);
        break;
    default:
        break;

    }

    //if(leftFound) leftPupilRefined = refinePupil(true);
      //rightRefined = refinePupil(false);



#ifdef REFINEPUPILS_DEBUG
    std::cout<<"REFINEPUPILS: leftPupilRefined = {"<<leftPupilRefined->x <<"," << leftPupilRefined->y << "}" << std::endl;
    std::cout<<"REFINEPUPILS: rightPupilRefined = {"<<rightPupilRefined->x <<"," << rightPupilRefined->y << "}" << std::endl;
#endif

    if(eyeGui != NULL)
    {
        //UO
        #ifndef WEBSERVICE
        showPupilsRefined();
        #endif
        //UO
    }

    return;
}

#ifdef TEST_MODE
#ifdef AUTOMATIC_TEST
#ifdef ONLY_REAL_PUPIL
void pupilDetection::bypassPupils(cv::Mat leftROI, cv::Mat rightROI)
{
    pupilrH->setLeftROI(leftROI);
    eyeGui->setLeftFrame(leftROI);
    pupilrH->setRightROI(rightROI);
    eyeGui->setRightFrame(rightROI);

    leftFound = true;
    rightFound = true;
}

void pupilDetection::setRealPupils(cv::Point* left, cv::Point* right)
{
    if(leftPupil==NULL) leftPupil = new cv::Point();
    if(rightPupil==NULL) rightPupil = new cv::Point();
    if(leftPupilRefined == NULL) leftPupilRefined = new cv::Point();
    if(rightPupilRefined == NULL) rightPupilRefined = new cv::Point();

    leftPupil->x = left->x;
    leftPupil->y = left->y;
    leftPupilRefined->x = left->x;
    leftPupilRefined->y = left->y;
    rightPupil->x = right->x;
    rightPupil->y = right->y;
    rightPupilRefined->x = right->x;
    rightPupilRefined->y = right->y;

    if(eyeGui != NULL)
    {
        showPupils();
    }
}
#endif
#endif
#endif

#ifdef FINDPUPILS_TEST
void pupilDetection::collectTestInfo(testInfo question)
{

    int* pBetter = NULL, *pTotal = NULL, *pEqual = NULL;

    switch (question)
    {
    case F_LEFT:
        pBetter = &(fLeft_Better);
        pEqual = &(fLeft_Equal);
        pTotal = &(fLeft_Total);
        break;
    case F_RIGHT:
        pBetter = &(fRight_Better);
        pEqual = &(fRight_Equal);
        pTotal = &(fRight_Total);
        break;
    default:
        break;
    }

    if(pBetter != NULL && pTotal != NULL)
    {
        char c = cv::waitKey(0);
        switch(c)
        {
        case 'y':
            (*pBetter)++;
            break;
        case 'e':
            (*pEqual)++;
            break;
        case 'n':
            break;
        default:
            break;
        }

        (*pTotal)++;
    }

}

void pupilDetection::printTestResults()
{
    std::cout<<std::endl;
    std::cout<<"TEST RESULTS: "<< std::endl;
    std::cout<<"              fLeft_Better/fLeft_Total = "<< fLeft_Better << "/"<< fLeft_Total <<" --> RATIO " << (float)fLeft_Better/fLeft_Total<<"%"<<std::endl;
    std::cout<<"              fLeft_Equal/fLeft_Total = "<< fLeft_Equal << "/"<< fLeft_Total <<" --> RATIO " << (float)fLeft_Equal/fLeft_Total<<"%"<<std::endl;
    std::cout<<"              fLeft_Worse/fLeft_Total = "<< fLeft_Total-fLeft_Better-fLeft_Equal << "/"<< fLeft_Total <<" --> RATIO " << (float)(fLeft_Total - fLeft_Better - fLeft_Equal)/fLeft_Total<<"%"<<std::endl;
    std::cout<<"              fRight_Better/fRight_Total = "<< fRight_Better << "/"<< fRight_Total <<" --> RATIO " << (float)fRight_Better/fRight_Total<<"%"<<std::endl;
    std::cout<<"              fRight_Equal/fRight_Total = "<< fRight_Equal << "/"<< fRight_Total <<" --> RATIO " << (float)fRight_Equal/fRight_Total<<"%"<<std::endl;
    std::cout<<"              fRight_Worse/fRight_Total = "<< fRight_Total-fRight_Better-fRight_Equal << "/"<< fRight_Total <<" --> RATIO " << (float)(fRight_Total - fRight_Better - fRight_Equal)/fRight_Total<<"%"<<std::endl;

    std::cout<<"    LEFT AVG LIGHT OVERALL: "<<avgLeftLight/nFrame<<std::endl;
    std::cout<<"    RIGHT AVG LIGHT OVERALL: "<<avgRightLight/nFrame<<std::endl;
}

#endif

cv::Point* pupilDetection::blackHoleV2(bool left)
{
    #define MASK_SIZE 5
    cv::Mat eyeROI;//, eyeROIThresholded;
    double avgLight;

    if(left)
    {
        eyeROI = pupilrH->getLeftROI();
        avgLight = leftAvgLight;
    }
    else
    {
        eyeROI = pupilrH->getRightROI();
        avgLight = rightAvgLight;
    }

//    int mask1[MASK_SIZE][MASK_SIZE] = {0, 0, 0, 0, 0,
//                       0, 0, 0, 0, 0,
//                       0, 0, 0, 0, 0,
//                       0, 0, 0, 0, 0,
//                       0, 0, 0, 0, 0};
    cv::Point tmp;
    int x, y;
    int startx, starty;
    int delta;
    int distance[eyeROI.rows-4][eyeROI.cols-4];
    int i, j;

    //For each point in the central zone of the image, compute the difference with the mask
    //Final value for that pixel is his "distance" from the ideal pupil.
    for(i=0, tmp.y=2; tmp.y<eyeROI.rows-2; tmp.y++, i++)
    {
        for(j=0, tmp.x=2; tmp.x<eyeROI.cols-2; tmp.x++, j++)
        {
            starty=tmp.y-2;
            startx=tmp.x-2;
            for(delta=0, y=0; y<MASK_SIZE; y++)
            {
                for(x=0;x<MASK_SIZE;x++)
                {
                    delta+=(int)(eyeROI.at<uchar>(starty+y, startx+x));
                }
            }
            //std::cout<<delta<<"\n";
            distance[i][j] = delta;
        }
    }

#ifdef HAVING_DEBUG
    std::cout<<"i="<<i<<", j="<<j<<"\n";
    std::cout.flush();
#endif


    //eyeROIThresholded = bin->newThresholded(eyeROI.rows, eyeROI.cols);

    //Check for the pixel whose distance is maximum. That value is needed to normalize others.
    int minDelta = INT_MAX;
    int maxDelta = INT_MIN;
    int bestX, bestY;  //in distance matrix!! Not in the image!!

    for(x=0;x<eyeROI.rows-4;x++)
    {
        for(y=0;y<eyeROI.cols-4;y++)
        {
            if(distance[x][y] > maxDelta)
            {
                maxDelta = distance[x][y];
            }
            if(distance[x][y] < minDelta)
            {
                minDelta = distance[x][y];
                bestX = x;
                bestY = y;
            }
        }
    }


    //Normalize pixels values in range (0, 255).
    int range = maxDelta - minDelta;
    for(x=0;x<eyeROI.rows-4;x++)
    {
        for(y=0;y<eyeROI.cols-4;y++)
        {
            distance[x][y] = (255*(distance[x][y]-minDelta))/range;
        }
    }

    //Set pixels' values in the thresholded ROI image. Whitest = worst candidate to be the pupil. Black Hole = pupil.
    cv::Point* bestCandidate = new cv::Point(-1,-1);

    for(y=2; y<eyeROI.rows-2; y++)
    {
        for(x=2; x<eyeROI.cols-2; x++)
        {
            //eyeROIThresholded.at<uchar>(y,x) = (uchar)(distance[y-2][x-2]);
            if(y-2==bestX && x-2==bestY)
            {
                bestCandidate->x = x;
                bestCandidate->y = y;
                //eyeROIThresholded.at<uchar>(y,x) = (uchar)255;
            }
        }
    }


//    //Define pupil zone
//    int sideLength = 3;
//    int endx, endy;
//    bool allWhite = false;
//    int distanceThreshold = 40;


//    while(!allWhite)
//    {
//        allWhite = true;

//        startx = bestCandidate->x - sideLength/2;
//        starty = bestCandidate->y - sideLength/2;
//        endx = bestCandidate->x + sideLength/2;
//        endy = bestCandidate->y + sideLength/2;

//        int x, y;
//        for(y=starty; y<=endy; y++)
//        {
//            if(y>=0 && y<eyeROIThresholded.rows)
//            {
//                if(y == starty || y == endy)
//                {
//                    for(x=startx; x<=endx; x++)
//                    {
//                       if(x>=0 && x<eyeROIThresholded.cols)
//                       {
//                           if(eyeROIThresholded.at<uchar>(y,x) < distanceThreshold)
//                           {
//                               eyeROIThresholded.at<uchar>(y,x) = 0;
//                               allWhite = false;
//                           }
//                           else
//                           {
//                             //  eyeROIThresholded.at<uchar>(y,x) = 255;
//                           }
//                       }
//                    }
//                }
//                else
//                {
//                    if(startx>=0 && startx<eyeROIThresholded.cols)
//                    {
//                        if(eyeROIThresholded.at<uchar>(y,startx) < distanceThreshold)
//                        {
//                           eyeROIThresholded.at<uchar>(y,startx) = 0;
//                            allWhite = false;
//                        }
//                        else
//                        {
//                           //  eyeROIThresholded.at<uchar>(y,startx) = 255;
//                        }
//                    }
//                    if(endx>=0 && endx<eyeROIThresholded.cols)
//                    {
//                        if(eyeROIThresholded.at<uchar>(y,endx) < distanceThreshold)
//                        {
//                            eyeROIThresholded.at<uchar>(y,endx) = 0;
//                            allWhite = false;
//                        }
//                        else
//                        {
//                           // eyeROIThresholded.at<uchar>(y,endx) = 255;
//                        }
//                    }
//                }
//            }
//        }

//        sideLength++;
//        sideLength++;
//    }

    if(bestCandidate->x != -1 && bestCandidate->y != -1)    return bestCandidate;
    else return NULL;

}


//Black Hole: find a single pupil through simple binarization
cv::Point* pupilDetection::blackHole(bool left)
{
    cv::Mat eyeROI, thresholdedMat;
    float avgLight;

    if(left)
    {
        eyeROI = pupilrH->getLeftROI();
        avgLight = leftAvgLight;
    }
    else
    {
        eyeROI = pupilrH->getRightROI();
        avgLight = rightAvgLight;
    }

    int cand=0;
    std::vector<cv::Point> candidates;

    //Threshold eye
    thresholdedMat = bin->newThresholded(eyeROI.rows, eyeROI.cols);
    for(int y=0; y<eyeROI.cols; y++)
    {
        if(y==0 || y==thresholdedMat.cols-1)
        {
            for(int x = 0; x<thresholdedMat.rows; x++)
                thresholdedMat.at<uchar>(x,y)=(char)255;
            continue;
        }

        thresholdedMat.at<uchar>(0,y) = (char)255;
        thresholdedMat.at<uchar>(thresholdedMat.rows-1, y) = (char)255;

        for(int x = 1; x<thresholdedMat.rows-1; x++)
        {
            //imagename->at<datatype>(row-1, col-1);
            uchar val = eyeROI.at<uchar>(x,y);

            uchar* tVal = &(thresholdedMat.at<uchar>(x,y));

            uchar up = eyeROI.at<uchar>((x-1), y);
            uchar down = eyeROI.at<uchar>(x+1, y);
            uchar left = eyeROI.at<uchar>(x, y-1);
            uchar right = eyeROI.at<uchar>(x, y+1);


            if((int)val <= (int)up    &&
               (int)val <= (int)down  &&
               (int)val <= (int)left  &&
               (int)val <= (int)right &&
                  (int)val < avgLight/3)
            {
                *tVal = 0;
                cand++;
                cv::Point p(y,x);
                candidates.push_back(p);
#ifdef SHOW_CANDIDATES
                circle(eyeROI, p, 3, 1234);
#endif
            }
            else
            {
                *tVal = (uchar)255;
            }
        }
    }

    std::cout << "Candidati: " << cand <<"\n";// << " - " << candidates.size() << "\n";
    std::cout.flush();


    //Choose best candidate
    std::vector<cv::Point>::iterator it = candidates.begin();
    while(it!=candidates.end())
    {
        std::cout<<"(x,y) = (" << (*it).x <<","<< (*it).y<<")\n";
        ++it;
    }
    if(candidates.size()>0)
    {
        double finalX = 0;
        double finalY = 0;
        for (int i=0; i<candidates.size();i++)
        {
            finalX+=candidates[i].x;
            finalY+=candidates[i].y;
        }
        finalX /= candidates.size();
        finalY /= candidates.size();
        cv::Point* pupil = new cv::Point(finalX,finalY);
        return pupil;
    }
    else return NULL;
}

//GreatCatch: find a single pupil through a morphologic algorithm
cv::Point* pupilDetection::greatCatch(bool left)
{
#ifdef GREATCATCH_DEBUG
    int h=0;
#endif

    cv::Mat eyeROI;
    float avgLight;
    bool equalizing = true;

    if(left)
    {
        if(equalizing) eyeROI = pupilrH->getLeftROIEq();
        else eyeROI = pupilrH->getLeftROI();
        avgLight = leftAvgLight;
    }
    else
    {
        if(equalizing) eyeROI = pupilrH->getRightROIEq();
        else eyeROI = pupilrH->getRightROI();
        avgLight = rightAvgLight;
    }


#ifdef GREATCATCH_DEBUG
    std::cout<<"GREATCATCH: OK "<< h++<<": Roi size = "<<eyeROI.cols << "x" << eyeROI.rows<<std::endl;
#endif
    cv::Mat tROI;
    std::stringstream sname, tname;
    int r, c;
    bool dilate = false;
    bool showROIs = false;
    bool maintain = true;
    binaryAnalyzer::setInformation* set;
    if(left)
    {
        sname << "LEFT";
        set = leftPupilSet;
    }
    else
    {
        sname << "RIGHT";
        set = rightPupilSet;
    }

#ifdef GREATCATCH_DEBUG
    std::cout<<"GREATCATCH: OK "<< h++<<std::endl;
#endif
    r = eyeROI.rows;
    c = eyeROI.cols;
    tname << "Thresholded Eye";
    tROI = bin->newThresholded(r, c);
#ifdef GREATCATCH_DEBUG
    std::cout<<"GREATCATCH: OK "<< h++<<std::endl;
#endif
    //Sharpen the eye ROI
    // sharpenImage(eyeROI);
    if(eyeGui && showROIs) eyeGui->showRoi(eyeROI, sname.str());
#ifdef GREATCATCH_DEBUG
    std::cout<<"GREATCATCH: OK "<< h++<<std::endl;
#endif
    //Select darkest points
    int lightThreshold = avgLight/3;
    tROI = bin->thresholdImg(eyeROI, lightThreshold);
    if(eyeGui && showROIs) eyeGui->showRoi(tROI, tname.str());
    //--------------------------------------------//
    // MORPHOLOGY ALGORITHM //
    //--------------------------------------------//
    //Close
    cv::Mat filteredROI = bin->open(tROI);
    if(showROIs && eyeGui) eyeGui->showRoi(filteredROI, "Black opened");
    filteredROI = bin->close(filteredROI);
    if(showROIs && eyeGui) eyeGui->showRoi(filteredROI, "Black closed");
    cv::Mat invROI(filteredROI);
    if(dilate)
    {
        //Dilate and invert
        invROI = bin->invert(filteredROI);
        cv::Mat dilatedROI = bin->dilate(invROI);
        invROI = bin->invert(dilatedROI);
        if(showROIs && eyeGui) eyeGui->showRoi(invROI, "Dilated");
    }
    else
    {
        //filteredROI.copyTo(invROI);
    }
    //if(showROIs && gui) gui->showRoi(invROI, "Inverted");
    //Erode and invert again to show results
    invROI = bin->invert(invROI);
    /*
    cv::Mat erodedROI = bin->erode(invROI);
    cv::Mat invErodedROI = bin->invert(erodedROI);
    if(showROIs && eyeGui) eyeGui->showRoi(invErodedROI, "Eroded");
    */
     //--------------------------------------------//
    // END OF MORPHOLOGY ALGORITHM //
    //--------------------------------------------//
    //--------------------------------------------//
    // SET ANALYSIS ALGORITHM //
    //--------------------------------------------//
    bin->clearOverallSets();
    //std::vector<binaryAnalyzer::setInformation> sets = bin->findSets(erodedROI, false);
    std::vector<binaryAnalyzer::setInformation> sets = bin->findSets(invROI, false);

    #ifdef GREATCATCH_DEBUG
    for(uint i=0; i<sets.size(); i++)
    {
        std::cout<<"Trovato set: ID "<<sets[i].id<<std::endl;
    }
    #endif

    int mergeThreshold = 2;
    sets = bin->glueSets(sets, mergeThreshold);
    //std::cout<<"Accorpati set entro distanza di "<<mergeThreshold<<" pixel"<<std::endl;
    sets = bin->computeSetSize(sets);

    #ifdef GREATCATCH_DEBUG
    for(uint i=0; i<sets.size(); i++)
    {
        if(sets[i].id != NOT_VALID_ID)
        {
            std::cout<<"Set "<< sets[i].id <<": dX = " << sets[i].width << ", dY = " << sets[i].height << ", center = " <<sets[i].center << std::endl;
        }
    }
    #endif

    std::vector<binaryAnalyzer::setInformation> filteredSets = bin->removeBorderSets(sets, r, c);
    #ifdef GREATCATCH_DEBUG
    std::cout<<"Rimasti "<<filteredSets.size() << " set dopo l'eliminazione ai bordi."<<std::endl;
    #endif

    int chosen = NOT_VALID_ID;
    int maxExtension = INT_MIN;
    int nFiltered = filteredSets.size();
    for(int i=0; i<nFiltered; i++)
    {
/*
        if(filteredSets[i].width >= 1.2*filteredSets[i].height)
        {
        */
            if(filteredSets[i].nPoints > maxExtension)
            {
                maxExtension = filteredSets[i].nPoints;
                chosen = i;
            }
        //}
    }
    cv::Point* pupil = NULL;
    if(set) free(set);
    if(chosen>=0)
    {
        set = new binaryAnalyzer::setInformation(filteredSets[chosen]);
        pupil = new cv::Point(filteredSets[chosen].center);
        //pupil = new cv::Point(filteredSets[chosen].centroid);
    }
    else
    {
        set = NULL;
    }
    //Debug print
    //if(set) std::cout<<"PupilSet.h = " << set->height << ", pupilSet.w = " << set->width << std::endl;
    //else std::cout<<"No PUPILSET!!!" << std::endl;
    if(left) leftPupilSet = set;
    else rightPupilSet = set;

    //UO
    #ifndef WEBSERVICE
    if(maintain)
    {
        cv::destroyWindow(sname.str());
        cv::destroyWindow(tname.str());
        cv::destroyWindow("Black closed");
        if(dilate)
        {
            cv::destroyWindow("Dilated");
        }
        //cv::destroyWindow("Inverted");
        cv::destroyWindow("Eroded");
    }
    #endif
    //UO

    //--------------------------------------------//
    // END OF SET ANALYSIS ALGORITHM //
    //--------------------------------------------//
#ifdef GREATCATCH_DEBUG
    std::cout<<"GREATCATCH: OK "<< h++<< ", EXITING"<<std::endl;
#endif
    return pupil;
}

cv::Point* pupilDetection::findCircles(bool left)
{
    cv::Mat eyeROI, houghsROI;
    std::vector<cv::Vec3f> circles;
    int minVal = INT_MAX;
    int minIndex = -1;
    int bestRadius = 0;
    cv::Point* center;

    if(left) eyeROI = pupilrH->getLeftROI();
    else eyeROI = pupilrH->getRightROI();

    cv::HoughCircles(eyeROI, circles, CV_HOUGH_GRADIENT, 1, 1, 4, 4, 3, eyeROI.rows/6 );
    std::cout<<"Trovati "<<circles.size()<<" cerchi."<<std::endl;
    eyeROI.copyTo(houghsROI);
    for(size_t i = 0; i<circles.size(); i++)
    {
        center = new cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        if((int) houghsROI.at<uchar>(*center) < minVal)
        {
            minVal = (int) houghsROI.at<uchar>(*center);
            minIndex = i;
            bestRadius = radius;
        }
    }
    if(minIndex != -1)
    {
        center->x = cvRound(circles[minIndex][0]);
        center->y = cvRound(circles[minIndex][1]);
        circle(houghsROI, *center, 0, 255);
        circle(houghsROI, *center, bestRadius, 200);
        cv::namedWindow("HOUGHS", CV_WINDOW_NORMAL);
        imshow("HOUGHS", houghsROI);
        cv::waitKey(0);
        cv::destroyWindow("HOUGHS");
        return center;
    }
    else
    {
        std::cout<<"NO CIRCLE!"<<std::endl;
        return NULL;
    }
}


/*************************
 *  PUPIL REFINERS
 ************************/
cv::Point* pupilDetection::upholster(bool left)
{

    #ifdef UPHOLSTER_DEBUG
    int counter;
    #endif

    #ifdef UPHOLSTER_WINDOWS
    std::string name;
    #endif

    cv::Mat roi;
    double avgLight;
    cv::Point* newPupil = new cv::Point();

    //Prepare data

    {
        if(left)
        {
            roi = pupilrH->getLeftROI();
            avgLight = this->leftAvgLight;
            *newPupil = *leftPupil;
            #ifdef UPHOLSTER_WINDOWS
            name = "Tappezzando...LEFT";
            #endif
        }
        else
        {
            roi = pupilrH->getRightROI();
            avgLight = this->rightAvgLight;
            *newPupil = *rightPupil;
            #ifdef UPHOLSTER_WINDOWS
            name = "Tappezzando...RIGHT";
            #endif
        }
    }

    cv::Mat whiteWall = bin->newThresholded(roi.rows, roi.cols);
    std::vector<cv::Point> tapestry;
    uint i, lastTapestrySize, newTapestrySize=0;
    int color = 0, colorStep = 1;
    double val;

    whiteWall.at<uchar>(*newPupil) = color;
    tapestry.push_back(*newPupil);

    do {
        #ifdef UPHOLSTER_WINDOWS
        if(eyeGui)
        {
            eyeGui->showRoi(whiteWall, name);
        }
        #endif

        lastTapestrySize = newTapestrySize;
        newTapestrySize = tapestry.size();

        val = 0;
        for(i=0; i<newTapestrySize; i++)
        {
            val+=roi.at<uchar>(tapestry[i]);
        }
        val/=newTapestrySize;
        val*=1.33 ;

        #ifdef UPHOLSTER_DEBUG
        std::cout<<"tValue = "<<val<<std::endl;
        std::cout<<"lastSize = "<<lastTapestrySize<<"; newSize = "<< newTapestrySize<<std::endl;
        counter = 0;
        #endif

        for(i=lastTapestrySize; i<newTapestrySize; i++)
        {
            color+=colorStep;
            if(color >= 200) color = 0;
            tapestry = coverNeighborhood(roi, tapestry, whiteWall, i, color, colorStep, val);
            #ifdef UPHOLSTER_DEBUG
            std::cout<<"CoverNeighborhood called "<<++counter <<" times."<<std::endl;
            //waitKey(0);
            #endif
        }
        //waitKey(0);

    } while(lastTapestrySize != tapestry.size());

    for(i=0; i<tapestry.size();i++)
    {
        whiteWall.at<uchar>(tapestry[i]) = 0;
    }

#ifdef UPHOLSTER_WINDOWS
    if(eyeGui)
    {
        eyeGui->showRoi(whiteWall, name);
    }
#endif

    whiteWall = bin->open(whiteWall);
#ifdef UPHOLSTER_WINDOWS
    if(eyeGui)
    {
        eyeGui->showRoi(whiteWall, name);
    }
#endif

    whiteWall = bin->close(whiteWall);
#ifdef UPHOLSTER_WINDOWS
    if(eyeGui)
    {
        eyeGui->showRoi(whiteWall, name);
    }
#endif

    //Find new center
    int xMin = roi.cols;
    int xMax = 0;
    int yMin = roi.rows;
    int yMax = 0;

    for(i=0; i<tapestry.size();i++)
    {
        if(whiteWall.at<uchar>(tapestry[i]) == 0) //Consider only final points in set
        {
            if(tapestry[i].x > xMax) xMax = tapestry[i].x;
            if(tapestry[i].x < xMin) xMin = tapestry[i].x;
            if(tapestry[i].y > yMax) yMax = tapestry[i].y;
            if(tapestry[i].y < yMin) yMin = tapestry[i].y;
        }
    }

    newPupil->x = (xMax + xMin)/2;
    newPupil->y = (yMax + yMin)/2;

    return newPupil;
}

std::vector<cv::Point> pupilDetection::coverNeighborhood(cv::Mat roi, std::vector<cv::Point> v, cv::Mat tImg, uint i, int color, int colorStep, double tValue)
{
#ifdef COVERNEIGHBORHOOD_DEBUG
    std::cout<<"COVERNEIGHBORHOOD: ENTERED with v[i] = ["<<v[i].x<<","<<v[i].y<<"]"<<std::endl;
#endif

    int xCoord, yCoord;
    int nAdded = 0;

    for(int y =-1; y<=1; y++)
    {
        yCoord = v[i].y+y;
        if(yCoord>=0 && yCoord<roi.rows) //check y inside roi
        {
            for(int x=-1; x<=1; x++)
            {
                xCoord = v[i].x+x;
                if(xCoord>=0 && xCoord<roi.cols)  //check x inside roi
                {
                    if(xCoord!=v[i].x || yCoord!=v[i].y) //check this is not the same point as v[i]
                    {
                        if(tImg.at<uchar>(yCoord, xCoord) > color-colorStep) //check this point is not yet in tapestry set
                        {
                            //tValue = 1.2*(roi.at<uchar>(v[i])+1);  //set threshold value
                            if(roi.at<uchar>(yCoord, xCoord) < tValue)
                            {
                                v.push_back(cv::Point(xCoord, yCoord));
                                tImg.at<uchar>(yCoord, xCoord) = color;
                                nAdded++;
                            }
                            else
                            {
                                #ifdef COVERNEIGHBORHOOD_DEBUG
                                std::cout<<"Point intensity is above threshold. Rejected."<<std::endl;
                                #endif
                            }
                        }
                        else
                        {
                            #ifdef COVERNEIGHBORHOOD_DEBUG
                            std::cout<<"Point already in set. Flying over."<<std::endl;
                            #endif
                        }
                    }
                    else
                    {
                        #ifdef COVERNEIGHBORHOOD_DEBUG
                        std::cout<<"Same point. Flying over."<<std::endl;
                        #endif
                    }
                }
                else
                {
                    #ifdef COVERNEIGHBORHOOD_DEBUG
                    std::cout<<"xCoord not ok"<<std::endl;
                    #endif
                }
            }
        }
        else
        {
            #ifdef COVERNEIGHBORHOOD_DEBUG
            std::cout<<"yCoord not ok"<<std::endl;
            #endif
        }
    }

#ifdef COVERNEIGHBORHOOD_DEBUG
    std::cout<<"Added "<<nAdded<<" points to the tapestry."<<std::endl;
#endif

    return v;
}

/*
cv::Point* pupilDetection::centerTheEllipse(bool left, bool filtered)
{
#ifdef ELLIPSE_DEBUG
    int h=0;
#endif
    cv::Mat roi;

    //REINTEGRATE
//    cv::Point* lastPupil, *newPupil;
//    lastPupil = new cv::Point();

//    if(left)
//    {
//        if(filtered)
//        {
//            roi = pupilrH->getLeftFilteredROI();
//            newPupil = leftPupilFiltered;
//        }
//        else
//        {
//            roi = pupilrH->getLeftROI();
//            newPupil = leftPupil;
//        }
//    }
//    else
//    {
//        if(filtered)
//        {
//            roi = pupilrH->getRightFilteredROI();
//            newPupil = rightPupilFiltered;
//        }
//        else
//        {
//            roi = pupilrH->getRightROI();
//            newPupil = rightPupil;
//        }
//    }
    return NULL;
}
*/


cv::Point* pupilDetection::darkCross(bool left, bool equalized)
{
#ifdef DARKCROSS_DEBUG
    int h=0;
#endif
    cv::Mat roi;
    cv::Point* lastPupil, *newPupil;

    lastPupil = new cv::Point();

    if(left)
    {
        if(!equalized) roi = pupilrH->getLeftROI();
        else roi = pupilrH->getLeftROIEq();
        newPupil = leftPupil;
    }
    else
    {
        if(!equalized) roi = pupilrH->getRightROI();
        else roi = pupilrH->getRightROIEq();
        newPupil = rightPupil;
    }
#ifdef DARKCROSS_DEBUG
    std::cout<<"DARKCROSS: OK " << h++ << ": Roi size = " << roi.cols << "x" << roi.rows << std::endl;
#endif

    cv::Point p;
    float value, darkestValue;
    do
    {
        *lastPupil = *newPupil;
        darkestValue = FLT_MAX;
        newPupil = new cv::Point();
        for(int i = -2; i<=2; i++)
        {
            for(int j=-2; j<=2; j++)
            {
                value = 0;
                p.x = lastPupil->x + i;
                p.y = lastPupil->y + j;
                for(int k1=-5; k1<=5; k1++)
                {
                    for(int k2=-5;k2<=5; k2++)
                    {
                        value += roi.at<uchar>(p.y+k1, p.x+k2);
                    }
                }
                if(value < darkestValue)
                {
                    darkestValue = value;
                    newPupil->x = p.x;
                    newPupil->y = p.y;
                }
            }
        }
    }
    while(*newPupil != *lastPupil);

#ifdef DARKCROSS_DEBUG
    std::cout<<"DARKCROSS: OK "<< h++<<": End of function." << std::endl;
#endif

    return newPupil;
}

/************************
 *    GETTERS
 ***********************/
cv::Point* pupilDetection::getRightPupil() {
    return rightPupil;
}

cv::Point* pupilDetection::getLeftPupil() {
    if(leftPupil == NULL) std::cout<<"NULL!"<<std::endl;
    return leftPupil;
}

cv::Point* pupilDetection::getLeftPupilRefined()
{
    return leftPupilRefined;
}

cv::Point* pupilDetection::getRightPupilRefined() {
    return rightPupilRefined;
}


/*******************************
 *        CORNERS FINDERS
 ******************************/
//Use Sobel to find edges
void pupilDetection::findEyeCorners()
{
    sobelCorners(true);
    sobelCorners(false);
}

void thresh_callback(int, void* );
RNG rng(12345);
Mat src_gray;
int thresh = 10;
int max_thresh = 50;

void pupilDetection::sobelCorners(bool left)
{
    std::cout<<"SOBELCORNERS: ENTERED"<<std::endl;

    cv::Mat roi, blurredRoi, sobelRoi, gradients, firstDer, secondDer;
    char *window_name, *roiWnd, *firstDerWnd, *secondDerWnd, *eqWnd;
    binaryAnalyzer::setInformation *set;

    cv::Point* pupil;
    if(left)
    {
        roi = pupilrH->getLeftROI();
        window_name = "SOBEL LEFT";
        roiWnd = "LEFT";
        firstDerWnd = "SOBEL 1 DERIVATIVE LEFT";
        secondDerWnd = "SOBEL 2 DERIVATIVE LEFT";
        eqWnd = "SOBEL 2 EQ LEFT";
        pupil = this->leftPupilRefined;
        set = this->leftPupilSet;
    }
    else
    {
        roi = pupilrH->getRightROI();
        window_name = "SOBEL RIGHT";
        roiWnd = "RIGHT";
        firstDerWnd = "SOBEL 1 DERIVATIVE RIGHT";
        secondDerWnd = "SOBEL 2 DERIVATIVE RIGHT";
        eqWnd = "SOBEL 2 EQ RIGHT";
        pupil = this->rightPupilRefined;
        set = this->rightPupilSet;
    }

    int scale = 1;
    int delta = 0;
    int ddepth = -1;//CV_16S;

    int c;

    GaussianBlur(roi, blurredRoi, Size(3,3), 0, 0, BORDER_DEFAULT );

    /// Create window
    //namedWindow(window_name, CV_WINDOW_NORMAL);
    namedWindow(roiWnd, CV_WINDOW_NORMAL);
    imshow(roiWnd, roi);
    namedWindow(firstDerWnd, CV_WINDOW_NORMAL);
    namedWindow(secondDerWnd, CV_WINDOW_NORMAL);
    namedWindow(eqWnd, CV_WINDOW_NORMAL);
    /// Generate grad_x and grad_y
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;

    ///First derivative
    /// Gradient X
    //Scharr( blurredRoi, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    Sobel(blurredRoi, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    //Scharr( blurredRoi, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    Sobel(blurredRoi, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradients);

    imshow(firstDerWnd, gradients);

    ///Second derivative
    /// Gradient X
    //Scharr( blurredRoi, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    Sobel(blurredRoi, grad_x, ddepth, 2, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    //Scharr( blurredRoi, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    Sobel(blurredRoi, grad_y, ddepth, 0, 2, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradients);

    imshow(secondDerWnd, gradients);

    /*
    Mat gradEq(gradients.rows, gradients.cols, CV_8U);
    equalizeHist(gradients, gradEq);
    imshow(eqWnd, gradEq);
    waitKey(0);
    */

    bin->twinkle(gradients, pupil, set);

    //std::cout<<"SOBELCORNERS: TWINKLE DONE."<<std::endl;

    //THRESHOLDING: threshold(gradients, gradients, 8, 255, THRESH_BINARY);
    //EQUALIZATION: equalizeHist(gradients, gradients);
    //imshow(eq, gradients);

    waitKey(0);
    //destroyWindow((window_name));
    destroyWindow(roiWnd);
    destroyWindow(firstDerWnd);
    destroyWindow(secondDerWnd);

    std::cout<<"SOBELCORNERS: EXITING"<<std::endl;
}




//Internal helpers
cv::Point pupilDetection::darkestRegion(cv::Point p[], int sizeP, uchar v[])
{
    float regionVal, minVal = 255*3;
    int index = -1;
    for(int i=0; i<sizeP; i++)
    {
        regionVal = 0;
        for(int j=i; j<i+3; j++)
        {
            regionVal += (int)v[j];
        }
        if(regionVal < minVal)
        {
            minVal = regionVal;
            index = i;
        }
    }
    return p[index];
}


cv::Mat pupilDetection::blurImage(cv::Mat& roi)
{
    cv::Mat blurred(roi.rows, roi.cols, CV_8U);
    std::cout<<"OK";

    //GaussianBlur(roi, blurred, Size(3,3), 3);
    equalizeHist(roi, blurred);


    /*
    GaussianBlur(blurred, blurred, Size(3,3), 3);
    GaussianBlur(blurred, blurred, Size(3,3), 3);
*/
    return blurred;
}



void pupilDetection::updateReduced()
{
    if(eyeGui!=NULL)
    {
        eyeGui->showLeft();
        eyeGui->showRight();
    }
}

//Show refined pupil
void pupilDetection::showPupilsRefined()
{
    //UO
    if (eyeGui != NULL) {
        eyeGui->showPupils(leftPupilRefined, rightPupilRefined, true);
    }
    //UO
    return;
}

void pupilDetection::showPupils()
{
    #ifdef SHOWPUPILS_DEBUG
    int h=0;
    std::cout<<"SHOWPUPILS: OK "<<h++<<": ENTERED"<<std::endl;
    #endif

    //Show original pupil
    //UO
    if (eyeGui != NULL) {
        eyeGui->showPupils(leftPupil, rightPupil, false);
    }
    //UO

    #ifdef SHOWPUPILS_DEBUG
    std::cout<<"SHOWPUPILS: OK "<<h++<<": DONE."<<std::endl;
    #endif
}


/** SharpenImage: applies unsharp mask to an img
                */
void pupilDetection::sharpenImage(cv::Mat& img)
{
    cv::Mat tmp;
    cv::GaussianBlur(img, tmp, cv::Size(5,5), 5);
    //cv::addWeighted(img, 1.5, tmp, -0.5, 0, img);
    cv::addWeighted(img, 2.5, tmp, -1.5, 0, img);
}
