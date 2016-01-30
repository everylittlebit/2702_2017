#include "2702_proc.h"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

using namespace std;

Mat templ;
// does a template search!
pos temple(Mat img, int* args)
{
    if(templ.empty())
    {
        templ = imread( "../opencv_lib/refrence.png", 1 );
        resize(templ, templ, Size(16,16));
    }
    else
    {
        ///don't do anything
    }


    /// Create the result matrix
    Mat result;
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create( result_rows, result_cols, CV_32FC1 );

    /// Do the Matching and Normalize
    int match_method = args ? args[0] : 0;
    if (match_method <=  CV_TM_SQDIFF)
    {
        match_method = CV_TM_SQDIFF;
    }
    else if(match_method >= CV_TM_CCOEFF_NORMED)
    {
        match_method = CV_TM_CCOEFF_NORMED;
    }

    matchTemplate( img, templ, result, match_method );
    //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    {
        matchLoc = minLoc;
    }
    else
    {
        matchLoc = maxLoc;
    }

    if(args)
    {
        Mat draw = result.clone();
        circle(draw, matchLoc, 20, Scalar(0, 0, 0));
        circle(draw, matchLoc, 19, Scalar(255, 255, 255));
        imshow("window", draw);

        Mat draw2 = img.clone();
        circle(draw2, matchLoc, 20, Scalar(0, 0, 0));
        circle(draw2, matchLoc, 19, Scalar(255, 255, 255));

        imshow("window2", draw2);
    }

    pos temp;
    if (minVal > 4000000)
    {
        temp.x = -1;
        temp.y = -1;
    }
    else
    {
        temp.x = matchLoc.x;
        temp.y = matchLoc.y;
    }
    temp.minVal = minVal;
    temp.maxVal = maxVal;
    //cout << temp.x << "," <<temp.y << endl;
    cout << (int)result.at<uchar>(temp.x,temp.y) << "," << (long)minVal << "," << (long)maxVal << endl;
    return temp;
}


// part of HSV_Convert
pos fixImage (Mat& inputImage, Mat& outputImage)
{
    int erodeValue = 3;
    int dilatevalue = 3;

    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(erodeValue,erodeValue));
    Mat dilateElement = getStructuringElement(MORPH_RECT, Size(dilatevalue,dilatevalue));

    dilate(inputImage, inputImage, dilateElement);
    erode(inputImage, inputImage, erodeElement);
    erode(inputImage, outputImage, erodeElement);
}
pos HSV_convert(Mat img, int* args)
{
    Mat hsvconvert;
    Mat hsvconvert2;
    cvtColor(img, hsvconvert, CV_BGR2HSV);
    Mat outputImage;
    if (args)
    {
        inRange(hsvconvert, Scalar(args[0], args[1], args[2]), Scalar(args[3], 255, 255), hsvconvert2);
        fixImage(hsvconvert2, outputImage);
        imshow("window", outputImage);
    }
    else
    {
        inRange(hsvconvert, Scalar(54, 89, 32), Scalar(91, 255, 255), hsvconvert2);
        fixImage(hsvconvert2, outputImage);
    }
    Moments center = moments(outputImage, true);
    pos ret;
    ret.x = center.m10/center.m00;
    ret.y = center.m01/center.m00;
    cout << ret.x << "," << ret.y << endl;
    return ret;

}

pos process(Mat img, int* args)
{
    return temple(img, args);
    //return HSV_convert(img, args);
}
