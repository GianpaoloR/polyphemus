#include <iostream>
#include <queue>
#include <stdio.h>

#include "polyphemus.h"


/**
 * Global variables to change polyphemus behaviour
 */

std::string pathCascade = "res/";

#ifdef TEST_MODE
std::string testPath = "/home/kronin/Scrivania/trainingSet/";
#endif

/**
    HAAR CASCADE XML FILES:

    haarcascade_eye.xml                         Find an eye. Only the circle around it.
    haarcascade_eye_tree_eyeglasses.xml         Find an eye. Only the circle around it. Works well with glasses.
    haarcascade_frontalface_alt.xml             Find a face.
    haarcascade_frontalface_alt2.xml            Find a face.
    haarcascade_frontalface_alt_tree.xml        Find a face.
    haarcascade_frontalface_default.xml         Find a face.
    haarcascade_fullbody.xml
    haarcascade_lefteye_2splits.xml             Find a left eye and the eyebrow over it.
    haarcascade_lowerbody.xml
    haarcascade_mcs_eyepair_big.xml             Find a pair of eye (like X-Men Cyclops)
    haarcascade_mcs_eyepair_small.xml           Find a pair of eye (like X-Men Cyclops)
    haarcascade_mcs_leftear.xml                 Find a left ear. Only if the face is on profile.
    haarcascade_mcs_lefteye.xml                 Find a left eye with its corners.
    haarcascade_mcs_mouth.xml                   Find a mouth.
    haarcascade_mcs_nose.xml                    Find a nose.
    haarcascade_mcs_rightear.xml                Find a right ear. Only if the face is on profile.
    haarcascade_mcs_righteye.xml                Find a right eye with its corners.
    haarcascade_mcs_upperbody.xml
    haarcascade_profileface.xml                 Find a profile face.
    haarcascade_righteye_2splits.xml            Find a right eye and the eyebrow over it.
    haarcascade_upperbody.xml
*/

std::string upperBodyCascadeName = pathCascade + "haarcascade_mcs_upperbody.xml";
std::string faceCascadeName = pathCascade + "haarcascade_frontalface_alt.xml";
std::string eyePairCascadeName = pathCascade + "haarcascade_mcs_eyepair_small.xml";
std::string eyeCascadeName = pathCascade + "haarcascade_eye.xml";
std::string leftEyeCascadeName = pathCascade + "haarcascade_mcs_lefteye.xml";
std::string rightEyeCascadeName = pathCascade + "haarcascade_mcs_righteye.xml";
std::string noseCascadeName = pathCascade + "haarcascade_mcs_nose.xml";
std::string mouthCascadeName = pathCascade + "haarcascade_mcs_mouth.xml";


/**
 * @function main
 */
int main() {

#ifdef WEBSERVICE  //WEBSERVICE MODE
    {
        //UO
    cgicc::Cgicc cgicc;

    if (Server::getServer()->init(cgicc)) {
        polyphemus paul(NO_PARAM, Server::getServer()->getFrame());
        //paul.enableDebugGui();
        paul.setScreenSize(Server::getServer()->getScreenWidthPixel(), Server::getServer()->getScreenHeightPixel());
        paul.init(faceCascadeName, eyePairCascadeName, eyeCascadeName, leftEyeCascadeName, rightEyeCascadeName, noseCascadeName, mouthCascadeName, upperBodyCascadeName);
        paul.trackGaze();

        Server::getServer()->setWatchingPoint(paul.getWatchingPoint());
        Server::getServer()->printResponse(paul.getRoiHandler());

        paul.release();

        delete Server::getServer();
        return 0;
    }

    Server::getServer()->printResponseError();
    //UO

} //END OF WEBSERVICE MODE
#endif

#ifdef TEST_MODE    //TEST MODE
{
    std::cout<<"Paul is now running in TEST MODE."<<std::endl;

    polyphemus paul(NO_PARAM, testPath);

    paul.retrieveDesktop();

    paul.enableDebugGui();

    paul.init(faceCascadeName, eyePairCascadeName, eyeCascadeName, leftEyeCascadeName, rightEyeCascadeName, noseCascadeName, mouthCascadeName, upperBodyCascadeName);
    std::cout <<"Paul initialized."<<std::endl;

    paul.trackGaze();

    paul.release();
    std::cout<<"Paul released."<<std::endl;
} //END OF TEST MODE
#endif


#ifdef ACTIVE_CAM    //ACTIVE CAMERA MODE
{
    //Active camera debugging
    polyphemus paul(NO_PARAM);

    std::cout<<"Paul is now running in ACTIVE CAMERA MODE."<<std::endl;

    paul.retrieveDesktop();

    paul.enableDebugGui();

    paul.init(faceCascadeName, eyePairCascadeName, eyeCascadeName, leftEyeCascadeName, rightEyeCascadeName, noseCascadeName, mouthCascadeName, upperBodyCascadeName);

    std::cout <<"Paul initialized."<<std::endl;

    paul.trackGaze();

    paul.release();

    std::cout<<"Paul released."<<std::endl;

} //END OF ACTIVE CAMERA MODE
#endif

    return 0;
}


/*
  FIND EDGES: code example


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );

int main( void )
{
    /// Load source image and convert it to gray
    src = imread( "rotate1.jpg", 1 );

      /// Convert image to gray and blur it
      cvtColor( src, src_gray, CV_BGR2GRAY );
      blur( src_gray, src_gray, Size(3,3) );

      /// Create Window
      char* source_window = "Source";
      namedWindow( source_window, CV_WINDOW_AUTOSIZE );
      imshow( source_window, NULL );

      createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
      thresh_callback( 0, 0 );

      waitKey(0);
      return(0);
}

// @function thresh_callback
void thresh_callback(int, void* )
{
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Detect edges using canny
  Canny( src_gray, canny_output, thresh, thresh*2, 3 );
  /// Find contours
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Draw contours
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {

       Scalar color = Scalar( 255, 255, 255 );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
     }

  /// Show in a window
  namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  imshow( "Contours", drawing );
}

*/
