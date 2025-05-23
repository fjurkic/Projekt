#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

// Dodavanje igrača
void addPlayer(FILE* file) {
    Player newPlayer;

    printf("Unesite ime igraca: ");
    fgets(newPlayer.name, MAX_NAME_LENGTH, stdin);
    newPlayer.name[strcspn(newPlayer.name, "\n")] = '\0';

    printf("Unesite broj dresa: ");
    scanf("%d", &newPlayer.jerseyNumber);
    getchar();

    printf("Unesite poziciju: ");
    fgets(newPlayer.position, MAX_NAME_LENGTH, stdin);
    newPlayer.position[strcspn(newPlayer.position, "\n")] = '\0';

    printf("Unesite broj odigranih utakmica: ");
    scanf("%d", &newPlayer.matchesPlayed);

    printf("Unesite broj golova: ");
    scanf("%d", &newPlayer.goalsScored); 

    fprintf(file, "%s,%d,%s,%d,%d\n", newPlayer.name, newPlayer.jerseyNumber,
        newPlayer.position, newPlayer.matchesPlayed, newPlayer.goalsScored);
    printf("Igrac uspjesno dodan!\n");
}

// Prikaz igrača
void displayPlayers(FILE* file) {
    Player player;
    char line[512];
    rewind(file);

    printf("\nPopis svih igraca:\n");
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%99[^,],%d,%99[^,],%d,%d",
            player.name, &player.jerseyNumber, player.position,
            &player.matchesPlayed, &player.goalsScored) == 5) {
            printf("\nIme: %s\n", player.name);
            printf("Broj dresa: %d\n", player.jerseyNumber);
            printf("Pozicija: %s\n", player.position);
            printf("Odigrane utakmice: %d\n", player.matchesPlayed);
            printf("Broj golova: %d\n", player.goalsScored);
        }
    }
}

// Ažuriranje igrača
void updatePlayer(FILE* file) {
    Player player;
    int jerseyNumber;

    printf("Unesite broj dresa igraca kojeg zelite azurirati: ");
    scanf("%d", &jerseyNumber);
    getchar();

    FILE* tempFile = fopen("temp.txt", "w");
    if (!tempFile) {
        perror("Ne mogu otvoriti privremenu datoteku");
        return;
    }

    rewind(file);
    int found = 0;
    char line[512];

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%99[^,],%d,%99[^,],%d,%d",
            player.name, &player.jerseyNumber, player.position,
            &player.matchesPlayed, &player.goalsScored) == 5) {
            if (player.jerseyNumber == jerseyNumber) {
                found = 1;
                printf("Unesite novo ime: ");
                fgets(player.name, MAX_NAME_LENGTH, stdin);
                player.name[strcspn(player.name, "\n")] = '\0';

                printf("Unesite novu poziciju: ");
                fgets(player.position, MAX_NAME_LENGTH, stdin);
                player.position[strcspn(player.position, "\n")] = '\0';

                printf("Unesite novi broj odigranih utakmica: ");
                scanf("%d", &player.matchesPlayed);

                printf("Unesite novi broj golova: ");
                scanf("%d", &player.goalsScored);
                getchar();
            }
            fprintf(tempFile, "%s,%d,%s,%d,%d\n", player.name, player.jerseyNumber,
                player.position, player.matchesPlayed, player.goalsScored);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("players.txt");
    rename("temp.txt", "players.txt");

    if (found)
        printf("Podaci o igracu su azurirani.\n");
    else
        printf("Igrač s brojem dresa %d nije pronađen.\n", jerseyNumber);
}

// Brisanje igrača
void deletePlayer(FILE* file) {
    Player player;
    int jerseyNumber;

    printf("Unesite broj dresa igraca kojeg zelite obrisati: ");
    scanf("%d", &jerseyNumber);
    getchar();

    FILE* tempFile = fopen("temp.txt", "w");
    if (!tempFile) {
        perror("Ne mogu otvoriti privremenu datoteku");
        return;
    }

    rewind(file);
    int deleted = 0;
    char line[512];

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%99[^,],%d,%99[^,],%d,%d",
            player.name, &player.jerseyNumber, player.position,
            &player.matchesPlayed, &player.goalsScored) == 5) {
            if (player.jerseyNumber != jerseyNumber) {
                fprintf(tempFile, "%s,%d,%s,%d,%d\n", player.name, player.jerseyNumber,
                    player.position, player.matchesPlayed, player.goalsScored);
            }
            else {
                deleted = 1;
            }
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("players.txt");
    rename("temp.txt", "players.txt");

    if (deleted)
        printf("Igrac s brojem dresa %d je obrisan.\n", jerseyNumber);
    else
        printf("Igrac s brojem dresa %d nije pronađen.\n", jerseyNumber);
}

// Glavni izbornik
void menu() {
    int choice;

    do {
        printf("\n--- Izbornik ---\n");
        printf("1. Dodaj igraca\n");
        printf("2. Prikaz svih igraca\n");
        printf("3. Azuriraj igraca\n");
        printf("4. Obrisi igraca\n");
        printf("5. Izlaz\n");
        printf("Odaberite opciju: ");
        scanf("%d", &choice);
        getchar();

        FILE* file;

        switch (choice) {
        case 1:
            file = fopen("players.txt", "a");
            if (file) {
                addPlayer(file);
                fclose(file);
            }
            else {
                perror("Ne mogu otvoriti datoteku");
            }
            break;
        case 2:
            file = fopen("players.txt", "r");
            if (file) {
                displayPlayers(file);
                fclose(file);
            }
            else {
                perror("Ne mogu otvoriti datoteku");
            }
            break;
        case 3:
            file = fopen("players.txt", "r");
            if (file) {
                updatePlayer(file);
            }
            else {
                perror("Ne mogu otvoriti datoteku");
            }
            break;
        case 4:
            file = fopen("players.txt", "r");
            if (file) {
                deletePlayer(file);
            }
            else {
                perror("Ne mogu otvoriti datoteku");
            }
            break;
        case 5:
            printf("Izlaz iz programa.\n");
            break;
        default:
            printf("Nevazeci odabir. Pokusajte ponovno.\n");
        }
    } while (choice != 5);
}
