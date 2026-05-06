CC = gcc

BREW_PREFIX ?= $(shell /opt/homebrew/bin/brew --prefix 2>/dev/null || brew --prefix 2>/dev/null || echo /opt/homebrew)
SDL2_PREFIX ?= $(BREW_PREFIX)/opt/sdl2
SDL2_TTF_PREFIX ?= $(BREW_PREFIX)/opt/sdl2_ttf
SDL2_IMAGE_PREFIX ?= $(BREW_PREFIX)/opt/sdl2_image

CFLAGS = -Wall -Wextra -g \
  -Iinclude \
  -I$(SDL2_PREFIX)/include/SDL2 \
  -I$(SDL2_TTF_PREFIX)/include/SDL2 \
  -I$(SDL2_IMAGE_PREFIX)/include/SDL2

LDFLAGS = \
  -L$(SDL2_PREFIX)/lib \
  -L$(SDL2_TTF_PREFIX)/lib \
  -L$(SDL2_IMAGE_PREFIX)/lib \
  -lSDL2 \
  -lSDL2_ttf -lSDL2_image

# Optional: for OpenBLAS later
# CFLAGS += -I/opt/homebrew/opt/openblas/include
# LDFLAGS += -L/opt/homebrew/opt/openblas/lib -lopenblas

TARGET = yukon
SRC = \
  src/main.c \
  src/ui/printer.c \
  src/game/deck.c \
  src/game/fileHandler.c \
  src/game/game.c \
  src/game/history.c \
  src/game/move.c \
  src/game/shuffler.c \
  src/game/stateHandler.c \
  src/game/timer.c \
  src/game/variables.c \
  src/gui/card_images.c \
  src/gui/gui.c \
  src/gui/gui_buttons.c \
  src/gui/gui_cards.c \
  src/gui/gui_columns.c \
  src/gui/gui_drag.c

OBJ = $(SRC:.c=.o)
BUILD_ARTIFACTS = $(TARGET) $(OBJ)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BUILD_ARTIFACTS)
	@echo "Preserved assets, decks, saves, and best_time.txt."
