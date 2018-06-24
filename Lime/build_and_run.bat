@echo off

msbuild /property:GenerateFullPaths=true /t:build

if %ERRORLEVEL% == 0 (
    start x64/Debug/Lime.exe
) else (
    echo FAILED!!!
)