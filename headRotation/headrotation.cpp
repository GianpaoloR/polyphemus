#include "headrotation.h"

HeadRotation::HeadRotation()
{
    rH = NULL;
}

void HeadRotation::setRoiHandler(roiHandler* rH) {
    this->rH = rH;
}

void HeadRotation::setScreenSize(int w, int h)
{
    this->vRes = h;
    this->hRes = w;
}

roiHandler* HeadRotation::getRoiHandler() {
    return rH;
}

double HeadRotation::evaluateDistance(cv::Point a, cv::Point b){
    double distance;
    int base,h;//h & b in pixel

    base=abs(a.x-b.x);
    h=abs(a.y-b.y);

    //Pitagora
    distance = (base*base) + (h*h);
    distance = sqrt(distance);

    return distance;
}


//KE
double HeadRotation::evaluateRotationX() {
    double distance_NM=0;
    int delta_y=0;

    if (rH->hasMouth() && rH->hasRightEye() && rH->hasLeftEye())
    {
        cv::Point m (rH->getMouth().x+(rH->getMouth().width/2), rH->getMouth().y+(rH->getMouth().height/2));
        cv::Point eR (rH->getRightEye().x+(rH->getRightEye().width/2), rH->getRightEye().y+(rH->getRightEye().height/2));
        cv::Point eL (rH->getLeftEye().x+(rH->getLeftEye().width/2), rH->getLeftEye().y+(rH->getLeftEye().height/2));

        cv::Point e  ((eR.x+eL.x)/2, (eR.y+eL.y)/2);

        //std::cout << evaluateDistance(e, m) << " " << rH->distance_mouth_eyes << acos(evaluateDistance(e, m)/distance_mouth_eyes) << std::endl;



        //rH->distance_mouth_eyes
    }

    //Vecchio metodo eventualmente da toglier l'ultimo else
    if (rH->hasMouth() && rH->hasNose())
    {
        cv::Point n (rH->getNose().x+(rH->getNose().width/2), rH->getNose().y+(rH->getNose().height/2));
        cv::Point m (rH->getMouth().x+(rH->getMouth().width/2), rH->getMouth().y+(rH->getMouth().height/2));
        distance_NM = evaluateDistance(n, m)* 100/rH->getFace().height;
        distance_NM = distance_NM-rH->distance_nose_mouth;
        if (distance_NM > 0)
        {
            delta_y = vRes/2/6*distance_NM;
        }
        else
        {
            delta_y = vRes/2/4*distance_NM;
        }
    }
    else if (rH->hasNose())
    {
        cv::Point f ((rH->getFace().width/2), (rH->getFace().height/2));
        cv::Point m (rH->getNose().x+(rH->getNose().width/2), rH->getNose().y+(rH->getNose().height/2));
        distance_NM = evaluateDistance(f, m)* 100/rH->getFace().height;
        distance_NM = distance_NM-rH->distance_nose_center;
        if (distance_NM > 0)
        {
            delta_y = vRes/2/2.5*distance_NM;
        }
        else
        {
            delta_y = vRes/2/2*distance_NM;
        }
        //std::cout << "Delta Nose: " << delta_y << std::endl;
    }
   /*  else if (fH->hasMouth())
   {
   cv::Point f ((fH->getFace().width/2), (fH->getFace().height/2));
   cv::Point m (fH->getMouth().x+(fH->getMouth().width/2), fH->getMouth().y+(fH->getMouth().height/2));
   distance_NM = evaluateDistance(f, m)* 100/fH->getFace().height;
   distance_NM = distance_NM-fH->distance_mouth_center;
   if (distance_NM > 0)
   {
   delta_y = vRes/2/2*distance_NM;
   }
   else
   {
   delta_y = vRes/2/2*distance_NM;
   }
   //std::cout << "Delta Mouth: " << delta_y << std::endl;
   }
   */
    return -delta_y;
}
//KE

double HeadRotation::evaluateRotationY() {
    double rotation = 0.0, addRotation;
    int numEvaluation = 0;
    if ((addRotation = evaluateRotationWithNoseY()) != 0) {
        rotation += addRotation;
        numEvaluation++;
    }
    if ((addRotation = evaluateRotationWithMouthY()) != 0) {
        rotation += addRotation;
        numEvaluation++;
    }
    if ((addRotation = evaluateRotationWithEyesY()) != 0) {
        rotation += addRotation;
        numEvaluation++;
    }

    //std::cout<<"Rotation: "<<rotation<<std::endl;
    if (numEvaluation > 0)
        return rotation / numEvaluation;
    else
        return 0;
}

double HeadRotation::evaluateRotationZ(haarAnalyzer* haar) {
    double rotation = 0.0;
    int numEvaluation = 0, step = 10;
    bool found = false;
    //list<int>::iterator itRotations;
    //list<int> rotations;
    cv::Size roundSize = cv::Size(0, 0);

    for (int i = 0; i < 180; i+=step) {
        cv::Mat rotated;
        rotateImage(rH->getGrayFrame(), i, rotated);
        //            std::string name = "Rotated ";
        //            std::stringstream sstm;
        //            sstm << name << i;
        //            name = sstm.str();
        //            cv::namedWindow(name, CV_WINDOW_NORMAL);
        //            imshow(name, rotated);
        if (haar->hasDetectFaceWithSize(rotated, roundSize)) {
            rotation += i;
            found = true;
            //std::cout<<"rotate "<<i<<std::endl;
            numEvaluation++;
        }
        else {
            if (found)
                break;
        }
    }

    found = false;
    for (int i = 0 - step; i >= -180; i-=step) {
        cv::Mat rotated;
        rotateImage(rH->getGrayFrame(), i, rotated);
        //            std::string name = "Rotated ";
        //            std::stringstream sstm;
        //            sstm << name << i;
        //            name = sstm.str();
        //            cv::namedWindow(name, CV_WINDOW_NORMAL);
        //            imshow(name, rotated);
        if (haar->hasDetectFaceWithSize(rotated, roundSize)) {
            rotation += i;
            found = true;
            //std::cout<<"rotate "<<i<<std::endl;
            numEvaluation++;
        }
        else {
            if (found)
                break;
        }
    }


    //std::cout<<"Rotation: "<<rotation / numEvaluation<<std::endl;
    if (numEvaluation > 0)
        return rotation / numEvaluation;
    else
        return 0;
}

double HeadRotation::evaluateRotationWithNoseY() {
    if (rH->hasNose()) {
        double left = rH->getNose().x + rH->getNose().width*0.5;
        //double right = rH->getFace().width - (rH->getNose().x + rH->getNose().width*0.5);
        //return ((right / fH->getFace().width) * 180) - 90;
        return 90 - ((left / rH->getFace().width) * 180);
    }
    else
        return 0;
}

double HeadRotation::evaluateRotationWithMouthY() {
    if (rH->hasMouth()) {
        double left = rH->getMouth().x + rH->getMouth().width*0.5;
        //double right = rH->getFace().width - (rH->getMouth().x + rH->getMouth().width*0.5);
        //return ((right / rH->getFace().width) * 180) - 90;
        return 90 - ((left / rH->getFace().width) * 180);
    }
    else
        return 0;
}

double HeadRotation::evaluateRotationWithEyesY() {
    //TODO: isn't so good, use the circle dimension evaluation

    if ((rH->hasLeftEye()) && (rH->hasRightEye())) {
        /*double left = (rH->getLeftEye().x + rH->getLeftEye().width*0.5 + rH->getRightEye().x + rH->getRightEye().width*0.5) / 2;
        double right = rH->getFace().width - ((rH->getLeftEye().x + rH->getLeftEye().width*0.5 + rH->getRightEye().x + rH->getRightEye().width*0.5) / 2);
        //return ((right / rH->getFace().width) * 180) - 90;
        return 90 - ((left / rH->getFace().width) * 180);*/
        //std::cout<<"Left width: "<<rH->getLeftEye().width<<" Right width: " <<rH->getRightEye().width<<std::endl;

        double leftSize = (rH->getLeftEye().width + rH->getLeftEye().height) / 2;
        double rightSize = (rH->getRightEye().width + rH->getRightEye().height) / 2;
        //double sizeDifference = leftSize - rightSize;
        //std::cout<<"Size difference: "<<(sizeDifference / ((leftSize + rightSize) / 2))<<std::endl;
        //return 90 - ((sizeDifference / ((leftSize + rightSize) / 2)) * 180);
        return 90 - ((leftSize / ((leftSize + rightSize) / 2)) * 90);
    }
    else
        return 0;

    //    if ((fH->hasLeftEye()) && (fH->hasRightEye()) && (fH->hasMouth())) {
    //        double left = fH->getLeftEye().x;
    //        double right = fH->getRightEye().x + fH->getRightEye().width;
    //        double med = fH->getMouth().x + fH->getMouth().width * 0.5;
    //        double leftDist = med - left;
    //        double rightDist = right - med;
    //        //return ((right / fH->getFace().width) * 180) - 90;
    //        return ((leftDist / ((leftDist + rightDist) / 2)) * 90) - 90;
    //    }
    //    else
    //        return NULL;
}

cv::Point HeadRotation::evaluatePosition (double rotation, cv::Rect face, cv::Mat frame){
    int x=0,y=0; //Coordinate punto osservato
    double distance_head; //Scostamento punto osservato posizione testa
    double eye_position, eye_position_y; //Proiezione occhi sullo schermo
    int difference_y = evaluateRotationX();
    //rotation:180=x:pi -> 180*x = pi*rotation ->
    rotation = rotation*M_PI/180;
    //Calcoliamo con il teorema dei seni la coordinata x
    distance_head = DISTANZA_MEDIA_SCHERMO * sin(rotation) / sin((M_PI/2) - rotation);
    distance_head = -distance_head * CM_TO_PIXEL; //1cm -> 37.795275591 pixel
    // x:hRes=position_face:frame.cols
    eye_position = hRes*(face.x+(face.width/2))/frame.cols;
    eye_position_y = vRes*(face.y+(face.height/2))/frame.rows;
    //Calcolare coord x;
    x=eye_position + distance_head;
    y=eye_position_y +difference_y;

    cv::Point obs(x,y);
    if (obs.x<0)
    {
        obs.x=0;
    }
    else if (obs.x > hRes)
    {
        obs.x = hRes;
    }
#ifndef WEBSERVICE
    //std::cout << "Rotation: " << rotation << " Distanza schermo in pixel: " << distance_head << " Proiezione: " << eye_position << std::endl;
    //std::c<<"Coordinata x: "<< x << " Coordinata y: " << y << " Posizione: " << test <<std::endl;
#endif
    return obs;
}

cv::Rect HeadRotation::thresholdImage(cv::Mat& img) {
    cv::Mat imgGray;
    //int threshold = 30;     //good with face and its details
    //int maxContoursSize = 70;     //good with face and its details
    //int threshold = 50;     //good with face
    //int maxContoursSize = 30;     //good with face
    int threshold = 30;
    uint maxContoursSize = 70;
    //int threshold = 50;     //good with mouth
    //int maxContoursSize = 2;     //good with mouth
    //int maxThreshold = 400;
    //cv::RNG rng(12345);
    cv::Mat cannyOutput;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    /// Convert image to gray and blur it
    if (img.channels() != 1)
        cv::cvtColor(img, imgGray, CV_BGR2GRAY);
    else
        img.copyTo(imgGray);
    cv::blur(imgGray, imgGray, cv::Size(3,3));

    do {
        threshold--;
        /// Detect edges using canny
        cv::Canny(imgGray, cannyOutput, threshold, threshold*2, 3);
        /// Find contours
        cv::findContours(cannyOutput, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    } while ((contours.size() < maxContoursSize) && (threshold > 0));

    cv::Mat drawing = cv::Mat::zeros(cannyOutput.size(), CV_8UC1);
    for (uint i = 0; i< contours.size(); i++) {
        cv::Scalar color = cv::Scalar(255);
        cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
    }

    cv::Mat subImg;

    /*
    int k = drawing.rows * 2 / 3 ; // I controlli li fa nei primi due terzi dell'immagine in tre punti
    int saveJL=0;
    //int saveJL_temp[3];
    for (int l = 0 ; l < 3 ; l++)
    {
        saveJL += searchWhiteAtL(k*(l+1)/3, drawing);
    }
    saveJL/=3;

    int saveJR=0;
    //int saveJL_temp[3];
    for (int l = 0 ; l < 3 ; l++)
    {
        saveJR += searchWhiteAtR(k*(l+1)/3, drawing);
    }
    saveJR/=3;
*/

    int saveJL=0;
    //int saveJL_temp[3];
    for (int l = 0 ; l < drawing.rows ; l++)
    {
        saveJL += searchWhiteAtL(l, drawing);
    }
    saveJL/=drawing.rows;

    int saveJR=0;
    //int saveJL_temp[3];
    for (int l = 0 ; l < drawing.rows ; l++)
    {
        saveJR += searchWhiteAtR(l, drawing);
    }
    saveJR/=drawing.rows;


    subImg = img(cv::Rect(saveJL, 0, saveJR - saveJL, img.rows));

    /// Show in a window
    if (canny_test)
    {
        cv::namedWindow("Contours", CV_WINDOW_NORMAL);
        imshow("Contours", drawing);

        cv::namedWindow("Resized face", CV_WINDOW_NORMAL);
        imshow("Tmp resized face", subImg);
    }

    return cv::Rect(saveJL, 0, saveJR - saveJL, img.rows);
}


int HeadRotation::searchWhiteAtL(int k, cv::Mat drawing)
{
    int white = 0;
    int saveJL = 0;

    bool found = false;

    for (int j = 1; j < drawing.cols && !found; j++)
    {
        if (drawing.at<uchar>(k, j) > 0)
        {
            white++;
            saveJL = j;
            found = true;
        }
        drawing.at<uchar>(k, j) = 255;
    }


    return saveJL;
}

int HeadRotation::searchWhiteAtR(int k, cv::Mat drawing)
{
    int white=0;
    int saveJR = drawing.cols;
    bool found = false;
    for (int j = drawing.cols - 2; j >= 0 && !found; j--)
    {
        if (drawing.at<uchar>(k, j) > 0)
        {
            white++;
            saveJR = j;
            found = true;
        }
        drawing.at<uchar>(k, j) = 255;
    }
    return saveJR;
}

void HeadRotation::processDistances()
{
    //-----------------------
    //  Eye to Eye distance

#ifdef WEBSERVICE
    //UO
    //std::string distanceEyes = Server::getServer()->getCookie("distanceEyes");
    rH->distance_eyes = Server::getServer()->getInfoDouble("distanceEyes");
    if (rH->distance_eyes != NULL) {
        //rH->distance_eyes = atof(distanceEyes.c_str());
        rH->itHasDistanceEyes = true;
    }
    //UO
#endif

    if ((!rH->itHasDistanceEyes) && (rH->hasLeftEye()) && (rH->hasRightEye())) {
        cv::Point eL (rH->getLeftEye().x+(rH->getLeftEye().width/2), rH->getLeftEye().y+(rH->getLeftEye().height/2));
        cv::Point eR (rH->getRightEye().x+(rH->getRightEye().width/2), rH->getRightEye().y+(rH->getRightEye().height/2));
        rH->distance_eyes=evaluateDistance(eL, eR)*100/rH->getFace().width;
        rH->itHasDistanceEyes = true;

#ifdef WEBSERVICE
        //UO
        Server::getServer()->setInfo("distanceEyes", rH->distance_eyes);
        //Server::getServer()->setCookie("distanceEyes", rH->distance_eyes);
        //UO
#endif
    }

    //------------------------------
    //    Mouth to Center distance

    #ifdef WEBSERVICE
    //UO
    //std::string distanceMouthCenter = Server::getServer()->getCookie("distanceMouthCenter");
    rH->distance_mouth_center = Server::getServer()->getInfoDouble("distanceMouthCenter");
    if (rH->distance_mouth_center != NULL) {
        //rH->distance_mouth_center = atof(distanceMouthCenter.c_str());
        rH->itHasDistanceMouthCenter = true;
    }
    //UO
    #endif

    if ((!rH->itHasDistanceMouthCenter) && (rH->hasMouth())) {
        cv::Point f ((rH->getFace().width/2), (rH->getFace().height/2));
        cv::Point m (rH->getMouth().x+(rH->getMouth().width/2), rH->getMouth().y+(rH->getMouth().height/2));
        rH->distance_mouth_center=evaluateDistance(m, f) * 100/rH->getFace().height;
        rH->itHasDistanceMouthCenter = true;

        #ifdef WEBSERVICE
        //UO
        Server::getServer()->setInfo("distanceMouthCenter", rH->distance_mouth_center);
        //Server::getServer()->setCookie("distanceMouthCenter", rH->distance_mouth_center);
        //UO
        #endif
    }

    //------------------------------
    //    Nose to Center distance

    #ifdef WEBSERVICE
    //UO
    //std::string distanceNoseCenter = Server::getServer()->getCookie("distanceNoseCenter");
    rH->distance_nose_center = Server::getServer()->getInfoDouble("distanceNoseCenter");
    if (rH->distance_nose_center != NULL) {
        //rH->distance_nose_center = atof(distanceNoseCenter.c_str());
        rH->itHasDistanceNoseCenter = true;
    }
    //UO
    #endif

    if ((!rH->itHasDistanceNoseCenter) && (rH->hasNose())) {
        cv::Point f ((rH->getFace().width/2), (rH->getFace().height/2));
        cv::Point n (rH->getNose().x+(rH->getNose().width/2), rH->getNose().y+(rH->getNose().height/2));
        rH->distance_nose_center=evaluateDistance(f, n)*100/rH->getFace().height;
        rH->itHasDistanceNoseCenter = true;

        #ifdef WEBSERVICE
        //UO
        Server::getServer()->setInfo("distanceNoseCenter", rH->distance_nose_center);
        //Server::getServer()->setCookie("distanceNoseCenter", rH->distance_nose_center);
        //UO
        #endif
    }

    //------------------------------
    //    Nose to Mouth distance

    #ifdef WEBSERVICE
    //UO
    //std::string distanceNoseMouth = Server::getServer()->getCookie("distanceNoseMouth");
    rH->distance_nose_mouth = Server::getServer()->getInfoDouble("distanceNoseMouth");
    if (rH->distance_nose_mouth != NULL) {
        //rH->distance_nose_mouth = atof(distanceNoseMouth.c_str());
        rH->itHasDistanceNoseMouth = true;
    }
    //UO
    #endif

    if ((!rH->itHasDistanceNoseMouth) && (rH->hasMouth()) && (rH->hasNose())) {
        cv::Point m (rH->getMouth().x+(rH->getMouth().width/2), rH->getMouth().y+(rH->getMouth().height/2));
        cv::Point n (rH->getNose().x+(rH->getNose().width/2), rH->getNose().y+(rH->getNose().height/2));
        rH->distance_nose_mouth=evaluateDistance(n, m)* 100/rH->getFace().height;
        rH->itHasDistanceNoseMouth = true;

        #ifdef WEBSERVICE
        //UO
        Server::getServer()->setInfo("distanceNoseMouth", rH->distance_nose_mouth);
        //Server::getServer()->setCookie("distanceNoseMouth", rH->distance_nose_mouth);
        //UO
        #endif
    }
}
