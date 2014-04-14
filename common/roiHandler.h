#ifndef ROIHANDLER_H
#define ROIHANDLER_H

#include "common/cvInclusions.h"
#include "common/helpers.h"

class roiHandler
{
public:
    //NEVER USED
//    cv::Mat leftEyeROI;
//    cv::Mat rightEyeROI;
//    cv::Mat leftEyeROIReduced;
//    cv::Mat rightEyeROIReduced;
//    cv::Mat leftEyeROIThresholded;
//    cv::Mat rightEyeROIThresholded;
//    cv::Mat leftEyeROIRotated;
//    cv::Mat rightEyeROIRotated;

    double distance_nose_mouth;
    double distance_mouth_center;
    double distance_eyes;
    double distance_nose_center;
    //KE
    double distance_mouth_eyes;
    //KE

    bool itHasDistanceNoseMouth;
    bool itHasDistanceMouthCenter;
    bool itHasDistanceEyes;
    bool itHasDistanceNoseCenter;
    //KE
    bool itHasDistanceMouthEyes;
    //KE

    roiHandler();
    void release();
    void setOldNose();
    void setGrayFrame(Mat m);
    cv::Mat& getGrayFrame();
    void setReducedEmpiricEyes();

    //Set center point
    void setInitMouthCenter(cv::Rect mouth);
    void setInitNoseCenter(cv::Rect nose);
    void setInitLeftEyeCenter(cv::Rect leftEye);
    void setInitRightEyeCenter(cv::Rect rightEye);

    //Get center point
    cv::Rect& getInitMouthCenter();
    cv::Rect& getInitNoseCenter();
    cv::Rect& getInitLeftEyeCenter();
    cv::Rect& getInitRightEyeCenter();

    //Set high-left point
    void setInitMouthHighLeft(cv::Rect mouth);
    void setInitNoseHighLeft(cv::Rect nose);
    void setInitLeftEyeHighLeft(cv::Rect leftEye);
    void setInitRightEyeHighLeft(cv::Rect rightEye);

    //Get high-left point
    cv::Rect& getInitMouthHighLeft();
    cv::Rect& getInitNoseHighLeft();
    cv::Rect& getInitLeftEyeHighLeft();
    cv::Rect& getInitRightEyeHighLeft();

    //Set high-left point
    void setInitMouthLowRight(cv::Rect mouth);
    void setInitNoseLowRight(cv::Rect nose);
    void setInitLeftEyeLowRight(cv::Rect leftEye);
    void setInitRightEyeLowRight(cv::Rect rightEye);

    //Get low-right point
    cv::Rect& getInitMouthLowRight();
    cv::Rect& getInitNoseLowRight();
    cv::Rect& getInitLeftEyeLowRight();
    cv::Rect& getInitRightEyeLowRight();

    bool itHasCenterPoint();
    bool itHasLowRightPoint();
    bool itHasHighLeftPoint();


    //Eye(s) setters
    void setEyePairROI(std::vector<cv::Rect> fVett);
    void setEyesHaarROI(std::vector<cv::Rect> fVett);
    void setLeftEyeROI(std::vector<cv::Rect> fVett);
    void setRightEyeROI(std::vector<cv::Rect> fVett);
    void setLeftEmpiricRotatedFrame(cv::Mat& img);
    void setRightEmpiricRotatedFrame(cv::Mat& img);
    //Eye(s) getters
    cv::Rect& getLeftEye();
    cv::Mat& getLeftEyeROI();
    cv::Rect& getRightEye();
    cv::Mat& getRightEyeROI();
    std::vector<cv::Rect> getEmpiricEyes();
    cv::Mat& getLeftEmpiricReducedROI();
    cv::Mat& getRightEmpiricReducedROI();
    cv::Mat& getEmpiricReducedROI(bool left);
    std::vector<cv::Rect> getReducedEmpiricEyes();
    cv::Mat& getLeftEmpiricRotatedFrame();
    cv::Mat& getRightEmpiricRotatedFrame();
    //Eye(s) cleaners
    void clearHaarEyes();
    //Eye(s) existance
    bool hasLeftEye();
    bool hasRightEye();
    //Eye(s) helpers
    void makeEmpiricEyes();

    //Mouth setter
    void setMouthROI(std::vector<cv::Rect> fVett);
    void setOldMouth();
    //Mouth getters
    cv::Rect& getMouth();
    cv::Mat& getMouthROI();
    cv::Mat& getOldHaarMouthROI();
    //Mouth cleaner
    void clearMouth();
    //Mouth existance
    bool hasMouth();

    //Nose setter
    void setNoseROI(std::vector<cv::Rect> fVett);
    //Nose getter
    cv::Rect& getNose();
    cv::Mat& getNoseROI();
    cv::Mat& getOldHaarNoseROI();
    //Nose cleaner
    void clearNose();
    //Nose existance
    bool hasNose();

    //Face setter
    void setFaceROI(std::vector<cv::Rect> fVett);
    void setReducedFaceROI(cv::Rect& reducedRect);
    //Face getter
    cv::Rect& getFace();
    cv::Mat& getFaceROI();
    cv::Mat& getReducedFaceROI();
    //Face cleaner
    void clearFace();
    //Face existance
    bool hasFace();



private:
    cv::Mat grayFrame;

    //Face
    cv::Rect faceHaarRect;
    cv::Mat faceHaarFrame;
    bool itHasFace;

    //Face reduced
    cv::Rect faceReducedRect;
    cv::Mat faceReducedFrame;

    //Upper body
    cv::Rect upperBodyHaarRect;
    cv::Mat upperBodyHaarFrame;
    bool itHasUpperBody;

    //Eye pair
    bool itHasEyePair;
    cv::Rect eyePairHaarRect;
    cv::Mat eyePairHaarFrame;
    cv::Mat leftEmpiricReducedFrame;
    cv::Mat rightEmpiricReducedFrame;
    cv::Mat leftEmpiricRotatedFrame;
    cv::Mat rightEmpiricRotatedFrame;
    std::vector<cv::Rect> empiricEyes;
    std::vector<cv::Rect> eyesReduced;


    //Left Eye
    bool itHasLeftEyeCenterInit;
    bool itHasLeftEyeHighLeftInit;
    bool itHasLeftEyeLowRightInit;
    cv::Rect leftEyeHaarRectCenterInit;
    cv::Rect leftEyeHaarRectHighLeftInit;
    cv::Rect leftEyeHaarRectLowRightInit;

    bool itHasLeftEye;
    cv::Rect leftEyeHaarRect;
    cv::Mat leftEyeHaarFrame;
    cv::Rect leftEyeEmpiricRect;
    cv::Mat leftEyeEmpiricFrame;

    //Right Eye
    bool itHasRightEyeCenterInit;
    bool itHasRightEyeHighLeftInit;
    bool itHasRightEyeLowRightInit;
    cv::Rect RightEyeHaarRectCenterInit;
    cv::Rect RightEyeHaarRectHighLeftInit;
    cv::Rect RightEyeHaarRectLowRightInit;
    bool itHasRightEye;
    cv::Rect rightEyeHaarRect;
    cv::Mat rightEyeHaarFrame;
    cv::Rect rightEyeEmpiricRect;
    cv::Mat rightEyeEmpiricFrame;

    //Empiric Eyes
    float eyeTop;
    float eyeWidth;
    float eyeHeight;
    std::vector<cv::Rect> eyeZone;
    void detectEyeZone();
    void setEmpiricEyes();
    void clearEmpiricEyes();

    //Nose
    bool itHasNoseCenterInit;
    bool itHasNoseHighLeftInit;
    bool itHasNoseLowRightInit;
    cv::Rect NoseHaarRectCenterInit;
    cv::Rect NoseHaarRectHighLeftInit;
    cv::Rect NoseHaarRectLowRightInit;


    cv::Rect noseHaarRect;
    cv::Mat noseHaarFrame;
    cv::Rect oldNoseHaarRect;
    cv::Mat oldNoseHaarFrame;
    bool itHasNose;

    //Nose matching
    cv::Rect noseMatchRect;
    cv::Mat noseMatchFrame;


    //Mouth
    cv::Mat oldMouthHaarFrame;
    cv::Rect oldMouthHaarRect;

    cv::Rect mouthHaarRect;
    cv::Mat mouthHaarFrame;
    bool itHasMouth;

    bool itHasMouthCenterInit;
    bool itHasMouthHighLeftInit;
    bool itHasMouthLowRightInit;
    cv::Rect MouthHaarRectCenterInit;
    cv::Rect MouthHaarRectHighLeftInit;
    cv::Rect MouthHaarRectLowRightInit;

};

#endif // ROIHANDLER_H
