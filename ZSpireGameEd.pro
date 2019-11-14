#-------------------------------------------------
#
# Project created by QtCreator 2018-10-07T08:56:11
#
#-------------------------------------------------

QT       += core gui widgets
unix:LIBS += -lGLEW -lSDL2 -lopenal -llua5.3 -lvulkan -lassimp -lz -lfreetype -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
unix:INCLUDEPATH += /usr/include/freetype2/
unix:INCLUDEPATH += /usr/include/bullet/

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
    Render/zs-pipeline.cpp \
    World/World.cpp \
    World/Misc.cpp \
    ProjEd/InspectorWin.cpp \
    ProjEd/ProjCreateWin.cpp \
    ProjEd/PropertyAreas.cpp \
    ProjEd/EdActions.cpp \
    ProjEd/EditorUI.cpp \
    Misc/ProjBuilder.cpp \
    Scripting/LuaScript.cpp \
    Scripting/zsensdk.cpp \
    Render/MatShaderProps.cpp \
    World/GameObject.cpp \
    World/GoProperty/go_filestream.cpp \
    World/GoProperty/go_properties.cpp \
    World/GoProperty/tile_properties.cpp \
    Misc/AssimpMeshLoader.cpp \
    Render/GizmosRenderer.cpp \
    Misc/GlyphManager.cpp \
    Misc/EditorManager.cpp \
    Scripting/zsensdk_input.cpp \
    ProjEd/EditorSettings.cpp \
    World/TerrainUtils.cpp \
    World/Physics.cpp \
    Misc/ThumbnailsMaster.cpp \
    Render/zs-material-defines.cpp \

HEADERS += \
        mainwin.h \
    ProjEd/headers/ProjectEdit.h \
    Render/headers/zs-pipeline.h \
    World/headers/World.h \
    World/headers/Misc.h \
    ProjEd/headers/InspectorWin.h \
    ProjEd/headers/ProjCreateWin.h \
    World/headers/2dtileproperties.h \
    ProjEd/headers/EdActions.h \
    Misc/headers/ProjBuilder.h \
    include_engine.h \
    Scripting/headers/LuaScript.h \
    Scripting/headers/zsensdk.h \
    World/headers/obj_properties.h \
    ProjEd/headers/InspEditAreas.h \
    triggers.h \
    Render/headers/MatShaderProps.h \
    Misc/headers/zs_types.h \
    Misc/headers/AssimpMeshLoader.h \
    Render/headers/GizmosRenderer.h \
    Misc/headers/GlyphManager.h \
    Misc/headers/EditorManager.h \
    ProjEd/headers/EditorSettings.h \
    World/headers/terrain.h \
    Misc/headers/ThumbnailsMaster.h


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
win32:INCLUDEPATH += $$PWD/../../LIBS/
win32:INCLUDEPATH += $$PWD/../../LIBS/BULLET/

#FREETYPE library
win32: LIBS += -L$$PWD/../../LIBS/ -lfreetype
win32:DEPENDPATH += $$PWD/../../LIBS/

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/freetype.lib

#ASSIMP library
win32: LIBS += -L$$PWD/../../LIBS/ -lassimp

win32:INCLUDEPATH += $$PWD/../../LIBS/ASSIMP
win32:DEPENDPATH += $$PWD/../../LIBS/ASSIMP

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/assimp.lib
#OpenAL library
win32: LIBS += -L$$PWD/../../LIBS/ -lOpenAL_x64

win32:DEPENDPATH += $$PWD/../../LIBS/OPENAL

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/OpenAL_x64.lib
#GLEW library
win32: LIBS += -L$$PWD/../../LIBS/ -lglew32s
win32:DEPENDPATH += $$PWD/../../LIBS/GLEW

win32: LIBS += -lOpenGL32

#SDL2 lib
win32: LIBS += -L$$PWD/../../LIBS/ -lSDL2
win32: LIBS += -L$$PWD/../../LIBS/ -lSDL2main
win32: LIBS += -L$$PWD/../../LIBS/ -lSDL2test
win32:DEPENDPATH += $$PWD/../../LIBS/SDL

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/SDL2.lib
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/SDL2main.lib
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/SDL2test.lib
#LUA 5.3
win32: LIBS += -L$$PWD/../../LIBS/ -llua53
win32:DEPENDPATH += $$PWD/../../LIBS/LUA
win32:DEPENDPATH += $$PWD/../../LIBS/LUABRIDGE

#Bullet release static libs
#win32: LIBS += -L$$PWD/../../LIBS/BULLET/STATIC_RELEASE -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
#Bullet debug static libs
win32: LIBS += -L$$PWD/../../LIBS/BULLET/STATIC_DEBUG -lBulletSoftBody_Debug -lBulletDynamics_Debug -lBulletCollision_Debug -lLinearMath_Debug
#-------------------------------------------------- ZSPIRE ENGINE Library ---------------------

unix: LIBS += -L$$PWD/../debug_engine/ -lzsengine -lvulkan -lopenal
win32: LIBS += -L$$PWD/../debug_engine/debug/ -lzsengine -lvulkan-1 -lOpenAL_x64

INCLUDEPATH += $$PWD/../zspirengine/headers
DEPENDPATH += $$PWD/../zspirengine/headers

unix: PRE_TARGETDEPS += $$PWD/../debug_engine/libzsengine.a
