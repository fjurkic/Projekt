#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>

#define MAX_NAME_LENGTH 100
#define MAX_LINE_LENGTH 256
#define DATA_FILENAME "players.txt"

// Enum za pozicije igrača
typedef enum {
    POSITION_GOALKEEPER,
    POSITION_DEFENDER,
    POSITION_MIDFIELDER,
    POSITION_FORWARD,
    POSITION_UNKNOWN
} Position;

// Enum za izbornik
typedef enum {
    MENU_ADD_PLAYER = 1,
    MENU_DISPLAY_PLAYERS,
    MENU_UPDATE_PLAYER,
    MENU_DELETE_PLAYER,
    MENU_SEARCH_PLAYER,
    MENU_EXIT
} MenuOption;

// Struktura za igrača
typedef struct {
    char name[MAX_NAME_LENGTH];
    int jerseyNumber;
    Position position;
    int matchesPlayed;
    int goalsScored;
} Player;

// Deklaracije funkcija
void menu(void);
int safeScanInt(const char* prompt, int* outValue);
void clearInputBuffer(void);

Player* loadPlayers(int* count);
int savePlayers(const Player* players, int count);

int comparePlayersByJersey(const void* a, const void* b);
Player* searchPlayerByJersey(Player* players, int count, int jerseyNumber);

void addPlayer(void);
void displayPlayers(void);
void updatePlayer(void);
void deletePlayer(void);
void searchPlayer(void);

const char* positionToString(Position pos);
Position stringToPosition(const char* str);

#endif
