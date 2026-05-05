CC = gcc

BREW_PREFIX ?= $(shell /opt/homebrew/bin/brew --prefix 2>/dev/null || brew --prefix 2>/dev/null || echo /opt/homebrew)
SDL2_PREFIX ?= $(BREW_PREFIX)/opt/sdl2
SDL2_TTF_PREFIX ?= $(BREW_PREFIX)/opt/sdl2_ttf
SDL2_IMAGE_PREFIX ?= $(BREW_PREFIX)/opt/sdl2_image

CFLAGS = -Wall -Wextra -g \
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
SRC = main.c printer.c deck.c variables.c fileHandler.c stateHandler.c shuffler.c game.c move.c history.c timer.c gui.c gui_buttons.c gui_columns.c gui_cards.c card_images.c gui_drag.c

OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
