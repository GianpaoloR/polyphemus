#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "cgi.h"
#include "base64.h"

#include "../common/helpers.h"

#include "../common/roiHandler.h"

#define PRIVILEGES 0777
#define WRONG_VAL 0

class Server {
public:
    bool init(cgicc::Cgicc cgicc);

    void setInfo(std::string key, int value);
    void setInfo(std::string key, long long value);
    void setInfo(std::string key, double value);
    void setInfo(std::string key, std::string value);
    void setInfo(std::string key, std::string type, std::string value);
    int getInfoInt(std::string key);
    long long getInfoLonglong(std::string key);
    double getInfoDouble(std::string key);
    std::string getInfoString(std::string key);

    void setCookie(std::string key, double value);
    void setCookie(std::string key, std::string value);
    std::string getCookie(std::string key);
    void initCookies(std::vector<cgicc::HTTPCookie> cookieList);
    void saveCookies();
    void setPersistentImage(std::string key, Mat image);
    cv::Mat getPersistentImage(std::string key);

    void setFrame(Mat frame);
    cv::Mat getFrame();
    void setPreviousFrame(Mat frame);
    cv::Mat getPreviousFrame();
    bool continueReading();

    std::string getSessionName();
    int getNumSnapshot();
    std::string getSnapshotType();
    int getScreenWidthPixel();
    int getScreenHeightPixel();
    bool getIsBoundingBox();

    void setWatchingPoint(cv::Point watchingPoint);

    void printResponse(roiHandler *roiHandler);
    void printResponseError();

    static Server* getServer();

private:
    Server();
    static Server* serverInstance;
    std::map<std::string, std::string> cookies;

    string infoFileName;
    map<string, string> infoString;
    map<string, int> infoInt;
    map<string, long long> infoLonglong;
    map<string, double> infoDouble;
    void readInfo();
    int copyInfo(std::string sessionFrom, std::string sessionTo);
    void writeInfo();
    void eraseSessionInfo();
    int removeDirectoryElements(const char *path, const char *noRemoveName);

    std::string ip;
    std::string sessionName;
    //    int seed;
    //    int token;
    bool createSessionName();

    int numSnapshot;
    std::string snapshotType;
    int screenWidthPixel;
    int screenHeightPixel;

    bool isBoundingBox;

    cv::Mat frame;
    cv::Mat previousFrame;
    bool isFirstFrame;

    long long serverReceiveImageTime;
    long long serverSendResponseTime;

    cv::Point watchingPoint;
};

#endif // SERVER_H
