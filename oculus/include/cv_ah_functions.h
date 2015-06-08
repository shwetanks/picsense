#ifndef _CV_AH_FUNCTIONS_H_
#define _CV_AH_FUNCTIONS_H_

#include <opencv2/opencv.hpp>

class cv_ah_functions {

public:
    static int deskew_image(const char *path, char* dump_image);
    static double compute_skew(cv::Mat& image, cv::Mat& normalized_image);
    static cv::Mat mat_normalize(cv::Mat& image);
    static cv::Mat rotate_image(cv::Mat& image, double angle);
};

#endif /* _CV_AH_FUNCTIONS_H_ */
