#include "floodFill.h"

FloodFill::FloodFill(){}

void FloodFill::findFloodFillRect(cv::Mat frame, cv::Point pt){
	if (areWeDoneHere==false)
	{	
		imageClone = frame.clone();
		cv::floodFill(imageClone,pt,0, &floodFillRect,cvScalarAll(60), cvScalarAll(60), 8 + (255 << 8)+cv::FLOODFILL_FIXED_RANGE);
		
		if ( (floodFillRect.width < 1) || (floodFillRect.width > 30) || (floodFillRect.height > 30) || (floodFillRect.height < 1) )
    	{
    		floodFillRect = cv::Rect(cv::Point(pt.x-2, pt.y-2),cv::Point(pt.x+2, pt.y+2));
    	}
		cv::rectangle(frame,cv::Point(floodFillRect.x, floodFillRect.y),cv::Point(floodFillRect.x+(floodFillRect.width), floodFillRect.y+(floodFillRect.height)), cv::Scalar(255, 255, 0),1,1);
		
		createZoomAreaFrame(frame,pt);
		imshow("Main Window",frame);		

		c = (char)cv::waitKey();
	    if (c=='s')
	    {
	    	if(floodFillRect.width < 4)
	    	{
	    		floodFillRect.width = floodFillRect.width+1;
	    		floodFillRect.x = floodFillRect.x-1;
	    	}
	    	if(floodFillRect.height < 4)
	    	{
	    		floodFillRect.height = floodFillRect.height+1;
	    		floodFillRect.y = floodFillRect.y-1;
	    	}
	     	allFloodFills.push_back(floodFillRect);
	    }
	    else if(c == 'i' || c == 'z'){
	    	cv::rectangle(frame,cv::Point(floodFillRect.x, floodFillRect.y),cv::Point(floodFillRect.x+(floodFillRect.width), floodFillRect.y+(floodFillRect.height)), cv::Scalar(0, 0, 0),1,1);
	    }
    }
}

void FloodFill::init(){
	zoomAreaSize = 50;
	resizeNumber = 7;
	pointMinBoundray.x = 0;
	pointMinBoundray.y = 0;
	pointMaxBoundray.x = 1280;
	pointMaxBoundray.y = 960;
	zoomAreaFrame = cv::Mat::zeros(zoomAreaSize,zoomAreaSize,CV_8UC3);
}

void FloodFill::createZoomAreaFrame(cv::Mat frame, cv::Point pt){
	init();
	minPoint.x = pt.x-(zoomAreaSize/2);
	minPoint.y = pt.y-(zoomAreaSize/2);
	maxPoint.x = pt.x+(zoomAreaSize/2);
	maxPoint.y = pt.y+(zoomAreaSize/2);

	if((minPoint.x > pointMinBoundray.x) && (minPoint.y > pointMinBoundray.y) && (maxPoint.x < pointMaxBoundray.x) && (maxPoint.y < pointMaxBoundray.y))
	{
	    zoomAreaFrame = frame(cv::Rect(minPoint, cv::Size(zoomAreaSize, zoomAreaSize)));
	    resize(zoomAreaFrame, zoomAreaFrame, cv::Size(), resizeNumber, resizeNumber, CV_INTER_LINEAR);
  	}
  	cv::imshow("Zoom Area", zoomAreaFrame);
}
