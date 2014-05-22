#include "twinkle.h"

twinkle::twinkle()
{
    bin = new binaryAnalyzer();
}

void twinkle::handleContours(cv::Mat mat)
{
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);

    Mat contoursInput(mat.rows, mat.cols, CV_8U);
    mat.copyTo(contoursInput);

    /// Find contours
    findContours( contoursInput, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    /// Draw contours
    Mat contoursImage = Mat::zeros( mat.size(), CV_8UC3 );
    for( uint i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( contoursImage, contours, i, color, 1, 8, hierarchy, 0, Point() );
     }

    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_NORMAL );
    imshow( "Contours", contoursImage);
    waitKey(0);

}

std::vector<double> twinkle::assignThresholdProbability(int rows, setInformation eyeSet)
{
    //double pAvg = 0, pPoint, dist;
    double pMax=DBL_MIN;
    //double distRange = ((double)(m.rows))/2;  //Mat center is not correct; should be eyeSet vertical center
    int eyeH = rows, eyeL = 0;

    for(uint i=0; i<eyeSet.elem.size();i++)
    {
        if(eyeSet.elem[i].y > eyeL) eyeL = eyeSet.elem[i].y;
        if(eyeSet.elem[i].y < eyeH) eyeH = eyeSet.elem[i].y;
    }

    double eyeCenter;
//    double distRange;
    double eyeHalfHeight;

    eyeCenter = ((double)(eyeL + eyeH))/ 2;
    eyeHalfHeight = ((double)(eyeL-eyeH))/2;
//    if(eyeHalfHeight <= ((double)(m.rows))/2) distRange = eyeHalfHeight;
//    else distRange = m.rows-eyeHalfHeight;

    pMax = eyeHalfHeight/rows;

//    for(int i=0; i<eyeSet.elem.size();i++)
//    {
//        dist = abs(eyeSet.elem[i].y - eyeCenter);
//        if(dist>distRange) dist = distRange;
//        pPoint = dist/distRange;

//        pAvg += pPoint;
//    }

//    pAvg /= eyeSet.elem.size();

    std::vector<double> pData;
    pData.push_back(pMax);
//    pData.push_back(pAvg);
    pData.push_back(eyeCenter);
//    pData.push_back(distRange);
//    pData.push_back(eyeHalfHeight);

    return pData;
}



std::vector<double> twinkle::assignSetsProbability(std::vector<setInformation> sets, std::vector<double> pData, int totRows)
{
    double center = pData[1];
    std::vector<double> setP;
    for(uint i=0; i<sets.size(); i++)
    {
        setP.push_back(computeProbability(sets[i], center, totRows));
    }

    return setP;
}


double twinkle::computeProbability(setInformation set, double center, int totRows)
{
    double dist, distAvg; //, pPoint, pAvg=0;
    for(uint i=0; i<set.elem.size();i++)
    {
        dist = abs(set.elem[i].y - center);

        distAvg += dist; //mean dist of the set from the eyeset center

//        if(dist>maxDist) dist = maxDist;
//        pPoint = dist/maxDist;

        //if(pPoint > pMax) pMax = pPoint;

//        pAvg += pPoint;
    }

//    pAvg /= set.elem.size();
    distAvg /= set.elem.size();

    distAvg /= totRows; //normalized mean distance from set to eyeset center

    return distAvg;
}


std::vector<setInformation> twinkle::filterSets(std::vector<setInformation> orig, std::vector<double> prob, std::vector<double> thresholds)
{
    std::vector<setInformation> ans;

    double alpha;
    double t;

//    if(mode== AVG)
//    {
//        alpha = 1.7;
//        t = alpha*thresholds[1];   //filter according to pAvg
//    }
//    if(mode == MAX)
//    {
//        alpha = 1.2;
//        t = alpha * thresholds[0]; //filter according to pMax
//    }

    alpha = 1.75 ;
    t = alpha*thresholds[0];

    std::cout<<"Threshold is "<<t<<std::endl;
    for(uint i=0; i<orig.size(); i++)
    {
        if(prob[i] <= t)
        {
            ans.push_back(orig[i]);
        }
    }

    return ans;
}

setInformation twinkle::twinkleMethod(cv::Mat mat, Point* pupil)//, setInformation* pupilSet)
{

    setInformation twinkleSet;
    bool contoursWanted = false;

    std::cout<<"TWINKLE: ENTERED"<<std::endl;

    //Phase 1: Threshold eye second derivatives
    int t = 15;
    Mat tMat = bin->newThresholded(mat.rows, mat.cols);
    tMat = bin->thresholdImg(mat, t);


    string wNameT = "Twinkle Threshold";
    namedWindow(wNameT, CV_WINDOW_NORMAL);
    imshow(wNameT, tMat);
    cv::waitKey(0);

    //Phase 2: Dilate horizontally and vertically
    cv::Mat dMat = twinkleDilate(tMat);
    string wNameD = "Twinkle Dilation";
    namedWindow(wNameD, CV_WINDOW_NORMAL);
    imshow(wNameD, dMat);
    cv::waitKey(0);

    //Phase 3: Find contours
    if(contoursWanted) this->handleContours(dMat);

    /*
    //Phase 4: label points
    std::vector<Point> corners = this->labelPoints(dMat);

    //Phase 5: Associate sets and corners
    std::vector<binaryAnalyzer::setInformation> sets = this->fillSetCornerData(dMat, corners);
    */

    if(sets == NULL) sets = new std::vector<setInformation>();
    *sets = bin->findSets(dMat, false);


    //Phase 6: get biggest starting set
    int eyeSetIndex;
    setInformation eyeSet = bin->getBiggestSet(*sets, &eyeSetIndex);
    Mat m = Mat::zeros(dMat.rows, dMat.cols, CV_8U);
    for(uint i=0; i<eyeSet.elem.size();i++)
    {
        m.at<uchar>(eyeSet.elem[i]) = 255;
    }
    m.at<uchar>(*pupil) = 100;
    namedWindow("Biggest", CV_WINDOW_NORMAL);
    imshow("Biggest", m);
    waitKey(0);

    //label sets probability
    int rows = m.rows;
    std::vector<double> pData = assignThresholdProbability(rows, eyeSet);  //pData[0] = max probability for eyeSet points; pData[1] = avg probability for eyeSet
    std::cout<<"pData[0] = pMax = "<<pData[0]<<"; pData[1] = eyeCenter = "<<pData[1]<<std::endl;
    std::vector<double> setsProb = assignSetsProbability(*sets, pData, rows);
    for(uint i=0; i<setsProb.size(); i++)
    {
        std::cout<<"Prob for set "<<i<<": " << setsProb[i]<<std::endl;
    }
    std::vector<setInformation> filteredSets = filterSets(*sets, setsProb, pData);
    Mat fMat = Mat::zeros(dMat.rows, dMat.cols, CV_8U);
    for(uint i=0; i<filteredSets.size();i++)
    {
        for(uint j=0; j<filteredSets[i].elem.size(); j++)
        {
            fMat.at<uchar>(filteredSets[i].elem[j]) = 255;
        }
    }
    namedWindow("Filtered", CV_WINDOW_NORMAL);
    imshow("Filtered", fMat);
    waitKey(0);

    /*
    //Phase 7: find internal limbus contour
    findLimbusContour(m, pupil);
    */
    /*
    //Phase 7: dilate pupil until covering limbus hole

    int opening[m.rows];
    int minY = m.rows, maxY = 0;
    for(int i=0; i<m.rows; i++) opening[i] = 0;
    this->fillHole(m, *pupil, opening, &minY, &maxY);
    for(int i=minY; i<=maxY; i++) std::cout<<opening[i]<<std::endl;
    //this->fillHoleIter(m, *pupil);
*/

/*
    //Phase 7: find nearest set among remaining ones

    namedWindow("Limbus", CV_WINDOW_NORMAL);
    Mat m2 = Mat::zeros(dMat.rows, dMat.cols, CV_8U);
    for(int i=0; i<pupilSet->elem.size(); i++)
    {
        m2.at<uchar>(pupilSet->elem[i]) = 255;
    }
    imshow("Limbus", m2);
    waitKey(0);

    namedWindow("Subtract", CV_WINDOW_NORMAL);
    Mat m3 = Mat::zeros(dMat.rows, dMat.cols, CV_8U);
    int tempVal;
    for(int i=0; i<m3.rows; i++)
    {
        for(int j=0; j<m3.cols; j++)
        {
            tempVal = m2.at<uchar>(i, j) - m.at<uchar>(i, j);
            if(tempVal < 0 ) tempVal = 0;
            m3.at<uchar>(i, j) = tempVal;
        }
    }
    imshow("Subtract", m3);
    waitKey(0);

    Point p1, p2;
    setInformation nearestSet = this->getNearestSet(eyeSetIndex, sets, &p1, &p2, pupilSet);
    line(m, p1, p2, 130);
    for(int i=0; i<nearestSet.elem.size();i++)
    {
        m.at<uchar>(nearestSet.elem[i]) = 255;
    }
    namedWindow("Nearest", CV_WINDOW_NORMAL);
    imshow("Nearest", m);
    waitKey(0);


*/


    /*

    //Phase 6: compute  distances among corners
    //Init a false boolean mat for corners couples
    bool connected[corners.size()][corners.size()];
    for(int i=0; i<corners.size(); i++)
    {
        for(int j=i+1; j<corners.size(); j++)
        {
            connected[i][j]=false;
        }
    }

    //Compute all distances among corners of different sets
    double distances[corners.size()][corners.size()];
    for(int i=0; i<corners.size(); i++)
    {
        for(int j=0; j<corners.size(); j++)
        {
            distances[i][j] = 0;
        }
    }

    for(int i=0; i<sets.size(); i++)
    {
        for(int j=0; j<sets[i].corners.size(); j++)
        {
            for(int k=i+1; k<sets.size(); k++)
            {
                for(int l=0; l<sets[k].corners.size(); l++)
                {
                    //Find row index and col index
                    int k1 = 0, k2 = 0;
                    int row, col;
                    //Find correct row index in boolean table
                    for(int n1 = 0; n1<i; n1++)
                    {
                        k1 += sets[n1].corners.size();
                    }
                    for(int n2 = 0; n2 < j; n2++) k1++;
                    //Find correct column index in boolean table
                    for(int n1 = 0; n1<k; n1++)
                    {
                        k2 += sets[n1].corners.size();
                    }
                    for(int n2 = 0; n2 < l; n2++) k2++;

                    row = k1;
                    col = k2;

                    distances[row][col] = sqrt((sets[i].corners[j].x - sets[k].corners[l].x)*(sets[i].corners[j].x - sets[k].corners[l].x) + (sets[i].corners[j].y - sets[k].corners[l].y)*(sets[i].corners[j].y - sets[k].corners[l].y));
                }
            }
        }
    }

    std::cout<<"Distances"<<std::endl;
    for(int i=0; i<corners.size(); i++)
    {
        std::cout<<std::endl;
        for(int j=0; j<corners.size(); j++)
        {
            std::cout << std::setw(3)<<round(distances[i][j]);
        }
    }

    waitKey(0);

    //Phase 6: find nearest corners from different sets. Join their sets together (drawLine).
    double dMin = DBL_MAX;
    Point p1, p2;
    int s1, s2;
    int c1,c2;
    double d;
    char c;
    namedWindow("Connecting...", CV_WINDOW_NORMAL);
    int ctr =0;
    int color = 200;
    do
    {
        //c=waitKey(2000);
        dMin = DBL_MAX;
        c1 = -1;
        c2 = -1;
        for(int i=0; i<corners.size(); i++)
        {
            for(int j=i+1; j<corners.size(); j++)
            {
                if(distances[i][j]>0 && distances[i][j]< dMin && connected[i][j]==false)
                {
                    dMin = distances[i][j];
                    c1 = i;
                    c2 = j;
                }
            }
        }

        if(c1 == -1) break;

        std::cout<< "Best distance = " << dMin << " between c1 = "<< c1 << "and c2 = " << c2;

        //get corner 1 coordinates
        int n = 0;
        for(int k1=0; k1<sets.size();k1++)
        {
            if(n+sets[k1].corners.size() <= c1)
            {
                n+=sets[k1].corners.size();
            }
            else
            {
                p1 = sets[k1].corners[c1-n];
                break;
            }
        }
        //get corner 2 coordinates
        n = 0;
        for(int k1=0; k1<sets.size();k1++)
        {
            if(n+sets[k1].corners.size() <= c2)
            {
                n+=sets[k1].corners.size();
            }
            else
            {
                p2 = sets[k1].corners[c2-n];
                break;
            }
        }
        std::cout<<"; corresponding points = "<< p1<<" and "<< p2<<std::endl;
        connected[c1][c2] = true;
        line(dMat, p1, p2, color);
        //color-=10;
        imshow("Connecting...", dMat);
        std::cout<<"Connecting: "<<ctr++<<std::endl;
        waitKey(0);
    }
    while(true);
    std::cout<<"Lines drawn."<<std::endl;

    //Phase 7: for each corner (label>=5) compute gradient (external or internal?)
    //namedWindow( "Connecting corners", CV_WINDOW_NORMAL );
*/
    return twinkleSet;
}

void twinkle::findLimbusContour(cv::Mat mat, Point* pupil)
{
    Point cursorUp(*pupil);
    Point cursorDown(cursorUp.x, cursorUp.y+1);
    Point cursorLeft(cursorUp);
    Point cursorRight(cursorUp.x+1, cursorUp.y);

    std::vector<Point> limbusContourLR, limbusContourUD;

    bool debug = true;
    Mat debMatLR(mat.rows, mat.cols, CV_8U);
    Mat debMatUD(mat.rows, mat.cols, CV_8U);

    mat.copyTo(debMatLR);
    mat.copyTo(debMatUD);

    if(debug)
    {
        namedWindow("Limbus contour LR", CV_WINDOW_NORMAL);
        namedWindow("Limbus contour UD", CV_WINDOW_NORMAL);
    }

    mat.at<uchar>(cursorUp) = 0;

    std::cout<<"limbusContourLR.size() = "<<limbusContourLR.size()<<std::endl;
    moveCursor(cursorUp, mat, &limbusContourLR, debug, debMatLR, UP);
    std::cout<<"After moveUp limbusContourLR.size() = "<<limbusContourLR.size()<<std::endl;
    moveCursor(cursorDown, mat, &limbusContourLR, debug, debMatLR, DOWN);
    std::cout<<"After moveDown limbusContourLR.size() = "<<limbusContourLR.size()<<std::endl;

    moveCursor(cursorLeft, mat, &limbusContourUD, debug, debMatUD, LEFT);
    std::cout<<"After moveLeft limbusContourUD.size() = "<<limbusContourUD.size()<<std::endl;
    moveCursor(cursorRight, mat, &limbusContourUD, debug, debMatUD, RIGHT);
    std::cout<<"After moveRight limbusContourUD.size() = "<<limbusContourUD.size()<<std::endl;
}

void twinkle::moveCursor(Point p, Mat m, std::vector<Point> *l, bool debug, Mat debMat, direction dir)
{
    while(m.at<uchar>(p) == 0)
    {
        if(dir == UP || dir == DOWN)
        {
            //Find left barrier
            findBarrier(p, m, l, debug, debMat, LEFT);

            //Find right barrier
            findBarrier(p, m, l, debug, debMat, RIGHT);
        }
        else
        {
            //Find up barrier
            findBarrier(p, m, l, debug, debMat, UP);

            //Find down barrier
            findBarrier(p, m, l, debug, debMat, DOWN);
        }

        if(debug)
        {
            if(dir == UP || dir == DOWN)
            {
                imshow("Limbus contour LR", debMat);
            }
            else
            {
                imshow("Limbus contour UD", debMat);
            }
            waitKey(0);
        }

        //Prepare for next iteration
        switch(dir)
        {
        case UP:
            p.y--;
            break;
        case DOWN:
            p.y++;
            break;
        case LEFT:
            p.x--;
            break;
        case RIGHT:
            p.x++;
            break;
        default: break;
        }

        if(dir == UP || dir == DOWN)
        {
            if(p.y<0 || p.y>=m.rows) break;
        }
        else
        {
            if(p.x<0 || p.x>=m.cols) break;
        }
    }

}


void twinkle::findBarrier(Point p, Mat m, std::vector<Point> *l, bool debug, Mat debMat, direction dir)
{
    Point p2;
    bool toAdd;

    p2=p;

    toAdd = true;

    while(m.at<uchar>(p2) == 0)
    {
        switch(dir)
        {
        case LEFT:
            p2.x--;
            break;
        case RIGHT:
            p2.x++;
            break;
        case UP:
            p2.y--;
            break;
        case DOWN:
            p2.y++;
            break;
        default: break;
        }

        if((dir==LEFT && p2.x<0) || (dir==RIGHT && p2.x>=m.cols) || (dir==UP && p2.y<0) || (dir==DOWN && p2.y>=m.rows))
        {
            toAdd = false;
            break;
        }
    }


    if(toAdd)
    {
        for(uint i=0; i<l->size(); i++)
        {
            if((*l)[i] == p2) toAdd = false;
        }
        if(toAdd)
        {
            l->push_back(p2);
            if(debug)
            {
                debMat.at<uchar>(p2) = 150;
            }
        }
    }
}

//Dilate: apply dilation morphologic algorithm
cv::Mat twinkle::twinkleDilate(const cv::Mat& roi)  //Work on neighborhood to define point
{
    #define NMASK 4
    #define SIZE 3

    /*
    int maskH[SIZE][SIZE] = {{128,128,128},
                            { 255,  0,255},
                            { 128,128,128}};  //128 stands for don't care

    int maskV[SIZE][SIZE] = {{128, 255, 128},
                             {128,   0, 128},
                             {128, 255, 128}};

    int maskDiagDown[SIZE][SIZE] = {{255, 128, 128},
                                    {128,   0, 128},
                                    {128, 128, 255}};

    int maskDiagUp[SIZE][SIZE] = {{128, 128, 255},
                                  {128,   0, 128},
                                  {255, 128, 128}};
    */

    int mask[NMASK][SIZE][SIZE] = {{{255, 128, 128},
                                    {128,   0, 128},
                                    {128, 128, 255}},

                                   {{128, 128, 255},
                                    {128,   0, 128},
                                    {255, 128, 128}},

                                   {{128,128,128},
                                    {255,  0,255},
                                    {128,128,128}},

                                   {{128, 255, 128},
                                    {128,   0, 128},
                                    {128, 255, 128}}};

    cv::Mat diffRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
    cv::Mat completeRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
    cv::Mat partialReturnRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
    cv::Mat returnRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
    cv::Mat currentRoi = roi;
    bool changeVal;
    int grayVal = 0;

    //Only diagonal dilation
    for(int m=0; m<4; m++)
    {
        grayVal +=51;
        for(int i=0; i<currentRoi.rows; i++)
        {
            for(int j=0; j<currentRoi.cols; j++)
            {
                if(currentRoi.at<uchar>(i,j) == mask[m][SIZE/2][SIZE/2])
                {
                    int k1min = i-SIZE/2;
                    int k1Max = i+SIZE/2;
                    int k2min = j-SIZE/2;
                    int k2Max = j+SIZE/2;
                    changeVal = false;
                    if(k1min >= 0 && k1Max<currentRoi.rows && k2min >=0 && k2Max<currentRoi.cols)
                    {
                        changeVal = true;
                        for(int k1=k1min; k1<=k1Max; k1++)
                        {
                            for(int k2=k2min; k2<=k2Max; k2++)
                            {
                                if(mask[m][k1-i+SIZE/2][k2-j+SIZE/2] != 128) //ignore don't care
                                {
                                    if(currentRoi.at<uchar>(k1,k2) != mask[m][k1-i+SIZE/2][k2-j+SIZE/2]) changeVal = false;
                                }
                            }
                        }
                    }
                    if(changeVal)
                    {
                        diffRoi.at<uchar>(i,j) = grayVal; //grayVal; --> grayVal to see differences (partial), 128; to prepare for algorithm, expression
                    }
                }
            }
        }

        if(m==1) //First merge
        {
            for(int i=0;i<roi.rows; i++)
            {
                for(int j=0; j<roi.cols; j++)
                {
                    if(diffRoi.at<uchar>(i,j) > 0)
                    {
                        partialReturnRoi.at<uchar>(i,j) = 255-roi.at<uchar>(i,j);
                    }
                    else
                    {
                        partialReturnRoi.at<uchar>(i,j) = roi.at<uchar>(i,j);
                    }
                }
            }
            currentRoi = partialReturnRoi;
//            char*pName = "partial";
//            namedWindow(pName, CV_WINDOW_NORMAL);
//            imshow(pName, currentRoi);
//            waitKey(0);
        }
    }


    for(int i=0;i<currentRoi.rows; i++)
    {
        for(int j=0; j<currentRoi.cols; j++)
        {
            if(diffRoi.at<uchar>(i,j) > 0)
            {
                completeRoi.at<uchar>(i,j) = diffRoi.at<uchar>(i,j);
            }
            else
            {
                completeRoi.at<uchar>(i,j) = currentRoi.at<uchar>(i,j);
            }

            if(diffRoi.at<uchar>(i,j) > 128)
            {
                returnRoi.at<uchar>(i,j) = 255-currentRoi.at<uchar>(i,j);
            }
            else
            {
                returnRoi.at<uchar>(i,j) = currentRoi.at<uchar>(i,j);
            }
        }
    }

//    char*dName = "Differences";
//    namedWindow(dName, CV_WINDOW_NORMAL);
//    imshow(dName, completeRoi);
//    waitKey(0);

    return returnRoi;
}

std::vector<Point> twinkle::labelPoints(cv::Mat mat)
{
    int labelCount[mat.rows*mat.cols];

    std::vector<Point>corners;
    int di, dj;
    int recAns;
    for(int i=0; i<mat.rows; i++)
    {
        for(int j=0; j<mat.cols; j++)
        {
            std::cout<<"(i,j) = ("<<i<<","<<j<<")"<<std::endl;
            labelCount[i*mat.cols+j] = 0;

            if(i>=1 && i<mat.rows-1 && j>=1 && j<mat.cols-1)
            {
                if(mat.at<uchar>(i,j)  == 255)
                {
                    std::vector<bool> checked(9,false);
                    for(int k = 0; k<9; k++)  //foreach point in mask perimeter
                    {
                        if(!checked[k] && k!=4)
                        {
                            std::cout<<"Unchecked point " << k <<": recursive stuff"<<std::endl;
                            recAns = 0;

                            switch(k%3)
                            {
                            case 0: dj = -1;
                                break;
                            case 1: dj = 0;
                                break;
                            case 2: dj = 1;
                                break;
                            }
                            switch(k/3)
                            {
                            case 0: di = -1;
                                break;
                            case 1: di = 0;
                                break;
                            case 2: di = 1;
                                break;
                            }

                            std::cout<<"di = "<<di<<", dj = " <<dj<<std::endl;

                            checked[k] = true;
                            if(mat.at<uchar>(i+di, j+dj) == 0)
                            {
                                recAns++;
                                recAns += checkNext(k, true, checked, mat, i, j);
                                recAns += checkNext(k, false, checked, mat, i, j);
                            }

                            std::cout<<"End of recursion: checked vector is ";
                            for(uint c=0; c<checked.size();c++) std::cout << checked[c] << " - ";
                            std::cout<<std::endl;

                            if(recAns > labelCount[i*mat.cols+j])
                            {
                                labelCount[i*mat.cols+j] = recAns;
                            }
                        }
                    }
                }
            }

            if(labelCount[i*mat.cols+j] > 0)
            {
                std::cout<<"Point is labelled with "<< labelCount[i*mat.cols+j]<<std::endl;
                if(labelCount[i*mat.cols+j] >= 5)
                {
                    Point p(j,i);
                    corners.push_back(p);
                    std::cout<<"----- New corner found!"<<std::endl;
                }
                //cv::waitKey(0);
            }
        }
    }

    std::cout<<"Found " << corners.size() << " corners."<<std::endl;


    //cv::Mat cornerImg(mat.rows, mat.cols, CV_8U);
    cv::Mat cornerImg = Mat::zeros(mat.rows, mat.cols, CV_8U);
    for(uint i=0; i<corners.size(); i++)
    {
        cornerImg.at<uchar>(corners[i]) = 255;
    }

    /// Show in a window
    namedWindow( "Corners", CV_WINDOW_NORMAL );
    imshow( "Corners", cornerImg);
    waitKey(0);

    return corners;

    /*
    cv::Mat labelledImg(dMat.rows, dMat.cols, CV_8U);
    int grayStep = 30;
    for(int i=0; i<dMat.rows;i++)
    {
        for(int j=0; j<dMat.cols; j++)
        {
            labelledImg.at<uchar>(i,j) = grayStep*labelCount[i*dMat.cols+j];
        }
    }

    /// Show in a window
    namedWindow( "Labelled", CV_WINDOW_NORMAL );
    imshow( "Labelled", labelledImg);
    waitKey(0);
    */
}

std::vector<setInformation> twinkle::fillSetCornerData(cv::Mat mat, std::vector<Point> corners)
{
    bool debug = true;
    bool multipleWindows = false;
    char input;

    std::vector<setInformation> sets = bin->findSets(mat, false);

    for(uint i=0; i<sets.size(); i++)
    {
        Mat m=Mat::zeros(mat.rows, mat.cols, CV_8U);
        std::cout<<"Set "<< i<<": "<< sets[i].nPoints << " punti."<<std::endl;

        for(uint j=0; j<sets[i].elem.size(); j++)
        {
            m.at<uchar>(sets[i].elem[j].y, sets[i].elem[j].x) = 150;
            for(uint c=0; c<corners.size(); c++)
            {
                if(sets[i].elem[j] == corners[c])
                {
                   std::cout<<"Found corner for set "<< i <<std::endl;
                   sets[i].corners.push_back(Point(corners[c]));
                   m.at<uchar>(sets[i].elem[j]) = 250;
                   break;
                }
            }
        }

        if(debug)
        {
            std::stringstream s;
            s<< "Set ";
            if(multipleWindows) s<< i;
            namedWindow(s.str(), CV_WINDOW_NORMAL);
            imshow(s.str(), m);
            input = waitKey(0);
            if(input == 'c') debug=false;
        }
    }

    return sets;
}

Size twinkle::nextSearchArea(Point* tl, Point last, Point previous, bool left)
{
    int dx = last.x-previous.x;
    int dy = last.y-previous.y;
    //double grad;
    Size s;

    //---------Special cases: vertical sliding-----------
    if(dx==0)
    {
        if(dy<0)
        {
            //grad = DBL_MAX;
            std::cout<<"TWINKLE: GRADIENT IS MAX"<<std::endl;
            tl->x = last.x-1;
            tl->y = last.y-3;
        }
        else
        {
            //grad = DBL_MIN;
            std::cout<<"TWINKLE: GRADIENT IS MIN"<<std::endl;
            tl->x = last.x-1;
            tl->y = last.y +1;
        }
        s.width = 3;
        s.height = 3;
        return s;
        //Tertium non datur
    }

    //-------General behaviour: diagonal sliding---------

    //Identify new point should last gradient be repeated
    Point hyp(last.x+dx, last.y+dy);
    //Identify area zone
    int minX, maxX;
    if(left)
    {
        minX = hyp.x;
        maxX = last.x -1;
    }
    else
    {
        minX = last.x+1;
        maxX = hyp.x;
    }
    int minY = std::min(last.y-1, hyp.y);
    int maxY = std::max(last.y+1, hyp.y);
    //Assign area size
    s.height = maxY-minY+1;
    s.width = maxX-minX+1;
    std::cout<<"NEXTSEARCHAREA: Area size is "<< s <<std::endl;
    //Update top left point coords and return area size
    tl->x = minX;
    tl->y = minY;
    return s;
}


Point twinkle::selectPathPoint(Point* tl, Point sp, Size kSize)
{
    Point ans(-1,-1);
    int val = 0;

    //Special case: part of the mask out of the image
    if(tl->x<0)
    {
        if(abs(tl->x) >= kSize.width) return ans;
        else
        {
            tl->x = 0;
            kSize.width -= abs(tl->x);
        }
    }
    if(tl->x >= twinkleMat.cols) return ans;

    if(tl->y<0)
    {
        if(abs(tl->y) >= kSize.height) return ans;
        else
        {
            tl->y=0;
            kSize.height -= abs(tl->y);
        }
    }
    if(tl->y >= twinkleMat.rows) return ans;

    for(int j=tl->y; j<tl->y+kSize.height; j++)
    {
        for(int i=tl->x; i<tl->x+kSize.width; i++)
        {
            if((i!=sp.x || j!=sp.y) && twinkleMat.at<uchar>(j,i) > val)
            {
                ans.x = i;
                ans.y = j;
                val = twinkleMat.at<uchar>(j,i);
            }
        }
    }

    return ans;
}


setInformation twinkle::getNearestSet(int startingIndex, std::vector<setInformation> sets, Point* p1, Point* p2, setInformation* pupilSet)
{
    double d, dMin = DBL_MAX;
    int bSet;
    bool valid = true;


    for(uint c=0; c<sets[startingIndex].corners.size(); c++)
    {
        std::cout<<"---------\n----------\nCorner di partenza: "<< c << std::endl;
        valid = true;
        //Check if this corner also is in the limbus set. If it is, step over (don't consider corners in the limbus).
        for(uint k1=0; k1<pupilSet->elem.size(); k1++)
        {
            if(sets[startingIndex].corners[c] == pupilSet->elem[k1])
            {
                valid = false;
                std::cout<<"Corner is not valid: limbus corner"<<std::endl;
            }
        }

        if(valid)
        {
            for(uint i=0; i<sets.size(); i++)
            {
                if(i!=startingIndex)
                {
                    std::cout<<"--------\nSet di confronto: "<<i<<", nCorner = "<<sets[i].corners.size()<<std::endl;
                    for(uint j=0; j<sets[i].corners.size(); j++)
                    {
                        std::cout<<"Corner di confronto: "<<j<<std::endl;
                        d = sqrt((sets[startingIndex].corners[c].x - sets[i].corners[j].x)*(sets[startingIndex].corners[c].x - sets[i].corners[j].x) + (sets[startingIndex].corners[c].y -sets[i].corners[j].y)*(sets[startingIndex].corners[c].y -sets[i].corners[j].y));
                        std::cout<<"Distance: " << d;
                        if(d<dMin)
                        {
                            std::cout<<" is the new dMin";
                            dMin = d;
                            bSet = i;
                            *p1 = sets[startingIndex].corners[c];
                            *p2 = sets[i].corners[j];
                        }

                        std::cout<<std::endl;
                    }
                }
            }
        }
    }

    return sets[bSet];

}

int twinkle::checkNext(int k, bool next, std::vector<bool>& checked, cv::Mat mat, int i, int j)
{
    int di, dj;
    int attiguousBlack = 0;
    int nextVal, prevVal;
    int localK;
    bool debug = true;


    if(debug)
    {
        std::cout<<"CheckNext: entered with following checked vector:"<<std::endl;
        for(uint c=0; c<checked.size();c++) std::cout<<"- " <<checked[c]<<" ";
        std::cout<<std::endl;
    }

    switch(k)
    {
    case 0:
        nextVal = 1;
        prevVal = 3;
        break;
    case 1:
        nextVal = 2;
        prevVal = 0;
        break;
    case 2:
        nextVal = 5;
        prevVal = 1;
        break;
    case 3:
        nextVal = 0;
        prevVal = 6;
        break;
    case 4:
        break;
    case 5:
        nextVal = 8;
        prevVal = 2;
        break;
    case 6:
        nextVal = 3;
        prevVal = 7;
        break;
    case 7:
        nextVal = 6;
        prevVal = 8;
        break;
    case 8:
        nextVal = 7;
        prevVal = 5;
        break;
    }

    if(next) localK=nextVal;
    else localK=prevVal;

    switch(localK%3)
    {
    case 0:
        dj = -1;
        break;
    case 1: dj = 0;
        break;
    case 2: dj = 1;
        break;
    }
    switch(localK/3)
    {
    case 0: di = -1;
        break;
    case 1: di = 0;
        break;
    case 2: di = 1;
        break;
    }

    if(!checked[localK])
    {
        if(mat.at<uchar>(i+di, j+dj) == 0)
        {
            attiguousBlack++;
            attiguousBlack += checkNext(localK, next, checked, mat, i, j);
        }

        checked[localK] = true;
    }

    if(debug)
    {
        std::cout<<"CheckNext: exiting with follow checked vector:"<<std::endl;
        for(uint c=0; c<checked.size();c++) std::cout<<"- " <<checked[c]<<" ";
        std::cout<<std::endl;
    }

    return attiguousBlack;
}


void twinkle::tryGoingDown(Mat m, Point p, std::string wName)
{
    //TODO!!! RIVEDERE alla luce di MOVEMASK!

    /*
    Size kSize(3,3);
    Point* tl = new Point(p);
    tl->x--;
    tl->y++;
    //grad = DBL_MIN;
    p = selectPathPoint(tl, p, kSize);
    if(m.at<uchar>(p) == 0)
    {
        std::cout<<"Old path"<<std::endl;
        return;
    }
    else
    {
        std::cout<<"New path"<<std::endl;
        m.at<uchar>(p) = 0;
        imshow(wName, m);
        waitKey(0);

        kSize = nextSearchArea(tll, pl, pl2, true);

        pl2 = pl;

        pl = selectPathPoint(tll, pl, kSize);

        if(pl.x != -1)
        {
            m.at<uchar>(pl) = 0;
            std::cout<<"TWINKLE: NEW LEFT POINT("<<pl.x<<","<<pl.y<<")"<<std::endl;
            imshow(wName, m);
            waitKey(0);
    }
    */
    return;
}


Point twinkle::moveMask(int k, Mat img, Point p, int* dirChosen)
{
    std::cout<<"MOVEMASK: ENTERED"<<std::endl;

    //Given a direction, only 3/8 movements are possible next time
    int m1 = (k+7)%8;
    int m2 = k;
    int m3 = (k+1)%8;

    //std::cout<<"MOVEMASK: possible directions selected."<<std::endl;

    Point tls[8];

    tls[0].x = -1;
    tls[0].y = -3;

    tls[1].x = 0;
    tls[1].y = -2;

    tls[2].x = 1;
    tls[2].y = -1;

    tls[3].x = 0;
    tls[3].y = 0;

    tls[4].x = -1;
    tls[4].y = 1;

    tls[5].x = -2;
    tls[5].y = 0;

    tls[6].x = -3;
    tls[6].y = -1;

    tls[7].x = -2;
    tls[7].y = -2;

    //std::cout<<"MOVEMASK: vector tls init ok."<<std::endl;

    //Find tl for three possible masks
    Point tlm[3];
    tlm[0].x = p.x+tls[m1].x;
    tlm[0].y = p.y+tls[m1].y;
    tlm[1].x = p.x+tls[m2].x;
    tlm[1].y = p.y+tls[m2].y;
    tlm[2].x = p.x+tls[m3].x;
    tlm[2].y = p.y+tls[m3].y;

    std::cout<<"MOVEMASK: Starting p = "<<p<<", possible m = "<<m1 << ", "<<m2<<", "<<m3<<std::endl;
    for(int i=0; i<3; i++)
    {
        //std::cout<<"TLM["<<i<<"] = "<<tlm[i].x<<","<<tlm[i].y<<std::endl;
    }
    //std::cout<<"MOVEMASK: vector tlm init ok."<<std::endl;

    double bestAvg = 0;
    int bestMask;
    double avg;

    //std::cout<<"MOVEMASK: entering bestAvg loop."<<std::endl;

    //Find best avg val for three masks in image
    for(int i=0; i<3; i++)
    {
        //std::cout<<"MOVEMASK: entering i loop."<<std::endl;
        int sum = 0;
        for(int y=tlm[i].y; y<tlm[i].y+3; y++)
        {
            //std::cout<<"MOVEMASK: entering y loop."<<std::endl;
            for(int x=tlm[i].x; x<tlm[i].x+3; x++)
            {
                //std::cout<<"MOVEMASK: entering x loop. X,Y = "<<x<<","<<y<<std::endl;
                sum += img.at<uchar>(y,x);
            }
        }
        avg = (double)sum/9;
        if(avg > bestAvg)
        {
            bestAvg = avg;
            bestMask = i;
        }
    }

    //std::cout<<"MOVEMASK: best avg ok."<<std::endl;

    //Select direction
    int bestDirection;
    switch(bestMask)
    {
    case 0: bestDirection = m1;
        break;
    case 1: bestDirection = m2;
        break;
    case 2: bestDirection = m3;
        break;
    default: break;
    }

    //std::cout<<"MOVEMASK: best direction ok"<<std::endl;

    Point next(p.x + tls[bestDirection].x +1, p.y+tls[bestDirection].y+1);

    std::cout<<"MOVEMASK: point assigned ("<<next<<") "<<std::endl;

    *dirChosen = bestDirection;

    return next;

}


/*
    //-------------------------------OLD LINE-------------------
    Point p(pupil->x, pupil->y);
    mat.copyTo(twinkleMat);

    double grad;

    char* wName;
    cv::Mat m = this->newThresholded(mat.rows, mat.cols);

    //m = this->thresholdImg(twinkleMat, 15);








    int r = 0;

    Point p2(p);
    Point* tl = new Point(p.x-1, p.y-3);
    Size kSize(3,3);

    //Draw pupil
    m.at<uchar>(p) = 0;

    //First movement: up
    std::cout<<"TWINKLE: FIRST TIME"<<std::endl;
    imshow(wName, m);
    waitKey(0);

    int dirUL = 0, dirUR = 0, dirDL = 4, dirDR = 4;

    p = this->moveMask(0, twinkleMat, p, &dirUL);
    std::cout<<"Chosen (up)"<<dirUL<<" direction."<<std::endl;
    m.at<uchar>(p) = 0;
    p2 = this->moveMask(4, twinkleMat, p2, &dirDL);
    std::cout<<"Chosen (down)"<<dirDL<<" direction."<<std::endl;
    m.at<uchar>(p2) = 0;
    imshow(wName, m);
    waitKey(0);

    //Second movement: to the four corners
    Point pl(p), pr(p);
    Point pl2(p2), pr2(p2);
    pl = this->moveMask(7, twinkleMat, p, &dirUL);
    std::cout<<"Chosen (ul)"<<dirUL<<" direction."<<std::endl;
    pr = this->moveMask(1, twinkleMat, p, &dirUR);
    std::cout<<"Chosen (ur)"<<dirUR<<" direction."<<std::endl;
    m.at<uchar>(pl) = 0;
    m.at<uchar>(pr) = 0;
    pl2 = this->moveMask(5, twinkleMat, p2, &dirDL);
    std::cout<<"Chosen (dl)"<<dirDL<<" direction."<<std::endl;
    pr2 = this->moveMask(3, twinkleMat, p2, &dirDR);
    std::cout<<"Chosen (dr)"<<dirDR<<" direction."<<std::endl;
    m.at<uchar>(pl2) = 0;
    m.at<uchar>(pr2) = 0;
    imshow(wName, m);
    waitKey(0);


    //Third movement: one left, one right
    pl = this->moveMask(6, twinkleMat, pl, &dirUL);
    std::cout<<"Chosen "<<dirUL<<" direction."<<std::endl;
    pr = this->moveMask(2, twinkleMat, pr, &dirUR);
    std::cout<<"Chosen "<<dirUR<<" direction."<<std::endl;
    m.at<uchar>(pl) = 0;
    m.at<uchar>(pr) = 0;
    pl2 = this->moveMask(6, twinkleMat, pl2, &dirDL);
    std::cout<<"Chosen (dl)"<<dirDL<<" direction."<<std::endl;
    pr2 = this->moveMask(2, twinkleMat, pr2, &dirDR);
    std::cout<<"Chosen (dr)"<<dirDR<<" direction."<<std::endl;
    m.at<uchar>(pl2) = 0;
    m.at<uchar>(pr2) = 0;
    imshow(wName, m);
    waitKey(0);

    //Loop left and right
    bool plOk = true, prOk = true;
    do{
        if(plOk)
        {
            pl = moveMask(dirUL, twinkleMat, pl, &dirUL);
            std::cout<<"Chosen (left)"<<dirUL<<" direction."<<std::endl;
            if(pl.x<=0 || pl.x>=m.rows || pl.y<=0 || pl.y>=m.cols) plOk = false;
            else m.at<uchar>(pl) = 0;














        }
        if(prOk)
        {
            pr = moveMask(dirUR, twinkleMat, pr, &dirUR);
            std::cout<<"Chosen (right)"<<dirUR<<" direction."<<std::endl;
            if(pr.x<=0 || pr.x>=m.rows || pr.y<=0 || pr.y>=m.cols) prOk = false;
            else m.at<uchar>(pr) = 0;
        }
        imshow(wName,m);
        waitKey(0);
    } while(plOk || prOk);


    grad = DBL_MAX;
    p = selectPathPoint(tl, p, kSize);
    m.at<uchar>(p) = 0;


    //Second movement: one left and one right
    //Point pl(p), pr(p);
    //Point pl2(pl), pr2(pr);
    Point* tll = new Point(pl.x-3, pl.y-1);
    pl = selectPathPoint(tll, pl, kSize);
    m.at<uchar>(pl) = 0;
    Point* tlr = new Point(pr.x+1, pr.y-1);
    pr = selectPathPoint(tlr, pr, kSize);
    m.at<uchar>(pr) = 0;
    imshow(wName, m);
    waitKey(0);


    bool left;

    //Loop
    do {
        std::cout<<"TWINKLE: ROUND "<<r++<<std::endl;

        if(pr.x!=-1 && pr!=pr2)
        {
            kSize = nextSearchArea(tlr, pr, pr2, false);

            pr2 = pr;

            pr = selectPathPoint(tlr, pr, kSize);

            if(pr.x != -1)
            {
                m.at<uchar>(pr) = 0;
                std::cout<<"TWINKLE: NEW RIGHT POINT("<<pr.x<<","<<pr.y<<")"<<std::endl;
                imshow(wName, m);
                waitKey(0);
                tryGoingDown(m, pr, wName);
            }
        }

        if(pl.x!=-1 && pl!=pl2)
        {
            kSize = nextSearchArea(tll, pl, pl2, true);

            pl2 = pl;

            pl = selectPathPoint(tll, pl, kSize);

            if(pl.x != -1)
            {
                m.at<uchar>(pl) = 0;
                std::cout<<"TWINKLE: NEW LEFT POINT("<<pl.x<<","<<pl.y<<")"<<std::endl;

                imshow(wName, m);



                waitKey(0);

                tryGoingDown(m, pl, wName);
            }
        }


        imshow(wName, m);


        waitKey(0);

    } while(pl.x != -1 || pr.x != -1); //m.at<uchar>(p) != 0);

    std::cout<<"TWINKLE: EXITING"<<std::endl;

    return twinkleSet;
}
*/
