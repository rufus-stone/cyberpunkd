#pragma once

namespace pnkd
{

static constexpr auto usage =
  R"(Welcome to cyberpunkd, a solution finder for the Cyberpunk 2077 breaching mini-game!

Use cyberpunkd to load a screenshot of the hacking mini-game and find the optimum solution
You must specify the <path> to the folder containing your game screenshots
If you are playing Cyberpunk 2077 through Steam, this will be: <path to Steam folder>/Steam/userdata/<your steam ID>/760/remote/<game ID>/screenshots

Usage:
  cyberpunkd <path> [options] [--verbose | --quiet]

Options:
  -h, --help              Show this screen
  -v, --version           Show app version
  -V, --verbose           Enable verbose logging (for debugging purposes - incompatible with quiet mode)
  -q, --quiet             Enable quiet mode. Only errors will be logged (incompatible with verbose mode)
  -t, --tessdata <path>   Path to the folder containing tesseract trained data
)";

} // namespace pnkd
