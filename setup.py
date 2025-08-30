from setuptools import setup, find_packages

with open("requirements.txt") as f:
    requirements = f.read().splitlines()

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="TitanVideo",
    version="1.0.0",
    author="TitanVideo Team",
    author_email="contact@titanvideo.ai",
    description="SOTA AI-powered Music Video Creator",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/tmanflan/TitanVideo",
    packages=find_packages(),
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: End Users/Desktop",
        "License :: OSI Approved :: BSD License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Topic :: Multimedia :: Video",
        "Topic :: Multimedia :: Sound/Audio :: Analysis",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
    ],
    python_requires=">=3.8",
    install_requires=requirements,
    entry_points={
        "console_scripts": [
            "titanvideo=TitanVideo:main",
        ],
    },
    include_package_data=True,
    package_data={
        "": ["*.html", "*.md", "*.txt"],
    },
    keywords="ai music video generator stable-diffusion whisper moviepy",
    project_urls={
        "Bug Reports": "https://github.com/tmanflan/TitanVideo/issues",
        "Source": "https://github.com/tmanflan/TitanVideo",
        "Documentation": "https://github.com/tmanflan/TitanVideo#readme",
    },
)
