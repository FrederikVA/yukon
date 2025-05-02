CC = gcc

CFLAGS = -Wall -Wextra -g \
  `sdl2-config --cflags` \
  -I/opt/homebrew/opt/sdl2_ttf/include

LDFLAGS = \
  `sdl2-config --libs` \
  -L/opt/homebrew/opt/sdl2_ttf/lib \
  -lSDL2_ttf -lSDL2_image

# Optional: for OpenBLAS later
# CFLAGS += -I/opt/homebrew/opt/openblas/include
# LDFLAGS += -L/opt/homebrew/opt/openblas/lib -lopenblas

TARGET = yukon
SRC = main.c printer.c deck.c variables.c fileHandler.c stateHandler.c shuffler.c game.c move.c gui.c gui_buttons.c gui_columns.c gui_cards.c card_images.c gui_drag.c

OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
