#include "twinkle.h"

twinkle::twinkle()
{
}

//Dilate: apply dilation morphologic algorithm
cv::Mat twinkle::twinkleDilate(const cv::Mat& roi)  //Work on neighborhood to define point
{
    #define NMASK 4
    #define SIZE 3
    int maskH[SIZE][SIZE] = {{128,128,128},
                            { 255,  0,255},
                            { 128,128,128}};  //128 stands for don't care

    int maskV[SIZE][SIZE] = {{128, 255, 128},
                             {128,   0, 128},
                             {128, 255, 128}};

    int maskDiagDown[SIZE][SIZE] = {{255, 128, 128},
                                    {128,   0, 128},
                                    {128, 128, 255}};

    int maskDiagUp[SIZE][SIZE] = {{128, 128, 255},
                                  {128,   0, 128},
                                  {255, 128, 128}};

    int mask[NMASK][SIZE][SIZE] = {{{255, 128, 128},
                                    {128,   0, 128},
                                    {128, 128, 255}},

                                   {{128, 128, 255},
                                    {128,   0, 128},
                                    {255, 128, 128}},

                                   {{128,128,128},
                                    {255,  0,255},
                                    {128,128,128}},

                                   {{128, 255, 128},
                                    {128,   0, 128},
                                    {128, 255, 128}}};


    cv::Mat diffRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
    cv::Mat completeRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
    cv::Mat partialReturnRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
    cv::Mat returnRoi = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
    cv::Mat currentRoi = roi;
    bool changeVal;
    int grayVal = 0;

    //Only diagonal dilation
    for(int m=0; m<4; m++)
    {
        grayVal +=51;
        for(int i=0; i<currentRoi.rows; i++)
        {
            for(int j=0; j<currentRoi.cols; j++)
            {
                if(currentRoi.at<uchar>(i,j) == mask[m][SIZE/2][SIZE/2])
                {
                    int k1min = i-SIZE/2;
                    int k1Max = i+SIZE/2;
                    int k2min = j-SIZE/2;
                    int k2Max = j+SIZE/2;
                    changeVal = false;
                    if(k1min >= 0 && k1Max<currentRoi.rows && k2min >=0 && k2Max<currentRoi.cols)
                    {
                        changeVal = true;
                        for(int k1=k1min; k1<=k1Max; k1++)
                        {
                            for(int k2=k2min; k2<=k2Max; k2++)
                            {
                                if(mask[m][k1-i+SIZE/2][k2-j+SIZE/2] != 128) //ignore don't care
                                {
                                    if(currentRoi.at<uchar>(k1,k2) != mask[m][k1-i+SIZE/2][k2-j+SIZE/2]) changeVal = false;
                                }
                            }
                        }
                    }
                    if(changeVal)
                    {
                        diffRoi.at<uchar>(i,j) = grayVal; //grayVal; --> grayVal to see differences (partial), 128; to prepare for algorithm, expression
                    }
                }
            }
        }

        if(m==1) //First merge
        {
            for(int i=0;i<roi.rows; i++)
            {
                for(int j=0; j<roi.cols; j++)
                {
                    if(diffRoi.at<uchar>(i,j) > 0)
                    {
                        partialReturnRoi.at<uchar>(i,j) = 255-roi.at<uchar>(i,j);
                    }
                    else
                    {
                        partialReturnRoi.at<uchar>(i,j) = roi.at<uchar>(i,j);
                    }
                }
            }
            currentRoi = partialReturnRoi;
//            char*pName = "partial";
//            namedWindow(pName, CV_WINDOW_NORMAL);
//            imshow(pName, currentRoi);
//            waitKey(0);
        }
    }


    for(int i=0;i<currentRoi.rows; i++)
    {
        for(int j=0; j<currentRoi.cols; j++)
        {
            if(diffRoi.at<uchar>(i,j) > 0)
            {
                completeRoi.at<uchar>(i,j) = diffRoi.at<uchar>(i,j);
            }
            else
            {
                completeRoi.at<uchar>(i,j) = currentRoi.at<uchar>(i,j);
            }

            if(diffRoi.at<uchar>(i,j) > 128)
            {
                returnRoi.at<uchar>(i,j) = 255-currentRoi.at<uchar>(i,j);
            }
            else
            {
                returnRoi.at<uchar>(i,j) = currentRoi.at<uchar>(i,j);
            }
        }
    }

//    char*dName = "Differences";
//    namedWindow(dName, CV_WINDOW_NORMAL);
//    imshow(dName, completeRoi);
//    waitKey(0);

    return returnRoi;
}
