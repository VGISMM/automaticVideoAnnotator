#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <sstream>
#include <string.h>

/* Incude classes */
#include "zoomArea/zoomArea.h"
#include "markArea/markArea.h"
#include "floodFill/floodFill.h"
#include "camShift/ourCamShift.h"

//using namespace cv;
//using namespace std;

#define IMAGEWIDTH 1280
#define IMAGEHEIGHT 960
#define largestObjectWIDTH 200
#define largestObjectHEIGHT 200

cv::Mat image;



bool leftBeforeRight = false;
cv::Mat imgLOI,myOrgImageClone, previousImage;

cv::vector<cv::Rect> currentAnnotiationRects;
cv::vector<cv::Rect> previousAnnotiationRects;

int frameNumber;
cv::string annotationsPath;
std::ofstream myfile;
cv::string pngPath;

char annotationString[100];
char finalannotationString[200];
char previousAnnotationString[100];
cv::vector<cv::string> annotationStringVector; 
cv::vector<cv::string> previousAnnotationStringVector; 


int leftClickX, leftClickY, rightClickX, rightClickY;
cv::Point leftClickPtr;

static void onMouse( int event, int x, int y, int, void *ptr)
{
    ZoomArea ZoomArea;
    FloodFill FloodFill;
    MarkArea MarkArea;

    ZoomArea.init();
    cv::Point *p = (cv::Point*)ptr;
    p->x = x;
    p->y = y;

    
    
    if(event == cv::EVENT_LBUTTONDOWN && (leftBeforeRight==false))
    {
        leftClickPtr = MarkArea.leftButtonClick(imgLOI,*p);
        leftBeforeRight = true;

    }

    else if((event == cv::EVENT_RBUTTONDOWN) && (leftBeforeRight==true) ) {
        MarkArea.rightButtonClick(imgLOI,*p,leftClickPtr);
        leftBeforeRight = false;
        if (MarkArea.allmarkedAreaRect.size())
        {
            currentAnnotiationRects.push_back(MarkArea.allmarkedAreaRect[0]);
            
            cv::rectangle(imgLOI,cv::Point(MarkArea.allmarkedAreaRect[0].x, MarkArea.allmarkedAreaRect[0].y),cv::Point(MarkArea.allmarkedAreaRect[0].x+(MarkArea.allmarkedAreaRect[0].width), MarkArea.allmarkedAreaRect[0].y+(MarkArea.allmarkedAreaRect[0].height)), cv::Scalar(0, 255, 0),1,1);
        }
        cv::imshow("Main Window", imgLOI);
    }

    if(event == cv::EVENT_MBUTTONDOWN){
        if (FloodFill.areWeDoneHere==false)
        {   
            FloodFill.findFloodFillRect(imgLOI,*p);

            if (FloodFill.allFloodFills.size())
            {
                currentAnnotiationRects.push_back(FloodFill.allFloodFills[0]);
                
                cv::rectangle(imgLOI,cv::Point(FloodFill.allFloodFills[0].x, FloodFill.allFloodFills[0].y),cv::Point(FloodFill.allFloodFills[0].x+(FloodFill.allFloodFills[0].width), FloodFill.allFloodFills[0].y+(FloodFill.allFloodFills[0].height)), cv::Scalar(0, 255, 0),1,1);
            }

            cv::imshow("Main Window", imgLOI);
            
        }
    }
    ZoomArea.createZoomAreaFrame(imgLOI,*p);
    
}

static void help()
{
    std::cout << "\nThis is a tool developed for annotating rectangles in a video sequence.\n"
            "You select an area by clicking left in the frame where you wish the top left corner of the rect to be.\n"
            "This is follwed by a right click for lower right corner of the rect.\n"
            "Additionally, the middle button can be used in the center of a color to use floodfill to create a rectangle around it.\n"

            "Usage: \n"
            "   ./annotateVideo [Direction to video to annotate] [Start annotation number]\n";

    std::cout << "\n\nHot keys: \n"
            "\tESC or q- quit the program\n"
            "\ta - Apply CAMSHIFT on previous rects in current frame. \n"
            "\ts - Save annotated \n"
            "\ti - If area is not good. 'i' can be used for ignoring it.\n"
            "\td - Save to file, and proceed to next frame. \n";
}

const char* keys =
{
    "{1|  | 0 | camera number}"
};

int main( int argc, const char** argv )
{
    help();
    cv::VideoCapture capture(argv[1]);
    std::cout << "Naming frames from: " << argv[2] << std::endl;
    frameNumber = atoi(argv[2]);
    std::stringstream annotationFilename;
    annotationFilename << "outputAnnotations/annotationsFrom" << frameNumber << ".txt";
    annotationsPath = annotationFilename.str();
    myfile.open(annotationsPath);

    int hsize = 16;
    float hranges[] = {0,180};
    const float* phranges = hranges;
    cv::CommandLineParser parser(argc, argv, keys);

    cv::namedWindow( "Main Window", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL );
    cv::Point mousePtr;
    cv::Point prevMousePtr;
    

    cv::Mat frame, hsv, hue, mask, hist, histimg = cv::Mat::zeros(200, 320, CV_8UC3), backproj;
    
    ZoomArea ZoomArea;
    ourCamShift ourCamShift;
    FloodFill FloodFill;
    bool paused = false;
    
    while(1)
    {

        currentAnnotiationRects.clear();
        capture >> image;

        imgLOI = image(cv::Rect(cv::Point(0,0), cv::Size(IMAGEWIDTH, IMAGEHEIGHT)));
        //if fail to read the image
        if (imgLOI.empty()) 
        { 
            std::cout << "Error loading the image" << std::endl;
            return -1; 
        }
        myOrgImageClone = imgLOI.clone();

        setMouseCallback( "Main Window", onMouse, &mousePtr);
        imshow( "Main Window", imgLOI );
        


        char k = cv::waitKey();

        if ( (k == 'q') || (k == 27) ) // push q or escape to quit.
        {
            break;
        }
        std::stringstream pngfilename;
        if ( k == 'd')
        {
            pngfilename << "outputAnnotations/" << frameNumber << ".png";
            pngPath = pngfilename.str();
            imwrite(pngPath,myOrgImageClone);
            std::cout << "Frame: " << frameNumber << "saved!" << std::endl;
            

            for (int i = 0; i < currentAnnotiationRects.size() ; ++i)
            {
                previousAnnotiationRects.push_back(currentAnnotiationRects[i]);
            }

        }
        else if ( k == 'a') {

            pngfilename << "outputAnnotations/" << frameNumber << ".png";
            pngPath = pngfilename.str();
            imwrite(pngPath,myOrgImageClone);

            std::cout << "Frame: " << frameNumber << "saved!" << std::endl;

            for (int i = 0; i < previousAnnotiationRects.size(); ++i)
            {
                ourCamShift.trackTheObject(myOrgImageClone.clone(),previousImage,previousAnnotiationRects[i]);

                //Draw white "candidate" rectangle, which later can be stored with 's' or ignored with 'i'
                cv::rectangle(imgLOI,cv::Point(ourCamShift.ffCamShiftRect.x, ourCamShift.ffCamShiftRect.y),cv::Point(ourCamShift.ffCamShiftRect.x+(ourCamShift.ffCamShiftRect.width), ourCamShift.ffCamShiftRect.y+(ourCamShift.ffCamShiftRect.height)), cv::Scalar(255, 255, 255),1,1);
            }

            for (int i = 0; i < previousAnnotiationRects.size(); ++i)
            {
                ourCamShift.trackTheObject(myOrgImageClone.clone(),previousImage,previousAnnotiationRects[i]);
                // If camshift results x point start in (0,0), a more bold rectangle is drawn fore more attention. Else, we draw a purple rectangle to indicate which rectangle the user must decide whether to keep or ignore.
                if ( (ourCamShift.ffCamShiftRect.x==0) && (ourCamShift.ffCamShiftRect.y==0) )
                {
                    cv::rectangle(imgLOI,cv::Point(ourCamShift.ffCamShiftRect.x, ourCamShift.ffCamShiftRect.y),cv::Point(ourCamShift.ffCamShiftRect.x+(ourCamShift.ffCamShiftRect.width), ourCamShift.ffCamShiftRect.y+(ourCamShift.ffCamShiftRect.height)), cv::Scalar(255, 0, 255),5,1);
                }else{
                    cv::rectangle(imgLOI,cv::Point(ourCamShift.ffCamShiftRect.x, ourCamShift.ffCamShiftRect.y),cv::Point(ourCamShift.ffCamShiftRect.x+(ourCamShift.ffCamShiftRect.width), ourCamShift.ffCamShiftRect.y+(ourCamShift.ffCamShiftRect.height)), cv::Scalar(255, 0, 255),1,1);
                
                }

                imshow( "Main Window", imgLOI );

                char newk = cv::waitKey();
                if (newk == 's')
                {
                    cv::rectangle(imgLOI,cv::Point(ourCamShift.ffCamShiftRect.x, ourCamShift.ffCamShiftRect.y),cv::Point(ourCamShift.ffCamShiftRect.x+(ourCamShift.ffCamShiftRect.width), ourCamShift.ffCamShiftRect.y+(ourCamShift.ffCamShiftRect.height)), cv::Scalar(0, 255, 0),1,1);

                    if (ourCamShift.ffCamShiftRect.x==0)
                    {
                        cv::rectangle(imgLOI,cv::Point(ourCamShift.ffCamShiftRect.x, ourCamShift.ffCamShiftRect.y),cv::Point(ourCamShift.ffCamShiftRect.x+(ourCamShift.ffCamShiftRect.width), ourCamShift.ffCamShiftRect.y+(ourCamShift.ffCamShiftRect.height)), cv::Scalar(0, 0, 0),5,1);
                        cv::waitKey();
                        std::cout << "Rect starts in 0, extra waitkey inserted to reannoate." << std::endl;
                    }

                    currentAnnotiationRects.push_back(ourCamShift.ffCamShiftRect);
                                   
                }
                if (newk == 'i'){
                    if (ourCamShift.ffCamShiftRect.x==0)
                    {
                        cv::rectangle(imgLOI,cv::Point(ourCamShift.ffCamShiftRect.x, ourCamShift.ffCamShiftRect.y),cv::Point(ourCamShift.ffCamShiftRect.x+(ourCamShift.ffCamShiftRect.width), ourCamShift.ffCamShiftRect.y+(ourCamShift.ffCamShiftRect.height)), cv::Scalar(0, 0, 0),5,1);
                    }
                    else{
                        cv::rectangle(imgLOI,cv::Point(ourCamShift.ffCamShiftRect.x, ourCamShift.ffCamShiftRect.y),cv::Point(ourCamShift.ffCamShiftRect.x+(ourCamShift.ffCamShiftRect.width), ourCamShift.ffCamShiftRect.y+(ourCamShift.ffCamShiftRect.height)), cv::Scalar(0, 0, 0),1,1);

                    }
                }
                imshow( "Main Window", imgLOI );  
  
            }

            // Guard for prevention skipping to next image.
            char proceed = cv::waitKey();
            if (proceed == 'd')
            {
            }

        }

        previousAnnotiationRects.clear();
        for (int i = 0; i < currentAnnotiationRects.size(); ++i)
        {
            // Guard for protecting agianst a mistakenly saved area larger that 200 x 200.
            if ( (currentAnnotiationRects[i].width < largestObjectWIDTH) && (currentAnnotiationRects[i].height < largestObjectHEIGHT) )
            {
                sprintf(annotationString+strlen(annotationString),"\t%i\t%i\t%i\t%i",currentAnnotiationRects[i].x,currentAnnotiationRects[i].y,currentAnnotiationRects[i].width,currentAnnotiationRects[i].height);
                previousAnnotiationRects.push_back(currentAnnotiationRects[i]);
            }            
        }

        if (previousAnnotiationRects.size())
        {
            sprintf(previousAnnotationString,"%lu%s",previousAnnotiationRects.size(),annotationString);
            sprintf(finalannotationString,"%i\t%lu%s",frameNumber,previousAnnotiationRects.size(),annotationString);
            
            std::cout << "String saved to file: " << finalannotationString << std::endl;

            myfile << finalannotationString << std::endl;
            memset(finalannotationString, 0, sizeof(finalannotationString));
            memset(annotationString, 0, sizeof(annotationString)); 
        }
        

        std::cout << "Frame just annotated was: " << frameNumber << std::endl;
        frameNumber++;
        previousImage = myOrgImageClone.clone();
        currentAnnotiationRects.clear();

    }

    myfile.close();
    return 0;
}
