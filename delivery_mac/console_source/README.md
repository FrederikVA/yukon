# Yukon Console Version - macOS

## Requirements

- macOS
- Xcode Command Line Tools (`xcode-select --install`)
- Homebrew
- SDL2 libraries, because this project uses one shared C codebase for both terminal and GUI modes:

```sh
brew install sdl2 sdl2_ttf sdl2_image
```

## Project Structure

```text
src/
  main.c
  game/      core game logic, file handling, moves, shuffle, timer
  gui/       SDL GUI implementation
  ui/        terminal printing
include/    header files
assets/     GUI image assets
decks/      deck files and test decks
saves/      saved game states
tests/      smoke tests
```

## Build

From this folder:

```sh
make
```

## Run Console Version

```sh
./yukon
```

The console version is the default mode. The program uses relative paths, so keep `decks`, `saves`, `assets`, and `best_time.txt` next to the executable.

## Saved Games And Decks

- Deck files are read from and written to `decks/`.
- Game states are saved to and loaded from `saves/`.
- The required test decks are included in `decks/`:
  - `cards51.txt`
  - `default.txt`
  - `dup.txt`
  - `testfile.txt`

Invalid decks are rejected with error messages for duplicates, missing cards, invalid card codes, or incorrect card counts.

## Smoke Test

After building:

```sh
tests/requirements_smoke.sh
```
