#-------------------------------------------------
#
# Project created by QtCreator 2016-07-29T16:11:42
#
#-------------------------------------------------

CONFIG += c++11
QT += core gui xml webenginewidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = edict
TEMPLATE = app

SOURCES  += main.cpp\
            mainwindow.cpp \
            mdictparser.cpp \
            ripemd.cc \
            dialogabout.cpp \
            dictmanager.cpp \
            dictionary.cpp \
            dictwebpage.cpp \
            ConfigParser.cpp \
            mdict.cpp \
            ccompleter.cpp \
            flowhelper.cpp \
            dialogoption.cpp

HEADERS  += mainwindow.h \
            mdictparser.h \
            ripemd.hh \
            dialogabout.h \
            dictmanager.h \
            dictionary.h \
            dictwebpage.h \
            datrie.h \
            ConfigParser.h \
            mdict.h \
            ccompleter.h \
            flowhelper.h \
            dialogoption.h

FORMS    += mainwindow.ui \
    dialogabout.ui \
    dictmanager.ui \
    dialogoption.ui

INCLUDEPATH += $$PWD/../thirdparty

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/lzo/release/ -llzo
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/lzo/debug/ -llzo
else:unix: LIBS += -L$$OUT_PWD/../thirdparty/lzo/ -llzo

INCLUDEPATH += $$PWD/../thirdparty/lzo
DEPENDPATH += $$PWD/../thirdparty/lzo

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/lzo/release/liblzo.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/lzo/debug/liblzo.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/lzo/release/lzo.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/lzo/debug/lzo.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/lzo/liblzo.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/zlib/release/ -lzlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/zlib/debug/ -lzlib
else:unix: LIBS += -L$$OUT_PWD/../thirdparty/zlib/ -lzlib

INCLUDEPATH += $$PWD/../thirdparty/zlib
DEPENDPATH += $$PWD/../thirdparty/zlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/zlib/release/libzlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/zlib/debug/libzlib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/zlib/release/zlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/zlib/debug/zlib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/zlib/libzlib.a

RESOURCES += \
    res.qrc

win32:RC_ICONS += $$PWD/res/dict.ico
