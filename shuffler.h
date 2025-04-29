#ifndef SHUFFLER_H
#define SHUFFLER_H

// Shuffles the deck randomly (SR command)
void randomShuffle();

// Shuffles the deck using a riffle shuffle, split at a given point (SI command)
void riffleShuffle(int split);

// Debbuging print
void printDeckDebug();
int countDeck();

#endif
