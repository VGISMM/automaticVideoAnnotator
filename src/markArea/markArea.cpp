#include "markArea.h"

MarkArea::MarkArea(){}


cv::Point MarkArea::leftButtonClick(cv::Mat frame, cv::Point pt){
    leftClickPtr = pt;
    std::cout << "Left Click" << pt << std::endl;
    return pt;
}

void MarkArea::rightButtonClick(cv::Mat frame, cv::Point pt, cv::Point leftClickPt){
	std::cout << "Right Click" << pt << std::endl;
	rightClickX = pt.x;
	rightClickY = pt.y;
	leftClickX = leftClickPt.x;
	leftClickY = leftClickPt.y;
	localWidth = rightClickX - leftClickX;
	localHeight = rightClickY - leftClickY;

	cv::rectangle(frame,cv::Point(leftClickX, leftClickY),cv::Point(rightClickX, rightClickY), cv::Scalar(255, 0, 0),1,1);
	imshow("Main Window",frame);

	newKey = cv::waitKey();

	if (newKey == 's')
	{
		cv::rectangle(frame,cv::Point(leftClickX, leftClickY),cv::Point(rightClickX, rightClickY), cv::Scalar(0, 255, 0),1,1);
		markedAreaRect.x = leftClickX;
		markedAreaRect.y = leftClickY;
		markedAreaRect.width = localWidth;
		markedAreaRect.height = localHeight;

		allmarkedAreaRect.push_back(markedAreaRect);

		imshow("Main Window",frame);
	}

	else if (newKey == 'i')
	{
		cv::rectangle(frame,cv::Point(leftClickX, leftClickY),cv::Point(rightClickX, rightClickY), cv::Scalar(0, 0, 0),1,1);
		imshow("Main Window",frame);
	}
}