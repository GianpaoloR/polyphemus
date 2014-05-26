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


/**
 * @brief flatField: Quello che sistema la luce è.
 */
void flatField(cv::Mat& image)
{
    int h=0;


    //std::cout<<"FLATFIELD: image: "<<image.cols <<" x "<< image.rows<<std::endl;

    std::vector<cv::Mat> fields(3), correctedFields(3);
    cv::Scalar media;
    int dim;
    cv::Mat flatField, correctedImage;
    //    flatField = cv::Mat(image.rows, image.cols, image.type());
    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    cv::cvtColor(image, flatField, CV_RGB2HSV);

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    //correctedImage = cv::Mat(flatField.rows, flatField.cols, flatField.type());
    flatField.copyTo(correctedImage);

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    split(flatField, fields);

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    split(correctedImage, correctedFields);

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    //Build mask size
    if (image.rows > image.cols) {
        dim = 3 * (image.rows / 4);
    }
    else {
        dim = 3 * (image.cols / 4);
    }
    blur(fields[2], fields[2], cv::Size(dim, dim), cv::Point(-1, -1));

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    GaussianBlur(fields[2], fields[2], cv::Size(31, 31), 0);

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    //cv::namedWindow("Flat field", CV_WINDOW_NORMAL);
    //imshow("Flat field", fields[2]);

    media = mean(fields[2]);

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    divide(correctedFields[2], fields[2], correctedFields[2], media.val[0], -1);

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    //medianBlur(correctedFields[2], correctedFields[2], 5);          //TODO: to evaluate, delete the pixel coloured error
    //    namedWindow("Flat fielded V channel", CV_WINDOW_NORMAL);
    //    imshow("Flat fielded V channel", correctedFields[2]);

    merge(correctedFields, correctedImage);

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

    cv::cvtColor(correctedImage, image, CV_HSV2RGB);

    if(flatfieldDebug) std::cout<<"FLATFIELD "<<h++<<std::endl;

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
