# Yukon GUI Version - macOS

## Requirements

- macOS
- Xcode Command Line Tools (`xcode-select --install`)
- Homebrew
- SDL2 libraries:

```sh
brew install sdl2 sdl2_ttf sdl2_image
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

The GUI version is written in C using SDL2, SDL2_ttf, and SDL2_image. The program uses relative paths, so keep the `assets` and `decks` folders in the same folder as the executable.

## Included Data Files

- `assets/` contains card images and suit graphics used by the GUI.
- `decks/` contains the test decks and default deck files.
- `best_time.txt` stores the best completion time.

The startup screen separates saved deck files from saved game state files.
