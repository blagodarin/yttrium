@echo off
cmake -DBUILD=libjpeg,libpng,libvorbis,openal,opengl -P "%~dp0\cmake\bootstrap\windows.cmake"
