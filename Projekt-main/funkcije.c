#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "header.h"

static void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int scanIntSafely(const char* prompt, int* outValue) {
    int temp;
    char after;
    printf("%s", prompt);
    if (scanf("%d%c", &temp, &after) != 2 || after != '\n') {
        clearInputBuffer();
        return 0;
    }
    *outValue = temp;
    return 1;
}

static void printPositionOptions(void) {
    printf("Dostupne pozicije:\n");
    printf("GK, CB, RB, LB, CDM, CM, CAM, LW, RW, ST, RM, LM\n");
}

static Position inputValidatedPosition(void) {
    char input[MAX_NAME_LENGTH];
    Position pos = PositionUnknown;

    do {
        printPositionOptions();
        printf("Unesite poziciju: ");
        fgets(input, MAX_NAME_LENGTH, stdin);
        input[strcspn(input, "\n")] = '\0';
        pos = stringToPosition(input);

        if (pos == PositionUnknown) {
            printf("Nepoznata pozicija. Pokusajte ponovno.\n");
        }
    } while (pos == PositionUnknown);

    return pos;
}

static const char* positionToString(Position pos) {
    switch (pos) {
    case PositionGk: return "GK";
    case PositionCb: return "CB";
    case PositionRb: return "RB";
    case PositionLb: return "LB";
    case PositionCdm: return "CDM";
    case PositionCm: return "CM";
    case PositionCam: return "CAM";
    case PositionLw: return "LW";
    case PositionRw: return "RW";
    case PositionSt: return "ST";
    case PositionRm: return "RM";
    case PositionLm: return "LM";
    default: return "Unknown";
    }
}

static Position stringToPosition(const char* str) {
    if (strcmp(str, "GK") == 0) return PositionGk;
    if (strcmp(str, "CB") == 0) return PositionCb;
    if (strcmp(str, "RB") == 0) return PositionRb;
    if (strcmp(str, "LB") == 0) return PositionLb;
    if (strcmp(str, "CDM") == 0) return PositionCdm;
    if (strcmp(str, "CM") == 0) return PositionCm;
    if (strcmp(str, "CAM") == 0) return PositionCam;
    if (strcmp(str, "LW") == 0) return PositionLw;
    if (strcmp(str, "RW") == 0) return PositionRw;
    if (strcmp(str, "ST") == 0) return PositionSt;
    if (strcmp(str, "RM") == 0) return PositionRm;
    if (strcmp(str, "LM") == 0) return PositionLm;
    return PositionUnknown;
}

static inline int compareByJerseyNumber(const void* a, const void* b) {
    return ((Player*)a)->jerseyNumber - ((Player*)b)->jerseyNumber;
}

Player* loadPlayers(int* count) {
    FILE* file = fopen(DATA_FILENAME, "r");
    if (!file) {
        if (errno == ENOENT) {
            *count = 0;
            totalPlayerCount = 0;
            return NULL;
        }
        perror("Greska pri otvaranju datoteke");
        *count = -1;
        totalPlayerCount = -1;
        return NULL;
    }

    rewind(file);  

    int capacity = 10;
    totalPlayerCount = 0;
    Player* players = malloc(capacity * sizeof(Player));
    if (!players) {
        perror("Greska pri alokaciji memorije");
        fclose(file);
        *count = -1;
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (totalPlayerCount >= capacity) {
            capacity *= 2;
            Player* temp = realloc(players, capacity * sizeof(Player));
            if (!temp) {
                perror("Greska pri prosirenju memorije");
                free(players);
                players = NULL;
                fclose(file);
                *count = -1;
                return NULL;
            }
            players = temp;
        }

        Player p;
        char positionStr[MAX_NAME_LENGTH];
        if (sscanf(line, "%99[^,],%d,%99[^,],%d,%d,%lf",
            p.name, &p.jerseyNumber, positionStr,
            &p.matchesPlayed, &p.goalsScored, &p.rating) == 6) {
            p.position = stringToPosition(positionStr);
            players[totalPlayerCount++] = p;
        }
    }

    fclose(file);
    *count = totalPlayerCount;
    return players;
}


int savePlayers(const Player* players, int count) {
    if (players == NULL || count <= 0) return 0;

    FILE* file = fopen(DATA_FILENAME, "w");
    if (!file) {
        perror("Greska pri spremanju datoteke");
        return 0;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%d,%s,%d,%d,%.2lf\n",
            players[i].name,
            players[i].jerseyNumber,
            positionToString(players[i].position),
            players[i].matchesPlayed,
            players[i].goalsScored,
            players[i].rating);
    }

    fclose(file);
    return 1;
}

Player* findPlayerByJersey(Player* players, int count, int jerseyNumber) {
    if (players == NULL || count <= 0) return NULL;
    Player key = { .jerseyNumber = jerseyNumber };
    return (Player*)bsearch(&key, players, count, sizeof(Player), compareByJerseyNumber);
}

void addPlayer(void) {
    Player* players = loadPlayers(&totalPlayerCount);
    if (totalPlayerCount < 0) return;

    Player newPlayer = { 0 };

    printf("Unesite ime igraca: ");
    fgets(newPlayer.name, MAX_NAME_LENGTH, stdin);
    newPlayer.name[strcspn(newPlayer.name, "\n")] = '\0';

    while (!scanIntSafely("Unesite broj dresa: ", &newPlayer.jerseyNumber))
        printf("Neispravan unos.\n");

    newPlayer.position = inputValidatedPosition();

    while (!scanIntSafely("Unesite broj utakmica: ", &newPlayer.matchesPlayed))
        printf("Neispravan unos.\n");

    while (!scanIntSafely("Unesite broj golova: ", &newPlayer.goalsScored))
        printf("Neispravan unos.\n");

    while (1) {
        printf("Unesite ocjenu igraca (0.00 - 10.00): ");
        if (scanf("%lf", &newPlayer.rating) == 1 &&
            newPlayer.rating >= 0.0 && newPlayer.rating <= 10.0) {
            clearInputBuffer();
            break;
        }
        printf("Neispravna ocjena.\n");
        clearInputBuffer();
    }

    Player* temp = realloc(players, (totalPlayerCount + 1) * sizeof(Player));
    if (!temp) {
        if (players != NULL) {
            free(players);
            players = NULL;
        }
        printf("Greska pri alokaciji memorije.\n");
        return;
    }
    players = temp;
    players[totalPlayerCount++] = newPlayer;

    qsort(players, totalPlayerCount, sizeof(Player), compareByJerseyNumber);

    if (savePlayers(players, totalPlayerCount))
        printf("Igrac uspjesno dodan!\n");

    if (players != NULL) {
        free(players);
        players = NULL;
    }
}

static void printPlayers(const Player* players, int index, int count) {
    if (index >= count) return;

    printf("\nIme: %s\nBroj dresa: %d\nPozicija: %s\nUtakmice: %d\nGolovi: %d\nOcjena: %.2lf\n",
        players[index].name,
        players[index].jerseyNumber,
        positionToString(players[index].position),
        players[index].matchesPlayed,
        players[index].goalsScored,
        players[index].rating);

    printPlayers(players, index + 1, count); 
}

void showAllPlayers(void) {
    Player* players = loadPlayers(&totalPlayerCount);
    if (totalPlayerCount <= 0 || players == NULL) {
        printf("Nema igraca za prikaz.\n");
        if (players != NULL) {
            free(players);
            players = NULL;
        }
        return;
    }

    printPlayers(players, 0, totalPlayerCount);  

    printf("\nUkupan broj igraca: %d\n", totalPlayerCount); 

    if (players != NULL) {
        free(players);
        players = NULL;
    }
}

void updatePlayer(void) {
    Player* players = loadPlayers(&totalPlayerCount);
    if (totalPlayerCount <= 0 || players == NULL) return;

    int jerseyNumber;
    if (!scanIntSafely("Unesite broj dresa igraca: ", &jerseyNumber)) {
        if (players != NULL) {
            free(players);
            players = NULL;
        }
        return;
    }

    qsort(players, totalPlayerCount, sizeof(Player), compareByJerseyNumber);
    Player* player = findPlayerByJersey(players, totalPlayerCount, jerseyNumber);

    if (!player) {
        printf("Igrac nije pronaden.\n");
        if (players != NULL) {
            free(players);
            players = NULL;
        }
        return;
    }

    printf("Novo ime (%s): ", player->name);
    fgets(player->name, MAX_NAME_LENGTH, stdin);
    player->name[strcspn(player->name, "\n")] = '\0';

    player->position = inputValidatedPosition();

    while (!scanIntSafely("Nove utakmice: ", &player->matchesPlayed)) printf("Neispravan unos.\n");
    while (!scanIntSafely("Novi golovi: ", &player->goalsScored)) printf("Neispravan unos.\n");

    while (1) {
        printf("Nova ocjena (%.2lf): ", player->rating);
        if (scanf("%lf", &player->rating) == 1 &&
            player->rating >= 0.0 && player->rating <= 10.0) {
            clearInputBuffer();
            break;
        }
        printf("Neispravna ocjena.\n");
        clearInputBuffer();
    }

    if (savePlayers(players, totalPlayerCount)) printf("Igrac azuriran.\n");

    if (players != NULL) {
        free(players);
        players = NULL;
    }
}

void removePlayer(void) {
    Player* players = loadPlayers(&totalPlayerCount);
    if (totalPlayerCount <= 0 || players == NULL) return;

    int jerseyNumber;
    if (!scanIntSafely("Unesite broj dresa za brisanje: ", &jerseyNumber)) {
        free(players);
        players = NULL;
        return;
    }

    Player* filtered = malloc(totalPlayerCount * sizeof(Player));
    if (!filtered) {
        printf("Greska pri alokaciji memorije.\n");
        free(players);
        players = NULL;
        return;
    }

    int newCount = 0;
    for (int i = 0; i < totalPlayerCount; i++) {
        if (players[i].jerseyNumber != jerseyNumber) {
            filtered[newCount++] = players[i];
        }
    }

    if (newCount == totalPlayerCount) {
        printf("Igrac nije pronaden.\n");
        free(players);
        players = NULL;
        free(filtered);
        filtered = NULL;
        return;
    }

    totalPlayerCount = newCount;

    if (savePlayers(filtered, newCount)) {
        printf("Igrac obrisan.\n");
    }

    if (players != NULL) {
        free(players);
        players = NULL;
    }
    if (filtered != NULL) {
        free(filtered);
        filtered = NULL;
    }
}

void searchPlayer(void) {
    Player* players = loadPlayers(&totalPlayerCount);
    if (totalPlayerCount <= 0 || players == NULL) return;

    printf("\n--- PODIZBORNIK PRETRAGE ---\n");
    printf("1. Pretraga po broju dresa\n");
    printf("2. Pretraga po imenu\n");

    int subChoice;
    if (!scanIntSafely("Izbor: ", &subChoice)) {
        printf("Neispravan unos.\n");
        free(players);
        players = NULL;
        return;
    }

    if (subChoice == 1) {
        int jerseyNumber;
        if (!scanIntSafely("Unesite broj dresa: ", &jerseyNumber)) {
            printf("Neispravan broj dresa.\n");
            free(players);
            players = NULL;
            return;
        }

        qsort(players, totalPlayerCount, sizeof(Player), compareByJerseyNumber);
        Player* player = findPlayerByJersey(players, totalPlayerCount, jerseyNumber);

        if (player) {
            printf("\nIme: %s\nBroj dresa: %d\nPozicija: %s\nUtakmice: %d\nGolovi: %d\nOcjena: %.2lf\n",
                player->name, player->jerseyNumber,
                positionToString(player->position),
                player->matchesPlayed, player->goalsScored, player->rating);
        }
        else {
            printf("Igrac nije pronaden.\n");
        }

    }
    else if (subChoice == 2) {
        char ime[MAX_NAME_LENGTH];
        printf("Unesite ime igraca: ");
        fgets(ime, MAX_NAME_LENGTH, stdin);
        ime[strcspn(ime, "\n")] = '\0';

        int found = 0;
        for (int i = 0; i < totalPlayerCount; i++) {
            if (strcmp(players[i].name, ime) == 0) {
                printf("\nIme: %s\nBroj dresa: %d\nPozicija: %s\nUtakmice: %d\nGolovi: %d\nOcjena: %.2lf\n",
                    players[i].name, players[i].jerseyNumber,
                    positionToString(players[i].position),
                    players[i].matchesPlayed, players[i].goalsScored, players[i].rating);
                found = 1;
            }
        }

        if (!found) {
            printf("Igrac nije pronaden.\n");
        }

    }
    else {
        printf("Nepostojeca opcija.\n");
    }

    if (players != NULL) {
        free(players);
        players = NULL;
    }
}
void clearAllPlayers(void) {
    char potvrda[10];
    printf("Jeste li sigurni da zelite obrisati SVE igrace? (da/ne): ");
    fgets(potvrda, sizeof(potvrda), stdin);
    potvrda[strcspn(potvrda, "\n")] = '\0';

    if (strcmp(potvrda, "da") == 0 || strcmp(potvrda, "DA") == 0) {
        if (remove(DATA_FILENAME) == 0) {
            printf("Svi igraci su obrisani.\n");
            totalPlayerCount = 0;
        }
        else {
            perror("Greska pri brisanju datoteke");
        }
    }
    else {
        printf("Brisanje otkazano.\n");
    }
}


void showMenu(void) {
    int running = 1;

    while (running) {
        printf("\n--- IZBORNIK ---\n");
        printf("1. Dodaj igraca\n2. Prikazi igrace\n3. Azuriraj igraca\n4. Obrisi igraca\n5. Pretrazi igraca\n6. Obrisi sve igrace\n7. Izlaz\n");

        int choice;
        if (!scanIntSafely("Izbor: ", &choice)) continue;

        switch ((MenuOption)choice) {
        case MenuAddPlayer: addPlayer(); break;
        case MenuDisplayPlayers: showAllPlayers(); break;
        case MenuUpdatePlayer: updatePlayer(); break;
        case MenuDeletePlayer: removePlayer(); break;
        case MenuSearchPlayer: searchPlayer(); break;
        case MenuClearAllPlayers: clearAllPlayers(); break;
        case MenuExit: {
            char odgovor[10];
            printf("Jeste li sigurni da zelite izaci iz programa? (da/ne): ");
            fgets(odgovor, sizeof(odgovor), stdin);
            odgovor[strcspn(odgovor, "\n")] = '\0';

            if (strcmp(odgovor, "da") == 0 || strcmp(odgovor, "DA") == 0) {
                running = 0;
                printf("Izlaz iz programa...\n");
            }
            else {
                printf("Povratak u izbornik.\n");
            }
            break;
        }
        default: printf("Nepostojeca opcija.\n"); break;
        }
    }
}
