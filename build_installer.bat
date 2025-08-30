@echo off
REM TitanVideo SOTA Desktop Installer Builder for Windows
REM This script builds a desktop installer for TitanVideo

echo 🚀 Building TitanVideo Desktop Installer
echo =========================================

echo 📦 Installing dependencies...
pip install -r requirements.txt
pip install pyinstaller

if %errorlevel% neq 0 (
    echo ❌ Failed to install dependencies
    pause
    exit /b 1
)

echo 🔨 Building executable with PyInstaller...
pyinstaller --onefile --windowed --name TitanVideo TitanVideo.py

if %errorlevel% neq 0 (
    echo ❌ Failed to build executable
    pause
    exit /b 1
)

echo 📦 Creating installer...
REM Check if WiX Toolset is available
candle >nul 2>&1
if %errorlevel% equ 0 (
    echo 📦 Creating MSI installer...
    REM Create WiX source
    echo ^<?xml version="1.0" encoding="UTF-8"?^> > TitanVideo.wxs
    echo ^<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi"^> >> TitanVideo.wxs
    echo   ^<Product Id="*" Name="TitanVideo" Language="1033" Version="1.0.0.0" Manufacturer="TitanVideo Team" UpgradeCode="PUT-GUID-HERE"^> >> TitanVideo.wxs
    echo     ^<Package InstallerVersion="200" Compressed="yes" InstallScope="perMachine" /^> >> TitanVideo.wxs
    echo     ^<MajorUpgrade DowngradeErrorMessage="A newer version of [ProductName] is already installed." /^> >> TitanVideo.wxs
    echo     ^<MediaTemplate /^> >> TitanVideo.wxs
    echo     ^<Feature Id="ProductFeature" Title="TitanVideo" Level="1"^> >> TitanVideo.wxs
    echo       ^<ComponentGroupRef Id="ProductComponents" /^> >> TitanVideo.wxs
    echo     ^</Feature^> >> TitanVideo.wxs
    echo   ^</Product^> >> TitanVideo.wxs
    echo   ^<Fragment^> >> TitanVideo.wxs
    echo     ^<Directory Id="TARGETDIR" Name="SourceDir"^> >> TitanVideo.wxs
    echo       ^<Directory Id="ProgramFilesFolder"^> >> TitanVideo.wxs
    echo         ^<Directory Id="INSTALLFOLDER" Name="TitanVideo" /^> >> TitanVideo.wxs
    echo       ^</Directory^> >> TitanVideo.wxs
    echo     ^</Directory^> >> TitanVideo.wxs
    echo   ^</Fragment^> >> TitanVideo.wxs
    echo   ^<Fragment^> >> TitanVideo.wxs
    echo     ^<ComponentGroup Id="ProductComponents" Directory="INSTALLFOLDER"^> >> TitanVideo.wxs
    echo       ^<Component Id="TitanVideo.exe" Guid="*"^> >> TitanVideo.wxs
    echo         ^<File Id="TitanVideo.exe" Source="dist\TitanVideo.exe" KeyPath="yes" /^> >> TitanVideo.wxs
    echo       ^</Component^> >> TitanVideo.wxs
    echo     ^</ComponentGroup^> >> TitanVideo.wxs
    echo   ^</Fragment^> >> TitanVideo.wxs
    echo ^</Wix^> >> TitanVideo.wxs

    candle TitanVideo.wxs
    light TitanVideo.wixobj -out dist\TitanVideo.msi
) else (
    echo ⚠️  WiX Toolset not found. Creating ZIP archive instead...
    powershell "Compress-Archive -Path 'dist\TitanVideo.exe' -DestinationPath 'dist\TitanVideo_Windows.zip' -Force"
)

echo ✅ Build complete!
echo 📁 Output files are in the 'dist' directory:
dir dist\

echo.
echo 🎉 TitanVideo is ready for distribution!
echo    - Run TitanVideo.exe to start the app
echo    - The app includes AI-powered music video generation

pause
