#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>

#define MAX_NAME_LENGTH 100
#define MAX_LINE_LENGTH 256
#define DATA_FILENAME "players.txt"


typedef enum {
    PositionGk,
    PositionCb,
    PositionRb,
    PositionLb,
    PositionCdm,
    PositionCm,
    PositionCam,
    PositionLw,
    PositionRw,
    PositionSt,
    PositionRm,
    PositionLm,
    PositionUnknown
} Position;


typedef enum {
    MenuAddPlayer = 1,
    MenuDisplayPlayers,
    MenuUpdatePlayer,
    MenuDeletePlayer,
    MenuSearchPlayer,
    MenuClearAllPlayers,
    MenuExit
} MenuOption;



typedef struct {
    char name[MAX_NAME_LENGTH];
    int jerseyNumber;
    Position position;
    int matchesPlayed;
    int goalsScored;
    double rating;
} Player;


extern int totalPlayerCount;


Player* loadPlayers(int* count);
int savePlayers(const Player* players, int count);
Player* findPlayerByJersey(Player* players, int count, int jerseyNumber);

void addPlayer(void);
void showAllPlayers(void);
void updatePlayer(void);
void removePlayer(void);
void searchPlayer(void);
void showMenu(void);
void clearAllPlayers(void);



#endif
