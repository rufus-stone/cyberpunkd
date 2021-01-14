# cyberpunkd
Cyberpunk 2077 hacking mini-game solver

![Build and Test](https://github.com/rufus-stone/cyberpunkd/workflows/Build%20and%20Test/badge.svg)

## Motivation

Because the real hacking mini-game is hacking the hacking mini-game.

## Installation

The simplest way to build from source is using CMake (version 3.15 or above is required). For example:

```sh
git clone git@github.com:rufus-stone/cyberpunkd.git

cd cyberpunkd

# Get CMake to create a new build directory
cmake -S . -B build

# Build the app
cmake --build build
```

### Requirements

cyberpunkd uses Conan to manage dependencies, so this must be installed on your system. Conan will retrieve the following main dependencies:

- [docopt](https://github.com/docopt/docopt.cpp)
- [spdlog](https://github.com/gabime/spdlog)
- [opencv](https://github.com/opencv/opencv)
- [tesseract](https://github.com/tesseract-ocr/tesseract)

## Usage

Use cyberpunkd to load a screenshot of the hacking mini-game and find the optimum solution.
You must specify the current buffer size of your in-game OS (this must be a numerical value), and the path to the folder containing your game screenshots.
If you are playing Cyberpunk 2077 through Steam, this will likely be: `<path to Steam folder>/Steam/userdata/<your steam ID>/760/remote/<game ID>/screenshots`
Tip: For improved OCR accuracy, set your Steam screenshots to keep an uncompressed copy and point cyberpunkd to this location instead.

For example:

```sh
# Point cyberpunkd at /path/to/screenshots and solve the puzzle with a buffer size of 6
./build/cyberpunkd 6 /path/to/screenshots
```