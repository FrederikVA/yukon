#ifndef FILEHANDLER_H
#define FILEHANDLER_H

// Functions for file handling
void createDeckFile(const char *filename);
int countLinesInFile(const char *filename);
void loadBoardFromFile(const char *filename);
void saveDeckToFile(const char *filename);
void reloadColumnsFromDeck();

void loadDeck(const char *filename);
void showDeck();

#endif
