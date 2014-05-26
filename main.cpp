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

} //END OF WEBSERVICE MODE
#endif

#ifdef TEST_MODE    //TEST MODE
{
    std::cout<<"Paul is now running in TEST MODE."<<std::endl;

    polyphemus paul(NO_PARAM, testPath);

    paul.retrieveDesktop();

    //paul.enableDebugGui();

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

#ifdef WITH_GUI
    paul.enableDebugGui();
#endif

    std::cout <<"Initialization...";
    paul.init(faceCascadeName, eyePairCascadeName, eyeCascadeName, leftEyeCascadeName, rightEyeCascadeName, noseCascadeName, mouthCascadeName, upperBodyCascadeName);
    std::cout <<" done."<<endl;

    paul.trackGaze();

    paul.release();

    std::cout<<"Paul released."<<std::endl;

} //END OF ACTIVE CAMERA MODE
#endif

    return 0;
}
