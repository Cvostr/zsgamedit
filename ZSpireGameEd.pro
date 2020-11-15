#-------------------------------------------------
#
# Project created by QtCreator 2018-10-07T08:56:11
#
#-------------------------------------------------

QT       += core gui widgets
unix:LIBS += -lGLEW -lSDL2 -lopenal -llua5.3 -lassimp -lvulkan -lfreetype -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
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
    code/Misc/AssimpMeshLoader.cpp \
    code/Misc/ProjBuilder.cpp \
    code/Misc/ThumbnailsMaster.cpp \
    code/ProjEd/ConsoleLog.cpp \
    code/ProjEd/DialogsMaster.cpp \
    code/ProjEd/EdActions.cpp \
    code/ProjEd/EditorFilemanager.cpp \
    code/ProjEd/EditorInput.cpp \
    code/ProjEd/EditorMenu.cpp \
    code/ProjEd/EditorResources.cpp \
    code/ProjEd/EditorSettings.cpp \
    code/ProjEd/EditorUI.cpp \
    code/ProjEd/GO_widget_item.cpp \
    code/ProjEd/InspectorWin.cpp \
    code/ProjEd/LocStringEditWin.cpp \
    code/ProjEd/ProjCreateWin.cpp \
    code/ProjEd/ProjectEdit.cpp \
    code/ProjEd/PropertyAreas.cpp \
    code/ProjEd/ResourcePicker.cpp \
    code/Render/GizmosRenderer.cpp \
    code/Render/zs-pipeline-editor.cpp \
    code/World/GameObject.cpp \
    code/World/GoProperty/MaterialPropertyUI.cpp \
    code/World/GoProperty/TerrainComponent.cpp \
    code/World/GoProperty/ZPScriptPropertyUI.cpp \
    code/World/GoProperty/go_properties.cpp \
    code/World/GoProperty/tile_properties.cpp \
    code/World/Misc.cpp \
    code/World/PhysicsPropertyUI.cpp \
    code/World/TerrainUtils.cpp \
    code/World/World.cpp \
    code/main.cpp \
    code/mainwin.cpp

HEADERS += \
    code/Misc/headers/AssimpMeshLoader.h \
    code/Misc/headers/EditorManager.h \
    code/Misc/headers/ProjBuilder.h \
    code/Misc/headers/ThumbnailsMaster.h \
    code/Misc/headers/zs_types.h \
    code/ProjEd/headers/ConsoleLog.h \
    code/ProjEd/headers/DialogsMaster.h \
    code/ProjEd/headers/EdActions.h \
    code/ProjEd/headers/EditorSettings.h \
    code/ProjEd/headers/GO_widget_item.h \
    code/ProjEd/headers/InspEditAreas.h \
    code/ProjEd/headers/InspectorWin.h \
    code/ProjEd/headers/LocStringEditWin.h \
    code/ProjEd/headers/ProjCreateWin.h \
    code/ProjEd/headers/ProjectEdit.h \
    code/Render/headers/GizmosRenderer.h \
    code/Render/headers/zs-pipeline.h \
    code/World/headers/Misc.h \
    code/World/headers/World.h \
    code/World/headers/terrain.h \
    code/include_engine.h \
    code/mainwin.h \
    code/triggers.h


FORMS += \
    code/ProjEd/ui/ConsoleLogWindow.ui \
    code/ProjEd/ui/LocStringFileEdit.ui \
    code/ProjEd/ui/buildconsole.ui \
    code/ProjEd/ui/editor.ui \
    code/ProjEd/ui/inspector_win.ui \
    code/ProjEd/ui/proj_create_win.ui \
    code/mainwin.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#-------------------------------------WINDOWS LIBS PATHS---------------------------
#FREETYPE library
win32: LIBS += -L$$PWD/../../LIBS/ -lfreetype
win32:INCLUDEPATH += $$PWD/../../LIBS/
win32:DEPENDPATH += $$PWD/../../LIBS/

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../LIBS/freetype.lib

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
win32: LIBS += -lOpenGL32

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

unix: LIBS += -L$$PWD/../build-ZSpireEngine-Desktop-Debug/ -lzsengine -lvulkan

INCLUDEPATH += $$PWD/../ENGINE/headers
DEPENDPATH += $$PWD/../ENGINE/headers

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../build-ZSpireEngine-Desktop-Debug/zsengine.lib
unix: PRE_TARGETDEPS += $$PWD/../build-ZSpireEngine-Desktop-Debug/libzsengine.a
