#include <stdio.h>
#include <iostream>
#include <time.h>
#include <opencv2/opencv.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>


class ourCamShift {
public:
	ourCamShift();
  	void trackTheObject(cv::Mat currentFrame, cv::Mat previousFrame, cv::Rect prevTrackingWindow);

  	cv::Point maxSeedPoint;
  	cv::Rect ffCamShiftRect;

  	//void init();
private:
	void findFloodFillRect(cv::Mat frame, cv::Point pt, bool overrule);
	bool determineContainerContent(cv::Mat frame, cv::Rect window);
	void findMaxSeedPoint(cv::Mat frame, cv::Rect window, bool returnedColor);
	cv::Mat luvFrame,mask; 
   	cv::Mat hist;

   	cv::Mat luvCurFrame, luvPrevFrame;
   	cv::Mat ffImage;

   	float angle;
    cv::Mat M, invM, rotated, cropped;
    cv::Size rect_size;
    cv::Point p;
    cv::Rect mean_shift_window;
    cv::Mat roiWindow;
  	cv::Mat backProject;
  	cv::RotatedRect trackBox;
  	cv::Rect camShiftRect;
  	cv::Rect meanshiftwindow;

    bool color;
    bool boolColor;
   
};