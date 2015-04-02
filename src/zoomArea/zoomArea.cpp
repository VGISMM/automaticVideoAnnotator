#include "zoomArea.h"

ZoomArea::ZoomArea(){}
void ZoomArea::init(){
	zoomAreaSize = 50;
	resizeNumber = 7;
	pointMinBoundray.x = 0;
	pointMinBoundray.y = 0;
	pointMaxBoundray.x = 1280;
	pointMaxBoundray.y = 960;

	zoomAreaFrame = cv::Mat::zeros(zoomAreaSize,zoomAreaSize,CV_8UC3);
}

/* Create the resized */
void ZoomArea::createZoomAreaFrame(cv::Mat frame, cv::Point pt){
	minPoint.x = pt.x-(zoomAreaSize/2);
	minPoint.y = pt.y-(zoomAreaSize/2);
	maxPoint.x = pt.x+(zoomAreaSize/2);
	maxPoint.y = pt.y+(zoomAreaSize/2);

	if((minPoint.x > pointMinBoundray.x) && (minPoint.y > pointMinBoundray.y) && (maxPoint.x < pointMaxBoundray.x) && (maxPoint.y < pointMaxBoundray.y))
	{
		inputImageClone = frame.clone();
		//std::cout << "point pointer from zoomArea class: " << minPoint << std::endl;
	    zoomAreaFrame = inputImageClone(cv::Rect(minPoint, cv::Size(zoomAreaSize, zoomAreaSize)));
	    circle(inputImageClone, pt,1, cv::Scalar(0,255,255),1, 8,2);
	    resize(zoomAreaFrame, zoomAreaFrame, cv::Size(), resizeNumber, resizeNumber, CV_INTER_LINEAR);
  	}
  	cv::imshow("Zoom Area", zoomAreaFrame);
  	cv::moveWindow("Zoom Area",0,0);
}