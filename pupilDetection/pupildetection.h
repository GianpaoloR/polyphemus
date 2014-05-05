#ifndef PUPILDETECTION_H
#define PUPILDETECTION_H

#include "common/cvInclusions.h"
#include "common/helpers.h"
#include "common/binaryanalyzer.h"
#include "twinkle.h"
#include "pupilgui.h"
#include "pupilrois.h"


#ifdef FINDPUPILS_TEST
///<summary>Enum value to switch active-cam collection of data from an eye to another.</summary>
enum testInfo
{
    F_LEFT,
    F_RIGHT
};
#endif


///<summary>Select the algorithm for pupil detection.</summary>
enum detectionMethodEnum
{
    GREAT_CATCH,
    BLACK_HOLE,
    BLACK_HOLE_v2
};

///<summary>Select algorithm for refinement of pupil position.</summary>
enum refinementMethodEnum
{
    DARKCROSS,
    UPHOLSTER,
    DARKCROSS_EQ
};

class pupilDetection
{
public:
    ///<summary>Constructor of pupilDetection module.</summary>
    ///<param name="bin">Reference to a binaryAnalizer module (pupilDetection can't work without it)</param>
    pupilDetection(binaryAnalyzer* bin);

    //Pupil finder
    ///<summary>Main method to find pupil features.</summary>
    void findPupils(Mat leftROI, Mat rightROI);
    #ifdef TEST_MODE
    #ifdef AUTOMATIC_TEST
    #ifdef ONLY_REAL_PUPIL
    ///<summary>When in TEST_MODE, substitutes the call to findPupils and assigns manually set data.</summary>
    void bypassPupils(cv::Mat leftROI, cv::Mat rightROI);
    ///<summary>When in TEST_MODE, draws manually set pupils on the ROI instead of estimated data.</summary>
    void setRealPupils(cv::Point* left, cv::Point* right);
    #endif
    #endif
    #endif
    //Pupil refiners
    void refinePupils();

    static const bool houghCircles = false; //true doesn't work yet

    //Pupil getters
    cv::Point* getLeftPupil();
    cv::Point* getRightPupil();
    cv::Point* getLeftPupilRefined();
    cv::Point* getRightPupilRefined();
    cv::Point* getLeftFilteredPupil();
    cv::Point* getLeftFilteredPupilRefined();
    cv::Point* getRightFilteredPupil();
    cv::Point* getRightFilteredPupilRefined();

    //Pupil existance
    bool leftFound;
    bool rightFound;
    bool leftFilteredFound;
    bool rightFilteredFound;
    bool eyeLineFound;

    //Corners finders
    void findEyeCorners();
    void updateReduced();

#ifdef FINDPUPILS_TEST
    void printTestResults();
#endif

private:
    //Internal roi handler
    pupilRois* pupilrH;

    detectionMethodEnum detectionMethod;
    refinementMethodEnum refinementMethod;



    //Avg Light
    float leftAvgLight;
    float rightAvgLight;

#ifdef FINDPUPILS_TEST
    float testLeftAvgLight;
    float testRightAvgLight;
#endif

    //Pupil coordinates in ?
    cv::Point* leftPupil;
    cv::Point* rightPupil;
    cv::Point* leftPupilRefined;
    cv::Point* rightPupilRefined;

    //Local copy of the binaryanalyzer
    binaryAnalyzer* bin;

    //Local gui
    pupilGui* eyeGui;

    //Twinkle reference
    twinkle* tw;

    //Binary sets
    setInformation *leftPupilSet, *rightPupilSet;
    setInformation *leftRefinedSet, *rightRefinedSet;

    //Internal pupil finders
    cv::Point* greatCatch(bool left);
    cv::Point* blackHole(bool left);
    cv::Point* blackHoleV2(bool left);
    cv::Point* findCircles(bool left);

    //Pupil refiners
    cv::Point* darkCross(bool left, bool equalized);
    cv::Point* upholster(bool left);
    cv::Point* centerTheEllipse(bool left, bool filtered);
    std::vector<Point> coverNeighborhood(cv::Mat roi, std::vector<cv::Point> v, Mat tImg, uint i, int color, int colorStep, double tValue);

    //Internal Corners
    std::vector<cv::Point> leftCorners;
    std::vector<cv::Point> rightCorners;
    void sobelCorners(bool left);

    //Eye line
    float m;
    float c;
    bool yConst;
    void linkPupils(std::vector<Rect> eyes);


    cv::Point darkestRegion(cv::Point p[], int sizeP, uchar v[]);

    cv::Mat blurImage(cv::Mat& roi);
    void sharpenImage(cv::Mat& img);
    void showPupils();
    void showPupilsRefined();

#ifdef FINDPUPILS_TEST
    //Test info for pupil recognition
    int fLeft_Better;  //counter for filtered left eye better than original eye
    int fRight_Better; //counter for filtered right eye better than original eye
    int fLeft_Equal;
    int fRight_Equal;
    int fLeft_Total;
    int fRight_Total;

    int nFrame;

    //Test method
    void collectTestInfo(testInfo question);
#endif

};

#endif // PUPILDETECTION_H
