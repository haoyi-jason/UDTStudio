#-------------------------------------------------
#
# Project created by QtCreator 2022-10-05T06:45:42
#
#-------------------------------------------------

QT       += core widgets network charts


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bms
TEMPLATE = app
DESTDIR = "$$PWD/../bin"

include(../lib/canopen/canopen.pri)
include(../lib/od/od.pri)
include(../lib/udtgui/udtgui.pri)


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
        bcu/bcupdomappingwidget.cpp \
        bcu/bms_bcumanagerwidget.cpp \
        bcu/bms_nodemanagerwidget.cpp \
        bms_busmanagerwidget.cpp \
        bms_busnodemanagerview.cpp \
        bms_busnodetreeview.cpp \
        bms_ui/focusededitor.cpp \
        bms_ui/numberpad.cpp \
        bmsstack.cpp \
        bmsstackmodel.cpp \
        bmsstackview.cpp \
        busdriver/canbusvci.cpp \
        canFrameListView/bms_canframelistview.cpp \
        canFrameListView/bms_canframemodel.cpp \
        main.cpp \
        mainwindow.cpp \
        screen/bcuscreenwidget.cpp \
        screen/nodescreenbcu.cpp \
        screen/nodescreenpack.cpp \
        system/bms_alarmcriteria.cpp \
        system/bms_logger.cpp \
        system/bms_systemconfigwidget.cpp \
        system/bms_systemmanagerwidget.cpp \
        widgets/bcustatewidget.cpp \
        widgets/bmucellinputwidget.cpp \
        widgets/bmucellinputwidgets.cpp \
        widgets/bmucellwidget.cpp \
        widgets/bmutempwidget.cpp \
        widgets/bmuwidget.cpp

HEADERS += \
        bcu/bcupdomappingwidget.h \
        bcu/bms_bcumanagerwidget.h \
        bcu/bms_nodemanagerwidget.h \
        bms_busmanagerwidget.h \
        bms_busnodemanagerview.h \
        bms_busnodetreeview.h \
        bms_ui/focusededitor.h \
        bms_ui/numberpad.h \
        bmsstack.h \
        bmsstackmodel.h \
        bmsstackview.h \
        busdriver/canbusvci.h \
        canFrameListView/bms_canframelistview.h \
        canFrameListView/bms_canframemodel.h \
        mainwindow.h \
        screen/bcuscreenwidget.h \
        screen/nodescreenbcu.h \
        screen/nodescreenpack.h \
        system/bms_alarmcriteria.h \
        system/bms_logger.h \
        system/bms_systemconfigwidget.h \
        system/bms_systemmanagerwidget.h \
        widgets/bcustatewidget.h \
        widgets/bmucellinputwidget.h \
        widgets/bmucellinputwidgets.h \
        widgets/bmucellwidget.h \
        widgets/bmutempwidget.h \
        widgets/bmuwidget.h

FORMS += \
        mainwindow.ui

#LIBS += -L"$$PWD/../../bin"
#LIBS += -lod -lcanopen -ludtgui

INCLUDEPATH += $$PWD/../lib/od/ $$PWD/../lib/canopen/ $$PWD/../lib/udtgui/
DEPENDPATH += $$PWD/../lib/od/ $$PWD/../lib/canopen/ $$PWD/../lib/utdgui/



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SUBDIRS +=

DISTFILES +=

RESOURCES += \
    img.qrc
