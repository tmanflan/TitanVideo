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
