#ifndef POLYPHEMUS_H
#define POLYPHEMUS_H

#include "constants.h"
#include "initFlags.h"

#ifdef WEBSERVICE
#include "clientServer/server.h"
#endif

#include "frameHandler/framehandler.h"
#include "headRotation/headrotation.h"
#include "frameHandler/guihandler.h"
#include "pupilDetection/pupildetection.h"
#include "gazeEstimation/gazeestimation.h"
#include "common/haaranalyzer.h"
#include "common/roiHandler.h"
#include "common/binaryanalyzer.h"
#include "common/helpers.h"
#include <limits>
//KE
#include <stasm4_1/stasm/stasm.h>
//KE


/// <summary> Contains options for Polyphemus initialization. </summary>
/// <remarks> Available options are:
/// NO_PARAM: if you only want to use Polyphemus, this should be your choice
/// PROFILE_WANTED: to use with a static video sample to profile performances
/// TWINKLE_WANTED: enables Twinkle algorithm instead of TrustHaar. Deprecated.
/// PROFILE_WANTED may be used in or with TWINKLE_WANTED. </remarks>
enum initParam
{
    NO_PARAM = 1,
    PROFILE_WANTED = 2,
    TWINKLE_WANTED = 4
};

class polyphemus
{
public:

    ///<summary> Length of the edge of the mask considered by darkCross algorithm.</summary>
    static const int nPoints = 3;

    ///<summary>Constructor to use when reading frames from a non-webcam stream (i.e. a video on disk).</summary>
    ///<param name="params">An integer representing a combination of initParam enum values.<seealso cref="initParam"/></param>
    ///<param name="externalFrame">A cv::Mat structure containing image from the stream.</param>
    polyphemus(int params, cv::Mat externalFrame);

    ///<summary>Constructor to use when reading frames from the webcam.</summary>
    ///<param name="params">An integer representing a combination of initParam enum values.<seealso cref="initParam"/></param>
    polyphemus(int params);

    #ifdef TEST_MODE
    ///<summary>Constructor for test system with image training set. </summary>
    polyphemus(int params, std::string testPath);
    #endif

    ///<summary> Method to call at the beginning to know user's screen data.</summary>
    void retrieveDesktop();

  #ifdef WEBSERVICE
    ///<summary> Encapsulates call to private headRotation method setScreenSize.</summary>
    void setScreenSize(int w, int h);

    ///<summary> Returns a point structure with the coordinates of the gaze estimated point on screen.</summary>
    cv::Point getWatchingPoint();

    ///<summary> Encapsulates call to private roiHandler method hasFace.</summary>
    bool hasFace();

    ///<summary> Encapsulates call to private roiHandler method getFace.</summary>
    cv::Rect& getFace();

    ///<summary> Encapsulates call to private roiHandler method hasLeftEye.</summary>
    bool hasLeftEye();

    ///<summary> Encapsulates call to private roiHandler method getLeftEye.</summary>
    cv::Rect& getLeftEye();

    ///<summary> Encapsulates call to private roiHandler method hasRightEye.</summary>
    bool hasRightEye();

    ///<summary> Encapsulates call to private roiHandler method getRightEye.</summary>
    cv::Rect& getRightEye();

    ///<summary> Encapsulates call to private roiHandler method hasNose.</summary>
    bool hasNose();

    ///<summary> Encapsulates call to private roiHandler method getNose.</summary>
    cv::Rect& getNose();

    ///<summary> Encapsulates call to private roiHandler method hasMouth.</summary>
    bool hasMouth();

    ///<summary> Encapsulates call to private roiHandler method getMouth.</summary>
    cv::Rect& getMouth();
  #endif


    #ifndef WEBSERVICE
    ///<summary> Only for debugging purpose. Allocates the common gui handler and set its reference for modules wishing to use it.</summary>
    void enableDebugGui();
    #endif

    ///<summary> Main initialization method, must be called after the starting allocation of Polyphemus.</summary>
    ///<remarks> Allocates haar cascades; initializes capture from cam; allocates debug gui windows. Each param could be empty if such cascade is not needed.</remarks>
    ///<param name = "face">A string representing the path of the xml file containing haar definitions for face detection.</param>
    ///<param name = "eyePair">A string representing the path of the xml file containing haar definitions for eye pair detection.</param>
    ///<param name = "singleEye">A string representing the path of the xml file containing haar definitions for single eye detection.</param>
    ///<param name = "leftEye">A string representing the path of the xml file containing haar definitions for solely left eye detection.</param>
    ///<param name = "rightEye">A string representing the path of the xml file containing haar definitions for solely rightEye detection.</param>
    ///<param name = "nose">A string representing the path of the xml file containing haar definitions for nose detection.</param>
    ///<param name = "mouth">A string representing the path of the xml file containing haar definitions for mouth detection.</param>
    ///<param name = "upperBody">A string representing the path of the xml file containing haar definitions for the whole upper body detection.</param>
    void init(std::string face, std::string eyePair, std::string singleEye, std::string leftEye, std::string rightEye, std::string nose, std::string mouth, std::string upperBody);

    ///<summary> Method that encapsulate all stuff relating to eye gaze tracking.
    void trackGaze();

    ///<summary> Call this method just before destroying polyphemus instance. It deallocates all data structures being used by the library.
    void release();

    //UO
    ///<summary> Returns the roi handler.</summary>
    roiHandler* getRoiHandler();
    //UO


private:

    ///<summary>Polyphemus private constructor, called from public ones.</summary>
    ///<param name="params">An integer representing a combination of initParam enum values.<seealso cref="initParam"/></param>
    void constructor(int params);

    //---------------------------------------------------------
    //                   MODULE REFERENCES
    //---------------------------------------------------------

    ///<summary> Reference to camera interface</summary>
    frameHandler* fH;
    ///<summary> Reference to common roi repository</summary>
    roiHandler* rH;
    ///<summary> Reference to pupil detection module</summary>
    pupilDetection* pD;
    ///<summary> Reference to gaze estimation module</summary>
    gazeEstimation* gE;

    ///<summary> Reference to haar analizer module</summary>
    haarAnalyzer* haar;

    ///<summary> Reference to morphology module</summary>
    binaryAnalyzer* bin;

    ///<summary> Reference to head position and rotation module</summary>
    HeadRotation* headRotation;

    #ifndef WEBSERVICE
    ///<summary> Reference to debug gui module</summary>
    guiHandler* gui;
    #endif

    //---------------------------------------------------------
    //                   SCREEN DATA
    //---------------------------------------------------------
    ///<summary> Local screen data. Contains display height in pixel.</summary>
    int screenHeight;
    ///<summary> Local screen data. Contains display width in pixel.</summary>
    int screenWidth;


    //---------------------------------------------------------
    //                    GAZE DATA & METHODS
    //---------------------------------------------------------
    ///<summary> WatchingPoint is a Point structure which contains gaze coordinates.</summary>
    cv::Point watchingPoint;

    ///<summary> Sets the initial position for the gaze point on screen. Needs previous work done by headRotation for y axes. Called by trackGaze(). Returns void.</summary>
    void estimateAndShowGazeStartingPoint();
    ///<summary> Encapsulates call to gE member methods to estimate local pupil displacement. Called by trackGaze(). Returns void.</summary>
    void estimatePupilGazeDisplacement(bool refined);
    ///<summary> Main function to merge together all gaze data computed before (head, eyes, x and y). Called by trackGaze(). Returns void.</summary>
    void computeFinalGaze();
    ///<summary> Internal function to merge together gaze data computed before (head + eyes). ONLY for X axis. Called by computeFinalGaze(). Returns void.</summary>
    void computeFinalGazeX();
    ///<summary> Internal function to merge together gaze data computed before (head + eyes). ONLY for Y axis. Called by computeFinalGaze(). Returns void.</summary>
    void computeFinalGazeY();    //TODO!!!
    #ifndef WEBSERVICE
    ///<summary> Asks guiHandler to refresh gaze window. Called by trackGaze(). Returns void.</summary>
    void updateGaze();
    #endif

    //---------------------------------------------------------
    //                   HAAR DATA & METHODS
    //---------------------------------------------------------
    ///<summary> Vector to be filled by Haar analysis (eye couple).</summary>
    std::vector<cv::Rect> eyePairs;
    ///<summary> Vector to be filled by Haar analysis (single eye).</summary>
    std::vector<cv::Rect> singleEyes;
    ///<summary> Vector to be filled by Haar analysis (left eye).</summary>
    std::vector<cv::Rect> leftEyes;
    ///<summary> Vector to be filled by Haar analysis (right eye).</summary>
    std::vector<cv::Rect> rightEyes;
    ///<summary> Vector to be filled by Haar analysis (nose).</summary>
    std::vector<cv::Rect> noses;
    ///<summary> Vector to be filled by Haar analysis (face).</summary>
    std::vector<cv::Rect> faces;
    ///<summary> Vector to be filled by Haar analysis (mouth).</summary>
    std::vector<cv::Rect> mouths;

    ///<summary> Find nose through Haar classifier. Called by trackGaze(). Returns void.</summary>
    void detectNoseWithHaar();
    ///<summary> Find mouth through Haar classifier. Called by trackGaze(). Returns void.</summary>
    void detectMouthWithHaar();
    ///<summary>Applies Haar analysis (eye pairs, left eye, right eye detection) and sets rois accordingly. Called by trackGaze(). Returns void.</summary>
    void detectEyesWithHaar();

    //---------------------------------------------------------
    //                   PUPIL DATA & METHODS
    //---------------------------------------------------------
    ///<summary>Left pupil coordinates in single eye frame.</summary>
    cv::Point* leftInEye;
    ///<summary>Left pupil coordinates in single eye frame after pupil refinement.</summary>
    cv::Point* leftInEyeRefined;
    ///<summary>Left pupil coordinates in face frame.</summary>
    cv::Point* leftInFace;
    ///<summary>Left pupil coordinates in face frame after pupil refinement.</summary>
    cv::Point* leftInFaceRefined;
    ///<summary>Right pupil coordinates in single eye frame.</summary>
    cv::Point* rightInEye;
    ///<summary>Right pupil coordinates in single eye frame after pupil refinement.</summary>
    cv::Point* rightInEyeRefined;
    ///<summary>Right pupil coordinates in face frame.</summary>
    cv::Point* rightInFace;
    ///<summary>Right pupil coordinates in face frame after pupil refinement.</summary>
    cv::Point* rightInFaceRefined;
    ///<summary>True if refinement is wanted.</summary>
    bool refined;

    ///<summary>Set Polyphemus left pupil data, either from pD data or from manually assigned test data. Called by setPupilData(). Return void.</summary>
    void setLeftPupil(Rect eyeRect);
    ///<summary>Set Polyphemus right pupil data, either from pD data or from manually assigned test data. Called by setPupilData(). Return void.</summary>
    void setRightPupil(Rect eyeRect);
    ///<summary>Set Polyphemus left pupil refined data. Called by setPupilData(). Returns void.</summary>
    void setLeftPupilRefined(Rect eyeRect);
    ///<summary>Set Polyphemus right pupil data. Called by setPupilData(). Returns void.</summary>
    void setRightPupilRefined(Rect eyeRect);

    //---------------------------------------------------------
    //                    TEST DATA & METHODS
    //---------------------------------------------------------
    #ifdef GREATCATCH_TEST
    ///<summary>Number of correct horizontal zone estimations. It is <= total training set images.</summary>
    int correctPlace;
    ///<summary>Number of wrong horizontal zone estimations. It is <= total training set images.</summary>
    int wrongPlace;
    #endif

#ifdef TEST_MODE
    #ifdef AUTOMATIC_TEST
    ///<summary>Estimated horizontal zone. 1 = LEFT, 2 = CENTER, 3 = RIGHT.</summary>
    int hZoneEst;
    ///<summary>True horizontal zone given by training set folders. 1 = LEFT, 2 = CENTER, 3 = RIGHT.</summary>
    int hZoneTrue;
    ///<summary>Number of correct horizontal zone estimations. It is <= total training set images.</summary>
    int hGazeRight;
    ///<summary>Number of correct horizontal zone estimations. It is <= total training set images.</summary>
    int hGazeWrong;
    ///<summary>Real left and right pupil coordinates in main frame.</summary>
    std::vector<int> realCoords;
    ///<summary>Real left pupil coordinates in face frame from training set image.</summary>
    cv::Point* realLeftInFace;
    ///<summary>Real right pupil coordinates in face frame from training set image.</summary>
    cv::Point* realRightInFace;
    ///<summary>Mean euclidean distance of left found pupil from left real pupil.</summary>
    double leftAvgError;
    ///<summary>Mean euclidean distance of right found pupil from right real pupil.</summary>
    double rightAvgError;
    ///<summary>Mean euclidean distance of left refined pupil from left real pupil.</summary>
    double leftRefAvgError;
    ///<summary>Mean euclidean distance of right refined pupil from right real pupil.</summary>
    double rightRefAvgError;

    int foundLeftPupils;
    int foundRightPupils;
    int leftNotFound;
    int rightNotFound;
    int falseRefinementLeft;
    int falseRefinementRight;
    int goodRefinementLeft;
    int goodRefinementRight;

    ///<summary>Compute eye horizontal ranges from manually set pupil coords in training set images. Called by trackGaze(). Returns void.</summary>
    void computeRealPupilDistances();
    ///<summary>Compute euclidean distance from the estimated pupil to the real one (manually set), and updates test avg perfomances.</summary>
    void updateErrorData();
    #endif
#endif

    //---------------------------------------------------------
    //                    FACE DATA & METHODS
    //---------------------------------------------------------
    ///<summary> Boolean value. If true, Haar analysis has found at least a face in current frame.</summary>
    bool facesFound;

    ///<summary>Applies further reduction on a COPY of face frame. Called by processFaceData().Returns void.</summary>
    void reduceFace();
    ///<summary>Applies mirroring, flat field, and grayscale conversion on original frame. Called by trackGaze().Returns void.</summary>
    void preProcessFrame();
    ///<summary>Reset face structures. Called by trackGaze() before beginning a new Haar analysis.Returns void.</summary>
    void clearPreviousFaces();
    ///<summary>Starts the Haar analysis (face detection) and sets faceFound flag. Called by trackGaze().Returns void.</summary>
    void findNewFaces();
    ///<summary>Sets roiHandler and guiHandler (if required) face rois and flags. Called by trackGaze().Returns void.</summary>
    void processFaceData();
    #ifndef WEBSERVICE
    ///<summary> Asks guiHandler to refresh face window. Called by trackGaze(). Returns void.</summary>
    void updateFace();
    #endif

    //---------------------------------------------------------
    //                    STASM METHODS
    //---------------------------------------------------------
    void stasm();
    void drawSingleLandmark();
    float landmarks[2* stasm_NLANDMARKS]; // x,y coords

    //---------------------------------------------------------
    //                    EYE METHODS
    //---------------------------------------------------------
    ///<summary> Reset left eye structures. Called by setPupilData(). Returns void.</summary>
    void clearOldLeftPupil();
    ///<summary> Reset right eye structures. Called by setPupilData(). Returns void.</summary>
    void clearOldRightPupil();
    ///<summary> Asks roiHandler to build unreduced eye rois empirically (antropometric measures). Called by trackGaze(). Returns void.</summary>
    void detectEyesEmpiric();
    ///<summary> Asks roiHandler to furtherly reduce empirical eye rois (antropometric measures). Needs detectEyesEmpiric() to be called before. Called by trackGaze(). Returns void.</summary>
    void reduceEmpiricEyes();
    ///<summary>Encapsulates call to pupilDetection methods. Called by trackGaze(). Returns void.</summary>
    void findPupils(std::vector<Rect> eyesReduced);
    ///<summary>Clear&set method for internal pupil data. Needs previous work by findPupils(). Called by trackGaze(). Returns void.
    void setPupilData();
    //---------------------------------------------------------
    //                    ROTATIONS DATA & METHODS
    //---------------------------------------------------------
    ///<summary> Estimate head rotation along y axis (pan movement) and sets headRotation's structures accordingly. Called by trackGaze(). Returns void.</summary>
    void estimateAndShowRotationsY();

    //---------------------------------------------------------
    //                    OTHER DATA & METHODS
    //---------------------------------------------------------
    #ifndef WEBSERVICE
    ///<summary> Asks guiHandler to refresh frame window. Called by trackGaze(). Returns void.</summary>
    void updateMain();
    #endif

    //------------------- DOC LINE (MONTOYA) --------------------

    bool twinkleFlag;
    bool profiling;

    void prepareProfiling();


    void templateMatching(); //TEMPORARY SIMONE'S METHOD
    void antropometricFilter(); //TEMPORARY SIMONE'S METHOD
    #ifdef KETROD_DEBUG
    void printFeatures(); //TEMPORARY SIMONE'S METHOD
    #endif


};

#endif // POLYPHEMUS_H
