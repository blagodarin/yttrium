@echo off
cmake -DBUILD=catch2,libjpeg,libpng,libvorbis,openal,opengl -P "%~dp0\cmake\bootstrap.cmake"
