#include "headrotation.h"

HeadRotation::HeadRotation()
{
    rH = NULL;
    angle_x=0;
    angle_y=0;
    angle_z=0;
    initKalman=false;
}

void HeadRotation::setRoiHandler(roiHandler* rH) {
    this->rH = rH;
}

#ifdef WITH_GUI
//SetDebugGui: enables gui for debugger
void HeadRotation::setDebugGui(guiHandler *gui)
{
    this->gui = gui;
    return;
}
#endif

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
double HeadRotation::evaluateRotationX(float * landmarks) {
    double distance_NM=0;
    int delta_y=0;
    
    cv::Point a;
    a.x=landmarks[13*2+1];
    a.y=landmarks[13*2];

    cv::Point b;
    b.x=landmarks[24*2+1];
    b.y=landmarks[24*2];

    cv::Point c;
    c.x=landmarks[15*2+1];
    c.y=landmarks[15*2];

    cv::Point d;
    d.x=landmarks[17*2+1];
    d.y=landmarks[17*2];

    cv::Point e;
    e.x=landmarks[14*2+1];
    e.y=landmarks[14*2];

    cv::Point f;
    f.x=landmarks[6*2+1];
    f.y=landmarks[6*2];

    //cvRound(landmarks[i*2+1]), cvRound(landmarks[i*2])) = 255;
    std::cout << "-------------------------------------------------"<< std::endl;


    if (rH->hasFace())
    //std::cout << (evaluateDistance(a, b)+evaluateDistance(c.y, d.y))/2 << std::endl;
        std::cout << (((b.y-a.y)+(d.y, c.y))/2) << std::endl;
    std::cout << "-------------------------------------------------"<< std::endl;



    return ((evaluateDistance(a, b)+evaluateDistance(c, d))/2)*evaluateDistance(c,d);
/*
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
     else if (fH->hasMouth())
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

double HeadRotation::evaluateRotationY(float* landmarks) {
    double rotation = 0.0, addRotation;
    int numEvaluation = 0;
    if ((addRotation = evaluateRotationWithNoseY(landmarks)) != 0) {
        rotation += addRotation;
        numEvaluation++;
    }
    if ((addRotation = evaluateRotationWithMouthY(landmarks)) != 0) {
        rotation += addRotation;
        numEvaluation++;
    }/*
    if ((addRotation = evaluateRotationWithEyesY()) != 0) {
        rotation += addRotation;
        numEvaluation++;
    }
*/
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
        //[2* stasm_NLANDMARKS]/            std::stringstream sstm;
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

double HeadRotation::evaluateRotationWithNoseY(float* landmarks) {
    double left = landmarks[52*2];
    /*
    if (rH->hasNose()) {
        left += (rH->getNose().x + rH->getNose().width*0.5);
        left = left / 2;
        //double right = rH->getFace().width - (rH->getNose().x + rH->getNose().width*0.5);
        //return ((right / fH->getFace().width) * 180) - 90;
    }
    */
    return 90 - ((left / rH->getFace().width) * 180);
}

double HeadRotation::evaluateRotationWithMouthY(float* landmarks) {
    double left = landmarks[67*2];
    /*
    if (rH->hasMouth()) {
        left += rH->getMouth().x + rH->getMouth().width*0.5;
        left = left / 2;
        //double right = rH->getFace().width - (rH->getMouth().x + rH->getMouth().width*0.5);
        //return ((right / rH->getFace().width) * 180) - 90;
    }
    */
    return 90 - ((left / rH->getFace().width) * 180);
}

double HeadRotation::evaluateRotationWithEyesY() {
    //TODO: isn't so good, use the circle dimension evaluation

    if ((rH->hasLeftEye()) && (rH->hasRightEye())) {

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

cv::Point HeadRotation::evaluatePosition (double rotation, cv::Rect face, cv::Mat frame, float* landmarks){
    int x=0,y=0; //Coordinate punto osservato
    double distance_head; //Scostamento punto osservato posizione testa
    double eye_position, eye_position_y; //Proiezione occhi sullo schermo
    int difference_y = evaluateRotationX(landmarks);
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



void HeadRotation::positPoint(float* landmarks)
{

    cv::Point nose, mouthL, mouthR, eyeL, eyeR, earL, earR;
    cv::Mat img = rH->getFaceROI();

    std::vector<CvPoint3D32f> modelPoints;


    modelPoints.push_back(cvPoint3D32f(-36.9522f,39.3518f,47.1217f));	//l eye
    modelPoints.push_back(cvPoint3D32f(35.446f,38.4345f,47.6468f));		//r eye
    modelPoints.push_back(cvPoint3D32f(-0.0697709f,18.6015f,87.9695f)); //nose
    modelPoints.push_back(cvPoint3D32f(-27.6439f,-29.6388f,73.8551f));	//l mouth
    modelPoints.push_back(cvPoint3D32f(28.7793f,-29.2935f,72.7329f));	//r mouth
    modelPoints.push_back(cvPoint3D32f(-87.2155f,15.5829f,-45.1352f));	//l ear
    modelPoints.push_back(cvPoint3D32f(85.8383f,14.9023f,-46.3169f));	//r ear


    nose.x=landmarks[52*2];
    nose.y=landmarks[52*2+1];

    eyeL.x=(landmarks[30*2]+landmarks[34*2])/2;
    eyeL.y=(landmarks[30*2+1]+landmarks[34*2+1])/2;

    eyeR.x=(landmarks[40*2]+landmarks[44*2])/2;
    eyeR.y=(landmarks[40*2+1]+landmarks[44*2+1])/2;

    earL.x=landmarks[1*2];
    earL.y=landmarks[1*2+1];

    earR.x=landmarks[11*2];
    earR.y=landmarks[11*2+1];


    mouthL.x=landmarks[59*2];
    mouthL.y=landmarks[59*2+1];

    mouthR.x=landmarks[65*2];
    mouthR.y=landmarks[65*2+1];



    cv::Point n;


   /* earR.y+=10;
    earR.x+=40+(evaluateRotationY(landmarks)*2.5);

    earL.y+=10;
    earL.x-=40-(evaluateRotationY(landmarks)*2.5);

*/
//    earR.x+=(evaluateRotationY(landmarks)*2);
//    earL.x-=(evaluateRotationY(landmarks)*2);


    //Media grandezza occhi lungo asse x
    eyeDim = ((landmarks[30*2]-landmarks[34*2])+(landmarks[44*2]-landmarks[40*2]))/2;
    //Distanza fronte.naso lungo asse y attenuata dalla grandezza degli occhi in modo che se mi allontano
    //Viene compreso che mi sono allontanato e che la grandezza e' diminuita a causa di un mio allontanamento.
    double dim=eyeDim*4.5-(landmarks[52*2+1]-landmarks[14*2+1]);
    #ifdef POSIT_DEBUG
    std::cout << "Distance Eyes: "  << eyeDim <<  std::endl;
    #endif

    //double distance_monitor =


    earR.y+=dim;
    earL.y+=dim;

    earL.x+=evaluateRotationY(landmarks)*2;
    earR.x+=evaluateRotationY(landmarks)*2;



    //std::cout << evaluateRotationY(landmarks) << std::endl;


    std::vector<CvPoint2D32f> srcImagePoints;

    int xOff=rH->getFaceROI().cols/2;
    int yOff=rH->getFaceROI().rows/2;
    srcImagePoints.push_back( cvPoint2D32f( eyeL.x-xOff ,  -eyeL.y+yOff ) );
    srcImagePoints.push_back( cvPoint2D32f( eyeR.x-xOff, -eyeR.y+yOff ) );
    srcImagePoints.push_back( cvPoint2D32f( nose.x-xOff, -nose.y+yOff ) );
    srcImagePoints.push_back( cvPoint2D32f( mouthL.x-xOff ,  -mouthL.y+yOff ) );
    srcImagePoints.push_back( cvPoint2D32f( mouthR.x-xOff, -mouthR.y+yOff ) );
    srcImagePoints.push_back( cvPoint2D32f( earL.x-xOff ,  -earL.y+yOff ) );
    srcImagePoints.push_back( cvPoint2D32f( earR.x-xOff, -earL.y+yOff ) );

    posit(modelPoints, srcImagePoints);

    #ifndef WEBSERVICE
    #ifdef WITH_GUI
    cv::circle(img, nose, 3, cv::Scalar(0,255,255), 1, 8, 0);
    cv::circle(img, eyeL, 3, cv::Scalar(0,255,255), 1, 8, 0);
    cv::circle(img, eyeR, 3, cv::Scalar(0,255,255), 1, 8, 0);

    cv::circle(img, mouthL, 3, cv::Scalar(0,255,255), 1, 8, 0);
    cv::circle(img, mouthR, 3, cv::Scalar(0,255,255), 1, 8, 0);


    cv::circle(img, earL, 3, cv::Scalar(0,255,255), 1, 8, 0);
    cv::circle(img, earR, 3, cv::Scalar(0,255,255), 1, 8, 0);

    cv::namedWindow("Posit",CV_WINDOW_NORMAL);
    imshow("Posit", img);
    #endif //WITH_GUI
    #endif //WEBSERVICE
}


void HeadRotation::posit(std::vector<CvPoint3D32f> modelPoints, std::vector<CvPoint2D32f> srcImagePoints)
{
    CvPOSITObject* positObject;
    //Create the POSIT object with the model points
    positObject = cvCreatePOSITObject( &modelPoints[0], (int)modelPoints.size() );

    //Estimate the pose
    CvMatr32f rotation_matrix = new float[9];
    CvVect32f translation_vector = new float[3];
    CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 100, 1.0e-4f);
    cvPOSIT( positObject, &srcImagePoints[0], FOCAL_LENGTH, criteria, rotation_matrix, translation_vector );
/*
    cout << "\n-.- SOURCE MODEL POINTS -.-\n";
    for ( size_t  p=0; p<modelPoints.size(); p++ )
        cout << modelPoints[p].x << ", " << modelPoints[p].y << ", " << modelPoints[p].z << "\n";

    cout << "\n-.- SOURCE IMAGE POINTS -.-\n";
    for ( size_t p=0; p<srcImagePoints.size(); p++ )
        cout << srcImagePoints[p].x << ", " << srcImagePoints[p].y << " \n";

    cout << "\n-.- ESTIMATED ROTATION\n";
    for ( size_t p=0; p<3; p++ )
        cout << rotation_matrix[p*3] << " | " << rotation_matrix[p*3+1] << " | " << rotation_matrix[p*3+2] << "\n";

    cout << "\n-.- ESTIMATED TRANSLATION\n";
    cout << translation_vector[0] << " | " << translation_vector[1] << " | " << translation_vector[2] << "\n";
*/
    //Project the model points with the estimated pose
    std::vector<CvPoint2D32f> projectedPoints;
    for ( size_t  p=0; p<modelPoints.size(); p++ )
    {
        CvPoint3D32f point3D;
        point3D.x = rotation_matrix[0] * modelPoints[p].x +
            rotation_matrix[1] * modelPoints[p].y +
            rotation_matrix[2] * modelPoints[p].z +
            translation_vector[0];
        point3D.y = rotation_matrix[3] * modelPoints[p].x +
            rotation_matrix[4] * modelPoints[p].y +
            rotation_matrix[5] * modelPoints[p].z +
            translation_vector[1];
        point3D.z = rotation_matrix[6] * modelPoints[p].x +
            rotation_matrix[7] * modelPoints[p].y +
            rotation_matrix[8] * modelPoints[p].z +
            translation_vector[2];
        CvPoint2D32f point2D = cvPoint2D32f( 0.0, 0.0 );
        if ( point3D.z != 0 )
        {
            point2D.x = FOCAL_LENGTH * point3D.x / point3D.z;
            point2D.y = FOCAL_LENGTH * point3D.y / point3D.z;
        }
        projectedPoints.push_back( point2D );
    }


    angle_x = radianti_gradi(atan2(rotation_matrix[2*3+1], rotation_matrix[2*3+2]));
    angle_y = radianti_gradi(atan2(-rotation_matrix[2*3], sqrt(rotation_matrix[2*3+1]*rotation_matrix[2*3+1] +  rotation_matrix[2*3+2]*rotation_matrix[2*3+2])));
    angle_z = radianti_gradi(atan2(rotation_matrix[1*3], rotation_matrix[0]));

    distance = getDistanceHead();

    #ifdef POSIT_DEBUG
    cout << "Distance head: " << distance << endl;
    #endif

    setGazeFromPosit();
    kalmanFilter();

    #ifdef POSIT_DEBUG
    cout << "Angle x: " <<radianti_gradi(atan2(rotation_matrix[2*3+1], rotation_matrix[2*3+2])) << endl;
    cout << "Angle y: " <<radianti_gradi(atan2(-rotation_matrix[2*3], sqrt(rotation_matrix[2*3+1]*rotation_matrix[2*3+1] +  rotation_matrix[2*3+2]*rotation_matrix[2*3+2]))) << endl;
    cout << "Angle z: " <<radianti_gradi(atan2(rotation_matrix[1*3], rotation_matrix[0])) << endl;
    #endif


    //guiHandler->setGazeFromPosit(x, y, 70);
/*
    double teta1=-sin(rotation_matrix[2*3]+3.14);
    double teta2=3.14-sin(rotation_matrix[2*3]+3.14);

    cout << "Angle teta1 " << radianti_gradi(teta1)<< endl;
    cout << "Angle teta2 " << radianti_gradi(teta2)<< endl;

    double psi1=-atan2((rotation_matrix[2*3+1])/ cos (teta1) , rotation_matrix[2*3+1]/cos(teta1) );
    double psi2=-atan2((rotation_matrix[2*3+1])/ cos (teta2) , rotation_matrix[2*3+1]/cos(teta2) );

    cout << "Angle psi1 " << radianti_gradi(psi1)<< endl;
    cout << "Angle psi2 " << radianti_gradi(psi2)<< endl;

    double phi1=-atan2((rotation_matrix[1*3+0])/ cos (teta1) , rotation_matrix[0]/cos(teta1) );
    double phi2=-atan2((rotation_matrix[1*3+0])/ cos (teta2) , rotation_matrix[0]/cos(teta2) );

    cout << "Angle phi1 " << radianti_gradi(phi1)<< endl;
    cout << "Angle phi2 " << radianti_gradi(phi2)<< endl;
    */
}

double HeadRotation::radianti_gradi(double radiante)
{
    return radiante * 180 / 3.1415;
}


double HeadRotation::getAngleX()
{
    return angle_x;
}

double HeadRotation::getAngleY()
{
    return angle_y;
}

double HeadRotation::getAngleZ()
{
    return angle_z;
}

double HeadRotation::getDistanceHead()
{
    double delta=(eyeDim-20)*2;
    return 100-(eyeDim+delta);
}


void HeadRotation::kalmanFilter()
{
    if (initKalman==false)
    {
        KF.init(4,2,0);
        measurement.create(2,1);
        measurement.setTo(Scalar(0));

        // intialization of KF...
        KF.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);

        measurement.setTo(Scalar(0));

        KF.statePre.at<float>(0) = obsPointX;
        KF.statePre.at<float>(1) = obsPointY;
        KF.statePre.at<float>(2) = 0;
        KF.statePre.at<float>(3) = 0;
        setIdentity(KF.measurementMatrix);
        setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
        setIdentity(KF.measurementNoiseCov, Scalar::all(10));
        setIdentity(KF.errorCovPost, Scalar::all(.1));
        // Image to show mouse tracking
        obsv.clear();
        kalmanv.clear();
        int i=0;
        initKalman=true;
    }
    else
    {
        // First predict, to update the internal statePre variable
        Mat prediction = KF.predict();
        Point predictPt(prediction.at<float>(0),prediction.at<float>(1));

        // Get mouse point
        //GetCursorPos(&mousePos);
        measurement(0) = obsPointX;
        measurement(1) = obsPointY;

        // The update phase
        Mat estimated = KF.correct(measurement);

        Point statePt(estimated.at<float>(0),estimated.at<float>(1));
        Point measPt(measurement(0),measurement(1));

        obsPointXKalmanCorrection=statePt.x;
        obsPointYKalmanCorrection=statePt.y;

        obsv.push_back(measPt);
        kalmanv.push_back(statePt);

        //for (int i = 0; i < obsv.size()-1; i++)
        //    line(img, mousev[i], mousev[i+1], Scalar(255,255,0), 1);
        //for (int i = 0; i < kalmanv.size()-1; i++)
        //    line(img, kalmanv[i], kalmanv[i+1], Scalar(0,155,255), 1);
    }
}


void HeadRotation::setGazeFromPosit()
{
    double x=0,y=0; //Coordinate punto osservato
    double eye_position, eye_position_y; //Proiezione occhi sullo schermo
    //rotation:180=x:pi -> 180*x = pi*rotation ->
    x = angle_x*M_PI/180;
    //Calacoliamo con il teorema dei seni la coordinata x
    double distance_x = distance * sin(x) / sin((M_PI/2) - x);
    distance_x = distance_x * CM_TO_PIXEL*2; //1cm -> 37.795275591 pixel
    //Calacoliamo con il teorema dei seni la coordinata y
    y = angle_y*M_PI/180;
    double distance_y = distance * sin(y) / sin((M_PI/2) - y);
    distance_y = distance_y * CM_TO_PIXEL; //1cm -> 37.795275591 pixel
    // x:hRes=position_face:frame.cols
    eye_position = hRes*(rH->getFace().x+(rH->getFace().width/2))/rH->getGrayFrame().cols;
    eye_position_y = vRes*(rH->getFace().y+(rH->getFace().height/2))/rH->getGrayFrame().rows;

    //Calcolare coord x;
    obsPointX = eye_position + distance_y;
    obsPointY = eye_position_y + distance_x;
}

double HeadRotation::getObsPointX()
{
    return obsPointX;
}

double HeadRotation::getObsPointY()
{
    return obsPointY;
}

double HeadRotation::getObsPointXKalmanCorrection()
{
    return obsPointXKalmanCorrection;
}

double HeadRotation::getObsPointYKalmanCorrection()
{
    return obsPointYKalmanCorrection;
}
