#-------------------------------------------------
#
# Project created by QtCreator 2018-10-07T08:56:11
#
#-------------------------------------------------

QT       += core gui widgets
unix:LIBS += -lGLEW -lSDL2 -lopenal -llua5.2 -lassimp -lvulkan
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    ProjEd/PropertyAreas.cpp \
    ProjEd/EdActions.cpp \
    Misc/oal_manager.cpp \
    ProjEd/EditorUI.cpp \
    Misc/ProjBuilder.cpp \
    Scripting/LuaScript.cpp \
    Scripting/zsensdk.cpp \
    Render/MatShaderProps.cpp \
    World/GameObject.cpp \
    World/GoProperty/go_filestream.cpp \
    World/GoProperty/go_properties.cpp \
    World/GoProperty/tile_properties.cpp \
    Misc/AssimpMeshLoader.cpp

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
    World/headers/obj_properties.h \
    ProjEd/headers/InspEditAreas.h \
    triggers.h \
    Render/headers/MatShaderProps.h \
    Misc/headers/zs_types.h \
    Misc/headers/AssimpMeshLoader.h


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

#-------------------------------------WINDOWS LIBS PATHS---------------------------
#ASSIMP library
win32: LIBS += -L$$PWD/../../LIBS/ -lassimp_x64

win32:INCLUDEPATH += $$PWD/../../LIBS/ASSIMP
win32:DEPENDPATH += $$PWD/../../LIBS/ASSIMP

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/assimp_x64.lib
#OpenAL library
win32: LIBS += -L$$PWD/../../LIBS/ -lOpenAL_x64

win32:INCLUDEPATH += $$PWD/../../LIBS/OPENAL
win32:DEPENDPATH += $$PWD/../../LIBS/OPENAL

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/OpenAL_x64.lib
#GLEW library
win32: LIBS += -L$$PWD/../../LIBS/ -lglew32s

win32:INCLUDEPATH += $$PWD/../../LIBS/GLEW
win32:DEPENDPATH += $$PWD/../../LIBS/GLEW

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/glew32s.lib

#SDL2 lib
win32: LIBS += -L$$PWD/../../LIBS/ -lSDL2
win32: LIBS += -L$$PWD/../../LIBS/ -lSDL2main
win32: LIBS += -L$$PWD/../../LIBS/ -lSDL2test

win32:INCLUDEPATH += $$PWD/../../LIBS/SDL
win32:DEPENDPATH += $$PWD/../../LIBS/SDL

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/SDL2.lib
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/SDL2main.lib
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/SDL2test.lib
#LUA 5.2
win32: LIBS += -L$$PWD/../../LIBS/ -llua52

win32:INCLUDEPATH += $$PWD/../../LIBS/LUA
win32:DEPENDPATH += $$PWD/../../LIBS/LUA
win32:INCLUDEPATH += $$PWD/../../LIBS/LUABRIDGE
win32:DEPENDPATH += $$PWD/../../LIBS/LUABRIDGE

#-------------------------------------------------- ZSPIRE ENGINE Library ---------------------

unix|win32: LIBS += -L$$PWD/../build-ZSpireEngine-Desktop-Debug/ -lzsengine -lvulkan

INCLUDEPATH += $$PWD/../ENGINE/headers
DEPENDPATH += $$PWD/../ENGINE/headers

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../build-ZSpireEngine-Desktop-Debug/zsengine.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../build-ZSpireEngine-Desktop-Debug/libzsengine.a
