CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = yukon
SRC = main.c printer.c deck.c variables.c fileHandler.c stateHandler.c shuffler.c game.c move.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o
