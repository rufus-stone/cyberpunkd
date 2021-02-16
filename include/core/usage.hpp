#pragma once

namespace pnkd
{

static constexpr auto usage =
  R"(Welcome to cyberpunkd, a solution finder for the Cyberpunk 2077 Breach Protocol mini-game!
  Because the real hacking mini-game is hacking the hacking mini-game.

Use cyberpunkd to load a screenshot of the hacking mini-game and find the optimum solution
You must specify the current <buffer_size> of your in-game OS (this must be a numerical value), and the <path> to the folder containing your game screenshots
If you are playing Cyberpunk 2077 through Steam, this will likely be: <path to Steam folder>/Steam/userdata/<your steam ID>/760/remote/<game ID>/screenshots
For improved OCR accuracy, set your Steam screenshots to keep an uncompressed copy and point cyberpunkd to this location instead.

Usage:
  cyberpunkd <buffer_size> <path> [options] [--verbose | --quiet]

Options:
  -h, --help              Show this screen
  -v, --version           Show app version
  -V, --verbose           Enable verbose logging (for debugging purposes - incompatible with quiet mode)
  -q, --quiet             Enable quiet mode. Only errors will be logged (incompatible with verbose mode)
  -t, --tessdata <path>   Path to the folder containing tesseract trained data
  -s, --sleep <seconds>   How many seconds to wait before checking for new screenshots (must be a number)
  -l, --headless          Run in headless mode (no GUI)
)";

} // namespace pnkd
