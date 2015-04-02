# automaticVideoAnnotator
Tool developed for annotating rectangles in a video sequence.

Features:

* Manually select area with mouse using left click followed by right click to mark upper left and lower right corner of the desired area.

* Find a rect based on floodfilling using seedpoint selected by mouse cursor triggerd by middle click. 

* Dedicated zoom area window based on cursor current position in main window.

* Uses CAMSHIFT (on u and v channels in LUV color spaces) to track a selected area through frames. 

* Saves video sequences into frames in syntax [framenumer].png 

* Save annotations to textfile with syntax: "[Framenumber] [number of annotation in frame] [Rect 1 - upper left corner x cordinate] [Rect 1 - upper left corner y cordinate] [Rect 1 - width] [Rect 1 - height] [Rect 2 - ...]"