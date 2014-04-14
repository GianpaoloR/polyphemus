#include "helpers.h"
#include "unistd.h"

bool rectInImage(Rect rect, Mat image) {
    return rect.x > 0 && rect.y > 0 && rect.x+rect.width < image.cols &&
            rect.y+rect.height < image.rows;
}

bool inMat(Point p,int rows,int cols) {
    return p.x >= 0 && p.x < cols && p.y >= 0 && p.y < rows;
}


//Given the X and Y gradient matrix, compute the magnitude matrix
Mat matrixMagnitude(const Mat &matX, const Mat &matY) {
    Mat mags(matX.rows,matX.cols,CV_64F);
    for (int y = 0; y < matX.rows; ++y) {
        const double *Xr = matX.ptr<double>(y), *Yr = matY.ptr<double>(y);
        double *Mr = mags.ptr<double>(y);
        for (int x = 0; x < matX.cols; ++x) {
            double gX = Xr[x], gY = Yr[x];
            double magnitude = sqrt((gX * gX) + (gY * gY));
            Mr[x] = magnitude;
        }
    }
    return mags;
}

double computeDynamicThreshold(const Mat &mat, double stdDevFactor) {
    Scalar stdMagnGrad, meanMagnGrad;
    meanStdDev(mat, meanMagnGrad, stdMagnGrad);
    double stdDev = stdMagnGrad[0] / sqrt(mat.rows*mat.cols);
    return stdDevFactor * stdDev + meanMagnGrad[0];
}


//rotate an rect
void rotateRect(Rect& src, double angle, Rect& dst) {
    RotatedRect rRect = RotatedRect(Point2f(src.x,src.y), Size2f(src.width,src.height), angle);
    dst = rRect.boundingRect();
}

//tests if the file exist
bool fileExists(const string& name) {
    return ( access( name.c_str(), F_OK ) != -1 );
}

//UO
//tests if the directory exist and if not creates it
int createDirectoryIfNotExist(const char *path, mode_t mode) {
    struct stat st;
    int status = 1;     //created
    if (stat(path, &st) != 0) {
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir(path, mode) != 0 && errno != EEXIST)
            status = -1;        //it wasn't created
    }
    else {
        if (!S_ISDIR(st.st_mode)) {
            errno = ENOTDIR;
            status = -1;
        }
        else {
            //std::cout << "EXIST" << std::endl;
            status = 0;     //already exist
        }
    }
    return status;
}

//recursive copy of a directory and its files and subfolders
int copyDirectory(const char *source, const char *destination, mode_t mode) {
    int r;
    r = createDirectoryIfNotExist(destination, mode);
    if (r == -1) {
        return r;
    }
    DIR *directory = opendir(source);

    if (directory) {
        struct dirent *p;
        r = 0;

        while ((r != -1) && (p = readdir(directory))) {

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if ((!strcmp(p->d_name, ".")) || (!strcmp(p->d_name, ".."))) {
                continue;
            }

            std::stringstream newSource;
            newSource << source << p->d_name;
            std::stringstream newDestination;
            newDestination << destination << p->d_name;

            struct stat statbuf;

            if (!stat(newSource.str().c_str(), &statbuf)) {
                if (S_ISDIR(statbuf.st_mode)) {
                    newSource << "/";
                    newDestination << "/";
                    r = copyDirectory(newSource.str().c_str(), newDestination.str().c_str(), mode);
                }
                else {
                    r = copyFile(newSource.str().c_str(), newDestination.str().c_str(), mode);
                }
            }
        }

        closedir(directory);
    }
    return r;
}

//copy a single file to a new source
int copyFile(const char *source, const char *destination, mode_t mode) {
    int in_fd, out_fd, n_chars;
    int bufferSize = 1024;
    char buf[bufferSize];

    /* open files */
    if ((in_fd = open(source, O_RDONLY)) == -1) {
        return -1;
    }

    if ((out_fd = creat(destination, mode)) == -1) {
        return -1;
    }

    /* copy files */
    while ((n_chars = read(in_fd, buf, bufferSize)) > 0) {
        if (write(out_fd, buf, n_chars) != n_chars) {
            return -1;
        }

        if (n_chars == -1) {
            return -1;
        }
    }

    /* close files */
    if ((close(in_fd) == -1) || (close(out_fd) == -1)) {
        return -1;
    }

    return 1;
}

long long timestamp_usec() {
    struct timeval now;
    gettimeofday(&now , NULL);
    return (long long)((long long)now.tv_sec*1000000 + (long long)now.tv_usec);
}

//UO

//
void eyeColourEvaluation(Mat image) {
    bool showWindow = false;
    GaussianBlur(image, image, Size(3,3), 3);

    //cv::addWeighted(img, 1.5, tmp, -0.5, 0, img);
    addWeighted(image, 2.5, image, -1.5, 0, image);     //using the blur, applies an unsharp mask

    vector<Mat> image_channels(3);
    Mat imageHSV, originalImageHSV, imageHSV2BGR;
    //int histSizeV[1]={256};

    if (showWindow) {
        namedWindow("image", CV_WINDOW_NORMAL);
        imshow("image", image);
    }

    //    // HSV values range
    //    float hrangesHS[2]={0.0, 255.0};
    //    float hrangesV[2]={0.0, 179.0};
    //    const float* rangesHS[1]={hrangesHS};
    //    const float* rangesV[1]={hrangesV};
    //    int channel[1]={0};

    //    // number of histogram lines
    //    int histSizeHS[1]={256};
    //    int histSizeV[1]={180};

    //    int i, img_h, img_wHS, img_wV, intensity_hue, intensity_sat, intensity_val;
    //    double maxH, maxS, maxV;
    //    double minH, minS, minV;
    //    Point pointMaxH, pointMaxS, pointMaxV;
    //    Point pointMinH, pointMinS, pointMinV;
    //    MatND hist_hue, hist_sat, hist_val;

    //    Mat imgH;
    //    image.convertTo(image, -1, 2, 0);

    //    if (showWindow) {
    //    namedWindow("contrastH", CV_WINDOW_NORMAL);
    //    imshow("contrastH", image);
    //    }

    //    split(image, image_channels);
    //    minMaxLoc(image_channels[0], &minH, &maxH, &pointMinH, &pointMaxH);
    //    minMaxLoc(image_channels[1], &minS, &maxS, &pointMinS, &pointMaxS);
    //    minMaxLoc(image_channels[2], &minV, &maxV, &pointMinV, &pointMaxV);
    //    image_channels[0].convertTo(image_channels[0], CV_8U, 255.0/(maxH - minH), -minH * 255.0/(maxH - minH));
    //    image_channels[1].convertTo(image_channels[1], CV_8U, 255.0/(maxS - minS), -minS * 255.0/(maxS - minS));
    //    image_channels[2].convertTo(image_channels[2], CV_8U, 255.0/(maxV - minV), -minV * 255.0/(maxV - minV));
    //    merge(image_channels, image);
    //    if (showWindow) {
    //    namedWindow("eq", CV_WINDOW_NORMAL);
    //    imshow("eq", image);
    //    }


    //    Mat grey;
    //    cvtColor(image, grey, CV_BGR2GRAY);

    //    Mat sobelx;
    //    Sobel(grey, sobelx, CV_32F, 1, 0);

    //    double minVal, maxVal;
    //    minMaxLoc(sobelx, &minVal, &maxVal); //find minimum and maximum intensities
    //    Mat draw;
    //    sobelx.convertTo(draw, CV_8U, 255.0/(maxVal - minVal), -minVal * 255.0/(maxVal - minVal));

    //    if (showWindow) {
    //    namedWindow("sobelx", CV_WINDOW_NORMAL);
    //    imshow("sobelx", draw);
    //    }



    cvtColor(image, originalImageHSV, CV_BGR2HSV);
    originalImageHSV.copyTo(imageHSV);

    if (showWindow) {
        cvtColor(originalImageHSV, imageHSV2BGR, CV_HSV2BGR);
        namedWindow("imageHSV", CV_WINDOW_NORMAL);
        imshow("imageHSV", imageHSV2BGR);
    }


    for (int i = 0; i < imageHSV.rows; i++) {
        for (int j = 0; j < imageHSV.cols; j++) {
            for (int z = 1; z < 3; z++) {
                int temp=imageHSV.at<cv::Vec3b>(i, j)[z]+255;

                if(temp>255) {
                    imageHSV.at<cv::Vec3b>(i, j)[z]=255;
                }
                else {
                    if(temp<0) {
                        imageHSV.at<cv::Vec3b>(i, j)[z]=0;
                    }
                    else {
                        imageHSV.at<cv::Vec3b>(i, j)[z]=temp;
                    }
                }
            }
        }
    }
    if (showWindow) {
        cvtColor(imageHSV, imageHSV2BGR, CV_HSV2BGR);
        namedWindow("imageHSV Smax and Vmax", CV_WINDOW_NORMAL);
        imshow("imageHSV Smax and Vmax", imageHSV2BGR);
    }


    Mat imgHLS;
    cvtColor(image, imgHLS, CV_BGR2HLS);
    Mat rngMask;
    inRange(imgHLS, Scalar(0, 200, 0), Scalar(255, 255, 255), rngMask);
    split(imgHLS, image_channels);
    for (int i = 0; i < imgHLS.rows; i++) {
        for (int j = 0; j < imgHLS.cols; j++) {
            for (int z = 1; z < 2; z++) {
                imgHLS.at<cv::Vec3b>(i, j)[z]=127;
            }
        }
    }
    if (showWindow) {
        cvtColor(imgHLS, imageHSV2BGR, CV_HLS2BGR);
        namedWindow("imageHLS L1/2", CV_WINDOW_NORMAL);
        imshow("imageHLS L1/2", image_channels[1]);
    }




    originalImageHSV.copyTo(imageHSV);
    for (int i = 0; i < imageHSV.rows; i++) {
        for (int j = 0; j < imageHSV.cols; j++) {
            for (int z = 1; z < 3; z++) {
                int temp=imageHSV.at<cv::Vec3b>(i, j)[z]+25;

                if(temp>255) {
                    imageHSV.at<cv::Vec3b>(i, j)[z]=255;
                }
                else {
                    if(temp<0) {
                        imageHSV.at<cv::Vec3b>(i, j)[z]=0;
                    }
                    else {
                        imageHSV.at<cv::Vec3b>(i, j)[z]=temp;
                    }
                }
            }
        }
    }
    if (showWindow) {
        cvtColor(imageHSV, imageHSV2BGR, CV_HSV2BGR);
        namedWindow("imageHSV S++ and V++", CV_WINDOW_NORMAL);
        imshow("imageHSV S++ and V++", imageHSV2BGR);
    }


    //    split(imageHSV, image_channels);
    //    //split(image, image_channels);

    //    //equalizeHist(image_channels[0], image_channels[0]);
    //    equalizeHist(image_channels[1], image_channels[1]);
    //    equalizeHist(image_channels[2], image_channels[2]);
    //    //merge(image_channels, imageHSV);

    //    calcHist(&image_channels[0], 1, channel, Mat(), hist_hue, 1, histSizeHS, rangesHS);
    //    calcHist(&image_channels[1], 1, channel, Mat(), hist_sat, 1, histSizeHS, rangesHS);
    //    calcHist(&image_channels[2], 1, channel, Mat(), hist_val, 1, histSizeV, rangesV);

    //    // max and min value of bins
    //    maxH=maxS=maxV=0.0;
    //    minH=minS=minV=0.0;
    //    minMaxLoc(hist_hue, &minH, &maxH, &pointMinH, &pointMaxH);
    //    minMaxLoc(hist_sat, &minS, &maxS, &pointMinS, &pointMaxS);
    //    minMaxLoc(hist_val, &minV, &maxV, &pointMinV, &pointMaxV);

    //    // image for drawing the histogram
    //    img_h=400;
    //    img_wHS=histSizeHS[0];
    //    img_wV=histSizeV[0];

    //    Mat histImg_hue(img_h, img_wHS, CV_8UC3, Scalar(0, 0, 0));
    //    Mat histImg_sat(img_h, img_wHS, CV_8UC3, Scalar(0, 0, 0));
    //    Mat histImg_val(img_h, img_wV, CV_8UC3, Scalar(0, 0, 0));

    //    // max height (90% of image height)
    //    int hpt=static_cast<int>(0.9*img_h);

    //    // draw a single line for each bin
    //    for (i=0; i<img_wHS; i++){
    //        // normalized height (with respect to the histogram image)
    //        intensity_hue=static_cast<int>(hist_hue.at<float>(i)*hpt/maxH);
    //        intensity_sat=static_cast<int>(hist_sat.at<float>(i)*hpt/maxS);
    //        if (i<img_wV){
    //            intensity_val=static_cast<int>(hist_val.at<float>(i)*hpt/maxV);
    //        }

    //        // effectively draw the line
    //        line(histImg_hue, Point(i,img_h), Point(i,img_h-intensity_hue), Scalar(255,0,0));
    //        line(histImg_sat, Point(i,img_h), Point(i,img_h-intensity_sat), Scalar(0,255,0));
    //        if (i<img_wV){
    //            line(histImg_val, Point(i,img_h), Point(i,img_h-intensity_val), Scalar(0,0,255));
    //        }
    //    }

    //    line(histImg_hue, Point(pointMaxH.y,img_h), Point(pointMaxH.y,img_h/2), Scalar(0,255,255));
    //    line(histImg_sat, Point(pointMaxS.y,img_h), Point(pointMaxS.y,img_h/2), Scalar(255,0,255));
    //    line(histImg_val, Point(pointMaxV.y,img_h), Point(pointMaxV.y,img_h/2), Scalar(255,255,0));

    //    if (showWindow) {
    //    cvtColor(histImg_hue, histImg_hue, CV_RGB2BGR);
    //    cvtColor(histImg_sat, histImg_sat, CV_RGB2BGR);
    //    cvtColor(histImg_val, histImg_val, CV_RGB2BGR);
    //    namedWindow("H hist", CV_WINDOW_NORMAL);
    //    namedWindow("S hist", CV_WINDOW_NORMAL);
    //    namedWindow("V hist", CV_WINDOW_NORMAL);
    //    imshow("H hist", histImg_hue);
    //    imshow("S hist", histImg_sat);
    //    imshow("V hist", histImg_val);
    //    }

    Mat rangeMask, rangeImage;
    for (int i = 0; i < 360; i+=30) {
        int iIn255 = (int)(i*255/360);
        int epsilon = (int)(15*255/360);

        inRange(imageHSV, Scalar(iIn255 - epsilon, 0, 0), Scalar(iIn255 + epsilon, 255, 255), rangeMask);

        bitwise_and(image, image, rangeImage, rangeMask);

        Mat r;
        cvtColor(rangeImage, r, CV_RGB2GRAY);
        std::string name = "rangeImage ";
        std::stringstream sstm;
        sstm << name << (i);
        name = sstm.str();
        if (showWindow) {
            namedWindow(name, CV_WINDOW_NORMAL);
            imshow(name, rangeImage);
        }
        rangeMask.release();
        rangeImage.release();
    }

    int erosionSize = 2;
    Mat erodeSettings = getStructuringElement(MORPH_ELLIPSE, Size(2*erosionSize + 1, 2*erosionSize+1), Point(erosionSize, erosionSize));
    Mat erodedImage;
    int dilateSize = 2;
    Mat dilateSettings = getStructuringElement(MORPH_ELLIPSE, Size(2*dilateSize + 1, 2*dilateSize+1), Point(dilateSize, dilateSize));
    Mat dilatedImage;
    /// Apply the erosion operation
    erode(image, erodedImage, erodeSettings);
    if (showWindow) {
        namedWindow("erodedImage", CV_WINDOW_NORMAL);
        imshow("erodedImage", erodedImage);
    }
    /// Apply the dilation operation
    dilate(image, dilatedImage, dilateSettings);
    if (showWindow) {
        namedWindow("dilatedImage", CV_WINDOW_NORMAL);
        imshow("dilatedImage", dilatedImage);
    }

    //    erodedImage.convertTo(erodedImage, -1, 2, 0);
    //    namedWindow("erodedImageH", CV_WINDOW_NORMAL);
    //    imshow("erodedImageH", erodedImage);

    Mat whiteImageHSV, blackImageHLS;
    cvtColor(image, whiteImageHSV, CV_RGB2HSV);
    cvtColor(erodedImage, blackImageHLS, CV_RGB2HLS);
    //    for (int i = 0; i < whiteImageHSV.rows; i++) {
    //        for (int j = 0; j < whiteImageHSV.cols; j++) {
    //            for (int z = 1; z < 3; z++) {
    //                int temp;
    //                if (z == 1)
    //                    temp=whiteImageHSV.at<cv::Vec3b>(i, j)[z]-25;
    //                else
    //                    temp=whiteImageHSV.at<cv::Vec3b>(i, j)[z]+25;

    //                if(temp>255) {
    //                    whiteImageHSV.at<cv::Vec3b>(i, j)[z]=255;
    //                }
    //                else {
    //                    if(temp<0) {
    //                        whiteImageHSV.at<cv::Vec3b>(i, j)[z]=0;
    //                    }
    //                    else {
    //                        whiteImageHSV.at<cv::Vec3b>(i, j)[z]=temp;
    //                    }
    //                }
    //            }
    //        }
    //    }
    //    cvtColor(whiteImageHSV, imageHSV2BGR, CV_HSV2BGR);
    //    namedWindow("imageHSV S-- and V++", CV_WINDOW_NORMAL);
    //    imshow("imageHSV S-- and V++", imageHSV2BGR);

    inRange(whiteImageHSV, Scalar(0, 0, 42), Scalar(255, 42, 255), rangeMask);
    bitwise_and(image, image, rangeImage, rangeMask);

    if (showWindow) {
        namedWindow("rangeImage white HSV", CV_WINDOW_NORMAL);
        imshow("rangeImage white HSV", rangeImage);
    }

    rangeMask.release();
    rangeImage.release();

    split(blackImageHLS, image_channels);
    equalizeHist(image_channels[1], image_channels[1]);
    merge(image_channels, blackImageHLS);

    inRange(blackImageHLS, Scalar(0, 0, 0), Scalar(255, 32, 255), rangeMask);
    bitwise_and(image, image, rangeImage, rangeMask);

    if (showWindow) {
        namedWindow("rangeImage black HLS", CV_WINDOW_NORMAL);
        imshow("rangeImage black HLS", rangeMask);
    }

    rangeMask.release();
    rangeImage.release();


    //    cv::Mat imgGray;
    //    int threshold = 50;
    //    int maxContoursSize = 5;
    //    cv::Mat cannyOutput;
    //    std::vector<std::vector<cv::Point> > contours;
    //    std::vector<cv::Vec4i> hierarchy;

    //    /// Convert image to gray and blur it
    //    if (image.channels() != 1)
    //        cv::cvtColor(image, imgGray, CV_BGR2GRAY);
    //    else
    //        image.copyTo(imgGray);
    //    cv::blur(imgGray, imgGray, cv::Size(3,3));

    //    do {
    //        threshold--;
    //        /// Detect edges using canny
    //        cv::Canny(imgGray, cannyOutput, threshold, threshold*2, 3);
    //        /// Find contours
    //        cv::findContours(cannyOutput, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    //    } while ((contours.size() < maxContoursSize) && (threshold > 0));
    //    cout << "contours.size() " << contours.size() << endl;

    //    cv::Mat drawing = cv::Mat::zeros(cannyOutput.size(), CV_8UC1);
    //    for (int i = 0; i< contours.size(); i++) {
    //        cv::Scalar color = cv::Scalar(255);
    //        cv::drawContours(drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point());
    //    }

    //    if (showWindow) {
    //    cv::namedWindow("Contours", CV_WINDOW_NORMAL);
    //    imshow("Contours", drawing);
    //    }

    //    waitKey(0);
}

/**
 * @brief flatField: Quello che sistema la luce è.
 */
void flatField(cv::Mat& image)
{
#ifdef FLATFIELD_DEBUG
    int h=0;
#endif

    //std::cout<<"FLATFIELD: image: "<<image.cols <<" x "<< image.rows<<std::endl;

    std::vector<cv::Mat> fields(3), correctedFields(3);
    cv::Scalar media;
    int dim;
    cv::Mat flatField, correctedImage;
    //    flatField = cv::Mat(image.rows, image.cols, image.type());
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif

    cv::cvtColor(image, flatField, CV_RGB2HSV);
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif
    //correctedImage = cv::Mat(flatField.rows, flatField.cols, flatField.type());
    flatField.copyTo(correctedImage);
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif
    split(flatField, fields);
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif
    split(correctedImage, correctedFields);
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif
    //Build mask size
    if (image.rows > image.cols) {
        dim = 3 * (image.rows / 4);
    }
    else {
        dim = 3 * (image.cols / 4);
    }
    blur(fields[2], fields[2], cv::Size(dim, dim), cv::Point(-1, -1));
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif
    GaussianBlur(fields[2], fields[2], cv::Size(31, 31), 0);
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif

    //cv::namedWindow("Flat field", CV_WINDOW_NORMAL);
    //imshow("Flat field", fields[2]);

    media = mean(fields[2]);
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif
    divide(correctedFields[2], fields[2], correctedFields[2], media.val[0], -1);
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif

    //medianBlur(correctedFields[2], correctedFields[2], 5);          //TODO: to evaluate, delete the pixel coloured error
    //    namedWindow("Flat fielded V channel", CV_WINDOW_NORMAL);
    //    imshow("Flat fielded V channel", correctedFields[2]);

    merge(correctedFields, correctedImage);
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif
    cv::cvtColor(correctedImage, image, CV_HSV2RGB);
#ifdef FLATFIELD_DEBUG
    std::cout<<"FLATFIELD "<<h++<<std::endl;
#endif
    //cv::namedWindow("Corrected image", CV_WINDOW_NORMAL);
    //cv::imshow("Corrected image", image);
}

//ComputeAvgLight: for each reduced eye zone. This will be useful when filtering candidate points.
float computeAvgLight(cv::Mat &image)
{
    int lightSum = 0;
    int den = image.rows*image.cols;

    for(int i=0;i<image.rows;i++)
    {
        for(int j=0; j<image.cols; j++)
        {
            lightSum += image.at<uchar>(i,j);
        }
    }

    return (float)lightSum /den;
}



//toGrayScale: returns grayscale image of the frame
cv::Mat toGrayScale(cv::Mat& frame)
{
    cv::Mat grayFrame;
#ifdef TOGRAYSCALE_DEBUG
    std::cout<<"TOGRAYSCALE: cols = "<<frame.cols<<", rows = "<<frame.rows<<std::endl;
#endif

    std::vector<cv::Mat> rgbChannels(3);
    cv::split(frame, rgbChannels);
    grayFrame = rgbChannels[2];
#ifdef TOGRAYSCALE_DEBUG
    std::cout<<"TOGRAYSCALE: cols = "<<grayFrame.cols<<", rows = "<<grayFrame.rows<<std::endl;
#endif

    return grayFrame;
}

//rotate an image
void rotateImage(Mat& src, double angle, Mat& dst) {
    int len = max(src.cols, src.rows);
    Point2f pt(len/2., len/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, Size(len, len));
}

//rotate: given an angle, rotates both eyeROIs in order to align eye corners along x axis
cv::Mat rotate(float alpha, cv::Mat& src)
{
    cv::Point center(src.cols/2.0F, src.rows/2.0F);
    cv::Mat rot_mat = cv::getRotationMatrix2D(center, alpha, 1.0);

    cv::warpAffine(src, src, rot_mat, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, 255);

    return rot_mat;
}

/**
* Search templ in img_dispaly.
**/
cv::Rect MatchingMethod( cv::Mat& img_display, cv::Mat templ )
{
    cv::Mat result;
    /// Source image to display
    //Mat img_display;

    int match_method = 3; //Varia tra 0 e 5. Indica il metodo da utilizzare per la ricerca dell'immagine 3-5 i migliori

    /// Create the result matrix
    int result_cols =  img_display.cols - templ.cols + 1;
    int result_rows = img_display.rows - templ.rows + 1;

    result.create( result_cols, result_rows, CV_32FC1 );

    /// Do the Matching and Normalize
    matchTemplate( img_display, templ, result, match_method );
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    { matchLoc = minLoc; }
    else
    { matchLoc = maxLoc; }

    /// Show me what you got
    rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
    //rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

    return cv::Rect (matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ));
}

void debugPrint(string msg)
{
#ifndef WEBSERVICE
#ifdef DEBUG
    std::cout<<msg<<std::endl;
#endif
#endif
}
