QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DESTDIR = bin
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += /home/chuwei/chw/srs_test/srs_git/srs_st/srs_st/include
INCLUDEPATH += /home/chuwei/chw/srs_test/srs_git/srs_st/srs_st/srs
LIBS +=  -L /home/chuwei/chw/srs_test/srs_git/srs_st/srs_st/lib -lst
LIBS += -ldl

SOURCES += \
    chw_adapt.cpp \
    main.cpp \
    mainwindow.cpp \
    srs/srs_app_st.cpp \
    srs/srs_kernel_error.cpp \
    srs/srs_protocol_log.cpp \
    srs/srs_protocol_st.cpp

HEADERS += \
    chw_adapt.h \
    mainwindow.h \
    srs/srs_app_st.hpp \
    srs/srs_kernel_error.hpp \
    srs/srs_protocol_log.hpp \
    srs/srs_protocol_st.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
