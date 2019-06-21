[![Codacy Badge](https://api.codacy.com/project/badge/Grade/7922968d5a6642938c121868dbdad91b)](https://www.codacy.com/app/Cvostr/zsgamedit?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Cvostr/zsgamedit&amp;utm_campaign=Badge_Grade) 

# ZS GamEdit

Is an simple opensource thing to make games (in future)

As UI library uses QT5, to draw graphics uses OpenGL

## Dependencies

Qt5, of course

GLEW

OpenAL

SDL2

Assimp

Lua 5.3 (+LuaBridge)

FreeType

## Building on linux
```
sudo apt install qt5-default
sudo apt install qtcreator (optional)
sudo apt install libglew-dev
sudo apt install libopenal-dev
sudo apt install libsdl2-dev
sudo apt install libassimp-dev
sudo apt install liblua5.3-dev
sudo apt install libfreetype6-dev
```
Next go to root of project and invoke
```
qmake
make
```
(hope, you knew it well without me xDD)

## Building on Windows
You can do it!

Install Visual Studio (installs compiler)

You can also find QtCreator for Windows. This IDE will find installed MSVC compiler

Of course, you have to manually search and build dependencies (Windows is a "best" OS ever xDD)
