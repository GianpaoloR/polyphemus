#ifndef HELPERS_H
#define HELPERS_H

#include "constants.h"
#include "cvInclusions.h"
#include "initFlags.h"

#include <iostream>
#include <queue>
#include <stdio.h>
#include <string.h>

//UO
#include <sys/types.h>
#include <sys/stat.h>    /* Fix up for Windows - inc mode_t */
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/time.h>
//UO

using namespace std;

bool rectInImage(Rect rect, Mat image);
bool inMat(Point p,int rows,int cols);
Mat matrixMagnitude(const Mat &matX, const Mat &matY);
double computeDynamicThreshold(const Mat &mat, double stdDevFactor);
void rotateImage(Mat& src, double angle, Mat& dst);
void rotateRect(Rect& src, double angle, Rect& dst);
bool fileExists (const string& name);
//UO
int createDirectoryIfNotExist(const char *path, mode_t mode);
int copyDirectory(const char *src, const char *dest, mode_t mode);
int copyFile(const char *src, const char *dest, mode_t mode);
long long timestamp_usec();
//UO
void circles(Mat src);
void flat(cv::Mat& image);
void flatField(cv::Mat& image);
float computeAvgLight(Mat &image);
void rotateFrame(double angle);
Mat toGrayScale(cv::Mat& frame);
cv::Mat rotate(float alpha, cv::Mat& src);
cv::Rect MatchingMethod( cv::Mat& img_display, cv::Mat templ );

void debugPrint(string msg);

struct setInformation
{
    int id;
    int nPoints;
    int width;
    int height;
    cv::Point center;
    cv::Point centroid;
    std::vector<cv::Point> elem;
    std::vector<int> sons;
    std::vector<Point> corners;
};


enum eyes
{
    LEFT,
    RIGHT
};

enum pupilType
{
    STASM,
    G_CATCH
};

#endif
