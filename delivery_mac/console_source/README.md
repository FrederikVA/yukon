# Yukon Console Version - macOS

## Requirements

- macOS
- Xcode Command Line Tools (`xcode-select --install`)
- Homebrew
- SDL2 libraries, because this project uses one shared codebase for both terminal and GUI modes:

```sh
brew install sdl2 sdl2_ttf sdl2_image
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

The console version is the default mode. The program uses relative paths for decks and saved data, so keep the `decks` folder in the same folder as the executable.

## Test Decks

The included `decks` folder contains the required test decks:

- `cards51.txt`
- `default.txt`
- `dup.txt`
- `testfile.txt`

Invalid decks are rejected with error messages for duplicates, missing cards, invalid card codes, or incorrect card counts.
