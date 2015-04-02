#include <stdio.h>
#include <iostream>
#include <time.h>
#include <opencv2/opencv.hpp>



class FloodFill {
public:
	FloodFill();
	void findFloodFillRect(cv::Mat frame, cv::Point pt);	
	cv::vector<cv::Rect> allFloodFills;
	bool areWeDoneHere=false;
	
private:
	void init();
	void createZoomAreaFrame(cv::Mat frame, cv::Point pt);
	cv::Mat imageClone;
	cv::Rect floodFillRect;
	cv::Mat zoomAreaFrame;
	char c;

	int zoomAreaSize;
	cv::Point minPoint,maxPoint, pointMinBoundray,pointMaxBoundray;
	int resizeNumber;
};