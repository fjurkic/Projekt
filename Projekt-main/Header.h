#ifndef HEADER_H
#define HEADER_H

#define MAX_NAME_LENGTH 100

// Struktura za igrača
typedef struct {
    char name[MAX_NAME_LENGTH];
    int jerseyNumber;
    char position[MAX_NAME_LENGTH];
    int matchesPlayed;
    int goalsScored;
} Player;

// Deklaracija funkcija
void addPlayer(FILE* file);
void displayPlayers(FILE* file);
void updatePlayer(FILE* file);
void deletePlayer(FILE* file);
void menu();

#endif
