# Yukon Executable - macOS

## Platform

macOS

## Requirements

This executable is dynamically linked against Homebrew SDL2 libraries. On another Mac, install:

```sh
brew install sdl2 sdl2_ttf sdl2_image
```

## Run Terminal Version

```sh
./yukon
```

## Run GUI Version

```sh
./yukon --gui
```

## Included Folders

- `assets/`: GUI images
- `decks/`: deck files and test decks
- `saves/`: saved game states
- `best_time.txt`: best completion time

Run the executable from this folder so all relative paths work correctly.
