#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

class MarkArea {
public:
	MarkArea();
	cv::Point leftButtonClick(cv::Mat frame, cv::Point pt);
	void rightButtonClick(cv::Mat frame, cv::Point pt, cv::Point leftClickPt);

	cv::vector<cv::Rect> allmarkedAreaRect;

private:
	bool leftBeforeRight;
	cv::Point leftClickPtr;
	int leftClickX, leftClickY, rightClickX, rightClickY;
	cv::Rect markedAreaRect;
	int localWidth, localHeight;
	cv::Mat imageClone;
	char newKey;

};