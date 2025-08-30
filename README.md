# TitanVideo - Premium AI Music Video Creator

**⚠️ IMPORTANT: TitanVideo requires a paid subscription for full access. Free tier available with limited features.**

TitanVideo is a state-of-the-art AI-powered music video creation application that uses advanced machine learning models to generate professional-quality music videos from audio files and user inputs.

## 💰 Subscription Plans

TitanVideo operates on a subscription model with 4 premium tiers:

| Plan | Price | Video Generations | Resolution | Commercial Use |
|------|-------|-------------------|------------|----------------|
| **Silver** | $7.99/mo | 25/month | 720p | ❌ |
| **Gold** | $14.99/mo | 100/month | 1080p | ❌ |
| **Platinum** | $29.99/mo | 500/month | 4K | ✅ |
| **Diamond** | $49.99/mo | Unlimited | 8K | ✅ |

### Lifetime Access Option 🎯
For users seeking **permanent, unlimited access** without recurring fees:

**🏆 Lifetime Enterprise Access: $999 (One-time payment)**
- ✅ **Never expires** - permanent access
- ✅ **All features unlocked** - including future updates
- ✅ **No monthly fees** - pay once, use forever
- ✅ **Priority support** - lifetime premium support
- ✅ **Commercial license** - full commercial usage rights

*Limited availability - contact sales@titanvideo.ai for lifetime access*

### Payment
Scan the QR code to subscribe:

![Subscription QR Code](qrcode.png)

**Payment Link:** https://titanvideo.ai/subscribe

---

## 👑 Lifetime Subscriber Installation

Lifetime subscribers receive a **premium installer** with pre-activated enterprise license:

### Building Lifetime Installer
```bash
# Quick build (auto-detects platform)
./build_lifetime.sh

# Or use platform-specific scripts:
# Linux/macOS
./build_lifetime_installer.sh

# Windows
build_lifetime_installer.bat
```

### Lifetime Installer Features
- 🎯 **Pre-activated License** - No setup required
- 📦 **Premium Branding** - Enhanced installer experience
- 🎁 **Welcome Package** - Comprehensive setup guide
- 🔧 **Auto Shortcuts** - Desktop and start menu integration
- 📞 **Priority Support** - Direct support contact information

### Distribution
Lifetime installers are distributed via:
- 📧 **Email delivery** after payment confirmation
- 💾 **Secure download link** with unique access code
- 📦 **Physical media** available for enterprise clients

See [LIFETIME_SUBSCRIBER_README.md](LIFETIME_SUBSCRIBER_README.md) for detailed lifetime subscriber guide.

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

### 1. Subscribe
Visit https://titanvideo.ai/subscribe or scan the QR code above to choose your plan.

### 2. Download & Install
```bash
# Download the installer for your platform
# Run the installer and follow setup instructions
```

### 3. Launch & Create
```bash
# Launch TitanVideo
# Select your audio file
# Choose AI models and settings
# Generate your music video!
```

## 📋 System Requirements

- **OS**: Windows 10+, macOS 10.15+, Ubuntu 18.04+
- **RAM**: 8GB minimum, 16GB recommended
- **Storage**: 5GB free space
- **GPU**: NVIDIA GPU with CUDA support (recommended)
- **Internet**: Required for license verification and AI processing

## 🔒 License & Copyright

**This software is proprietary and protected by copyright.**

- **Free Tier**: Limited personal use (5 videos/month)
- **Commercial Use**: Requires Platinum or Diamond subscription
- **Distribution**: Strictly prohibited without written permission
- **Reverse Engineering**: Not permitted

See [LICENSE](LICENSE) and [COPYRIGHT](COPYRIGHT.md) for full terms.

## �️ Development

### Prerequisites
- Python 3.8+
- CUDA-compatible GPU (recommended)
- Valid TitanVideo subscription

### Installation
```bash
git clone https://github.com/tmanflan/TitanVideo.git
cd TitanVideo
pip install -r requirements.txt
```

### Building Desktop Installer
```bash
# Linux/macOS
./build_installer.sh

# Windows
build_installer.bat
```

## 🤝 Contributing

**Note: Contributions require signed contributor agreement and are subject to license terms.**

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📞 Support

- **Free Tier**: Community forum support
- **Paid Plans**: Email support within 24 hours
- **Enterprise**: Dedicated account manager
- **Documentation**: docs.titanvideo.ai

## ⚖️ Legal

- **License**: Proprietary - See [LICENSE](LICENSE)
- **Copyright**: © 2025 Raymond Flanary - See [COPYRIGHT](COPYRIGHT.md)
- **Privacy**: Data collected for service provision and license enforcement
- **Terms**: Available at https://titanvideo.ai/terms

## 🙏 Acknowledgments

- OpenAI for Whisper model
- Stability AI for Stable Diffusion
- Hugging Face for model hosting
- MoviePy community for video processing

---

**TitanVideo - Where AI Meets Music Video Creation** 🎬✨

*Prices in USD. Subscription required for commercial use.*
- 🎬 **Video Synthesis**: Professional video creation with MoviePy
- 🎭 **Advanced Effects**: Real-time video effects and post-processing
- 🌐 **Web Interface**: Modern web-based UI for easy interaction
- 🖥️ **Desktop App**: Native desktop application with installer
- 🚀 **SOTA AI Models**: Integration with latest AI models (Stable Diffusion, AnimateDiff, etc.)

## 📦 Desktop Installers

The project includes automated build scripts for creating native desktop installers:

- **Windows**: MSI installer using WiX Toolset
- **macOS**: DMG package
- **Linux**: AppImage

## 🏗️ Architecture

- **Frontend**: CustomTkinter GUI + WebSocket interface
- **Backend**: Python with AI models (Stable Diffusion, Whisper, etc.)
- **Video Processing**: OpenCV + MoviePy
- **Audio Analysis**: Librosa + SciPy
- **AI Models**: Hugging Face Transformers + Diffusers

## 🤖 AI Models Used (All FREE)

- **Whisper** - Open-source speech recognition (no API costs)
- **Stable Diffusion v1.5** - Open-source image generation
- **AnimateDiff** - Open-source video generation
- **DialoGPT** - Open-source text generation for script creation

## 🎯 Usage

1. **Launch the app** - Either the desktop version or run `python TitanVideo.py`
2. **Select audio file** - Choose your music file (MP3, WAV, etc.)
3. **Enter metadata** - Song title, artist, album information
4. **Configure settings** - Video resolution, style, effects
5. **Generate video** - Click generate and watch the AI create your music video!
6. **Export** - Save your video in multiple formats

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
├── FREE_USAGE.md         # Detailed free usage guide
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

- OpenAI for the open-source Whisper model
- Stability AI for Stable Diffusion
- Hugging Face for model hosting
- MoviePy community for video processing
- All contributors and users

---

**Made with ❤️ using cutting-edge AI technology - 100% FREE!**

*All AI models and processing run locally on your hardware. No data is sent to external servers.*

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