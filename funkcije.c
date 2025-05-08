#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

// Funkcija za dodavanje novog igrača
void addPlayer(FILE* file) {
    Player newPlayer;

    printf("Unesite ime igrača: ");
    fgets(newPlayer.name, MAX_NAME_LENGTH, stdin);
    newPlayer.name[strcspn(newPlayer.name, "\n")] = '\0'; // uklanjanje newline karaktera

    printf("Unesite broj dresa: ");
    scanf("%d", &newPlayer.jerseyNumber);

    getchar(); // za čišćenje newline nakon unosa broja

    printf("Unesite poziciju: ");
    fgets(newPlayer.position, MAX_NAME_LENGTH, stdin);
    newPlayer.position[strcspn(newPlayer.position, "\n")] = '\0';

    printf("Unesite broj odigranih utakmica: ");
    scanf("%d", &newPlayer.matchesPlayed);

    printf("Unesite broj golova: ");
    scanf("%d", &newPlayer.goalsScored);

    // Dodajemo novog igrača u tekstualnu datoteku
    fprintf(file, "%s,%d,%s,%d,%d\n", newPlayer.name, newPlayer.jerseyNumber, newPlayer.position, newPlayer.matchesPlayed, newPlayer.goalsScored);
    printf("Igrač uspješno dodan!\n");
}

// Funkcija za prikaz svih igrača
void displayPlayers(FILE* file) {
    Player player;
    rewind(file); // Postavljanje pokazivača na početak datoteke

    printf("Popis svih igrača:\n");
    while (fscanf(file, "%99[^,],%d,%99[^,],%d,%d\n", player.name, &player.jerseyNumber, player.position, &player.matchesPlayed, &player.goalsScored) == 5) {
        printf("\nIme: %s\n", player.name);
        printf("Broj dresa: %d\n", player.jerseyNumber);
        printf("Pozicija: %s\n", player.position);
        printf("Odigrane utakmice: %d\n", player.matchesPlayed);
        printf("Broj golova: %d\n", player.goalsScored);
    }
}

// Funkcija za ažuriranje podataka o igraču
void updatePlayer(FILE* file) {
    Player player;
    int jerseyNumber;
    long pos;

    printf("Unesite broj dresa igrača kojeg želite ažurirati: ");
    scanf("%d", &jerseyNumber);
    getchar(); // za čišćenje newline nakon unosa

    FILE* tempFile = fopen("temp.txt", "w+");
    if (!tempFile) {
        perror("Ne mogu otvoriti privremenu datoteku");
        return;
    }

    rewind(file);

    int found = 0;

    while (fscanf(file, "%99[^,],%d,%99[^,],%d,%d\n", player.name, &player.jerseyNumber, player.position, &player.matchesPlayed, &player.goalsScored) == 5) {
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
        }

        fprintf(tempFile, "%s,%d,%s,%d,%d\n", player.name, player.jerseyNumber, player.position, player.matchesPlayed, player.goalsScored);
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove("players.txt");
        rename("temp.txt", "players.txt");
        printf("Podaci o igraču su ažurirani.\n");
    }
    else {
        printf("Igrač s brojem dresa %d nije pronađen.\n", jerseyNumber);
    }
}

// Funkcija za brisanje igrača
void deletePlayer(FILE* file) {
    Player player;
    int jerseyNumber;
    FILE* tempFile;

    printf("Unesite broj dresa igrača kojeg želite obrisati: ");
    scanf("%d", &jerseyNumber);
    getchar(); // za čišćenje newline

    tempFile = fopen("temp.txt", "w+");
    if (tempFile == NULL) {
        perror("Ne mogu otvoriti privremenu datoteku");
        return;
    }

    rewind(file);

    int deleted = 0;

    while (fscanf(file, "%99[^,],%d,%99[^,],%d,%d\n", player.name, &player.jerseyNumber, player.position, &player.matchesPlayed, &player.goalsScored) == 5) {
        if (player.jerseyNumber != jerseyNumber) {
            fprintf(tempFile, "%s,%d,%s,%d,%d\n", player.name, player.jerseyNumber, player.position, player.matchesPlayed, player.goalsScored);
        }
        else {
            deleted = 1;
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("players.txt");
    rename("temp.txt", "players.txt");

    if (deleted) {
        printf("Igrač s brojem dresa %d je obrisan.\n", jerseyNumber);
    }
    else {
        printf("Igrač s brojem dresa %d nije pronađen.\n", jerseyNumber);
    }
}

// Funkcija za prikaz izbornika
void menu(FILE* file) {
    int choice;

    do {
        printf("\n--- Izbornik ---\n");
        printf("1. Dodaj igrača\n");
        printf("2. Prikaz svih igrača\n");
        printf("3. Ažuriraj igrača\n");
        printf("4. Obriši igrača\n");
        printf("5. Izlaz\n");
        printf("Odaberite opciju: ");
        scanf("%d", &choice);
        getchar(); // za čišćenje newline

        switch (choice) {
        case 1:
            addPlayer(file);
            break;
        case 2:
            displayPlayers(file);
            break;
        case 3:
            updatePlayer(file);
            break;
        case 4:
            deletePlayer(file);
            break;
        case 5:
            printf("Izlaz iz programa.\n");
            break;
        default:
            printf("Nevažeći odabir. Pokušajte ponovno.\n");
        }
    } while (choice != 5);
}
