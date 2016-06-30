TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    System.cpp

INCLUDEPATH += /home/zju/usr/include \
               /usr/include/eigen3

LIBS += -Lusr/lib/x86_64-linux-gnu -lglut -lGLU -lGL -pthread

HEADERS += \
    common.h \
    window.h \
    System.h

