#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

class ZoomArea {
public:
	ZoomArea();
	void init();
	void createZoomAreaFrame(cv::Mat frame, cv::Point pt);
	cv::Mat zoomAreaFrame;
private:
	int zoomAreaSize;
	cv::Mat inputImageClone;
	cv::Point minPoint,maxPoint, pointMinBoundray,pointMaxBoundray;
	int resizeNumber;

};