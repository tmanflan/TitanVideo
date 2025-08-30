#!/bin/bash

# TitanVideo SOTA Desktop Installer Builder
# This script builds a desktop installer for TitanVideo

echo "🚀 Building TitanVideo Desktop Installer"
echo "========================================"

# Check if we're on a supported platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    PLATFORM="windows"
else
    echo "❌ Unsupported platform: $OSTYPE"
    exit 1
fi

echo "📍 Detected platform: $PLATFORM"

# Install dependencies
echo "📦 Installing dependencies..."
pip install -r requirements.txt
pip install pyinstaller

# Create dist directory
mkdir -p dist

# Build with PyInstaller
echo "🔨 Building executable with PyInstaller..."
if [[ "$PLATFORM" == "windows" ]]; then
    pyinstaller --onefile --windowed --name TitanVideo TitanVideo.py
else
    pyinstaller TitanVideo.spec
fi

# Create installer based on platform
if [[ "$PLATFORM" == "linux" ]]; then
    echo "📦 Creating AppImage for Linux..."
    # Create AppImage
    mkdir -p AppDir
    cp -r dist/TitanVideo/* AppDir/
    cp UI_Interface.html AppDir/
    cp README.md AppDir/
    cp LICENSE AppDir/

    # Create desktop file
    cat > AppDir/TitanVideo.desktop << EOF
[Desktop Entry]
Name=TitanVideo
Exec=TitanVideo
Icon=TitanVideo
Type=Application
Categories=AudioVideo;Video;
EOF

    # Download appimagetool if not present
    if [ ! -f appimagetool ]; then
        wget -O appimagetool https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
        chmod +x appimagetool
    fi

    ./appimagetool AppDir TitanVideo.AppImage
    mv TitanVideo.AppImage dist/

elif [[ "$PLATFORM" == "macos" ]]; then
    echo "📦 Creating DMG for macOS..."
    # Create DMG
    hdiutil create -volname "TitanVideo" -srcfolder dist/TitanVideo.app -ov -format UDZO dist/TitanVideo.dmg

elif [[ "$PLATFORM" == "windows" ]]; then
    echo "📦 Creating MSI installer for Windows..."
    # Create MSI using WiX Toolset (requires WiX to be installed)
    if command -v candle &> /dev/null; then
        # Create WiX source
        cat > TitanVideo.wxs << EOF
<?xml version="1.0" encoding="UTF-8"?>
<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">
  <Product Id="*" Name="TitanVideo" Language="1033" Version="1.0.0.0" Manufacturer="TitanVideo Team" UpgradeCode="PUT-GUID-HERE">
    <Package InstallerVersion="200" Compressed="yes" InstallScope="perMachine" />
    <MajorUpgrade DowngradeErrorMessage="A newer version of [ProductName] is already installed." />
    <MediaTemplate />
    <Feature Id="ProductFeature" Title="TitanVideo" Level="1">
      <ComponentGroupRef Id="ProductComponents" />
    </Feature>
  </Product>
  <Fragment>
    <Directory Id="TARGETDIR" Name="SourceDir">
      <Directory Id="ProgramFilesFolder">
        <Directory Id="INSTALLFOLDER" Name="TitanVideo" />
      </Directory>
    </Directory>
  </Fragment>
  <Fragment>
    <ComponentGroup Id="ProductComponents" Directory="INSTALLFOLDER">
      <Component Id="TitanVideo.exe" Guid="*">
        <File Id="TitanVideo.exe" Source="dist/TitanVideo.exe" KeyPath="yes" />
      </Component>
    </ComponentGroup>
  </Fragment>
</Wix>
EOF
        candle TitanVideo.wxs
        light TitanVideo.wixobj -out dist/TitanVideo.msi
    else
        echo "⚠️  WiX Toolset not found. Creating ZIP archive instead..."
        cd dist
        zip -r TitanVideo_Windows.zip TitanVideo
        cd ..
    fi
fi

echo "✅ Build complete!"
echo "📁 Output files are in the 'dist' directory:"
ls -la dist/

echo ""
echo "🎉 TitanVideo is ready for distribution!"
echo "   - Run the executable/installer to start the app"
echo "   - The app includes AI-powered music video generation"
