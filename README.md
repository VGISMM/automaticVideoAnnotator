# automaticVideoAnnotator
Tool developed for annotating rectangles in a video sequence.

Features:

* Manually select area with mouse using left click followed by right click to mark upper left and lower right corner of the desired area.

* Find a rect based on floodfilling using seedpoint selected by mouse cursor triggerd by middle click. 

* Dedicated zoom area window based on cursor current position in main window.

* Uses CAMSHIFT (on u and v channels in LUV color spaces) to track a selected area through frames. 

* Saves video sequences into frames in syntax [framenumer].png 

* Save annotations to textfile with syntax: "[Framenumber] [number of annotation in frame] [Rect 1 - upper left corner x cordinate] [Rect 1 - upper left corner y cordinate] [Rect 1 - width] [Rect 1 - height] [Rect 2 - ...]"
* 

std::cout << "\nThis is a tool developed for annotating rectangles in a video sequence.\n"
            "You select an area by clicking left in the frame where you wish the top left corner of the rect to be.\n"
            "This is follwed by a right click for lower right corner of the rect.\n"
            "Additionally, the middle button can be used in the center of a color to use floodfill to create a rectangle around it.\n"

            "Usage: \n"
            "   ./annotateVideo [Direction to video to annotate] [Start annotation number]\n";

    std::cout << "\n\nHot keys: \n"
            "\tESC or q- quit the program\n"
            "\ts - Save annotated and Apply CAMSHIFT on previous rects in current frame. \n"
            "\tz or i - If area is not good. 'z' or 'i' can be used for ignoring it.\n"
            "\td - Save to file, and proceed to next frame. \n";
