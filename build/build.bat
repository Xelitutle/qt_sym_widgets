@echo off
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Debug
cmake --build .build_msvc --config %CONFIG%
