# TitanVideo - SOTA Music Video Creator

TitanVideo is a state-of-the-art AI-powered music video creation application that uses advanced machine learning models to generate professional-quality music videos from audio files and user inputs.

## ✨ Features

- 🎵 **AI Audio Analysis**: Real-time audio feature extraction using librosa and beat detection
- 🎤 **Speech-to-Text**: Automatic lyrics transcription using OpenAI Whisper
- 🎨 **AI Image Generation**: High-quality image generation using Stable Diffusion
- 🎬 **Video Synthesis**: Professional video creation with MoviePy
- 🎭 **Advanced Effects**: Real-time video effects and post-processing
- 🌐 **Web Interface**: Modern web-based UI for easy interaction
- 🖥️ **Desktop App**: Native desktop application with installer
- 🚀 **SOTA AI Models**: Integration with latest AI models (Stable Diffusion, AnimateDiff, etc.)

## 🚀 Quick Start

### Desktop Installation

1. **Download the installer** for your platform from the releases page
2. **Run the installer** and follow the setup wizard
3. **Launch TitanVideo** from your applications menu

### Manual Installation

#### Prerequisites
- Python 3.8+
- CUDA-compatible GPU (recommended for AI models)

#### Installation Steps
```bash
# Clone the repository
git clone https://github.com/tmanflan/TitanVideo.git
cd TitanVideo

# Install dependencies
pip install -r requirements.txt

# Run the application
python TitanVideo.py
```

#### Building Desktop Installer
```bash
# Linux/macOS
./build_installer.sh

# Windows
build_installer.bat
```

## 🎯 Usage

1. **Launch the app** - Either the desktop version or run `python TitanVideo.py`
2. **Select audio file** - Choose your music file (MP3, WAV, etc.)
3. **Enter metadata** - Song title, artist, album information
4. **Configure settings** - Video resolution, style, effects
5. **Generate video** - Click generate and watch the AI create your music video!
6. **Export** - Save your video in multiple formats

## 🏗️ Architecture

- **Frontend**: CustomTkinter GUI + WebSocket interface
- **Backend**: Python with AI models (Stable Diffusion, Whisper, etc.)
- **Video Processing**: OpenCV + MoviePy
- **Audio Analysis**: Librosa + SciPy
- **AI Models**: Hugging Face Transformers + Diffusers

## 🤖 AI Models Used

- **Stable Diffusion v1.5**: Image generation from text prompts
- **AnimateDiff**: Video generation from images
- **Whisper**: Speech recognition and transcription
- **DialoGPT**: Text generation for script creation

## 📦 Desktop Installers

The project includes automated build scripts for creating native desktop installers:

- **Windows**: MSI installer using WiX Toolset
- **macOS**: DMG package
- **Linux**: AppImage

## 🛠️ Development

### Project Structure
```
TitanVideo/
├── TitanVideo.py          # Main application
├── TitanVideo.spec        # PyInstaller spec file
├── build_installer.sh     # Linux/macOS build script
├── build_installer.bat    # Windows build script
├── requirements.txt       # Python dependencies
├── UI_Interface.html      # Web interface
├── SIMOD.cpp             # C++ AI models
├── Tvid.cpp              # C++ backend
└── README.md             # This file
```

### Building from Source
```bash
# Install Python dependencies
pip install -r requirements.txt

# For C++ components (optional)
# Requires OpenCV, FFmpeg, WebSocket++
make
```

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is licensed under the BSD 3-Clause License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- OpenAI for Whisper model
- Stability AI for Stable Diffusion
- Hugging Face for model hosting
- MoviePy community for video processing
- All contributors and users

---

**Made with ❤️ using cutting-edge AI technology**