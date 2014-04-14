#include "gazeestimation.h"

gazeEstimation::gazeEstimation()
{
//    maxEyeDispAlongX = 39;//40.5;  //Empiric
//    minEyeDispAlongX = 34;  //31.5;  //Empiric
//    eyeDeltaTotAlongX = maxEyeDispAlongX - minEyeDispAlongX;
    leftMinDisp = 0.1805;
    leftMaxDisp = 0.2328;
    leftEyeRange = 0.0523;
    rightMinDisp = 0.1534;
    rightMaxDisp = 0.19;//0.2028;
    rightEyeRange = 0.0366;//0.0494;

    collectingData = true;

    if(collectingData)
    {
        minLeftDist = DBL_MAX;
        minRightDist = DBL_MAX;
        maxLeftDist = DBL_MIN;
        maxRightDist = DBL_MIN;
    }

    minHorizontalThreshold = 0.385;
    maxHorizontalThreshold = 0.575;

    nextGazeIndex = 0;

}

void gazeEstimation::setScreenResolution(int width, int height)
{
    this->screenWidth = width;
    this->screenHeight = height;
}

int gazeEstimation::getFinalXDisplacement()
{
    return this->xGazeDisplacement;
}

int gazeEstimation::getHorizontalResponse()
{
    return this->horizontalZone;
}

void gazeEstimation::setRealDistances(int realLeft, int realRight, cv::Rect face)
{
    //Using face coords!
    double center = (double)face.width/2 - 0.5;  //0.5 is crucial to correctly get center coords.
    std::cout<<"Center is "<< center<<std::endl;

    //Get real left distance pupil/center
    realLeftToC = (double)realLeft - center;
    if(realLeftToC < 0) realLeftToC = -realLeftToC;
    std::cout<<"RealLeftToC = " <<realLeftToC<<std::endl;

    //Get real right distance pupil/center
    realRightToC = (double)realRight - center;
    if(realRightToC < 0) realRightToC = -realRightToC;
    std::cout<<"RealRightToC = " <<realRightToC<<std::endl;

    //Get both percentages with respect to face width
    realLeftToC = realLeftToC/face.width;
    realRightToC = realRightToC/face.width;
//    std::cout<<"RealLeftToC % = " <<realLeftToC<<std::endl;
//    std::cout<<"RealRightToC % = " <<realRightToC<<std::endl;

    assert(realLeftToC < 1 && realRightToC < 1);

    if(isCollecting())
    {
        //Update LEFT Distances
        if(realLeftToC < minLeftDist)
        {
            minLeftDist = realLeftToC;
        }
        if(realLeftToC > maxLeftDist)
        {
            maxLeftDist = realLeftToC;
        }

        //Update RIGHT Distances
        if(realRightToC < minRightDist)
        {
            minRightDist = realRightToC;
        }
        if(realRightToC > maxRightDist)
        {
            maxRightDist = realRightToC;
        }
    }
}

void gazeEstimation::followGaze(cv::Point* leftPupil, cv::Point* rightPupil, cv::Rect face)
{
        #ifndef WEBSERVICE
            #ifdef FOLLOWGAZE_DEBUG
            int h=0;
            std::cout << "FOLLOW GAZE: " << h++ << ": ENTERED. SCREEN: "<< this->screenWidth << "x" << this->screenHeight <<std::endl;
            #endif
        #endif

        evaluateGaze(leftPupil, rightPupil, face);

        #ifndef WEBSERVICE
            #ifdef FOLLOWGAZE_DEBUG
            std::cout << "FOLLOW GAZE: " << h++ << ": Gaze evaluated" <<std::endl;
            #endif
        #endif

        horizontalZone = (int)(finalHorizGaze / 0.333) +1;
        if(horizontalZone == 4) horizontalZone--;

        #ifndef WEBSERVICE
            #ifdef DEBUG
            std::cout << "FOLLOW GAZE: Horizontal component: "<< horizontalZone << std::endl;
            #endif

        #endif
}

void gazeEstimation::evaluateGaze(cv::Point* leftPupil, cv::Point* rightPupil, cv::Rect face)
{

#ifdef EVALUATEGAZE_DEBUG
    std::cout<<"EVALUATEGAZE: ENTERED" << std::endl;
#endif

    double leftToC, rightToC;
    int nFound = 0;

    finalHorizGaze = 0;
    xGazeDisplacement = 0;
    double leftHorizGaze = 0;
    double rightHorizGaze = 0;


#ifdef EVALUATEGAZE_DEBUG
    int h = 0;
    std::cout<<"EVALUATEGAZE: MaxL = " << this->leftMaxDisp <<", MinL = " << this->leftMinDisp<<std::endl;
    std::cout<<"EVALUATEGAZE: MaxL = " << this->rightMaxDisp <<", MinR = " << this->rightMinDisp<<std::endl;
#endif

    if(leftPupil != NULL)
    {
        nFound++;
        #ifdef EVALUATEGAZE_DEBUG
                std::cout<<"EVALUATEGAZE: LEFTPUPIL =  "<<leftPupil->x<< ", center =  "<<(double)face.width/2 -0.5<< std::endl;
        #endif

        leftToC = (double)leftPupil->x - ((double)face.width/2 -0.5);
        #ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: LeftToC =  "<<leftToC<< std::endl;
        #endif
        if(leftToC < 0) leftToC = -leftToC;
        #ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: LeftToC after correction =  "<<leftToC<< std::endl;
        #endif
        leftToC /= face.width;

#ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: INITIAL LEFTTOC = " << leftToC << std::endl;
#endif

        #ifdef TEST_MODE
        #ifndef AUTOMATIC_TEST
        if(collectingData)
        {
            collectDistData(leftToC, true);
        }
        #endif
        #endif

        //Adjust data
        if(leftToC < this->leftMinDisp) leftToC = this->leftMinDisp;
        if(leftToC > this->leftMaxDisp) leftToC = this->leftMaxDisp;
#ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: Before: LEFTDIST = " << leftToC << std::endl;
#endif
        //Prepare for further algorithms
        leftToC -= this->leftMinDisp;   //esce un 0 <= leftToC <= eyeRange
#ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: A: LEFTDIST = " << leftToC << std::endl;
#endif
        //Take account of double decimal error
        if(leftToC < 0) leftToC = 0;
        if(leftToC > this->leftEyeRange) leftToC = this->leftEyeRange;

#ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: B: LEFTDIST = " << leftToC << std::endl;
#endif
        leftToC = this->leftEyeRange - leftToC;


#ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: After: LEFTDIST = " << leftToC << std::endl;
 #endif

        //leftHorizGaze += linearApprox(leftToC, true);
        leftHorizGaze += exponentialApprox(leftToC, true);

#ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: LEFTDIST = "<<leftToC<<", Gaze LEFT = "<< leftHorizGaze<<std::endl;
#endif
    }

    if(rightPupil != NULL)
    {
        nFound++;

        #ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: RIGHTPUPIL =  "<< rightPupil->x<< ", center =  "<<(double)face.width/2 -0.5<< std::endl;
        #endif

        rightToC = (double)rightPupil->x - ((double)face.width/2 -0.5);
        #ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: RightToC =  "<<rightToC<< std::endl;
        #endif

        if(rightToC < 0) rightToC = -rightToC;
        #ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: RightToC after correction =  "<<rightToC<< std::endl;
        #endif

        rightToC /= face.width;

        #ifdef TEST_MODE
        #ifndef AUTOMATIC_TEST
        if(collectingData)
        {
            collectDistData(rightToC, false);
        }
        #endif
        #endif

        //Adjust data
        if(rightToC < this->rightMinDisp) rightToC = this->rightMinDisp;
        if(rightToC > this->rightMaxDisp) rightToC = this->rightMaxDisp;
#ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: Before: RIGHTDIST = " << rightToC << std::endl;
#endif
        //Prepare for further algorithms
        rightToC -= this->rightMinDisp;

        //Take account of double decimal errors
        if(rightToC < 0) rightToC = 0;
        if(rightToC > this->rightEyeRange) rightToC = this->rightEyeRange;

#ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: After: RIGHTDIST = " << rightToC << std::endl;
#endif

        //rightHorizGaze += linearApprox(rightToC, false);
        rightHorizGaze += exponentialApprox(rightToC, false);

#ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: RIGHTDIST = "<<rightToC<<", Gaze RIGHT = "<< rightHorizGaze<<std::endl;
#endif
    }

    finalHorizGaze = leftHorizGaze + rightHorizGaze;

#ifndef WEBSERVICE
    #ifdef EVALUATEGAZE_DEBUG
        std::cout<<"EVALUATEGAZE: FINAL GAZE X = "<<finalHorizGaze<< ", nFound = "<<nFound<<std::endl;
    #endif
#endif

    finalHorizGaze /= nFound;
    xGazeDisplacement /= nFound;

#ifdef EVALUATEGAZE_DEBUG
#ifndef WEBSERVICE
    std::cout<<"EVALUATEGAZE: FINAL AVG RATIO = " << finalHorizGaze <<", FINAL DISPLACEMENT = " << xGazeDisplacement<<std::endl;
#endif
    assert(xGazeDisplacement >= -screenWidth/2 && xGazeDisplacement <= screenWidth/2);
#endif
}

void gazeEstimation::normalizeRatio()
{
    if(finalHorizGaze < maxHorizontalThreshold && finalHorizGaze > minHorizontalThreshold)
    {
        float num = finalHorizGaze - minHorizontalThreshold;
        float den = maxHorizontalThreshold - minHorizontalThreshold;
        finalHorizGaze = num/den;
    }
    else
    {
        if(finalHorizGaze <= minHorizontalThreshold)
        {
            finalHorizGaze = 0.1;
        }
        if(finalHorizGaze >= maxHorizontalThreshold)
        {
            finalHorizGaze = 0.9;
        }
    }
}

void gazeEstimation::avgGaze()
{
    int den;
    uint i;

    if(gazeSample.size() < 4)
    {
        gazeSample.push_back(finalHorizGaze);
    }
    else
    {
        gazeSample[nextGazeIndex] = finalHorizGaze;
        nextGazeIndex = (nextGazeIndex+1)%4;
    }
    for(avgHorizGaze = 0, den = 0, i = 0; i<gazeSample.size(); i++)
    {
        avgHorizGaze += gazeSample[i];
        den++;
    }
    avgHorizGaze /= den;
    std::cout<<"Average gaze = " << avgHorizGaze <<std::endl;
    return;
}

double gazeEstimation::getFinalHorizontalGaze()
{
    return finalHorizGaze;
}

double gazeEstimation::getAvgHorizGaze()
{
    return avgHorizGaze;
}

double gazeEstimation::linearApprox(double dist, bool left)
{
    double distUpdated = prepareApprox(dist,left);

    double ratio = (double)distUpdated / dEyeMax;

    #ifdef LINEARAPPROX_DEBUG
    std::cout<<"LINEARAPPROX: Entered with dist = "<<distUpdated<<". Ratio is "<<ratio<<std::endl;
    #endif

    //Screen data from ratio
    double dScreen = ratio*dScreenMax;

    if(rightGaze == false)
    {
        dScreen = -dScreen;
    }

    #ifdef LINEARAPPROX_DEBUG
        std::cout<<"LINEARAPPROX: displ = "<< dScreen <<" [screenSize = "<<screenWidth<<"x"<<screenHeight<<"]"<<std::endl;
    #endif


    xGazeDisplacement += dScreen;

    double gazeX = screenCenter + dScreen;

    double screenRatio = gazeX / screenWidth;

    return screenRatio;
}

double gazeEstimation::prepareApprox(double dist, bool left)
{
#ifdef PREPAREAPPROX_DEBUG
    std::cout<<"PREPAREAPPROX: Entered with dist = "<<dist<<std::endl;
#endif
    //Eye displacement data
    dEyeMax = left?this->leftEyeRange/2:this->rightEyeRange/2;
#ifdef PREPAREAPPROX_DEBUG
    std::cout<<"PREPAREAPPROX: dEyeMax is  "<<dEyeMax<<std::endl;
#endif

    if(dist >= dEyeMax)
    {
        #ifdef PREPAREAPPROX_DEBUG
        std::cout<<"PREPAREAPPROX: dist is >= dEyeMax: gaze RIGHT"<<std::endl;
        #endif

        dist = dist - dEyeMax;
        rightGaze = true;

        #ifdef PREPAREAPPROX_DEBUG
        std::cout<<"PREPAREAPPROX: dist from range center is "<<dist <<std::endl;
        #endif
    }
    else
    {
        #ifdef PREPAREAPPROX_DEBUG
        std::cout<<"PREPAREAPPROX: dist is < dEyeMax: gaze LEFT"<<std::endl;
        #endif

        dist = dEyeMax - dist;
        rightGaze = false;

        #ifdef PREPAREAPPROX_DEBUG
        std::cout<<"PREPAREAPPROX: dist from range center is "<<dist <<std::endl;
        #endif
    }

    //Screen displacement data
    #ifdef PREPAREAPPROX_DEBUG
    std::cout<<"PREPAREAPPROX: screenSize = "<<screenWidth<<"x"<<screenHeight<<std::endl;
    #endif

    screenCenter = (double)screenWidth/2 -0.5;
    dScreenMax = screenCenter;
#ifdef PREPAREAPPROX_DEBUG
    std::cout<<"PREPAREAPPROX: screenCenter = "<<screenCenter<< ", dScreenMax = " << dScreenMax << std::endl;
#endif

    return dist;
}

double gazeEstimation::exponentialApprox(double dist, bool left)
{
    double distUpdated = prepareApprox(dist, left);

    double dScreen;

    //Normal exp
    //double alpha = log(2)/dEyeMax;
    //dScreen = dScreenMax*(exp(alpha*dist)-1);

    //Double exp
    double alpha = log(2)/dEyeMax;
    dScreen = dScreenMax*(pow((exp(alpha*distUpdated)-1), 0.7));
/*
    //Base changing --> useless
    double beta = 0;
    double k = M_E * beta;
    dScreen = dScreenMax * (pow(k, ((log(2)/log(k))*(distUpdated/dEyeMax)))-1);
*/
/*
    //DS = DSMax * base^-(dxMax - dx)

    double e = -(dEyeMax-distUpdated);
    double base = 1.5;
    double exponentialFactor = pow(base, e);
    dScreen = dScreenMax*exponentialFactor;
*/
    if(rightGaze == false)
    {
        dScreen = -dScreen;
    }

#ifdef EXPONENTIALAPPROX_DEBUG
    std::cout<<"EXPONENTIALAPPROX: xDispl = "<<dScreen<<std::endl;
#endif
    xGazeDisplacement += dScreen;

    double gazeX = screenCenter + dScreen;

    double screenRatio = gazeX / screenWidth;

#ifdef EXPONENTIALAPPROX_DEBUG
    std::cout<<"EXPONENTIALAPPROX: X = "<<gazeX<<", screenCenter = " << screenCenter << ", ratio = " << screenRatio<<std::endl;
#endif

    return screenRatio;
}


void gazeEstimation::collectDistData(double a, bool left)
{
    std::cout<<"COLLECTDISTDATA: ENTERED. Press 'g' if good pupils."<<std::endl;
    char c2 = cv::waitKey(0);
    if(c2 == 'g')
    {
        if(left)
        {
            if(a < minLeftDist)
            {
                minLeftDist = a;
            }
            if(a > maxLeftDist)
            {
                maxLeftDist = a;
            }

        }
        else
        {
            if(a < minRightDist)
            {
                minRightDist = a;
            }
            if(a > maxRightDist)
            {
                maxRightDist = a;
            }
        }
    }
}

bool gazeEstimation::isCollecting()
{
    return collectingData;
}

void gazeEstimation::printCollectedData()
{
    //UO
#ifndef WEBSERVICE
    std::cout<<"MIN LEFT OVERALL = " << minLeftDist << ", MAX LEFT OVERALL = " << maxLeftDist << std::endl;
    std::cout<<"MIN RIGHT OVERALL = " << minRightDist << ", MAX RIGHT OVERALL = " << maxRightDist << std::endl;
#endif
    //UO
}
