#ifndef TWINKLE_H
#define TWINKLE_H

#include "common/cvInclusions.h"

class twinkle
{
public:
    twinkle();
    cv::Mat twinkleDilate(const cv::Mat& roi);  //Work on neighborhood to define point
};

#endif // TWINKLE_H
