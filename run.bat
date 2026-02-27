@echo off

cmake -S . -B build
cmake --build build

build\myapp.exe