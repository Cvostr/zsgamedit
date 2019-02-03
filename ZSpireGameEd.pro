#-------------------------------------------------
#
# Project created by QtCreator 2018-10-07T08:56:11
#
#-------------------------------------------------

QT       += core gui
LIBS += -lGLEW -lassimp -lSDL2 -lopenal -lzsengine -llua5.2
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ZSpireGameEd
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwin.cpp \
    ProjEd/ProjectEdit.cpp \
    Render/zs-shader.cpp \
    Render/zs-math.cpp \
    Render/zs-pipeline.cpp \
    Render/zs-mesh.cpp \
    Render/zs-texture.cpp \
    World/World.cpp \
    World/Misc.cpp \
    ProjEd/InspectorWin.cpp \
    ProjEd/ProjCreateWin.cpp \
    World/zs-camera.cpp \
    World/go_properties.cpp \
    World/tile_properties.cpp \
    ProjEd/PropertyAreas.cpp \
    ProjEd/EdActions.cpp \
    Misc/oal_manager.cpp \
    ProjEd/EditorUI.cpp \
    Misc/ProjBuilder.cpp \
    Scripting/LuaScript.cpp

HEADERS += \
        mainwin.h \
    ProjEd/headers/ProjectEdit.h \
    Render/headers/zs-shader.h \
    Render/headers/zs-math.h \
    Render/headers/zs-pipeline.h \
    Render/headers/zs-mesh.h \
    Render/headers/zs-texture.h \
    World/headers/World.h \
    World/headers/Misc.h \
    ProjEd/headers/InspectorWin.h \
    ProjEd/headers/ProjCreateWin.h \
    World/headers/zs-camera.h \
    World/headers/2dtileproperties.h \
    ProjEd/headers/EdActions.h \
    Misc/headers/oal_manager.h \
    Misc/headers/ProjBuilder.h \
    include_engine.h \
    Scripting/headers/LuaScript.h \
    Scripting/headers/zsensdk.h \
    World/headers/obj_properties.h


FORMS += \
        mainwin.ui \
    ProjEd/ui/editor.ui \
    ProjEd/ui/inspector_win.ui \
    ProjEd/ui/proj_create_win.ui \
    ProjEd/ui/buildconsole.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
