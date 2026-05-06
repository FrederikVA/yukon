# Yukon GUI Version - macOS

## Requirements

- macOS
- Xcode Command Line Tools (`xcode-select --install`)
- Homebrew
- SDL2 libraries:

```sh
brew install sdl2 sdl2_ttf sdl2_image
```

## Project Structure

```text
src/
  main.c
  game/      shared rules, deck loading, save/load, moves, shuffle, timer
  gui/       SDL window, drawing, buttons, saved lists, drag/drop
  ui/        terminal output
include/    header files
assets/     card and suit images used by the GUI
decks/      deck files and test decks
saves/      saved game states
tests/      terminal smoke tests
```

## Build

From this folder:

```sh
make
```

## Run GUI Version

```sh
./yukon --gui
```

The GUI version is written in C using SDL2, SDL2_ttf, and SDL2_image. The program uses relative paths, so keep `assets`, `decks`, `saves`, and `best_time.txt` next to the executable.

## Saved Games And Decks

- `SAVED DECKS` are deck files from `decks/`.
- `SAVED GAMES` are game-state files from `saves/`.
- Shuffling or saving a game does not overwrite the included test decks.

## Smoke Test

After building:

```sh
tests/requirements_smoke.sh
```
