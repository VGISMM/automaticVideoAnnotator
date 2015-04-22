#include "ourCamShift.h"
ourCamShift::ourCamShift(){}

void ourCamShift::trackTheObject(cv::Mat currentFrame, cv::Mat previousFrame, cv::Rect prevTrackingWindow){
    const int channels[] = {1, 2};
    float range[] = { 0, 256 };
    const float *ranges[] = { range, range };
    const int histSize[] = { 256,256 };

    cvtColor(currentFrame, luvCurFrame, CV_BGR2Luv);
    cvtColor(previousFrame, luvPrevFrame, CV_BGR2Luv);

    roiWindow = luvPrevFrame(prevTrackingWindow);

    calcHist(&roiWindow, 1, channels, cv::noArray(), hist, 2, histSize, ranges, true, false);
    normalize(hist, hist, 0, 255, cv::NORM_MINMAX);

    calcBackProject(&luvCurFrame, 1, channels, hist, backProject, ranges);
    cv::Mat sel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
    dilate(backProject, backProject, sel, cv::Point(-1, -1), 2, 1, 3);
    morphologyEx( backProject, backProject, cv::MORPH_CLOSE, sel, cv::Point(-1,-1), 1 );
    //morphologyEx( backProject, backProject, cv::MORPH_OPEN, sel, cv::Point(-1,-1), 1 );
    //threshold( backProject, backProject, 50, 255, 0 );

    cv::TermCriteria criteria(cv::TermCriteria::COUNT + 
                                  cv::TermCriteria::EPS, 
                                  20, 0.01);
    camShiftRect = prevTrackingWindow;
    cv::CamShift(backProject, camShiftRect, criteria);
    //std::cout << "CAMSHIFT : " << camShiftRect << std::endl;

    meanshiftwindow = prevTrackingWindow;
    cv::meanShift(backProject,meanshiftwindow,criteria);
    //std::cout << "MEANSHIFT: " << meanshiftwindow << std::endl;
    
    if ( (camShiftRect.width > 0) && (camShiftRect.width < 30) && (camShiftRect.height < 30) && (camShiftRect.height > 0) )
    {
        boolColor = determineContainerContent(currentFrame, camShiftRect);
        findMaxSeedPoint(luvCurFrame,camShiftRect,boolColor);
        //std::cout << "CAMSHIFT" << std::endl;
        ffCamShiftRect = camShiftRect;
        findFloodFillRect(currentFrame,maxSeedPoint,false);
    }
    else if ( (meanshiftwindow.width > 0) && (meanshiftwindow.width < 30) && (meanshiftwindow.height < 30) && (meanshiftwindow.height > 0))
    {
        boolColor = determineContainerContent(currentFrame, meanshiftwindow);
        findMaxSeedPoint(luvCurFrame,meanshiftwindow,boolColor);
        //std::cout << "MEANSHIFT" << std::endl;
        ffCamShiftRect = meanshiftwindow;
        findFloodFillRect(currentFrame,maxSeedPoint,false);
    }
    else
    {
        //findMaxSeedPoint(luvCurFrame,camShiftRect,boolColor);
        ffCamShiftRect = prevTrackingWindow;
        //findFloodFillRect(currentFrame,maxSeedPoint,true);
    }
}

void ourCamShift::findMaxSeedPoint(cv::Mat frame, cv::Rect window, bool returnedColor){
    int greenL = 0;
    int greenU = 255;
    int greenV = 255;

    int redL = 0;
    int redU = 0;
    int redV = 0;

    for (int i = window.x; i < (window.x+window.width); ++i)
    {
        for (int j = window.y; j < (window.y+window.height); ++j)
        {
            //std::cout << "Looking for MAX val in camshiftRect: (" << i << ","<< j<<"), " << "Value: " <<  luvCurFrame.at<cv::Vec3b>(j,i) << std::endl; 
            if (color == 1)
            {
                /* code */
            
               if ( (frame.at<cv::Vec3b>(j,i)[0]>greenL) && (frame.at<cv::Vec3b>(j,i)[1]<greenU) )
                {
                 
                    //std::cout << "Green - Looking for MAX val in camshiftRect: (" << i << ","<< j<<"), " << "Value: " <<  frame.at<cv::Vec3b>(j,i) << std::endl; 
                     greenL = frame.at<cv::Vec3b>(j,i)[0];
                     greenU = frame.at<cv::Vec3b>(j,i)[1];
                     greenV = frame.at<cv::Vec3b>(j,i)[2];
                     maxSeedPoint.x = i;
                     maxSeedPoint.y = j;
                } 
            }
            else if(color == 0){
                if ( (frame.at<cv::Vec3b>(j,i)[1]>redU) && (frame.at<cv::Vec3b>(j,i)[2]>redV) )
                {
                 
                    //std::cout << "Red - Looking for MAX val in camshiftRect: (" << i << ","<< j<<"), " << "Value: " <<  frame.at<cv::Vec3b>(j,i) << std::endl; 
                     redL = frame.at<cv::Vec3b>(j,i)[0];
                     redU = frame.at<cv::Vec3b>(j,i)[1];
                     redV = frame.at<cv::Vec3b>(j,i)[2];
                     maxSeedPoint.x = i;
                     maxSeedPoint.y = j;
                }
            }   
        }
    }
}

bool ourCamShift::determineContainerContent(cv::Mat frame, cv::Rect window){
    int counterUnder = 0;
    int counterOver = 0;
    int thresholdL = 90;
    int thresholdU = 90;

    for (int i = camShiftRect.x; i < (camShiftRect.x+camShiftRect.width); ++i)
    {
        for (int j = camShiftRect.y; j < (camShiftRect.y+camShiftRect.height); ++j)
        {
            if ( (luvCurFrame.at<cv::Vec3b>(j,i)[0]>thresholdL) && (luvCurFrame.at<cv::Vec3b>(j,i)[1]>thresholdU) )
            {
                // red
                ++counterOver;
            }
            if ( (luvCurFrame.at<cv::Vec3b>(j,i)[0]>thresholdL) && (luvCurFrame.at<cv::Vec3b>(j,i)[1]<thresholdU) )
            {
                //green
                ++counterUnder;
            }
        }
    }
    //std::cout << "Counts on red: " << counterOver << " Counts on green: " << counterUnder << std::endl;
    if (counterOver > counterUnder)
    {
        // red
        color = 0;
    }
    else if(counterOver < counterUnder)
    {
        color = 1;
    }
    return color;
}

void ourCamShift::findFloodFillRect(cv::Mat frame, cv::Point pt, bool overrule){
    ffImage = frame.clone();
    cv::Rect ffCamShiftFloodRect;
    cv::floodFill(ffImage,pt,0, &ffCamShiftFloodRect,cvScalarAll(60), cvScalarAll(60), 8 + (255 << 8)+cv::FLOODFILL_FIXED_RANGE);

    if ((ffCamShiftFloodRect.width > 0) && (ffCamShiftFloodRect.width < 30) && (ffCamShiftFloodRect.height > 0) && (ffCamShiftFloodRect.height < 30))
    {
        ffCamShiftRect = ffCamShiftFloodRect;
    } else if(overrule)
    {
        ffCamShiftRect = ffCamShiftFloodRect;
    }

    if(ffCamShiftRect.width < 4)
    {
        ffCamShiftRect.width = ffCamShiftRect.width+1;
        ffCamShiftRect.x = ffCamShiftRect.x-1;
    }
    if(ffCamShiftRect.height < 4)
    {
        ffCamShiftRect.height = ffCamShiftRect.height+1;
        ffCamShiftRect.y = ffCamShiftRect.y-1;
    }

}