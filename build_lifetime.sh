#!/bin/bash

# TitanVideo Lifetime Installer Quick Build Script
# Run this to build the lifetime subscriber installer

echo "🏗️  Building TitanVideo Lifetime Installer..."
echo "============================================="

# Check platform and run appropriate build script
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "🐧 Linux detected - building AppImage..."
    ./build_lifetime_installer.sh

elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "🍎 macOS detected - building DMG..."
    ./build_lifetime_installer.sh

elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    echo "🪟 Windows detected - building MSI/ZIP..."
    ./build_lifetime_installer.bat

else
    echo "❌ Unsupported platform: $OSTYPE"
    echo "Supported platforms: Linux, macOS, Windows"
    exit 1
fi

echo ""
echo "✅ Build process complete!"
echo "📁 Check the 'dist' directory for your lifetime installer"
echo ""
echo "🎯 Ready to distribute to lifetime subscribers!"
