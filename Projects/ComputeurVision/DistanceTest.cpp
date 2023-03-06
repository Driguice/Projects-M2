#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


using namespace cv;
using namespace std;


double KnownDistance= 0.40;
double knownWidth= 0.30;
int maxCorners = 4;
const int MAXTRACKBAR = 25;
const std::string source_window = "myImage";
Mat myImage;
Mat thr;
Mat frame;
std::vector<cv::Point2f> corner_pts;

//////////////////////////////////////////////////////////////////////////////Trouver les coins ////////////////////////////////////////////////
																		   //
void trackFeatures( int, void* )														   //
{																		   //						
    maxCorners = MAX(maxCorners, 1);														   //
    std::vector<cv::Point2f> corners;														   //
    double qualityLevel = 0.01;														   //
    double minDistance = 10;
    int blockSize = 3, gradientSize = 3;
    bool useHarrisDetector = true;
    double k = 0.04;

    cv::Mat copy = myImage.clone();

    cv::goodFeaturesToTrack( thr,
                         corners,
                         maxCorners,
                         qualityLevel,
                         minDistance,
                         cv::Mat(),
                         blockSize,
                         gradientSize,
                         useHarrisDetector,
                         k );


    std::cout << "** Number of corners detected: " << corners.size() << "\n";
    int radius = 8;
    for( size_t i = 0; i < corners.size(); i++ )
    {
        std::cout << " -- Original Corner Detected [" << i << "]  (" << corners[i].x << "," << corners[i].y << ")\n" ;
        cv::circle( copy, corners[i], radius, cv::Scalar(0,255,0), cv::FILLED );
    }


    cv::Size winSize =  cv::Size( 5, 5 );
    cv::Size zeroZone = cv::Size( -1, -1 );
    cv::TermCriteria criteria = cv::TermCriteria( cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 40, 0.001 );


    std::vector<cv::Point2f> refinedCorners(corners);
    cv::cornerSubPix( thr, refinedCorners, winSize, zeroZone, criteria );

    for( size_t i = 0; i < refinedCorners.size(); i++ )
    {
        std::cout << " -- Refined Corner [" << i << "]  (" << refinedCorners[i].x << "," << refinedCorners[i].y << ")\n" ;
        cv::circle( copy, refinedCorners[i], radius, cv::Scalar(0,0,255), cv::FILLED );
    }

    cv::namedWindow( source_window.c_str() );
    cv::imshow( source_window, copy );

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//void trackFeatures( int, void* );


//////////////////////Fonction de calcul de la distance ///////////////////////
double distanceCamera (double knownWidth, double focalLength,double perWidth)//
{									       //
	return  (knownWidth * focalLength) / perWidth;                      //
}									      //
                                                                           //
////////////////////////////////////////////////////////////////////////////

double distanceCamera (double knownWidth, double focalLength,double perWidth);



int main( int argc, char** argv )
{
/*
     VideoCapture cap(0);
     
     if(!cap.isOpened())
	{
		return -1;
	}
		
	for(;;) // Boucle d'Acquisition
	{
	cap >> frame; // Obtenir une image de la camera
	//if (i%3){ //réduction de la vitesse
	imshow( "video origin", frame );
	if(waitKey(1) >= 0) break;
	
	if( frame.empty() )
	{
	return -1;
	}
	
	}


	Mat subImg = frame(cv::Range(0, 100), cv::Range(0, 100)); //frame important Pour vidéo
*/


    myImage = imread("/home/drice/distanceTest/dist/dRef.jpg");
    if (myImage.empty())
    {
        cout << "Could not open or find the image!" << endl;
        return -1;
    }
        
    int largest_area=0;
    int largest_contour_index=0;
    Rect bounding_rect;
    
    
    cvtColor( myImage, thr, COLOR_BGR2GRAY ); //Convert to gray
    threshold( thr, thr, 125, 255, THRESH_BINARY ); //Threshold the gray
    vector<vector<Point> > contours; // Vector for storing contours
    //cv::namedWindow( source_window );
    //cv::createTrackbar( "Max corners:", source_window, &maxCorners, MAXTRACKBAR, trackFeatures );

    findContours( thr, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE ); // Find the contours in the image
    for( size_t i = 0; i< contours.size(); i++ ) // iterate through each contour.
    	{		
    		double area = contourArea( contours[i] );  //  Find the area of contour
		if( area > largest_area )
        	{
		  	 largest_area = area;
		    	 largest_contour_index = i;               //Store the index of largest contour
		    	 bounding_rect = boundingRect( contours[i] ); // Find the bounding rectangle for biggest contour
            	}
        }
    
  
 
     std::cout << "La largeur de la feuille de référence en pixel est de : " << bounding_rect.width <<endl;
     
     double focalLength = ( bounding_rect.width*KnownDistance)/knownWidth;
     double inches = distanceCamera (knownWidth, focalLength,bounding_rect.width);
     std::cout << "\n La distance focale de l'objet de référence est de : " << focalLength <<endl;
     //trackFeatures( 0, 0 );
     waitKey();
     drawContours( myImage, contours,largest_contour_index, Scalar( 0, 255, 0 ), 2 ); // Draw the largest contour using previously stored index.
     std::ostringstream str;
		str << "distance : " << inches << "m";
		cv::putText(myImage, //target image
		str.str(), //text
		cv::Point(35, myImage.rows / 4), //top-left position
		cv::FONT_HERSHEY_DUPLEX,
		1.0,
		CV_RGB(118, 185, 0), //font color
		2);
     
     imshow( "Image ref",myImage);
	    	
     waitKey();
   
    
    
    String folder = "/home/drice/distanceTest/dist/d*.jpg";
    vector<String> filenames;
    glob(folder, filenames);
    
    for (size_t i = 0; i < filenames.size(); ++i)
    	{	
		int largest_area1=0;
    		int largest_contour_index1=0;
                Rect bounding_rect1;
		Mat Image = imread(filenames[i]);
		Mat thr1;
		cvtColor( Image, thr1, COLOR_BGR2GRAY ); //Convert to gray
	    	threshold( thr1, thr1, 125, 255, THRESH_BINARY ); //Threshold the gray
		vector<vector<Point> > contours1; // Vector for storing contours
		
		findContours( thr1, contours1, RETR_CCOMP, CHAIN_APPROX_SIMPLE ); // Find the contours in the image
	   	for( size_t i = 0; i< contours1.size(); i++ ) // iterate through each contour.
	    		{	 
		       	 double area1 = contourArea( contours1[i] );  //  Find the area of contour
				 if( area1> largest_area1)
					{
					  	 largest_area1 = area1;
					    	 largest_contour_index1 = i;               //Store the index of largest contour
					    	 bounding_rect1 = boundingRect( contours1[i] ); // Find the bounding rectangle for biggest contour
			    	
		       		}
		        
	       	}
    		        
	   	
	       
		double inches = distanceCamera (knownWidth, focalLength,bounding_rect1.width);
		std::cout << "\n La distance entre la caméra et l'objet est de " << inches<<" cm "<<endl;
		drawContours(Image, contours1,largest_contour_index1, Scalar( 0, 255, 0 ), 2 ); // Draw the largest contour using previously stored index.
	    	
	    	std::ostringstream str;
		str << "distance : " << inches << "m";
		cv::putText(Image, //target image
		str.str(), //text
		cv::Point(35, Image.rows / 4), //top-left position
		cv::FONT_HERSHEY_DUPLEX,
		1.0,
		CV_RGB(118, 185, 0), //font color
		2);
		
		imshow( "result",Image);
	    	waitKey();
	    	
        }
        return 0;
}



















































