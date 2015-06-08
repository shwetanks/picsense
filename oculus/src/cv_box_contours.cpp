#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


#include "opencv2/opencv.hpp"

std::vector<cv::Rect> find_letters(cv::Mat img)
{
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);
    cv::imwrite("thresh_morph.jpg", img_threshold);

    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1);
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
        if (contours[i].size()>100)
        {
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height)
                boundRect.push_back(appRect);
        }
    return boundRect;
}

int main(int argc,char** argv)
{
    //Read
    cv::Mat img1=cv::imread(argv[1]);
    //Detect
    std::vector<cv::Rect> letter_rectangles = find_letters(img1);
    //Display
    for(int i=0; i< letter_rectangles.size(); i++)
        cv::rectangle(img1,letter_rectangles[i],cv::Scalar(0,255,0),3,8,0);
    cv::imwrite( "imgOut1.jpg", img1);
    return 0;
}



int _main_(int argc, char* argv[]) {
    Mat large = imread(argv[1], 1);
    Mat rgb;
    // downsample and use it for processing
    pyrDown(large, rgb);
    Mat small;
    cvtColor(rgb, small, CV_BGR2GRAY);
    // morphological gradient
    Mat grad;
    Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
    // binarize
    Mat bw;
    threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
    // connect horizontally oriented regions
    Mat connected;
    morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
    morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
    // find contours
    Mat mask = Mat::zeros(bw.size(), CV_8UC1);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    // filter contours
    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
    {
        Rect rect = boundingRect(contours[idx]);
        Mat maskROI(mask, rect);
        maskROI = Scalar(0, 0, 0);
        // fill the contour
        drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
        // ratio of non-zero pixels in the filled region
        double r = (double)countNonZero(maskROI)/(rect.width*rect.height);

        if (r > .45 /* assume at least 45% of the area is filled if it contains text */
                &&
                (rect.height > 8 && rect.width > 8) /* constraints on region size */
                /* these two conditions alone are not very robust. better to use something
                   like the number of significant peaks in a horizontal projection as a third condition */
           )
        {
            rectangle(rgb, rect, Scalar(0, 255, 0), 2);
        }
    }
    imwrite(string("box_contours.jpg"), rgb);

    return 0;
}
