#include "cv_ah_functions.h"

using namespace cv;

/*
    apply hough transform to detect skew


 */

double cv_ah_functions::compute_skew (Mat& im, Mat& orig) {
    double skew   = 0;
    double max_r  = sqrt(pow(.5*im.cols,2) + pow(.5*im.rows,2));
    int angleBins = 180;

    // accumulator plane
    Mat acc = Mat::zeros(Size(2*max_r,angleBins), CV_32SC1);
    int cenx = im.cols/2;
    int ceny = im.rows/2;
    for(int x=1; x<im.cols-1; x++) {
        for(int y=1; y<im.rows-1; y++) {
            if(im.at<uchar>(y,x) == 255) {
                for(int t=0; t < angleBins; t++) {
                    double r =(x-cenx)*cos((double)t/angleBins*CV_PI)+(y-ceny)*sin((double)t /angleBins*CV_PI);
                    r+=max_r;
                    acc.at<int>(t,int(r))++;
                }
            }
        }
    }
    Mat thresh;
    normalize(acc, acc, 255, 0, NORM_MINMAX);
    convertScaleAbs(acc, acc);
    Point maxLoc;
    minMaxLoc(acc, 0, 0, 0, &maxLoc);
    double theta = (double)maxLoc.y/angleBins * CV_PI;
    double rho = maxLoc.x - max_r;
    if(abs(sin(theta)) < 0.000001) {
        Point2d p1 = Point2d(rho+im.cols/2,0);
        Point2d p2 = Point2d(rho+im.cols/2,im.rows);
        line(orig, p1, p2, Scalar(0, 0, 255), 1);
        skew=90;
    } else {
        double m = -cos(theta)/sin(theta);
        double b = rho/sin(theta) + im.rows/2.-m * im.cols/2.;
        Point2d p1 = Point2d(0,b);
        Point2d p2 = Point2d(im.cols, im.cols * m + b);
        line(orig, p1, p2, Scalar(0,0,255), 1);
        double skewangle;
        skewangle= (p1.x - p2.x) > 0 ? (atan2(p1.y-p2.y,p1.x-p2.x)*180./CV_PI):(atan2(p2.y-p1.y,p2.x-p1.x)*180./CV_PI);
        skew=skewangle;
    }
    return skew;
}


/*
    Applies cv::adaptiveThreshold transforming a grayscale image to binary image

    for T(x,y) being threshold for each pixel, THRESH_BINARY holds

    dst(x,y) = |---    maxValue if src(x,y) > T(x,y)
    |---    0 otherwise

    for adaptive method CV_ADAPTIVE_THRESH_GUASSIAN_C, T(x,y) is a weighted-sum (cross-correlation with a Guassian window)
    of blockSize X blockSize neighbourhood of (x,y) minuc C

    where
    blockSize: size of pixel neighborhood (15)
    C: constant subtracted from mean or weighted mean (-2)
 */

Mat cv_ah_functions::mat_normalize (Mat& input) {
    Mat thresh = input.clone();
    cv::adaptiveThreshold(thresh, thresh, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 15, -2);

    // single-channel unsigned 0 matrix
    Mat norm = Mat::zeros(input.size(), CV_8UC1);
    int x, y;

    for(x = 1 ; x < thresh.cols - 1  ; x++) {
        for(y = 1 ; y < thresh.rows - 1 ; y++) {
            bool b1 = thresh.at<uchar>(y-1, x) == 0 ||  thresh.at<uchar>(y-1, x-1) == 0 || thresh.at<uchar>(y-1, x+1) == 0;
            bool b2 = thresh.at<uchar>(y+1, x) == 0 ||  thresh.at<uchar>(y+1, x-1) == 0 || thresh.at<uchar>(y+1, x+1) == 0;

            uchar pix=thresh.at<uchar>(y, x);
            if((!b1 && pix == 255 && b2))
                norm.at<uchar>(y, x) = 255;
        }
    }
    return norm;
}

Mat cv_ah_functions::rotate_image(Mat& im,double thetaRad) {
    cv::Mat rotated;
    double nw = abs(sin(thetaRad))*im.rows+abs(cos(thetaRad))*im.cols;
    double nh = abs(cos(thetaRad))*im.rows+abs(sin(thetaRad))*im.cols;
    cv::Mat rot_mat = cv::getRotationMatrix2D(Point2d(nw*.5,nh*.5), thetaRad*180/CV_PI, 1);
    Mat pos = Mat::zeros(Size(1,3),CV_64FC1);
    pos.at<double>(0)=(nw-im.cols)*.5;
    pos.at<double>(1)=(nh-im.rows)*.5;
    Mat res = rot_mat*pos;
    rot_mat.at<double>(0,2) += res.at<double>(0);
    rot_mat.at<double>(1,2) += res.at<double>(1);
    cv::warpAffine(im, rotated, rot_mat,Size(nw,nh), cv::INTER_LANCZOS4);
    return rotated;
}


int cv_ah_functions::deskew_image(const char *image_path, char* dump_image) {
    Mat gray;
    Mat input_image = imread(image_path, 1);
    cv::cvtColor(input_image, gray, CV_BGR2GRAY);
    cv::threshold(gray, gray, 230, 255, cv::THRESH_OTSU);

    Mat norm_image = cv_ah_functions::mat_normalize(gray);
    double gradient = cv_ah_functions::compute_skew(norm_image, input_image);

    if (0 != gradient) {
        fprintf(stdout, "image %s skewed by %f\n", image_path, gradient);
        Mat deskewed = cv_ah_functions::rotate_image(input_image, (gradient * CV_PI/180));
        cv::imwrite(dump_image, deskewed);
        return -1;
    }
    return 0;
}
