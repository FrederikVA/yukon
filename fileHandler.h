#ifndef FILEHANDLER_H
#define FILEHANDLER_H

// Functions for file handling
void createDeckFile(const char *filename);
int countLinesInFile(const char *filename);
void loadBoardFromFile(const char *filename);
int saveDeckToFile(const char *filename);
int saveGameState(const char *filename);
int loadGameState(const char *filename);
int validateDeckFile(const char *filename);
void showDeckFiles();

void clearColumns();
void clearFoundations();
void reloadColumnsFromDeck();

int loadDeck(const char *filename);
void showDeck();

#endif
