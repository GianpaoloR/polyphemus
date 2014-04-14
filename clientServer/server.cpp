#include "server.h"

Server::Server() {
    isFirstFrame = true;
    sessionName = "default";
    //    token = 0;
    infoFileName = "info.info";
    snapshotType = "";
}

bool Server::init(cgicc::Cgicc cgicc) {
    std::cout << "Access-Control-Allow-Origin: \"*\"\r\n";

    serverReceiveImageTime = timestamp_usec();

    //gets the fields of the request
    cgicc::form_iterator fiImage = cgicc.getElement("image");
    cgicc::form_iterator fiScreenWidthPixel = cgicc.getElement("screenWidthPixel");
    cgicc::form_iterator fiScreenHeightPixel = cgicc.getElement("screenHeightPixel");
    cgicc::form_iterator fiNumSnapshot = cgicc.getElement("numSnapshot");
    cgicc::form_iterator fiSnapshotType = cgicc.getElement("snapshotType");
    //    cgicc::form_iterator fiSeed = cgicc.getElement("seed");
    //    cgicc::form_iterator fiToken = cgicc.getElement("token");
    //    cgicc::form_iterator fiSessionName = cgicc.getElement("sessionName");
    cgicc::form_iterator fiOldIp = cgicc.getElement("ip");
    if (((fiImage != (*cgicc).end()) && (!fiImage->isEmpty()))
            && ((fiScreenWidthPixel != (*cgicc).end()) && (!fiScreenWidthPixel->isEmpty()))
            && ((fiScreenHeightPixel != (*cgicc).end()) && (!fiScreenHeightPixel->isEmpty()))
            && ((fiNumSnapshot != (*cgicc).end()) && (!fiNumSnapshot->isEmpty()))
            && ((fiSnapshotType != (*cgicc).end()) && (!fiSnapshotType->isEmpty()))
            //            && ((fiSeed != (*cgicc).end()) && (!fiSeed->isEmpty()))
            //            && ((fiToken != (*cgicc).end()) && (!fiToken->isEmpty()))
            //            && ((fiSessionName != (*cgicc).end()))
            && ((fiOldIp != (*cgicc).end()))
            ) {

        //TODO: if the diff flag is true, use the difference construction of the image
        //      using the rules in the .js file

        std::string stringUrlImage = (**fiImage);
        const char * urlImage = stringUrlImage.c_str();

        if (strlen(urlImage) > 4) {
            char * imageDecoded;

            //if the url has not the "data:" in the firsts characters, probably is because isn't decoded
            if (urlImage[4] != ':') {
                imageDecoded = CGI_decode_url(urlImage);
            }
            else {
                imageDecoded = new char[stringUrlImage.size() + 1];
                copy(stringUrlImage.begin(), stringUrlImage.end(), imageDecoded);
                imageDecoded[stringUrlImage.size()] = '\0';
            }

            //find the comma that splits the header of the url image, to the data of the image
            int iPreviousComma = -1;
            int strlenImageDecoded = strlen(imageDecoded);
            for (int i = 0; i < strlenImageDecoded; i++) {
                if (imageDecoded[i] == ',') {
                    iPreviousComma = i + 1;
                    break;
                }
            }

            if (iPreviousComma > -1) {
                //takes the data of the image
                char imageDecodeWithoutContent[strlenImageDecoded - iPreviousComma];
                for (int i = iPreviousComma, j = 0; i < strlenImageDecoded; i++, j++) {
                    imageDecodeWithoutContent[j] = imageDecoded[i];
                }

                //convert the url image in a byte array
                std::vector<BYTE> imageByteArray = base64_decode(imageDecodeWithoutContent);

                //convert the byte array in a Mat
                cv::Mat image = cv::imdecode(imageByteArray, CV_LOAD_IMAGE_COLOR);


                /*SAVE THE IMAGE*/
                long long now = timestamp_usec();
                std::stringstream fileName;
                fileName << "stream/" << now << ".jpeg";
                int i = 0;
                while (fileExists(fileName.str().c_str())) {
                    fileName << "stream/" << now << i << ".jpeg";
                    i++;
                }
                cv::imwrite(fileName.str(), image);

                numSnapshot = atoi((**fiNumSnapshot).c_str());
                snapshotType = (**fiSnapshotType).c_str();
                screenWidthPixel = atoi((**fiScreenWidthPixel).c_str());
                screenHeightPixel = atoi((**fiScreenHeightPixel).c_str());
                //                seed = atoi((**fiSeed).c_str());
                //                token = atoi((**fiSeed).c_str());
                //                sessionName = (**fiSessionName).c_str();
                std::string oldIp = (**fiOldIp).c_str();

                ip = cgicc.getEnvironment().getRemoteAddr();


                //activate those lines to see the print of the code and see the debug in the webservice mode
                //                                saveCookies();
                std::cout << "Content-type:text/html\r\n\r\n";

                createSessionName();

                if (oldIp.compare("") != 0) {
                    if (ip.compare(oldIp) != 0) {
                        if (copyInfo(oldIp, ip) == -1) {
                            //error on copy
                            eraseSessionInfo();
                        }
                        std::stringstream filePath;
                        filePath << "session/" << oldIp << "/";
                        removeDirectoryElements(filePath.str().c_str(), "");
                    }
                }

                readInfo();

                setPreviousFrame(getPersistentImage("frame"));
                setFrame(image);
                setPersistentImage("frame", image);
                setPersistentImage("previousFrame", previousFrame);

                /*GET COOKIES*/

                // get environment variables
                //initCookies(cgicc.getEnvironment().getCookieList());
                //                const cgicc::CgiEnvironment& env = cgicc.getEnvironment();
                //                cgicc::const_cookie_iterator cci;
                //                for(cci = env.getCookieList().begin(); cci != env.getCookieList().end(); cci++) {
                //                    Server::getServer()->setCookie(cci->getName(), cci->getValue());
                //                    //std::cout << "<tr><td>" << cci->getName() << "</td><td>";
                //                    //std::cout << cci->getValue();
                //                    //std::cout << "</td></tr>\n";
                //                }
                //std::cout << "</table><\n";

                //std::cout << "<br/>\n";
                //std::cout << "</body>\n";
                //std::cout << "</html>\n";

                return true;
            }
        }
    }

    return false;
}

void Server::setCookie(std::string key, double value) {
    if (cookies.find(key) == cookies.end()) {
        std::ostringstream ostringstream;
        ostringstream << value;
        cookies[key] = ostringstream.str();
    }
}

void Server::setCookie(std::string key, std::string value) {
    if (cookies.find(key) == cookies.end())
        cookies[key] = value;
}

void Server::setInfo(std::string key, int value) {
    infoInt[key] = value;
}

void Server::setInfo(std::string key, long long value) {
    infoLonglong[key] = value;
}

void Server::setInfo(std::string key, double value) {
    infoDouble[key] = value;
}

void Server::setInfo(std::string key, std::string value) {
    infoString[key] = value;
}

void Server::setInfo(std::string key, std::string type, std::string value) {
    if (type.compare("int") == 0) {
        setInfo(key, atoi(value.c_str()));
    }
    else if (type.compare("long long") == 0) {
        setInfo(key, atoll(value.c_str()));
    }
    else if (type.compare("double") == 0) {
        setInfo(key, atof(value.c_str()));
    }
    else if (type.compare("string") == 0) {
        setInfo(key, value);
    }
}

int Server::getInfoInt(std::string key) {
    map<string, int>::iterator iter = infoInt.find(key);
    if (iter != infoInt.end()) {
        return iter->second;
    }
    return NULL;
}

long long Server::getInfoLonglong(std::string key) {
    map<string, long long>::iterator iter = infoLonglong.find(key);
    if (iter != infoLonglong.end()) {
        return iter->second;
    }
    return NULL;
}

double Server::getInfoDouble(std::string key) {
    map<string, double>::iterator iter = infoDouble.find(key);
    if (iter != infoDouble.end()) {
        return iter->second;
    }
    return NULL;
}

std::string Server::getInfoString(std::string key) {
    map<string, string>::iterator iter = infoString.find(key);
    if (iter != infoString.end()) {
        return iter->second;
    }
    return "";
}

std::string Server::getCookie(std::string key) {
    if (cookies.find(key) != cookies.end())
        return cookies[key];
    else
        return "";
}

void Server::initCookies(std::vector<cgicc::HTTPCookie> cookieList) {
    for(cgicc::const_cookie_iterator cci = cookieList.begin(); cci != cookieList.end(); cci++) {
        setCookie(cci->getName(), cci->getValue());
    }
}

void Server::saveCookies() {
    //    std::cout << "Set-Cookie:UserID=XYZ;\r\n";
    //    std::cout << "Set-Cookie:Password=XYZ123;\r\n";
    for (std::map<std::string, std::string>::iterator it = cookies.begin(); it != cookies.end(); ++it)
        std::cout << "Set-Cookie:" << it->first << "=" << it->second << ";\r\n";

    std::cout << "Set-Cookie:Domain=localhost;\r\n";
    std::cout << "Set-Cookie:Path=/;\n";
}


void Server::readInfo() {
    std::stringstream fileName;
    fileName << "session/" << sessionName << "/" << infoFileName;
    int fd = open(fileName.str().c_str(), O_RDWR | O_CREAT, PRIVILEGES); // open or create lockfile
    if (fd != -1) {
        int rc = flock(fd, LOCK_EX | LOCK_NB); // grab exclusive lock, fail if can't obtain.
        if (rc) {
            // fail
        }
        else {
            ifstream readFile;
            readFile.open(fileName.str().c_str());
            if (readFile.is_open()) {
                string line;
                while (getline(readFile, line)) {
                    std::istringstream iss(line);
                    string key = "";
                    std::getline(iss, key, ':');

                    string type = "";
                    std::getline(iss, type, ':');

                    string value(line);
                    value.erase(0, key.length() + 1 + type.length() + 1);

                    //cout << key << ":" << type << ":" << value << endl;
                    setInfo(key, type, value);
                }
                readFile.close();

                map<string, long long>::iterator iter = infoLonglong.find("Time last update");
                if (iter != infoLonglong.end()) {
                    long long timeLastupdate = iter->second;
                    long long now = timestamp_usec();
                    long long diff = now - timeLastupdate;
                    if ((diff < 0) || (diff > 600000000)) {
                        //10*60*1000000 -> 10 minutes
                        eraseSessionInfo();
                    }
                }
                else {
                    eraseSessionInfo();
                }
            }
        }
    }
    else {

    }
    close(fd);
}

int Server::copyInfo(string sessionFrom, string sessionTo) {
    std::stringstream source;
    source << "session/" << sessionFrom << "/";
    std::stringstream destination;
    destination << "session/" << sessionTo << "/";
    return copyDirectory(source.str().c_str(), destination.str().c_str(), PRIVILEGES);
}

void Server::writeInfo() {
    std::stringstream fileName;
    fileName << "session/" << sessionName << "/" << infoFileName;
    int fd = open(fileName.str().c_str(), O_RDWR | O_CREAT, PRIVILEGES); // open or create lockfile
    if (fd != -1) {
        int rc = flock(fd, LOCK_EX | LOCK_NB); // grab exclusive lock, fail if can't obtain.
        if (rc) {
            // fail
        }
        else {
            long long now = timestamp_usec();
            map<string, long long>::iterator iter = infoLonglong.find("Time creation");
            if (iter == infoLonglong.end()) {
                infoLonglong["Time creation"] = now;
            }
            infoLonglong["Time last update"] = now;
            //myfile << "Time last update:int:" << now << endl;

            ofstream myfile;
            myfile.open (fileName.str().c_str());
            for (std::map<std::string, int>::iterator it = infoInt.begin(); it != infoInt.end(); ++it) {
                myfile << it->first << ":int:" << it->second << endl;
            }
            for (std::map<std::string, long long>::iterator it = infoLonglong.begin(); it != infoLonglong.end(); ++it) {
                myfile << it->first << ":long long:" << it->second << endl;
            }
            for (std::map<std::string, double>::iterator it = infoDouble.begin(); it != infoDouble.end(); ++it) {
                myfile << it->first << ":double:" << it->second << endl;
            }
            for (std::map<std::string, std::string>::iterator it = infoString.begin(); it != infoString.end(); ++it) {
                myfile << it->first << ":string:" << it->second << endl;
            }

            myfile.close();
        }
    }
    else {

    }
    close(fd);
}

void Server::eraseSessionInfo() {
    std::stringstream filePath;
    filePath << "session/" << sessionName << "/" << infoFileName;
    removeDirectoryElements(filePath.str().c_str(), infoFileName.c_str());
    infoInt.clear();
    infoLonglong.clear();
    infoDouble.clear();
    infoString.clear();
}

int Server::removeDirectoryElements(const char *path, const char *noRemoveName) {
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;
        r = 0;

        while (!r && (p=readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if ((!strcmp(p->d_name, ".")) || (!strcmp(p->d_name, "..")) || (!strcmp(p->d_name, noRemoveName))) {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2;
            buf = (char *)malloc(len);

            if (buf) {
                struct stat statbuf;

                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode)) {
                        r2 = removeDirectoryElements(buf, noRemoveName);
                        if (!r2) {
                            r2 = rmdir(buf);
                        }
                    }
                    else {
                        r2 = unlink(buf);
                    }
                }

                free(buf);
            }

            r = r2;
        }

        closedir(d);
    }
    return r;
}

void Server::setPersistentImage(std::string key, cv::Mat image) {
    /*
        In the /opt/lampp/etc/httpd.conf add those lines

            #UO: added to make this folder not reachable by internet users
            <Directory "/opt/lampp/htdocs/polyphemus/session">
              Order allow,deny
              Deny from all
            </Directory>
    */

    std::stringstream fileName;
    fileName << "session/";
    if (createDirectoryIfNotExist(fileName.str().c_str(), PRIVILEGES) != -1) {
        fileName << sessionName << "/";
        if (createDirectoryIfNotExist(fileName.str().c_str(), PRIVILEGES) != -1) {
            fileName << key << ".jpeg";
            cv::imwrite(fileName.str(), image);
        }
    }
}

cv::Mat Server::getPersistentImage(std::string key) {
    std::stringstream fileName;
    fileName << "session/" << sessionName << "/" << key << ".jpeg";
    if (fileExists(fileName.str().c_str())) {
        return cv::imread(fileName.str());
    }
    else {
        return cv::Mat();
    }
}

void Server::setFrame(cv::Mat frame) {
    this->frame = frame;
}

void Server::setPreviousFrame(cv::Mat frame) {
    this->previousFrame = frame;
}

cv::Mat Server::getFrame() {
    return frame;
}

cv::Mat Server::getPreviousFrame() {
    return previousFrame;
}

bool Server::continueReading() {
    if (isFirstFrame) {
        isFirstFrame = false;
        return true;
    }
    return false;
}

bool Server::createSessionName() {
    /*static const char alphanum[] =
            "0123456789"
            //"!@#$%^&*"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    int stringLength = sizeof(alphanum) - 1;

    //TODO:use the user seed to the srand() and if it doesn't arrive means that the user has already a sessionName
    //send the sessionName to the user
    //create a log file in the folder
    //if the log file has an old updating date, the folder must be delete

    int srandSeed;
    if (!ip.empty()) {
        int multiplier = 1;
        srandSeed = 0;
        std::istringstream iss(ip);
        for (std::string token; std::getline(iss, token, '.'); ) {
            multiplier *= 255;
            srandSeed += atoi(token.c_str());
            srandSeed *= multiplier;
        }
    }
    else {
        srandSeed = seed - screenWidthPixel * screenHeightPixel;
        if (srandSeed <= 1) {
            srandSeed = seed + screenWidthPixel * screenHeightPixel;
        }
    }

    srand(srandSeed);
    bool folderCreated = false;
    do {
        std::stringstream folderName;
        folderName << "session/";
        if (createDirectoryIfNotExist(folderName.str().c_str(), PRIVILEGES) != -1) {
            std::stringstream newSessionName;
            long long now = timestamp_usec();
            std::stringstream nowStr;
            nowStr << now;

            for (int i = 0; i < nowStr.str().length(); i++) {
                newSessionName << nowStr.str()[i] << alphanum[rand() % stringLength];
            }
            folderName << newSessionName.str() << "/";
            std::cout << folderName.str() << std::endl;
            if (createDirectoryIfNotExist(folderName.str().c_str(), PRIVILEGES) == 1) {
                folderCreated = true;
                sessionName = newSessionName.str();
            }
        }
    } while (!folderCreated);*/

    bool folderCreated = false;
    std::stringstream folderName;
    folderName << "session/";
    if (createDirectoryIfNotExist(folderName.str().c_str(), PRIVILEGES) != -1) {
        folderName << ip << "/";
        if (createDirectoryIfNotExist(folderName.str().c_str(), PRIVILEGES) != -1) {
            folderCreated = true;
            sessionName = ip;
        }
    }

    return folderCreated;
}

std::string Server::getSessionName() {
    return sessionName;
}

int Server::getNumSnapshot() {
    return numSnapshot;
}

std::string Server::getSnapshotType() {
    return snapshotType;
}

int Server::getScreenWidthPixel() {
    return screenWidthPixel;
}

int Server::getScreenHeightPixel() {
    return screenHeightPixel;
}

void Server::setWatchingPoint(cv::Point watchingPoint) {
    this->watchingPoint = watchingPoint;
}

void Server::printResponse(roiHandler* roiHandler) {
    int x = 0;
    int y = 0;

    serverSendResponseTime = timestamp_usec();

    //Server::getServer()->setCookie("key", "value");

    //    srand((unsigned)timestamp_usec());
    //    x = (rand()%(screenWidthPixel+1));
    //    y = (rand()%(screenHeightPixel+1));


    //    cv::Point watchingPoint = paul.getWatchingPoint();
    //    x = watchingPoint.x;
    //    y = watchingPoint.y;

    x = watchingPoint.x;
    y = watchingPoint.y;

    /*SET COOKIES*/
    //                std::cout << "Set-Cookie:UserID=XYZ;\r\n";
    //                std::cout << "Set-Cookie:Password=XYZ123;\r\n";
    //                std::cout << "Set-Cookie:Domain=localhost;\r\n";
    //                std::cout << "Set-Cookie:Path=/;\n";

    //    saveCookies();
    //    std::cout << "Content-type:text/html\r\n\r\n";
    std::cout << "{";
    std::cout << "\"x\": " << x;
    std::cout << ", \"y\": " << y;
    std::cout << ", \"numSnapshot\": " << numSnapshot;

    if (roiHandler->hasFace()) {
        std::cout << ", \"face\": {\"x\": " << roiHandler->getFace().x << ", \"y\": " << roiHandler->getFace().y << ", \"width\": " << roiHandler->getFace().width << ", \"height\": " << roiHandler->getFace().height << "}";
    }
    else {
        std::cout << ", \"face\": null";
    }

    if (roiHandler->hasLeftEye()) {
        std::cout << ", \"leftEye\": {\"x\": " << roiHandler->getFace().x + roiHandler->getLeftEye().x << ", \"y\": " << roiHandler->getFace().y + roiHandler->getLeftEye().y << ", \"width\": " << roiHandler->getLeftEye().width << ", \"height\": " << roiHandler->getLeftEye().height << "}";
    }
    else {
        std::cout << ", \"leftEye\": null";
    }

    if (roiHandler->hasRightEye()) {
        std::cout << ", \"rightEye\": {\"x\": " << roiHandler->getFace().x + roiHandler->getRightEye().x << ", \"y\": " << roiHandler->getFace().y + roiHandler->getRightEye().y << ", \"width\": " << roiHandler->getRightEye().width << ", \"height\": " << roiHandler->getRightEye().height << "}";
    }
    else {
        std::cout << ", \"rightEye\": null";
    }

    if (roiHandler->hasNose()) {
        std::cout << ", \"nose\": {\"x\": " << roiHandler->getFace().x + roiHandler->getNose().x << ", \"y\": " << roiHandler->getFace().y + roiHandler->getNose().y << ", \"width\": " << roiHandler->getNose().width << ", \"height\": " << roiHandler->getNose().height << "}";
    }
    else {
        std::cout << ", \"nose\": null";
    }

    if (roiHandler->hasMouth()) {
        std::cout << ", \"mouth\": {\"x\": " << roiHandler->getFace().x + roiHandler->getMouth().x << ", \"y\": " << roiHandler->getFace().y + roiHandler->getMouth().y << ", \"width\": " << roiHandler->getMouth().width << ", \"height\": " << roiHandler->getMouth().height << "}";
    }
    else {
        std::cout << ", \"mouth\": null";
    }

    std::cout << ", \"snapshotType\": \"" << snapshotType << "\"" << std::endl;
    if (snapshotType.compare("training") == 0) {
        if ((roiHandler->hasFace()) && (roiHandler->hasLeftEye()) && (roiHandler->hasRightEye()) && (roiHandler->hasNose()) && (roiHandler->hasMouth())) {
            std::cout << ", \"train\": { \"isDone\": true }" << std::endl;
        }
    }

    std::cout << ", \"ip\": \"" << ip << "\"" << std::endl;

    std::cout << ", \"timeTable\": { \"times\": { \"serverReceiveImage\": \"" << serverReceiveImageTime << "\", \"serverSendResponse\": \"" << serverSendResponseTime << "\" } } " << std::endl;

    std::cout << "}" << std::endl;

    writeInfo();

    return;
}

void Server::printResponseError() {
    /*SET COOKIES*/
    //    std::cout << "Set-Cookie:UserID=XYZ;\r\n";
    //    std::cout << "Set-Cookie:Password=XYZ123;\r\n";
    //    std::cout << "Set-Cookie:Domain=localhost;\r\n";
    //    std::cout << "Set-Cookie:Path=/;\n";

    std::cout << "Content-type:text/html\r\n\r\n";
    std::cout << "{\"error\": \"Image not found.\"}" << std::endl;

    writeInfo();
}

Server* Server::getServer() {
    if (serverInstance == NULL) {
        serverInstance = new Server;
    }
    return serverInstance;
}

Server* Server::serverInstance = NULL;


/* Esempio sui thread nel caso in cui si voglia usarli per salvare le immagini */
//#include <iostream>
//#include <cstdlib>
//#include <pthread.h>
//#include <unistd.h>
//#include <fstream>

//using namespace std;

//#define NUM_THREADS 5

//struct thread_data{
//    int  thread_id;
//    char *message;
//};


//void *PrintHello(void *threadarg) {
//    struct thread_data *my_data;

//    my_data = (struct thread_data *) threadarg;

//    cout << "Thread ID : " << my_data->thread_id;
//    cout << " Message : " << my_data->message << endl;

//    ofstream myfile;
//    std::stringstream fileName;
//    fileName << "AAA/";
//    if (fileExists(fileName.str().c_str())) {
//        cout << "exist\n";
//    }
//    else {
//        cout << "not\n";
//        cout << createDirectoryIfNotExist(fileName.str().c_str(), 0700);
//    }
//    fileName << my_data->thread_id << ".txt";
//    myfile.open(fileName.str().c_str());
//    myfile << my_data->message << "s to a file.\n";
//    myfile.close();

//    pthread_exit(NULL);
//}

//int main() {
//    int rc;
//    int i;
//    pthread_t threads[NUM_THREADS];
//    struct thread_data td[NUM_THREADS];
//    pthread_attr_t attr;
//    void *status;

//    // Initialize and set thread joinable
//    pthread_attr_init(&attr);
//    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

//    for(i = 0; i < NUM_THREADS; i++) {
//        cout << "main() : creating thread, " << i << endl;
//        td[i].thread_id = i;
//        td[i].message = "This is message";
//        rc = pthread_create(&threads[i], NULL, PrintHello, (void *)&td[i]);
//        if (rc) {
//            cout << "Error:unable to create thread," << rc << endl;
//            exit(-1);
//        }
//    }

//    // free attribute and wait for the other threads
//    pthread_attr_destroy(&attr);
//    for (i = 0; i < NUM_THREADS; i++) {
//        rc = pthread_join(threads[i], &status);
//        if (rc) {
//            cout << "Error:unable to join," << rc << endl;
//            exit(-1);
//        }
//        cout << "Main: completed thread id :" << i;
//        cout << "  exiting with status :" << status << endl;
//    }

//    cout << "Main: program exiting." << endl;
//    pthread_exit(NULL);
//}
