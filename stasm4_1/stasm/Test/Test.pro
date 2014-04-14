#-------------------------------------------------
#
# Project created by QtCreator 2014-04-02T12:43:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Test
TEMPLATE = app


SOURCES += main.cpp
    ../../stasm/*.cpp
    ../../stasm/MOD_1/*.cpp


#two configuration: release and debug
CONFIG(release,debug|release){
    INCLUDEPATH += /home/ketrod/Applicazioni/build_with_Qt/release/include
    LIBS += -L/home/ketrod/Applicazioni/build_with_Qt/release/lib \
    #if library name is lib[name].lib, you should write -l[name]
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_features2d \
    -lopencv_calib3d \
    -lopencv_contrib \
    -lopencv_flann \
    -lopencv_gpu \
    -lopencv_legacy \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_ts \
    -lopencv_video \
    -lX11
}

CONFIG(debug,debug|release){
     #DEFINES += "DEBUG"
     #DEFINES += "RETRIEVEDESKTOP_DEBUG"
     #DEFINES += "PREPROCESS_DEBUG"
     #DEFINES += "FLATFIELD_DEBUG"
     #DEFINES += "TOGRAYSCALE_DEBUG"
     #DEFINES += "SETGRAYFRAME_DEBUG"
     #DEFINES += "GETGRAYFRAME_DEBUG"
     #DEFINES += "DETECTFACES_DEBUG"
     #DEFINES += "FINDNEWFACES_DEBUG"
     #DEFINES += "DETECTUPPERBODY_DEBUG"
     #DEFINES += "PROCESSFACEDATA_DEBUG"
     #DEFINES += "SETFACEROI_DEBUG"
     #DEFINES += "MAKEEMPIRICEYES_DEBUG"
     #DEFINES += "SETEMPIRICEYES_DEBUG"
     #DEFINES += "GREATCATCH_DEBUG"
     #DEFINES += "GREATCATCH_TEST"
     #DEFINES += "FINDPUPILS_DEBUG"
     #DEFINES += "FINDPUPILS_WINDOWS"
     #DEFINES += "FINDPUPILS_TEST"
     #DEFINES += "DARKCROSS_DEBUG"
     #DEFINES += "CLEARPUPILS_DEBUG"
     #DEFINES += "SHOWPUPILS_DEBUG"
     #DEFINES += "SETLEFTPUPIL_DEBUG"
     #DEFINES += "SETRIGHTPUPIL_DEBUG"
     #DEFINES += "FOLLOWGAZE_DEBUG"
     #DEFINES += "EVALUATEGAZE_DEBUG"
     #DEFINES += "ESTIMATEANDSHOWGAZESTARTINGPOINT_DEBUG"
     #DEFINES += "COMPUTEFINALGAZEX_DEBUG"
     #DEFINES += "TRACKGAZE_DEBUG"
     #DEFINES += "EXPONENTIALAPPROX_DEBUG"
     #DEFINES += "LINEARAPPROX_DEBUG"
     #DEFINES+= "UPHOLSTER_DEBUG"
     #DEFINES+= "UPHOLSTER_WINDOWS"
     DEFINES += "KETROD_DEBUG"
     #DEFINES += "REFINEPUPILS_DEBUG"
     #DEFINES += "WEBSERVICE"
     DEFINES += "ACTIVE_CAM"
     #DEFINES += "TEST_MODE"
     #DEFINES += "AUTOMATIC_TEST"
     #DEFINES += "ONLY_REAL_PUPIL"

     INCLUDEPATH += /home/ketrod/Applicazioni/build_with_Qt/debug/include
     LIBS += -L/home/ketrod/Applicazioni/build_with_Qt/debug/lib \
    #if library name is lib[name].lib, you should write -l[name]
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_features2d \
    -lopencv_calib3d \
    -lopencv_contrib \
    -lopencv_flann \
    -lopencv_gpu \
    -lopencv_legacy \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_ts \
    -lopencv_video \
    -lX11
}

# remove possible other optimization flags
QMAKE_CXXFLAGS += -O
QMAKE_CXXFLAGS -= -O1
QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS -= -O3


CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS+=-pg
    QMAKE_LFLAGS+=-pg
}

OTHER_FILES += \
    otherCode.txt
