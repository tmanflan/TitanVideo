#!/bin/bash

# Test script for TitanVideo desktop build
echo "🧪 Testing TitanVideo Desktop Build"
echo "==================================="

# Check if Python is available
if ! command -v python &> /dev/null; then
    echo "❌ Python not found"
    exit 1
fi

echo "✅ Python found: $(python --version)"

# Check if pip is available
if ! command -v pip &> /dev/null; then
    echo "❌ pip not found"
    exit 1
fi

echo "✅ pip found"

# Check if required files exist
files=("TitanVideo.py" "requirements.txt" "UI_Interface.html" "README.md")
for file in "${files[@]}"; do
    if [ ! -f "$file" ]; then
        echo "❌ $file not found"
        exit 1
    fi
done

echo "✅ All required files found"

# Test importing main modules
echo "🔍 Testing imports..."
python -c "
try:
    import tkinter
    print('✅ tkinter available')
except ImportError:
    print('⚠️  tkinter not available (expected on some systems)')

try:
    import customtkinter
    print('✅ customtkinter available')
except ImportError:
    print('❌ customtkinter not available')

try:
    import torch
    print('✅ torch available')
except ImportError:
    print('❌ torch not available')

try:
    import diffusers
    print('✅ diffusers available')
except ImportError:
    print('❌ diffusers not available')
"

echo ""
echo "🎯 To build the desktop installer:"
echo "   Linux/macOS: ./build_installer.sh"
echo "   Windows: build_installer.bat"
echo ""
echo "📦 The installer will create:"
echo "   - Linux: TitanVideo.AppImage"
echo "   - Windows: TitanVideo.msi or TitanVideo_Windows.zip"
echo "   - macOS: TitanVideo.dmg"
