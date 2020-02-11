QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testhttppacket.cpp

INCLUDEPATH += $$PWD/../../Applications/brainiest-server/
HEADERS += $$PWD/../../Applications/brainiest-server/HttpPacket.hpp
SOURCES += $$PWD/../../Applications/brainiest-server/HttpPacket.cpp
