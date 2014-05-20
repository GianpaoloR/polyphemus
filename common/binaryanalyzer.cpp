
#include "binaryanalyzer.h"

binaryAnalyzer::binaryAnalyzer()
{
    #ifdef WITH_GUI
    gui = NULL;
    #endif //WITH_GUI

    mat_id = 0;
    setId = 0;
    oneIsLast = true;
    overallSets.clear();
}

#ifdef WITH_GUI
//SetDebugGui: enables gui for debugger
void binaryAnalyzer::setDebugGui(guiHandler *gui)
{
    this->gui = gui;
}
#endif //WITH_GUI

//NewThresholded: initializes a new zero filled cv::Mat. Returns the mat.
cv::Mat binaryAnalyzer::newThresholded(int r, int c)
{
    cv::Mat m(r, c, CV_8U);

    for(int i=0; i<r; i++)
    {
        for(int j=0; j<c; j++)
        {
            m.at<uchar>(i,j) = 255;
        }
    }

    return m;
}

//ThresholdImg: select points whose value is under tVal. Returns a binary image with selected points in black.
cv::Mat binaryAnalyzer::thresholdImg(const cv::Mat &roi, int tVal)
{
    cv::Mat tImg = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);

    for(int i=0; i<roi.rows; i++)
    {
        for(int j=0; j<roi.cols; j++)
        {
            if(roi.at<uchar>(i,j) > tVal)
            {
                tImg.at<uchar>(i,j) = 255;
            }
        }
    }
    return tImg;
}

//Close: apply closing morphologic algorithm
cv::Mat binaryAnalyzer::close(const cv::Mat& roi)
{
    cv::Mat m1 = invert(roi);
    cv::Mat m2=open(m1);
    cv::Mat final = invert(m2);
    return final;
}

//Open: apply opening morphologic algorithm
cv::Mat binaryAnalyzer::open(const cv::Mat& roi)
{
    cv::Mat m1 = erode(roi);
    cv::Mat m2 = dilate(m1);
    return m2;
}



//Dilate: apply dilation morphologic algorithm
cv::Mat binaryAnalyzer::dilate(const cv::Mat& roi)
{
    #define SIZE 3
    int mask[SIZE][SIZE] = {{255,255,255},
                            {255,255,255},
                            {255,255,255}};
    cv::Mat returnRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);

    for(int i=0; i<roi.rows; i++)
    {
        for(int j=0; j<roi.cols; j++)
        {
            if(roi.at<uchar>(i,j) == mask[SIZE/2][SIZE/2])
            {
                for(int k1=i-SIZE/2; k1<=i+SIZE/2; k1++)
                {
                    if(k1>=0 && k1<roi.rows)
                    {
                        for(int k2=j-SIZE/2; k2<=j+SIZE/2; k2++)
                        {
                            if(k2>=0 && k2<roi.cols)
                            {
                                returnRoi.at<uchar>(k1,k2) = mask[k1-i+SIZE/2][k2-j+SIZE/2];
                            }
                        }
                    }
                }
            }
        }
    }
    return returnRoi;
}

//Erode: applies erosion morphologic algorithm
cv::Mat binaryAnalyzer::erode(const cv::Mat& roi)
{
    #define SIZE 3
    int structElem[SIZE][SIZE] =
    {
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255}
    };

    cv::Mat returnRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
    bool ok;

    for(int i=0; i<roi.rows; i++)
    {

        for(int j=0; j<roi.cols; j++)
        {
            if(roi.at<uchar>(i,j) == structElem[SIZE/2][SIZE/2])
            {
                ok = true;
                for(int k1=i-SIZE/2; k1<=i+SIZE/2 && ok; k1++)
                {
                    if(k1>=0 && k1<roi.rows)
                    {
                        for(int k2=j-SIZE/2; k2<=j+SIZE/2 && ok; k2++)
                        {
                            if(k2>=0 && k2<roi.cols)
                            {
                                //General behaviour
                                if(roi.at<uchar>(k1,k2) != structElem[k1-i+SIZE/2][k2-j+SIZE/2])
                                {
                                    ok = false;
                                }
                            }
                        }
                    }
                }
                if(ok)
                {
                    returnRoi.at<uchar>(i,j) = 255;
                }
            }
        }
    }

    return  returnRoi;
}

//Invert: inverts black and white pixels in a binary image.
cv::Mat binaryAnalyzer::invert(const cv::Mat& roi)
{
    cv::Mat inverse = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);

    for(int i=0; i<roi.rows; i++)
    {
        for(int j=0; j<roi.cols; j++)
        {
            if(roi.at<uchar>(i,j) == 0)
            {
                inverse.at<uchar>(i,j) = 255;
            }
        }
    }

    return inverse;
}

//ClearOverallSets: destroy old sets data
void binaryAnalyzer::clearOverallSets()
{
    overallSets.clear();
    return;
}

/** FindSets: analyze the image to find autonomous sets. If timeIntegration, compare previous and last images and
 *            associate parent and sons. Returns sets in this image.
 */
std::vector<setInformation> binaryAnalyzer::findSets(const cv::Mat& img, bool timeIntegration)
{
    using namespace cv;
    using namespace std;

    int i, j;
    Point p;
    assigned = cv::Mat::zeros(img.rows, img.cols, CV_8U );

    if(timeIntegration)
    {
        lastFatherId = setId-1;
    }

    //Temporal Shift
    if(oneIsLast)
    {
        previousImg = &img2;
        lastImg = &img1;

        previousSets = &sets2;
        sets = &sets1;

        oneIsLast = false;
    }
    else
    {
        previousImg = &img1;
        lastImg = &img2;

        previousSets = &sets1;
        sets = &sets2;

        oneIsLast = true;
    }

    lastImg->release();
    img.copyTo(*lastImg);

    sets->clear();

    if(!timeIntegration)
    {
        previousSets->clear();
    }

    //Find all sets in this image
    for(i=0; i<img.rows; i++)
    {
        for(j=0; j<img.cols; j++)
        {
            p.x = j;
            p.y = i;
            if(img.at<uchar>(p) == 255)
            {
                if(assigned.at<uchar>(p) == 0)          //Unassigned point
                {
                    assigned.at<uchar>(p) = setId%255 +1; //To avoid 0 value and overflow
                    lastSet = new setInformation();
                    lastSet->nPoints=1;
                    lastSet->id = setId%255 +1;
                    lastSet->height = 0;
                    lastSet->width = 0;

                    lastSet->elem.push_back(p);
                    findNeighbors(p);

                    if(lastSet->nPoints > NOISE_SIZE)
                    {
                        sets->push_back(*lastSet);
                        setId++;
                        delete(lastSet);
                    }

                    lastImg->at<uchar>(p) = 255;

                    //std::cout << "SetId = "<<lastSet->id << "  " <<setId<< ", nPoints = "<< lastSet->nPoints<<std::endl;
                }
            }
        }
    }

    //setId--;


    if(timeIntegration)
    {
        setHistoryUpdate();
        updateSetsOverall();
    }

    return *sets;
}

//FindNeighbors: recursive algorithm called by findSets
void binaryAnalyzer::findNeighbors(cv::Point p)
{
    for(int i=p.y-1; i <= p.y+1; i++)
    {
        if(i>=0 && i<lastImg->rows)
        {
            for(int j=p.x-1; j<=p.x+1; j++)
            {
                if(j>=0 && j<lastImg->cols)
                {
                    cv::Point n(j,i);
                    if(lastImg->at<uchar>(n) == 255)
                    {
                        if(assigned.at<uchar>(n) == 0)
                        {
                            assigned.at<uchar>(n) = (lastSet->id)%255 +1;  //To avoid 0 values and overflow
                            lastSet->nPoints++;
                            lastSet->elem.push_back(n);

                            findNeighbors(n);

                            lastImg->at<uchar>(p) = 255;
                        }
                    }
                }
            }
        }
    }
    return;
}

/** SetHistoryUpdate: if timeIntegration was required, this function is called to associate parents and sons
 *                  and track sets' evolution from previous to last image.
 */
void binaryAnalyzer::setHistoryUpdate()
{
    cv::Point pSon, pFather;
    bool found;

    born.clear();
    alive.clear();
    dead.clear();

    std::cout<<"Previous size= "<<previousSets->size() << ", now size = " << sets->size() << std::endl;
    //Associate sons and fathers
    for(uint i=0; i<sets->size(); i++)                               //For each last set...
    {
        found = false;
        pSon = ((*sets)[i]).elem[0];                                //take first point...
        for(uint j=0; !found && j<previousSets->size(); j++)         //search for it among possible fathers...
        {
            for(uint k=0; !found && k<((*previousSets)[j]).elem.size(); k++)
            {
                pFather = ((*previousSets)[j]).elem[k];
                if(pSon == pFather)                                 //If you find it, break father's lookup (a son has only ONE father)
                {
                    found = true;
                    ((*previousSets)[j]).sons.push_back(i);
                }
            }
        }
    }

    //For each father, choose legal son and consider others like strangers

    int biggestSize, biggestSet;
    int sonIndex, nSons;
    for(uint i=0; i<previousSets->size(); i++)
    {
        biggestSize = INT_MIN;
        nSons = (*previousSets)[i].sons.size();
        if(nSons>0)
        {
            for(int j=0; j<nSons; j++)
            {
                sonIndex = (*previousSets)[i].sons[j];
                if((*sets)[sonIndex].nPoints > biggestSize)
                {
                    biggestSet = sonIndex;
                    biggestSize = (*sets)[sonIndex].nPoints;
                }
            }
            //Assign to the legal son the same id of his father
            (*sets)[biggestSet].id = (*previousSets)[i].id;
            alive.push_back((*previousSets)[i].id);
        }
        else  //If father doesn't have any son, he has died
        {
            dead.push_back((*previousSets)[i].id);
        }
    }

    //Check for newborn sets
    uint bufLimit;
    if(lastFatherId%255+sets->size()>255)
    {
        bufLimit = lastFatherId%255+sets->size()-255;
    }
    else
    {
        bufLimit = sets->size();
    }

    for(uint i=0; i<bufLimit; i++)
    {
        if((*sets)[i].id > lastFatherId%255+1)
        {
            born.push_back((*sets)[i].id);
            //overallSets.push_back((*sets)[i]);
        }
    }
    if(bufLimit<sets->size())
    {
        for(uint i=bufLimit; i<sets->size(); i++)
        {
            born.push_back((*sets)[i].id);
        }
    }


    //Output evolution
    std::cout << "Sets in this image: " << born.size()+alive.size()<< std::endl;

    std::cout << "\tNew born:"<<std::endl;
    for(uint i=0; i<born.size(); i++)
    {
        std::cout<<"\tID " << born[i]<<std::endl;
    }
    //std::cout << std::endl;

    std::cout << "\tStill alive:"<<std::endl;
    for(uint i=0; i<alive.size(); i++)
    {
        std::cout<<"\tID " << alive[i] << std::endl;
    }
    //std::cout << std::endl;

    std::cout << "\tDead:"<<std::endl;
    for(uint i=0; i<dead.size(); i++)
    {
        std::cout<<"\tID " << dead[i] << std::endl;
    }
    std::cout << std::endl;

    return;
}

/** UpdateSetsOverall: if timeIntegration was required, incrementally update set found list.
 */
void binaryAnalyzer::updateSetsOverall()
{
    uint j;
    cv::Point p;
    bool valid;
    for(uint i=0; i<sets->size(); i++)
    {
        valid = true;
        for(uint a=0; a<(*sets)[i].elem.size(); a++)
        {
            p = (*sets)[i].elem[a];
            if(p.x == 0 ||
               p.x == lastImg->cols-1 ||
               p.y == 0 ||
               p.y == lastImg->rows-1)
            {
                valid = false;
                break;
            }
        }
        if(valid)
        {
            for(j=0; j<overallSets.size(); j++)
            {
                if((*sets)[i].id == overallSets[j].id)
                break;
            }
            if(j==overallSets.size())
            {
                overallSets.push_back((*sets)[i]);
            }
        }
    }
    return;
}

/** GlueSets: merge sets nearer to each other than glueDist param. Returns a modified sets vector, with
 *            different ids and pixel lists for merged sets.
 */
std::vector<setInformation> binaryAnalyzer::glueSets(std::vector<setInformation> sets,
                                                                     int glueDist)
{
    cv::Point p1, p2;
    int xD, yD;
    bool merged;
    for(uint i=0; i<sets.size(); i++)
    {
        if(sets[i].id != NOT_VALID_ID)
        {
            for(uint j=i+1;j<sets.size(); j++)
            {
                if(sets[j].id != NOT_VALID_ID)
                {
                    merged = false;
                    for(uint k=0; !merged && k<sets[i].elem.size(); k++)
                    {
                        p1 = sets[i].elem[k];
                        for(uint l=0; !merged && l<sets[j].elem.size(); l++)
                        {
                            p2 = sets[j].elem[l];
                            xD = abs(p2.x - p1.x);
                            yD = abs(p2.y - p1.y);

                            if(xD <= glueDist && yD <= glueDist)
                            {
                                merged = true;
                            }
                        }
                    }
                    if(merged)
                    {
                        sets[i].nPoints += sets[j].nPoints;
                        sets[j].id = NOT_VALID_ID;
                        for(uint k=0; k<sets[j].elem.size(); k++)
                        {
                            sets[i].elem.push_back(sets[j].elem[k]);
                        }

                    }
                }
            }
        }
    }
    return sets;
}

/** ComputeSetSize: for each valid set, find its smallest rectangle hull. Updates sets vector.
 */
std::vector<setInformation> binaryAnalyzer::computeSetSize(std::vector<setInformation> sets)
{
    cv::Point p;
    cv::Point m;


    for(uint i=0; i<sets.size(); i++)
    {
        m.x = 0;
        m.y = 0;
        if(sets[i].id != NOT_VALID_ID)
        {
            int l=INT_MAX, r=INT_MIN, t=INT_MAX, b=INT_MIN;
            for(uint k=0; k<sets[i].elem.size(); k++)
            {
                p = sets[i].elem[k];
                if(p.x < l)
                {
                    l = p.x;
                }
                if(p.x > r)
                {
                    r = p.x;
                }
                if(p.y < t)
                {
                    t = p.y;
                }
                if(p.y > b)
                {
                    b = p.y;
                }

                m.x += p.x;
                m.y += p.y;
            }
            sets[i].width = r-l+1;
            sets[i].height = b-t+1;
            sets[i].center.x = (int)(l+r)/2;
            sets[i].center.y = (int)(t+b)/2;
            sets[i].centroid.x = m.x / sets[i].elem.size();
            sets[i].centroid.y = m.y / sets[i].elem.size();
        }
    }
    return sets;
}

/** RemoveBorderSets: filter a set list maintaining only sets far from the borders of the image.
 *                  Returns a new set vector, containing only final sets.
 */
std::vector<setInformation> binaryAnalyzer::removeBorderSets(std::vector<setInformation> sets, int r, int c)
{
    std::vector<setInformation> newSets;
    cv::Point p;
    bool remove;
    for(uint i=0; i<sets.size(); i++)
    {
        if(sets[i].id != NOT_VALID_ID)
        {
            remove = false;
            for(uint k=0; !remove && k<sets[i].elem.size(); k++)
            {
                p = sets[i].elem[k];
                if(p.x == 0 || p.x == c-1  ||
                   p.y == 0 || p.y == r-1)
                {
                    remove = true;
                    //UO
                    #ifndef WEBSERVICE
                        //KE
                        #ifdef DEBUG
                            std::cout<<"Set "<<sets[i].id<<" rimosso."<<std::endl;
                        #endif
                        //KE
                    #endif
                    //UO
                }
            }
            if(!remove)
            {
                newSets.push_back(sets[i]);
            }
        }
    }
    return newSets;
}

/** ResetId: utility to reset the id counter if desired.
 */
void binaryAnalyzer::resetId()
{
    setId = 0;
}

/** GetOverallSets: getter method. Returns all valid sets when time integrating.
 */
std::vector<setInformation> binaryAnalyzer::getOverallSets()
{
    return overallSets;
}


setInformation binaryAnalyzer::getBiggestSet(std::vector<setInformation> sets, int* index)
{
    int bSet = -1, bSize = INT_MIN;
    for(uint i=0; i<sets.size(); i++)
    {
        if(sets[i].nPoints > bSize)
        {
            bSize = sets[i].nPoints;
            bSet = i;
        }
    }

    *index = bSet;

    return sets[bSet];
}



void binaryAnalyzer::fillHoleIter(cv::Mat& roi, cv::Point start)
{
    Point p(start.x, start.y);
    namedWindow("Filling", CV_WINDOW_NORMAL);
    int color = 150;
    int colorStep = 10;

    //Find highest point in convex set
    while(roi.at<uchar>(p.y-1, p.x)!=255)
    {
        std::cout<<"p moving up"<<std::endl;
        p.y--;
        roi.at<uchar>(p) = color;
        color-=colorStep;
        imshow("Filling", roi);
        waitKey(0);
    }

    Point pl(p), pr(p);

    while(roi.at<uchar>(pl.y, pl.x-1)!=255)
    {
        std::cout<<"pl moving left"<<std::endl;
        pl.x--;
        roi.at<uchar>(pl) = color;
        color-=colorStep;
        imshow("Filling", roi);
        waitKey(0);
        while(roi.at<uchar>(pl.y-1, pl.x)!=255)
        {
            std::cout<<"pl moving up"<<std::endl;
            pl.y--;
            roi.at<uchar>(pl) = color;
            color-=colorStep;
            imshow("Filling", roi);
            waitKey(0);
        }
    }

    while(roi.at<uchar>(pr.y, pr.x+1)!=255)
    {
        std::cout<<"pr moving right"<<std::endl;
        pr.x++;
        roi.at<uchar>(pr) = color;
        color-=colorStep;
        imshow("Filling", roi);
        waitKey(0);
        while(roi.at<uchar>(pr.y-1, pr.x)!=255)
        {
            std::cout<<"pr moving up"<<std::endl;
            pr.y--;
            roi.at<uchar>(pr) = color;
            color-=colorStep;
            imshow("Filling", roi);
            waitKey(0);
        }
    }

    p = pl.y<=pr.y ? pl : pr;

    //Cover line by line
    p.y--;
    roi.at<uchar>(p) = 255;
    imshow("Filling", roi);
    waitKey(0);

    while(true)//roi.at<uchar>(p.y+1, p.x) != 255)
    {
        //Cape return
        std::cout<<"p moving down"<<std::endl;
        p.y++;
        //roi.at<uchar>(p) = color;
        //color-=colorStep;
        imshow("Filling", roi);
        waitKey(0);

        if(roi.at<uchar>(p) == 255)
        {
            do
            {
                std::cout<<"p moving right"<<std::endl;
                p.x++;
                roi.at<uchar>(p) = color;
                color-=colorStep;
                imshow("Filling", roi);
                waitKey(0);
            }
            while(roi.at<uchar>(p) == 255);
        }
        else
        {
            do
            {
                std::cout<<"p moving left"<<std::endl;
                p.x--;
                //roi.at<uchar>(p) = color;
                //color-=colorStep;
                imshow("Filling", roi);
                waitKey(0);
            }
            while(roi.at<uchar>(p) != 255);
            p.x++;
        }


        //Line covering
        Point p2(p.x, p.y);
        do
        {
            roi.at<uchar>(p2) = 255;
            p2.x++;
            imshow("Filling", roi);
            waitKey(0);
            std::cout<<"p2 moving right"<<std::endl;

        }
        while(roi.at<uchar>(p2) != 255);

    }


}

void binaryAnalyzer::fillHole(cv::Mat& roi, cv::Point start, int* opening, int* minY, int* maxY)
{
    std::cout<<"Fillhole: start = (" << start.x <<", "<<start.y<<")"<<std::endl;
    for(int i=start.y-1; i<=start.y+1; i++)
    {
        for(int j=start.x-1; j<=start.x+1; j++)
        {
            if(i!=start.y || j!= start.x)
            {
                if(roi.at<uchar>(i,j) == 0)
                {
                    roi.at<uchar>(i,j) = 150;
                    opening[i]++;
                    if(i<*minY) *minY = i;
                    if(i>*maxY) *maxY = i;
                    namedWindow("Filling", CV_WINDOW_NORMAL);
                    imshow("Filling", roi);
                    waitKey(0);
                    fillHole(roi, Point(j,i), opening, minY, maxY);
                }
            }
        }
    }
}


//---------------------------------------------------------
