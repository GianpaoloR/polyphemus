#------------------------------
#
# Project created by QtCreator
#
#------------------------------

QT       += core gui
QT       += widgets

TARGET = polyphemus.cgi
TEMPLATE = app

SOURCES += main.cpp\
    polyphemus.cpp \
    common/binaryanalyzer.cpp \
    headRotation/headrotation.cpp \
    pupilDetection/pupildetection.cpp \
    clientServer/server.cpp \
    clientServer/cgi.cpp \
    clientServer/base64.cpp \
    clientServer/cgicc/XMLPI.cpp \
    clientServer/cgicc/XHTMLDoctype.cpp \
    clientServer/cgicc/MStreamable.cpp \
    clientServer/cgicc/HTTPXHTMLHeader.cpp \
    clientServer/cgicc/HTTPStatusHeader.cpp \
    clientServer/cgicc/HTTPResponseHeader.cpp \
    clientServer/cgicc/HTTPRedirectHeader.cpp \
    clientServer/cgicc/HTTPPlainHeader.cpp \
    clientServer/cgicc/HTTPHTMLHeader.cpp \
    clientServer/cgicc/HTTPHeader.cpp \
    clientServer/cgicc/HTTPCookie.cpp \
    clientServer/cgicc/HTTPContentHeader.cpp \
    clientServer/cgicc/HTMLElementList.cpp \
    clientServer/cgicc/HTMLElement.cpp \
    clientServer/cgicc/HTMLDoctype.cpp \
    clientServer/cgicc/HTMLAttributeList.cpp \
    clientServer/cgicc/HTMLAttribute.cpp \
    clientServer/cgicc/FormFile.cpp \
    clientServer/cgicc/FormEntry.cpp \
    clientServer/cgicc/CgiUtils.cpp \
    clientServer/cgicc/CgiInput.cpp \
    clientServer/cgicc/CgiEnvironment.cpp \
    clientServer/cgicc/Cgicc.cpp \
    gazeEstimation/gazeestimation.cpp \
    frameHandler/framehandler.cpp \
    common/haaranalyzer.cpp \
    common/helpers.cpp \
    frameHandler/guihandler.cpp \
    common/roiHandler.cpp \
    pupilDetection/pupilgui.cpp \
    pupilDetection/pupilrois.cpp \
    stasm4_1/stasm/asm.cpp \
    stasm4_1/stasm/classicdesc.cpp \
    stasm4_1/stasm/convshape.cpp \
    stasm4_1/stasm/err.cpp \
    stasm4_1/stasm/eyedet.cpp \
    stasm4_1/stasm/eyedist.cpp \
    stasm4_1/stasm/faceroi.cpp \
    stasm4_1/stasm/hat.cpp \
    stasm4_1/stasm/hatdesc.cpp \
    stasm4_1/stasm/landmarks.cpp \
    stasm4_1/stasm/misc.cpp \
    stasm4_1/stasm/pinstart.cpp \
    stasm4_1/stasm/print.cpp \
    stasm4_1/stasm/shape17.cpp\
    stasm4_1/stasm/shapehacks.cpp \
    stasm4_1/stasm/shapemod.cpp \
    stasm4_1/stasm/startshape.cpp \
    stasm4_1/stasm/stasm.cpp \
    stasm4_1/stasm/stasm_lib.cpp \
    stasm4_1/stasm/MOD_1/facedet.cpp \
    stasm4_1/stasm/MOD_1/initasm.cpp \
    pupilDetection/twinkle.cpp

HEADERS  += constants.h\
    polyphemus.h \
    common/binaryanalyzer.h \
    headRotation/headrotation.h \
    pupilDetection/pupildetection.h \
    clientServer/server.h \
    clientServer/cgi.h \
    clientServer/base64.h \
    clientServer/cgicc/XMLPI.h \
    clientServer/cgicc/XMLDeclaration.h \
    clientServer/cgicc/XHTMLDoctype.h \
    clientServer/cgicc/MStreamable.h \
    clientServer/cgicc/HTTPXHTMLHeader.h \
    clientServer/cgicc/HTTPStatusHeader.h \
    clientServer/cgicc/HTTPResponseHeader.h \
    clientServer/cgicc/HTTPRedirectHeader.h \
    clientServer/cgicc/HTTPPlainHeader.h \
    clientServer/cgicc/HTTPHTMLHeader.h \
    clientServer/cgicc/HTTPHeader.h \
    clientServer/cgicc/HTTPCookie.h \
    clientServer/cgicc/HTTPContentHeader.h \
    clientServer/cgicc/HTMLElementList.h \
    clientServer/cgicc/HTMLElement.h \
    clientServer/cgicc/HTMLDoctype.h \
    clientServer/cgicc/HTMLClasses.h \
    clientServer/cgicc/HTMLBooleanElement.h \
    clientServer/cgicc/HTMLAttributeList.h \
    clientServer/cgicc/HTMLAttribute.h \
    clientServer/cgicc/HTMLAtomicElement.h \
    clientServer/cgicc/FormFile.h \
    clientServer/cgicc/FormEntry.h \
    clientServer/cgicc/CgiUtils.h \
    clientServer/cgicc/CgiInput.h \
    clientServer/cgicc/CgiEnvironment.h \
    clientServer/cgicc/CgiDefs.h.in \
    clientServer/cgicc/CgiDefs.h \
    clientServer/cgicc/Cgicc.h \
    gazeEstimation/gazeestimation.h \
    frameHandler/framehandler.h \
    common/haaranalyzer.h \
    common/cvInclusions.h \
    common/helpers.h \
    frameHandler/guihandler.h \
    common/roiHandler.h \
    pupilDetection/pupilgui.h \
    pupilDetection/pupilrois.h \
    stasm4_1/stasm/asm.h \
    stasm4_1/stasm/atface.h \
    stasm4_1/stasm/basedesc.h \
    stasm4_1/stasm/classicdesc.h \
    stasm4_1/stasm/convshape.h \
    stasm4_1/stasm/err.h \
    stasm4_1/stasm/eyedet.h \
    stasm4_1/stasm/eyedist.h \
    stasm4_1/stasm/faceroi.h \
    stasm4_1/stasm/hatdesc.h \
    stasm4_1/stasm/hat.h \
    stasm4_1/stasm/landmarks.h \
    stasm4_1/stasm/landtab_muct77.h \
    stasm4_1/stasm/misc.h \
    stasm4_1/stasm/pinstart.h \
    stasm4_1/stasm/print.h \
    stasm4_1/stasm/shape17.h \
    stasm4_1/stasm/shapehacks.h \
    stasm4_1/stasm/shapemod.h \
    stasm4_1/stasm/startshape.h \
    stasm4_1/stasm/stasm.h \
    stasm4_1/stasm/stasm_landmarks.h \
    stasm4_1/stasm/stasm_lib_ext.h \
    stasm4_1/stasm/stasm_lib.h \
    stasm4_1/stasm/MOD_1/facedet.h \
    stasm4_1/stasm/MOD_1/initasm.h \
    initFlags.h \
    pupilDetection/twinkle.h

#two configuration: release and debug
CONFIG(release,debug|release){
     #DEFINES += "ACTIVE_CAM"
     #DEFINES += "WITH_GUI"
     DEFINES += "TEST_MODE"
     DEFINES += "PROFILING"

    INCLUDEPATH += /home/abuntu/build_with_Qt/release/include
    LIBS += -L/home/abuntu/build_with_Qt/release/lib \
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
     #DEFINES += "TRACKGAZE_DEBUG"
     #DEFINES += "EXPONENTIALAPPROX_DEBUG"
     #DEFINES += "LINEARAPPROX_DEBUG"
     #DEFINES+= "UPHOLSTER_DEBUG"
     #DEFINES+= "UPHOLSTER_WINDOWS"
     #DEFINES += "KETROD_DEBUG"
     #DEFINES += "REFINEPUPILS_DEBUG"
     #DEFINES += "WEBSERVICE"
     DEFINES += "ACTIVE_CAM"
     #DEFINES += "TEST_MODE"
     #DEFINES += "AUTOMATIC_TEST"
     #DEFINES += "ONLY_REAL_PUPIL"
     DEFINES += "WITH_GUI"
     #DEFINES += "POSIT_DEBUG"
     #DEFINES += "STASM"

     INCLUDEPATH += /home/abuntu/build_with_Qt/debug/include
     LIBS += -L/home/abuntu/build_with_Qt/debug/lib \
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
    QMAKE_CXXFLAGS += -Wno-deprecated

#QMAKE_CXXFLAGS += -fopenmp
#LIBS += -fopenmp

OTHER_FILES += \
    otherCode.txt \
    otherCode.jp
