#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "structures.h"
#include "variables.h"
#include "shuffler.h"
#include "deck.h"
#include "timer.h"

extern const char ranks[];
extern const char suits[];

int validateDeckFile(const char *filename);
void clearColumns();
void clearFoundations();
void reloadColumnsFromDeck();

static char lastFileError[200] = "";

static void setLastFileError(const char *format, const char *value) {
    snprintf(lastFileError, sizeof(lastFileError), format, value);
}

static void setLastFileCountError(int count) {
    snprintf(lastFileError, sizeof(lastFileError),
             "Deck does not contain exactly 52 cards (found %d).", count);
}

const char *getLastFileError(void) {
    return lastFileError;
}

void createDeckFile(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Error creating deck file.\n");
        exit(1);
    }

    for (int s = 0; s < 4; s++) {
        for (int r = 0; r < 13; r++) {
            fprintf(f, "%c%c\n", ranks[r], suits[s]);
        }
    }
    fclose(f);
}

int countLinesInFile(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;

    int count = 0;
    char line[10];
    while (fgets(line, sizeof(line), f)) {
        count++;
    }
    fclose(f);
    return count;
}

// Appends to a pile represented as a singly linked list while preserving card order.
static void appendCard(Pile *pile, Card *card) {
    if (pile->top == NULL) {
        pile->top = card;
        return;
    }

    Card *tail = pile->top;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = card;
}

// Copies a card node so deck, columns, foundations, and saved states do not share mutable list nodes.
static Card *copyCard(Card *source) {
    Card *copy = (Card *)malloc(sizeof(Card));
    if (!copy) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    copy->rank = source->rank;
    copy->suit = source->suit;
    copy->face_up = source->face_up;
    copy->next = NULL;
    return copy;
}

static void setCurrentFileName(const char *filename) {
    int len = (int)strlen(filename);

    if (len >= 4 && strcmp(filename + len - 4, ".txt") == 0) {
        snprintf(currentFile, sizeof(currentFile), "%s", filename);
    } else {
        snprintf(currentFile, sizeof(currentFile), "%s.txt", filename);
    }
}

// Game-state extension: saved games are kept under saves/ using relative paths for delivery portability.
static void buildStatePath(char *path, int pathSize, const char *filename) {
    int len = (int)strlen(filename);

    if (len >= 4 && strcmp(filename + len - 4, ".txt") == 0) {
        snprintf(path, pathSize, "saves/%s", filename);
    } else {
        snprintf(path, pathSize, "saves/%s.txt", filename);
    }
}

static void buildLegacyStatePath(char *path, int pathSize, const char *filename) {
    int len = (int)strlen(filename);

    if (len >= 4 && strcmp(filename + len - 4, ".txt") == 0) {
        snprintf(path, pathSize, "%s", filename);
    } else {
        snprintf(path, pathSize, "%s.txt", filename);
    }
}

static int ensureSaveDirectory(void) {
    struct stat st;

    if (stat("saves", &st) == 0) {
        return S_ISDIR(st.st_mode);
    }

    return mkdir("saves", 0755) == 0;
}

static int ensureDeckDirectory(void) {
    struct stat st;

    if (stat("decks", &st) == 0) {
        return S_ISDIR(st.st_mode);
    }

    return mkdir("decks", 0755) == 0;
}

static int countPileCards(Pile *pile) {
    int count = 0;
    Card *current = pile->top;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

static void writePile(FILE *f, Pile *pile) {
    Card *current = pile->top;
    fprintf(f, "%d\n", countPileCards(pile));
    while (current != NULL) {
        fprintf(f, "%c%c %d\n", current->rank, current->suit, current->face_up);
        current = current->next;
    }
}

// Reads one saved pile back as a linked list, including each card's face-up/face-down state.
static int readPile(FILE *f, Pile *pile) {
    int count = 0;
    if (fscanf(f, "%d", &count) != 1 || count < 0 || count > 52) {
        return 0;
    }

    pile->top = NULL;
    for (int i = 0; i < count; i++) {
        char cardCode[3];
        int faceUp = 0;
        if (fscanf(f, "%2s %d", cardCode, &faceUp) != 2) {
            return 0;
        }
        if (!strchr("A23456789TJQK", cardCode[0]) ||
            !strchr("CDHS", cardCode[1]) ||
            cardCode[2] != '\0' ||
            (faceUp != 0 && faceUp != 1)) {
            return 0;
        }

        Card source;
        source.rank = cardCode[0];
        source.suit = cardCode[1];
        source.face_up = faceUp;
        source.next = NULL;
        appendCard(pile, copyCard(&source));
    }

    return 1;
}

// Loads a validated deck file into the deck linked list with every card initially face down.
static int loadDeckFromFile(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        return 0;
    }

    char buffer[10];
    Card *newDeck = NULL;
    Card *tail = NULL;

    while (fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\n")] = 0;

        Card *new_card = (Card *)malloc(sizeof(Card));
        if (!new_card) {
            printf("Memory allocation failed!\n");
            exit(1);
        }
        new_card->rank = buffer[0];
        new_card->suit = buffer[1];
        new_card->face_up = 0;
        new_card->next = NULL;

        if (newDeck == NULL) {
            newDeck = new_card;
        } else {
            tail->next = new_card;
        }
        tail = new_card;
    }

    fclose(f);
    clearDeck();
    deck = newDeck;
    return 1;
}

// STARTUP display helper: lays the deck into 7 columns row-wise, still using linked-list piles.
void loadBoardFromFile(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Failed to open deck file. Did you load in a deck?\n");
        return;
    }

    clearColumns();

    char buffer[10];
    int card_count = 0;
    while (fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\n")] = 0;

        Card source;
        source.rank = buffer[0];
        source.suit = buffer[1];
        source.face_up = 0;
        source.next = NULL;

        appendCard(&columns[card_count % 7], copyCard(&source));
        card_count++;
    }

    fclose(f);
}

// Implements LD: load a named deck from decks/ or create the default ordered deck when no name is given.
int loadDeck(const char *filename) {
    clearColumns();
    clearFoundations();
    lastFileError[0] = '\0';

    if (filename != NULL && strcmp(filename, "") != 0) {
        setCurrentFileName(filename);
    } else if (filename == NULL) {
        strcpy(currentFile, "cards.txt");
        createDefaultDeck();
        reloadColumnsFromDeck();
        return 1;
    }

    char fullPath[200];
    snprintf(fullPath, sizeof(fullPath), "decks/%s", currentFile);

    FILE *f = fopen(fullPath, "r");
    if (!f) {
        printf("Error: Deck file does not exist: %s\n", fullPath);
        snprintf(lastFileError, sizeof(lastFileError),
                 "Deck file does not exist: %s", fullPath);
        return 0;
    }
    fclose(f);

    if (!validateDeckFile(fullPath)) {
        return 0;
    }

    if (!loadDeckFromFile(fullPath)) {
        printf("Error: Could not load deck file: %s\n", fullPath);
        return 0;
    }
    loadBoardFromFile(fullPath);
    return 1;
}

// Implements SW by turning the startup column cards face up without changing deck order.
void showDeck() {
    // Turn all cards face up
    for (int i = 0; i < 7; i++) {
        Card *current = columns[i].top;
        while (current) {
            current->face_up = 1;
            current = current->next;
        }
    }
}

void showDeckFiles() {
    struct dirent *entry;
    DIR *dp = opendir("decks");

    if (dp == NULL) {
        printf("Error: Could not open decks folder.\n");
        return;
    }

    printf("Available decks:\n");
    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) { // Regular file
            printf(" - %s\n", entry->d_name);
        }
    }

    closedir(dp);
}

// Implements SD: writes the current 52-card deck to decks/ as one rank+suit code per line.
int saveDeckToFile(const char *filename) {
    if (deck == NULL) {
        printf("No deck to save.\n");
        return 0;
    }

    if (countDeck() != 52) {
        printf("Deck is not valid (not 52 cards). Save cancelled.\n");
        return 0;
    }

    char fullPath[200];
    if (filename != NULL && strcmp(filename, "") != 0) {
        setCurrentFileName(filename);
    }
    snprintf(fullPath, sizeof(fullPath), "decks/%s", currentFile);

    if (!ensureDeckDirectory()) {
        printf("Failed to create decks directory.\n");
        return 0;
    }

    FILE *f = fopen(fullPath, "w");
    if (!f) {
        printf("Failed to open file for writing: %s\n", fullPath);
        return 0;
    }

    Card *current = deck;
    while (current != NULL) {
        fprintf(f, "%c%c\n", current->rank, current->suit);
        current = current->next;
    }

    fclose(f);
    printf("Deck saved to file: %s\n", fullPath);
    return 1;
}

// Extension command S: saves the full game state, not just the deck, so play can resume exactly.
int saveGameState(const char *filename) {
    char path[200];
    buildStatePath(path, sizeof(path), filename);

    if (!ensureSaveDirectory()) {
        printf("Failed to create saves directory.\n");
        return 0;
    }

    FILE *f = fopen(path, "w");
    if (!f) {
        printf("Failed to open file for writing: %s\n", path);
        return 0;
    }

    fprintf(f, "YUKON_STATE 1\n");
    fprintf(f, "PHASE %d\n", currentPhase == PLAY ? 1 : 0);
    fprintf(f, "ELAPSED %ld\n", getGameElapsed());
    fprintf(f, "DECK %d\n", countDeck());
    Card *current = deck;
    while (current != NULL) {
        fprintf(f, "%c%c %d\n", current->rank, current->suit, current->face_up);
        current = current->next;
    }

    fprintf(f, "COLUMNS\n");
    for (int i = 0; i < 7; i++) {
        writePile(f, &columns[i]);
    }

    fprintf(f, "FOUNDATIONS\n");
    for (int i = 0; i < 4; i++) {
        writePile(f, &foundations[i]);
    }

    fclose(f);
    printf("Game state saved to file: %s\n", path);
    return 1;
}

// Extension command L: restores deck, columns, foundations, timer, and phase from a saved state file.
int loadGameState(const char *filename) {
    char path[200];
    char marker[20];
    int version = 0;
    int phaseValue = 0;
    long elapsed = 0;
    int deckCount = 0;

    buildStatePath(path, sizeof(path), filename);

    FILE *f = fopen(path, "r");
    if (!f) {
        char legacyPath[200];
        buildLegacyStatePath(legacyPath, sizeof(legacyPath), filename);
        f = fopen(legacyPath, "r");
        if (!f) {
            printf("Error: Game state file does not exist: %s\n", path);
            return 0;
        }
        snprintf(path, sizeof(path), "%s", legacyPath);
    }

    if (fscanf(f, "%19s %d", marker, &version) != 2 ||
        strcmp(marker, "YUKON_STATE") != 0 ||
        version != 1) {
        fclose(f);
        return 0;
    }

    if (fscanf(f, "%19s %d", marker, &phaseValue) != 2 ||
        strcmp(marker, "PHASE") != 0 ||
        (phaseValue != 0 && phaseValue != 1)) {
        fclose(f);
        return 0;
    }

    if (fscanf(f, "%19s %ld", marker, &elapsed) != 2 ||
        strcmp(marker, "ELAPSED") != 0 ||
        elapsed < 0) {
        fclose(f);
        return 0;
    }

    if (fscanf(f, "%19s %d", marker, &deckCount) != 2 ||
        strcmp(marker, "DECK") != 0 ||
        deckCount < 0 ||
        deckCount > 52) {
        fclose(f);
        return 0;
    }

    clearDeck();
    Card *deckTail = NULL;
    for (int i = 0; i < deckCount; i++) {
        char cardCode[3];
        int faceUp = 0;
        if (fscanf(f, "%2s %d", cardCode, &faceUp) != 2) {
            fclose(f);
            clearDeck();
            return 0;
        }

        Card source;
        source.rank = cardCode[0];
        source.suit = cardCode[1];
        source.face_up = faceUp;
        source.next = NULL;
        Card *copy = copyCard(&source);
        if (deck == NULL) {
            deck = copy;
        } else {
            deckTail->next = copy;
        }
        deckTail = copy;
    }

    if (fscanf(f, "%19s", marker) != 1 || strcmp(marker, "COLUMNS") != 0) {
        fclose(f);
        return 0;
    }

    clearColumns();
    clearFoundations();
    for (int i = 0; i < 7; i++) {
        if (!readPile(f, &columns[i])) {
            fclose(f);
            clearColumns();
            clearFoundations();
            return 0;
        }
    }

    if (fscanf(f, "%19s", marker) != 1 || strcmp(marker, "FOUNDATIONS") != 0) {
        fclose(f);
        clearColumns();
        clearFoundations();
        return 0;
    }

    for (int i = 0; i < 4; i++) {
        if (!readPile(f, &foundations[i])) {
            fclose(f);
            clearColumns();
            clearFoundations();
            return 0;
        }
    }

    fclose(f);
    currentPhase = phaseValue ? PLAY : STARTUP;
    setGameElapsed(elapsed, currentPhase == PLAY);
    printf("Game state loaded from file: %s\n", path);
    return 1;
}

// Requirement for LD <filename>: reject invalid card codes, duplicates, and decks not containing 52 cards.
int validateDeckFile(const char *filename) {
    const char *validRanks = "A23456789TJQK";
    const char *validSuits = "CDHS"; // Clubs, Diamonds, Hearts, Spades

    int cardSeen[13][4] = {0}; // [rank][suit] table

    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error opening file for validation: %s\n", filename);
        snprintf(lastFileError, sizeof(lastFileError),
                 "Error opening file for validation: %s", filename);
        return 0;
    }

    char line[10];
    int lineCount = 0;

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0; // Remove newline
        if (strlen(line) != 2) {
            printf("Invalid card format: '%s'\n", line);
            setLastFileError("Invalid card format: '%s'", line);
            fclose(f);
            return 0;
        }

        char rank = line[0];
        char suit = line[1];

        const char *rankPtr = strchr(validRanks, rank);
        const char *suitPtr = strchr(validSuits, suit);

        if (!rankPtr || !suitPtr) {
            printf("Invalid card detected: '%s'\n", line);
            setLastFileError("Invalid card detected: '%s'", line);
            fclose(f);
            return 0;
        }

        int rankIndex = rankPtr - validRanks;
        int suitIndex = suitPtr - validSuits;

        if (cardSeen[rankIndex][suitIndex]) {
            printf("Duplicate card detected: '%s'\n", line);
            setLastFileError("Duplicate card detected: '%s'", line);
            fclose(f);
            return 0;
        }

        cardSeen[rankIndex][suitIndex] = 1;
        lineCount++;
    }

    fclose(f);

    if (lineCount != 52) {
        printf("Deck does not contain exactly 52 cards (found %d).\n", lineCount);
        setLastFileCountError(lineCount);
        return 0;
    }

    return 1; // OK
}

// Frees all tableau columns, each stored as a singly linked list of cards.
void clearColumns() {
    for (int i = 0; i < 7; i++) {
        Card *current = columns[i].top;
        while (current != NULL) {
            Card *next = current->next;
            free(current);
            current = next;
        }
        columns[i].top = NULL;
    }
}

// Frees all four foundation piles, each stored as a singly linked list of cards.
void clearFoundations() {
    for (int i = 0; i < 4; i++) {
        Card *current = foundations[i].top;
        while (current != NULL) {
            Card *next = current->next;
            free(current);
            current = next;
        }
        foundations[i].top = NULL;
    }
}

// Rebuilds the startup view from the current deck order after LD, SI, or SR.
void reloadColumnsFromDeck() {
    clearColumns();

    Card *current = deck;
    int card_count = 0;

    while (current != NULL) {
        appendCard(&columns[card_count % 7], copyCard(current));
        current = current->next;
        card_count++;
    }
}
