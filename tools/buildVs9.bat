@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

set DOTNET4=%WinDir%\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe
set DOTNET3=%WinDir%\Microsoft.NET\Framework\v3.5\MSBuild.exe
set DOTNET2=%WinDir%\Microsoft.NET\Framework\v2.0.50727\MSBuild.exe

if exist "%DOTNET4%" (
	set MSBUILD=%DOTNET4%
) else if exist "%DOTNET3%" (
	set MSBUILD=%DOTNET3%
) else if exist "%DOTNET2%" (
	set MSBUILD=%DOTNET2%
) else (
	echo "Can't locate MSBuild.exe"
	exit
)

echo ------------- Building doc.vcproj
echo.
%MSBUILD% ..\Win32\Ocerus.sln /t:doc /p:Configuration=Release

echo ------------- Building Ocerus.vcproj
echo.
%MSBUILD% ..\Win32\Ocerus.sln /t:_Ocerus /p:Configuration=Release

@echo on
