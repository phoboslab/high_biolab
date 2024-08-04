# Biolab Disaster

This is the source code for the 2d platformer Biolab Disaster, using the [high_impact](https://github.com/phoboslab/high_impact) game engine.

Play the WASM version here: https://phoboslab.org/high_impact/biolab

More about high_impact in my Blog [Porting my JavaScript Game Engine to C for No Reason](https://phoboslab.org/log/2024/08/high_impact)

## Building

A Makefile that converts all assets and compiles with different platforms and
renderers is provided. See the first few lines in `Makefile` for more info.

### All
1. Clone this repository
2. `git submodule update --init`

### Windows Sokol
1. Download and install msys2: https://www.msys2.org/
2. start the msys2 ucrt64 shell
3. install gcc: `pacman -S mingw-w64-ucrt-x86_64-gcc`
5. install make: `pacman -S make`
6. `make sokol`
7. run `build/game_sokol.exe`

### Windows SDL2
1. Download and install msys2: https://www.msys2.org/
2. start the msys2 ucrt64 shell
3. install gcc: `pacman -S mingw-w64-ucrt-x86_64-gcc`
5. install make: `pacman -S make`
6. install sdl2: `pacman -S mingw-w64-ucrt-x86_64-SDL2`
7. `make sdl`
8. copy `C:\msys64\ucrt64\bin\SDL2.dll` into `build` and run `build/game_sdl.exe`

### macOS Sokol
1. `make sokol`
2. run `build/game_sokol`

### macOS SDL2
1. Install homebrew: https://brew.sh/
2. install sdl2: `brew install sdl2`
3. `make sdl`
4. run `build/game_sdl`

### Debian/Ubuntu Sokol
1. `apt install libx11-dev libxcursor-dev libxi-dev libasound2-dev`
2. `make sokol`
3. run `build/game_sokol`

### Debian/Ubuntu SDL2
1. `apt install libsdl2-dev`
2. `make sdl`
3. run `build/game_sdl`

### Arch Sokol
1. `pacman install libx11 libxcursor libxi alsa-lib`
2. `make sokol`
3. run `build/game_sokol`

### Arch SDL2
1. `pacman -S sdl2`
2. `make sdl`
3. run `build/game_sdl`

### Fedora Sokol
1. `dnf install libx11-devel libxcursor-devel libxi-devel alsa-lib-devel`
2. `make sokol`
3. run `build/game_sokol`

### Fedora SDL2
1. `dnf install SDL2-devel`
2. `make sdl`
3. run `build/game_sdl`

### OpenSUSE Sokol
1. `zypper install libx11-devel libxcursor-devel libxi-devel alsa-lib-devel`
2. `make sokol`
3. run `build/game_sokol`

### OpenSUSE SDL2
1. `zypper install SDL2-devel`
2. `make sdl`
3. run `build/game_sdl`

### Emscripten
1. Install and activate `emsdk`
2. `make wasm`
3. copy `build/wasm/*` to your webserver


## License

All high_impact and game code is MIT Licensed, though some of the libraries (`high_impact/libs/`) come with their own (permissive) license. Check the header files.

**Note that the assets (sound, music, graphics) don't come with any license. You cannot use those in your own games**
