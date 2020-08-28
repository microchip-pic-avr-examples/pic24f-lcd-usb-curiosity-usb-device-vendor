#-------------------------------------------------
#
# Project created by QtCreator 2012-06-06T13:21:53
#
#-------------------------------------------------

QT += core gui
QT += widgets


TARGET = libusb_Demo
TEMPLATE = app




SOURCES += main.cpp\
        mainwindow.cpp \
    usbcomm.cpp

HEADERS  += mainwindow.h \
    usbcomm.h

FORMS    += mainwindow.ui


#-------------------------------------------------
# Add LibUSB library
#-------------------------------------------------
LIBS += -L./ -llibusb-1.0

#-------------------------------------------------
# Make sure to add the required libraries or
# frameoworks for the demo work depending on
# what OS is being used
#-------------------------------------------------
macx: LIBS += -framework CoreFoundation
win32: LIBS += -lSetupAPI

#-------------------------------------------------
# Make sure output directory for object file and
# executable is in the correct subdirectory
#-------------------------------------------------
macx {
    DESTDIR = mac
    OBJECTS_DIR = mac
    MOC_DIR = mac
    UI_DIR = mac
    RCC_DIR = mac
}
unix: !macx {
    DESTDIR = linux
    OBJECTS_DIR = linux
    MOC_DIR = linux
    UI_DIR = linux
    RCC_DIR = linux
}
win32 {
    DESTDIR = windows
    OBJECTS_DIR = windows
    MOC_DIR = windows
    UI_DIR = windows
    RCC_DIR = windows
}

OTHER_FILES +=

win32: LIBS += -L$$PWD/ -llibusb-1.0

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
