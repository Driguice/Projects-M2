#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


using namespace cv;
using namespace std;

std::vector<cv::Point2f> get_positions(cv::Mat& image)
{
    if (image.channels() > 1)
    {
        std::cout << "get_positions: !!! Input image must have a single channel" << std::endl;
        return std::vector<cv::Point2f>();
    }

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(image, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // Approximate contours to polygons and then get the center of the objects
    std::vector<std::vector<cv::Point> > contours_poly(contours.size());
   
    std::vector<cv::Point2f> center(contours.size());
    std::vector<float> radius(contours.size());
    for (unsigned int i = 0; i < contours.size(); i++ )
    {
        cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 5, true );
        cv::minEnclosingCircle((cv::Mat)contours_poly[i], center[i], radius[i]);
    }

    return center;
}  


int main()
{


  Mat frame;//Declaring a matrix to load the frames//
   namedWindow("Video Player");//Declaring the video to show the video//
  
   VideoCapture cap("rubix.mp4");//Declaring an object to load video from device//  
   
   if(!cap.isOpened()){ //This section prompt an error message if no video stream is found//
      cout << "No video stream detected" << endl;
      system("pause");
      return-1;
   }

   
   while (true){ //Taking an everlasting loop to show the video//
      cap >> frame;
      int current_Frame;//Declaring an integer variable to store the position of the current frame//
      current_Frame = cap.get(CAP_PROP_POS_FRAMES);//Reading the position of current frame//
      if (frame.empty()){ //Breaking the loop if no video frame is detected//
         break;
      }
      cout << "Current Frame Number:" << current_Frame << endl;
      imshow("Video Player", frame);//Showing the video//
      char c = (char)waitKey(25);//Allowing 25 milliseconds frame processing time and initiating break condition//
      if (c == 27){ //If 'Esc' is entered break the loop//
         break;
      }
   
   

   
	 
	 //  Create a few window titles for debugging purposes
	 std::string wnd1 = "Input", wnd2 = "Red Objs", wnd3 = "Output";   

	 // These are the HSV values used later to isolate RED-ish colors
	 int low_h = 160, low_s = 140, low_v = 50;
	 int high_h = 179, high_s = 255, high_v = 255;

	 cv::Mat hsv_frame, red_objs;
	 cv::Mat orig_frame = frame.clone();
	 cv::imshow(wnd1, orig_frame);
        
         // Convert BGR frame to HSV to be easier to separate the colors
        cv::cvtColor(frame, hsv_frame, COLOR_BGR2HSV);

        // Isolate red colored objects and save them in a binary image
        cv::inRange(hsv_frame,
                    cv::Scalar(low_h,  low_s,  low_v),
                    cv::Scalar(high_h, high_s, high_v),
                    red_objs);

        // Remove really small objects (mostly noises)
        cv::erode(red_objs, red_objs, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
        cv::dilate(red_objs, red_objs, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7)));

        cv::Mat objs = red_objs.clone();
        int largest_area=0;
        int largest_contour_index=0;
        Rect bounding_rect;
        vector<vector<Point> > contours; // Vector for storing contours
			
		findContours( objs , contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE ); // Find the contours in the image
	   	for( size_t i = 0; i< contours.size(); i++ ) // iterate through each contour.
	    		{	 
		       	 double area = contourArea( contours[i] );  //  Find the area of contour
				 if( area> largest_area)
					{
					  	 largest_area = area;
					    	 largest_contour_index = i;               //Store the index of largest contour
					    	 bounding_rect = boundingRect( contours[i] ); // Find the bounding rectangle for biggest contour
			    	
		       		}
		        
	       	}
	       	drawContours( frame, contours,largest_contour_index, Scalar( 0, 255, 0 ), 2 ); // Draw the largest contour using previously stored index.
        cv::imshow(wnd2, objs);
          // Retrieve a vector of points with the (x,y) location of the objects
        std::vector<cv::Point2f> points = get_positions(objs);
        
	 
        // Draw a small green circle at those locations for educational purposes
        for (unsigned int i = 0; i < points.size(); i++)
            cv::circle(frame, points[i], 3, cv::Scalar(0, 255, 0), -1, 8, 0);
            	
    	cv::imshow(wnd3, frame);
         char key = cv::waitKey(33);
        if (key == 27) {   /* ESC was pressed */
            //cv::imwrite("out1.png", orig_frame);
            //cv::imwrite("out2.png", red_objs);
            //cv::imwrite("out3.png", frame);
            break;
        }
     
    }
   
    return 0;
}
