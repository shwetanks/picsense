#ifndef _PICSENSE_CV_FUNCTIONS_H_
#define _PICSENSE_CV_FUNCTIONS_H_

#include <opencv2/opencv.hpp>

class cv_functions {

public:
    cv::Mat deskew_image(const char *path, char* dump_image);
    double compute_skew(cv::Mat& image, cv::Mat& normalized_image);
    cv::Mat mat_normalize(cv::Mat& image);
    cv::Mat rotate_image(cv::Mat& image, double angle);
};

#endif /* _PICSENSE_CV_FUNCTIONS_H_ */
