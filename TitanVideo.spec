# -*- mode: python ; coding: utf-8 -*-

block_cipher = None

a = Analysis(
    ['TitanVideo.py'],
    pathex=[],
    binaries=[],
    datas=[
        ('UI_Interface.html', '.'),
        ('requirements.txt', '.'),
        ('README.md', '.'),
        ('LICENSE', '.'),
    ],
    hiddenimports=[
        'customtkinter',
        'torch',
        'torchvision',
        'torchaudio',
        'diffusers',
        'transformers',
        'accelerate',
        'librosa',
        'PIL',
        'cv2',
        'moviepy',
        'whisper',
        'scipy',
        'numpy',
        'matplotlib',
        'seaborn',
        'pandas',
        'sklearn',
        'requests',
        'webrtcvad',
        'wave',
        'audioop',
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
    [],
    exclude_binaries=True,
    name='TitanVideo',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)

coll = COLLECT(
    exe,
    a.binaries,
    a.zipfiles,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='TitanVideo',
)
