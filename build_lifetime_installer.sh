#!/bin/bash

# TitanVideo Lifetime Subscriber Desktop Installer Builder
# This script builds a premium desktop installer for lifetime subscribers
# Includes pre-activated enterprise license and enhanced branding

echo "👑 Building TitanVideo Lifetime Subscriber Installer"
echo "=================================================="

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

# Create lifetime subscriber directory
LIFETIME_DIR="lifetime_build"
mkdir -p $LIFETIME_DIR
cd $LIFETIME_DIR

# Copy source files
echo "📋 Copying source files..."
cp ../TitanVideo.py .
cp ../license_manager.py .
cp ../UI_Interface.html .
cp ../README.md .
cp ../LICENSE .
cp ../requirements.txt .
cp ../TitanVideo.spec . 2>/dev/null || echo "No spec file found, will create one"

# Generate pre-activated lifetime license
echo "🔑 Generating lifetime enterprise license..."
python3 -c "
import sys
sys.path.append('.')
from license_manager import LicenseManager
import base64
from datetime import datetime, timedelta

lm = LicenseManager()
expiry_date = datetime.now() + timedelta(days=3650)  # 10 years

license_data = {
    'package': 'enterprise',
    'machine_id': 'LIFETIME_SUBSCRIBER',
    'expiry': expiry_date.isoformat(),
    'activated': datetime.now().isoformat(),
    'lifetime_access': True,
    'subscriber_type': 'lifetime'
}

license_data['signature'] = lm._generate_signature(license_data)

import os
os.makedirs(os.path.dirname(lm.license_file), exist_ok=True)
with open(lm.license_file, 'w') as f:
    import json
    json.dump(license_data, f, indent=2)

print('✅ Lifetime license generated')
"

# Install dependencies in isolated environment
echo "📦 Setting up isolated Python environment..."
python3 -m venv lifetime_env
source lifetime_env/bin/activate

pip install --upgrade pip
pip install -r requirements.txt
pip install pyinstaller

# Create enhanced spec file for lifetime build
echo "🔧 Creating enhanced PyInstaller spec..."
cat > TitanVideo_Lifetime.spec << 'EOF'
# -*- mode: python ; coding: utf-8 -*-

block_cipher = None

a = Analysis(
    ['TitanVideo.py'],
    pathex=[],
    binaries=[],
    datas=[
        ('UI_Interface.html', '.'),
        ('README.md', '.'),
        ('LICENSE', '.'),
    ],
    hiddenimports=[
        'customtkinter',
        'torch',
        'diffusers',
        'transformers',
        'librosa',
        'moviepy',
        'whisper',
        'cv2',
        'PIL',
        'scipy',
        'numpy',
        'webrtcvad',
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)

pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    [],
    name='TitanVideo_Lifetime',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    icon='titanvideo_icon.ico' if os.path.exists('titanvideo_icon.ico') else None,
)
EOF

# Build the application
echo "🔨 Building lifetime subscriber executable..."
if [[ "$PLATFORM" == "windows" ]]; then
    pyinstaller --onefile --windowed --name TitanVideo_Lifetime TitanVideo_Lifetime.spec
else
    pyinstaller TitanVideo_Lifetime.spec
fi

# Create premium installer package
echo "📦 Creating premium lifetime installer..."

if [[ "$PLATFORM" == "linux" ]]; then
    echo "🏗️  Creating AppImage for Linux lifetime subscribers..."

    # Create enhanced AppDir
    mkdir -p AppDir
    cp -r dist/* AppDir/ 2>/dev/null || cp TitanVideo_Lifetime AppDir/
    cp UI_Interface.html AppDir/
    cp README.md AppDir/
    cp LICENSE AppDir/

    # Create premium desktop file
    cat > AppDir/TitanVideo_Lifetime.desktop << 'EOF'
[Desktop Entry]
Name=TitanVideo Lifetime
Exec=TitanVideo_Lifetime
Icon=titanvideo_lifetime
Type=Application
Categories=AudioVideo;Video;Graphics;
Comment=Professional AI-Powered Music Video Creator - Lifetime Access
EOF

    # Create AppRun script
    cat > AppDir/AppRun << 'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
exec "${HERE}/TitanVideo_Lifetime" "$@"
EOF
    chmod +x AppDir/AppRun

    # Download and use appimagetool
    if [ ! -f ../appimagetool ]; then
        wget -O ../appimagetool https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
        chmod +x ../appimagetool
    fi

    ../appimagetool AppDir TitanVideo_Lifetime.AppImage
    mv TitanVideo_Lifetime.AppImage ../dist/

elif [[ "$PLATFORM" == "macos" ]]; then
    echo "🏗️  Creating premium DMG for macOS lifetime subscribers..."

    # Create app bundle
    mkdir -p TitanVideo_Lifetime.app/Contents/MacOS
    mkdir -p TitanVideo_Lifetime.app/Contents/Resources

    cp TitanVideo_Lifetime TitanVideo_Lifetime.app/Contents/MacOS/
    cp UI_Interface.html TitanVideo_Lifetime.app/Contents/Resources/
    cp README.md TitanVideo_Lifetime.app/Contents/Resources/
    cp LICENSE TitanVideo_Lifetime.app/Contents/Resources/

    # Create Info.plist
    cat > TitanVideo_Lifetime.app/Contents/Info.plist << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>TitanVideo_Lifetime</string>
    <key>CFBundleIdentifier</key>
    <string>com.titanvideo.lifetime</string>
    <key>CFBundleName</key>
    <string>TitanVideo Lifetime</string>
    <key>CFBundleVersion</key>
    <string>1.0.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0.0</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.12</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleSignature</key>
    <string>????</string>
</dict>
</plist>
EOF

    # Create DMG
    hdiutil create -volname "TitanVideo Lifetime" -srcfolder TitanVideo_Lifetime.app -ov -format UDZO ../dist/TitanVideo_Lifetime.dmg

elif [[ "$PLATFORM" == "windows" ]]; then
    echo "🏗️  Creating premium MSI for Windows lifetime subscribers..."

    if command -v candle &> /dev/null; then
        # Create enhanced WiX source for lifetime version
        cat > TitanVideo_Lifetime.wxs << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">
  <Product Id="*" Name="TitanVideo Lifetime" Language="1033" Version="1.0.0.0" Manufacturer="TitanVideo" UpgradeCode="LIFETIME-GUID-HERE">
    <Package InstallerVersion="200" Compressed="yes" InstallScope="perMachine" />
    <MajorUpgrade DowngradeErrorMessage="A newer version of [ProductName] is already installed." />
    <MediaTemplate />
    <Feature Id="ProductFeature" Title="TitanVideo Lifetime" Level="1">
      <ComponentGroupRef Id="ProductComponents" />
    </Feature>
    <Property Id="ARPPRODUCTICON" Value="TitanVideo.ico" />
    <Icon Id="TitanVideo.ico" SourceFile="titanvideo_icon.ico" />
    <Property Id="ARPHELPLINK" Value="https://titanvideo.ai/support" />
    <Property Id="ARPURLINFOABOUT" Value="https://titanvideo.ai" />
  </Product>
  <Fragment>
    <Directory Id="TARGETDIR" Name="SourceDir">
      <Directory Id="ProgramFilesFolder">
        <Directory Id="INSTALLFOLDER" Name="TitanVideo Lifetime" />
      </Directory>
      <Directory Id="ProgramMenuFolder">
        <Directory Id="ApplicationProgramsFolder" Name="TitanVideo Lifetime" />
      </Directory>
    </Directory>
  </Fragment>
  <Fragment>
    <ComponentGroup Id="ProductComponents" Directory="INSTALLFOLDER">
      <Component Id="TitanVideo_Lifetime.exe" Guid="*">
        <File Id="TitanVideo_Lifetime.exe" Source="dist/TitanVideo_Lifetime.exe" KeyPath="yes" />
      </Component>
      <Component Id="UI_Interface.html" Guid="*">
        <File Id="UI_Interface.html" Source="UI_Interface.html" />
      </Component>
      <Component Id="README.md" Guid="*">
        <File Id="README.md" Source="README.md" />
      </Component>
      <Component Id="LICENSE" Guid="*">
        <File Id="LICENSE" Source="LICENSE" />
      </Component>
    </ComponentGroup>
    <ComponentGroup Id="MenuComponents" Directory="ApplicationProgramsFolder">
      <Component Id="StartMenuShortcut" Guid="*">
        <Shortcut Id="TitanVideoLifetimeShortcut"
                 Name="TitanVideo Lifetime"
                 Description="Professional AI-Powered Music Video Creator - Lifetime Access"
                 Target="[INSTALLFOLDER]TitanVideo_Lifetime.exe"
                 WorkingDirectory="INSTALLFOLDER" />
        <RegistryValue Root="HKCU" Key="Software/TitanVideo/Lifetime" Name="installed" Type="integer" Value="1" KeyPath="yes" />
      </Component>
    </ComponentGroup>
  </Fragment>
</Wix>
EOF
        candle TitanVideo_Lifetime.wxs
        light TitanVideo_Lifetime.wixobj -out ../dist/TitanVideo_Lifetime.msi
    else
        echo "⚠️  WiX Toolset not found. Creating enhanced ZIP archive..."
        cd dist
        mkdir TitanVideo_Lifetime_Premium
        cp TitanVideo_Lifetime.exe TitanVideo_Lifetime_Premium/
        cp ../UI_Interface.html TitanVideo_Lifetime_Premium/
        cp ../README.md TitanVideo_Lifetime_Premium/
        cp ../LICENSE TitanVideo_Lifetime_Premium/

        # Create premium installer script
        cat > TitanVideo_Lifetime_Premium/install.bat << 'EOF'
@echo off
echo ========================================
echo   TitanVideo Lifetime Subscriber Setup
echo ========================================
echo.
echo Welcome to TitanVideo Lifetime Access!
echo.
echo This installer will set up TitanVideo with:
echo • Lifetime enterprise license
echo • All premium features unlocked
echo • No subscription fees ever
echo.
echo Installing...
echo.

REM Create desktop shortcut
powershell "$s=(New-Object -COM WScript.Shell).CreateShortcut('%userprofile%\Desktop\TitanVideo Lifetime.lnk');$s.TargetPath='%~dp0TitanVideo_Lifetime.exe';$s.Save()"

echo Installation complete!
echo.
echo Your TitanVideo Lifetime license is pre-activated.
echo Enjoy unlimited access to all features!
echo.
pause
EOF

        zip -r TitanVideo_Lifetime_Windows.zip TitanVideo_Lifetime_Premium
        mv TitanVideo_Lifetime_Windows.zip ../../dist/
        cd ..
    fi
fi

# Create premium welcome package
echo "🎁 Creating lifetime subscriber welcome package..."
mkdir -p ../dist/lifetime_welcome

cat > ../dist/lifetime_welcome/README_LIFETIME.txt << 'EOF'
========================================
   TITANVIDEO LIFETIME SUBSCRIBER
========================================

🎉 CONGRATULATIONS! 🎉

You are now a lifetime subscriber of TitanVideo!

BENEFITS OF YOUR LIFETIME ACCESS:
=================================

✅ UNLIMITED USAGE
   • No monthly video limits
   • No duration restrictions
   • No resolution caps
   • No subscription fees EVER

✅ ALL ENTERPRISE FEATURES
   • AI Director Mode
   • Custom Models
   • White-label options
   • API Unlimited access
   • Batch processing
   • 8K resolution support

✅ LIFETIME SUPPORT
   • Priority customer support
   • Early access to new features
   • Direct communication with developers

INSTALLATION:
=============

1. Run the installer (AppImage/DMG/MSI)
2. Launch TitanVideo
3. Your lifetime license is pre-activated!
4. Start creating amazing music videos

SUPPORT:
========

• Email: support@titanvideo.ai
• Website: https://titanvideo.ai/lifetime-support
• Priority: 24/7 premium support

========================================
   ENJOY YOUR LIFETIME ACCESS!
========================================
EOF

# Copy installer to welcome package
if [[ "$PLATFORM" == "linux" ]]; then
    cp ../dist/TitanVideo_Lifetime.AppImage ../dist/lifetime_welcome/
elif [[ "$PLATFORM" == "macos" ]]; then
    cp ../dist/TitanVideo_Lifetime.dmg ../dist/lifetime_welcome/
elif [[ "$PLATFORM" == "windows" ]]; then
    cp ../dist/TitanVideo_Lifetime.msi ../dist/lifetime_welcome/ 2>/dev/null || cp ../dist/TitanVideo_Lifetime_Windows.zip ../dist/lifetime_welcome/
fi

# Create final distribution archive
echo "📦 Creating final lifetime subscriber distribution..."
cd ../dist
if [[ "$PLATFORM" == "linux" ]]; then
    tar -czf TitanVideo_Lifetime_Linux.tar.gz lifetime_welcome/
elif [[ "$PLATFORM" == "macos" ]]; then
    zip -r TitanVideo_Lifetime_macOS.zip lifetime_welcome/
elif [[ "$PLATFORM" == "windows" ]]; then
    powershell "Compress-Archive -Path lifetime_welcome -DestinationPath TitanVideo_Lifetime_Windows.zip" 2>/dev/null || zip -r TitanVideo_Lifetime_Windows.zip lifetime_welcome/
fi

cd ..

echo ""
echo "✅ Lifetime subscriber build complete!"
echo "📁 Premium installer created in: dist/"
echo ""
echo "🎯 LIFETIME SUBSCRIBER FEATURES:"
echo "   • Pre-activated enterprise license"
echo "   • Enhanced installer with premium branding"
echo "   • Welcome package with instructions"
echo "   • Desktop shortcuts and menu entries"
echo "   • Priority support information"
echo ""
echo "🚀 Ready for distribution to lifetime subscribers!"

# Cleanup
deactivate
cd ..
rm -rf $LIFETIME_DIR
