#-------------------------------------------------
#
# Project created by QtCreator 2019-07-12T12:31:14
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TARGET = qfcgi
TEMPLATE = lib

CONFIG += link_pkgconfig

DEFINES += LIBQFCGI_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# install target
unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
        qfcgi/connection.cpp \
        qfcgi/fcgi.cpp \
        qfcgi/fdbuilder.cpp \
        qfcgi/localbuilder.cpp \
        qfcgi/record.cpp \
        qfcgi/request.cpp \
        qfcgi/stream.cpp \
        qfcgi/tcpbuilder.cpp

HEADERS += \
        qfcgi.h \
        qfcgi/builder.h \
        qfcgi/connection.h \
        qfcgi/fcgi.h \
        qfcgi/fdbuilder.h \
        qfcgi/localbuilder.h \
        qfcgi/record.h \
        qfcgi/request.h \
        qfcgi/stream.h \
        qfcgi/tcpbuilder.h

headers.path    = /usr/include/libqfcgi
headers.files  += $$HEADERS
INSTALLS       += headers

