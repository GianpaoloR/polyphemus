// minimal.cpp: Display the landmarks of a face in an image.
//
// This demonstrates stasm_search_single.

#include <stdio.h>
#include <stdlib.h>
#include "opencv/highgui.h"
#include "stasm4_1/stasm/stasm_lib.h"




int main()
{
    static const char* path = "../stasm4_1/data/testface.jpg";
    cv::Mat_<unsigned char> img(cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE));
    if(!img.data)
    {
        printf("Cannotload %s\n", path);
        exit(-1);
    }

    int foundface;
    float landmarks[2* stasm_NLANDMARKS]; // x,y coords

    if (!stasm_search_single(&foundface, landmarks,
            (char*)img.data, img.cols, img.rows, path, "../stasm4_1/data"))

    {
        printf("Error in stasm_search_single: %s\n", stasm_lasterr());
        exit(1);
    }

    if (!foundface)
        printf("No face found in %s\n", path);
    else
    {
    // draw the landmarks on the image as white dots
        stasm_force_points_into_image(landmarks, img.cols, img.rows);
        for (int i = 0; i < stasm_NLANDMARKS; i++)
            img(cvRound(landmarks[i*2+1]), cvRound(landmarks[i*2])) = 255;
    }
    cv::imshow("stasm minimal", img);
    cv::waitKey();
    return 0;

}
