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

- C++17
- [Conan](https://conan.io/) for managing dependencies

Conan will retrieve the following main dependencies:

- [docopt](https://github.com/docopt/docopt.cpp)
- [spdlog](https://github.com/gabime/spdlog)
- [opencv](https://github.com/opencv/opencv)
- [tesseract](https://github.com/tesseract-ocr/tesseract)

## Usage

Use cyberpunkd to load screenshots of the hacking mini-game and find the optimum solution.

You must specify the current buffer size of your in-game OS (this must be a numerical value), and the path to the folder containing your game screenshots.
If you are playing Cyberpunk 2077 through Steam, this will likely be: `<path to Steam folder>/Steam/userdata/<your steam ID>/760/remote/<game ID>/screenshots`

Tip: For improved OCR accuracy, set your Steam screenshots to keep an uncompressed copy and point cyberpunkd to this location instead. Also, when taking the screenshot, ensure your cursor is not obscuring any part of the grid or target sequences.

Once started, cyberpunkd will monitor your screenshots folder for any new images, and automatically solve them whenever they are detected.

For example:

```sh
# Point cyberpunkd at /path/to/screenshots and solve puzzles with a buffer size of 6
./build/cyberpunkd 6 /path/to/screenshots
```

When a new screenshot is detected, cyberpunkd will generate output like the following:

```sh
[2021-01-28 15:45:04.118] [info] Processing new screenshot: /path/to/screenshots/20210101120000_1.jpg
[2021-01-28 15:45:04.341] [info] Grid:

1C BD 55 E9 55
1C BD 1C 55 E9
55 E9 E9 BD BD
55 FF FF 1C 1C
FF E9 1C BD FF

[2021-01-28 15:45:04.482] [info] Target Sequences:

1C 1C 55
55 FF 1C
BD E9 BD 55
55 1C FF BD

[2021-01-28 15:45:04.574] [info] Generated 3029 candidate routes
[2021-01-28 15:45:04.575] [info] Refined 3029 candidates down to 9 optimal solution(s)
[2021-01-28 15:45:04.576] [info] Solve 1 of 4 target sequences ([0: 1C 1C 55]) in 3 moves:
[2021-01-28 15:45:04.576] [info]     0 -> 5 -> 8

[2021-01-28 15:45:04.576] [info] Solve 1 of 4 target sequences ([1: 55 FF 1C]) in 3 moves:
[2021-01-28 15:45:04.576] [info]     2 -> 17 -> 18

[2021-01-28 15:45:04.576] [info] Solve 2 of 4 target sequences ([0: 1C 1C 55, 1: 55 FF 1C]) in 5 moves:
[2021-01-28 15:45:04.576] [info]     4 -> 24 -> 22 -> 7 -> 8

[2021-01-28 15:45:04.576] [info] Solve 1 of 4 target sequences ([2: BD E9 BD 55]) in 4 moves:
[2021-01-28 15:45:04.576] [info]     1 -> 11 -> 13 -> 8

[2021-01-28 15:45:04.576] [info] Solve 2 of 4 target sequences ([0: 1C 1C 55, 2: BD E9 BD 55]) in 7 moves:
[2021-01-28 15:45:04.576] [info]     0 -> 5 -> 8 -> 13 -> 11 -> 1 -> 2

[2021-01-28 15:45:04.576] [info] Solve 2 of 4 target sequences ([1: 55 FF 1C, 2: BD E9 BD 55]) in 7 moves:
[2021-01-28 15:45:04.576] [info]     2 -> 17 -> 18 -> 13 -> 11 -> 1 -> 4

[2021-01-28 15:45:04.576] [info] Solve 1 of 4 target sequences ([3: 55 1C FF BD]) in 4 moves:
[2021-01-28 15:45:04.576] [info]     2 -> 22 -> 24 -> 14

[2021-01-28 15:45:04.576] [info] Solve 2 of 4 target sequences ([0: 1C 1C 55, 3: 55 1C FF BD]) in 6 moves:
[2021-01-28 15:45:04.576] [info]     0 -> 5 -> 8 -> 18 -> 16 -> 1

[2021-01-28 15:45:04.576] [info] Solve 2 of 4 target sequences ([2: BD E9 BD 55, 3: 55 1C FF BD]) in 7 moves:
[2021-01-28 15:45:04.576] [info]     1 -> 11 -> 13 -> 8 -> 5 -> 20 -> 23
```
