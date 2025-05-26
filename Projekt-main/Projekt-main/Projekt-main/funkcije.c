#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "header.h"

// Static funkcije za internu upotrebu

static void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int safeScanInt(const char* prompt, int* outValue) {
    int temp;
    char after;
    printf("%s", prompt);
    if (scanf("%d%c", &temp, &after) != 2 || after != '\n') {
        clearInputBuffer();
        return 0; // Neuspješan unos
    }
    *outValue = temp;
    return 1;
}

const char* positionToString(Position pos) {
    switch (pos) {
    case POSITION_GOALKEEPER: return "Goalkeeper";
    case POSITION_DEFENDER: return "Defender";
    case POSITION_MIDFIELDER: return "Midfielder";
    case POSITION_FORWARD: return "Forward";
    default: return "Unknown";
    }
}

Position stringToPosition(const char* str) {
    if (strcmp(str, "Goalkeeper") == 0) return POSITION_GOALKEEPER;
    if (strcmp(str, "Defender") == 0) return POSITION_DEFENDER;
    if (strcmp(str, "Midfielder") == 0) return POSITION_MIDFIELDER;
    if (strcmp(str, "Forward") == 0) return POSITION_FORWARD;
    return POSITION_UNKNOWN;
}

Player* loadPlayers(int* count) {
    FILE* file = fopen(DATA_FILENAME, "r");
    if (!file) {
        if (errno == ENOENT) {
            // Datoteka ne postoji, vrati praznu listu
            *count = 0;
            return NULL;
        }
        perror("Ne mogu otvoriti datoteku za čitanje");
        *count = -1;
        return NULL;
    }

    Player* players = NULL;
    char line[MAX_LINE_LENGTH];
    int capacity = 10;
    int n = 0;
    players = malloc(capacity * sizeof(Player));
    if (!players) {
        perror("Nemoguće alocirati memoriju");
        fclose(file);
        *count = -1;
        return NULL;
    }

    while (fgets(line, sizeof(line), file)) {
        if (n >= capacity) {
            capacity *= 2;
            Player* temp = realloc(players, capacity * sizeof(Player));
            if (!temp) {
                perror("Nemoguće realocirati memoriju");
                free(players);
                fclose(file);
                *count = -1;
                return NULL;
            }
            players = temp;
        }
        Player p = { 0 };
        char positionStr[MAX_NAME_LENGTH];
        // Format: name,jerseyNumber,position,matchesPlayed,goalsScored\n
        int scanned = sscanf(line, "%99[^,],%d,%99[^,],%d,%d",
            p.name, &p.jerseyNumber, positionStr,
            &p.matchesPlayed, &p.goalsScored);
        if (scanned == 5) {
            p.position = stringToPosition(positionStr);
            players[n++] = p;
        }
        else {
            fprintf(stderr, "Neispravan zapis u datoteci: %s", line);
        }
    }

    fclose(file);
    *count = n;
    return players;
}

int savePlayers(const Player* players, int count) {
    FILE* file = fopen(DATA_FILENAME, "w");
    if (!file) {
        perror("Ne mogu otvoriti datoteku za pisanje");
        return 0;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%d,%s,%d,%d\n",
            players[i].name,
            players[i].jerseyNumber,
            positionToString(players[i].position),
            players[i].matchesPlayed,
            players[i].goalsScored);
    }
    fclose(file);
    return 1;
}

int comparePlayersByJersey(const void* a, const void* b) {
    const Player* p1 = (const Player*)a;
    const Player* p2 = (const Player*)b;
    return p1->jerseyNumber - p2->jerseyNumber;
}

Player* searchPlayerByJersey(Player* players, int count, int jerseyNumber) {
    Player key = { 0 };
    key.jerseyNumber = jerseyNumber;
    return (Player*)bsearch(&key, players, count, sizeof(Player), comparePlayersByJersey);
}

void addPlayer(void) {
    int count = 0;
    Player* players = loadPlayers(&count);
    if (count < 0) {
        printf("Greška pri učitavanju igrača.\n");
        free(players);
        return;
    }

    Player newPlayer = { 0 };

    printf("Unesite ime igraca: ");
    fgets(newPlayer.name, MAX_NAME_LENGTH, stdin);
    newPlayer.name[strcspn(newPlayer.name, "\n")] = '\0';

    while (!safeScanInt("Unesite broj dresa: ", &newPlayer.jerseyNumber)) {
        printf("Neispravan unos, pokušajte ponovno.\n");
    }

    printf("Unesite poziciju (Goalkeeper, Defender, Midfielder, Forward): ");
    char posInput[MAX_NAME_LENGTH];
    fgets(posInput, MAX_NAME_LENGTH, stdin);
    posInput[strcspn(posInput, "\n")] = '\0';
    newPlayer.position = stringToPosition(posInput);

    while (!safeScanInt("Unesite broj odigranih utakmica: ", &newPlayer.matchesPlayed)) {
        printf("Neispravan unos, pokušajte ponovno.\n");
    }
    while (!safeScanInt("Unesite broj golova: ", &newPlayer.goalsScored)) {
        printf("Neispravan unos, pokušajte ponovno.\n");
    }

    Player* temp = realloc(players, (count + 1) * sizeof(Player));
    if (!temp) {
        perror("Nemoguće alocirati memoriju za dodavanje igrača");
        free(players);
        return;
    }
    players = temp;
    players[count] = newPlayer;
    count++;

    qsort(players, count, sizeof(Player), comparePlayersByJersey);

    if (savePlayers(players, count)) {
        printf("Igrac uspjesno dodan!\n");
    }

    free(players);
}

void displayPlayers(void) {
    int count = 0;
    Player* players = loadPlayers(&count);
    if (count <= 0) {
        printf("Nema igraca za prikaz.\n");
        free(players);
        return;
    }

    printf("\nPopis svih igraca:\n");
    for (int i = 0; i < count; i++) {
        printf("\nIme: %s\n", players[i].name);
        printf("Broj dresa: %d\n", players[i].jerseyNumber);
        printf("Pozicija: %s\n", positionToString(players[i].position));
        printf("Odigrane utakmice: %d\n", players[i].matchesPlayed);
        printf("Broj golova: %d\n", players[i].goalsScored);
    }

    free(players);
}

void updatePlayer(void) {
    int count = 0;
    Player* players = loadPlayers(&count);
    if (count <= 0) {
        printf("Nema igraca za azurirati.\n");
        free(players);
        return;
    }

    int jerseyNumber;
    if (!safeScanInt("Unesite broj dresa igraca kojeg zelite azurirati: ", &jerseyNumber)) {
        printf("Neispravan unos.\n");
        free(players);
        return;
    }

    qsort(players, count, sizeof(Player), comparePlayersByJersey);
    Player* p = searchPlayerByJersey(players, count, jerseyNumber);
    if (!p) {
        printf("Igrac s brojem dresa %d nije pronaden.\n", jerseyNumber);
        free(players);
        return;
    }

    printf("Unesite novo ime (trenutno: %s): ", p->name);
    fgets(p->name, MAX_NAME_LENGTH, stdin);
    p->name[strcspn(p->name, "\n")] = '\0';

    printf("Unesite novu poziciju (Goalkeeper, Defender, Midfielder, Forward): ");
    char posInput[MAX_NAME_LENGTH];
    fgets(posInput, MAX_NAME_LENGTH, stdin);
    posInput[strcspn(posInput, "\n")] = '\0';
    p->position = stringToPosition(posInput);

    while (!safeScanInt("Unesite novi broj odigranih utakmica: ", &p->matchesPlayed)) {
        printf("Neispravan unos, pokušajte ponovno.\n");
    }
    while (!safeScanInt("Unesite novi broj golova: ", &p->goalsScored)) {
        printf("Neispravan unos, pokušajte ponovno.\n");
    }

    if (savePlayers(players, count)) {
        printf("Podaci o igracu su azurirani.\n");
    }

    free(players);
}

void deletePlayer(void) {
    int count = 0;
    Player* players = loadPlayers(&count);
    if (count <= 0) {
        printf("Nema igraca za brisanje.\n");
        free(players);
        return;
    }

    int jerseyNumber;
    if (!safeScanInt("Unesite broj dresa igraca kojeg zelite obrisati: ", &jerseyNumber)) {
        printf("Neispravan unos.\n");
        free(players);
        return;
    }

    int newCount = 0;
    Player* filtered = malloc(count * sizeof(Player));
    if (!filtered) {
        perror("Nemoguće alocirati memoriju");
        free(players);
        return;
    }

    for (int i = 0; i < count; i++) {
        if (players[i].jerseyNumber != jerseyNumber) {
            filtered[newCount++] = players[i];
        }
    }

    if (newCount == count) {
        printf("Igrac s brojem dresa %d nije pronađen.\n", jerseyNumber);
        free(players);
        free(filtered);
        return;
    }

    if (savePlayers(filtered, newCount)) {
        printf("Igrac s brojem dresa %d je obrisan.\n", jerseyNumber);
    }

    free(players);
    free(filtered);
}

void searchPlayer(void) {
    int count = 0;
    Player* players = loadPlayers(&count);
    if (count <= 0) {
        printf("Nema igraca za pretrazivanje.\n");
        free(players);
        return;
    }

    int jerseyNumber;
    if (!safeScanInt("Unesite broj dresa za pretragu: ", &jerseyNumber)) {
        printf("Neispravan unos.\n");
        free(players);
        return;
    }

    qsort(players, count, sizeof(Player), comparePlayersByJersey);

    Player* p = searchPlayerByJersey(players, count, jerseyNumber);
    if (p) {
        printf("\nIgrac pronaden:\n");
        printf("Ime: %s\n", p->name);
        printf("Broj dresa: %d\n", p->jerseyNumber);
        printf("Pozicija: %s\n", positionToString(p->position));
        printf("Odigrane utakmice: %d\n", p->matchesPlayed);
        printf("Broj golova: %d\n", p->goalsScored);
    }
    else {
        printf("Igrac nije pronaden.\n");
    }

    free(players);
}

void menu(void) {
    int running = 1;
    while (running) {
        printf("\n--- Izbornik ---\n");
        printf("1. Dodaj igraca\n");
        printf("2. Prikaz svih igraca\n");
        printf("3. Azuriraj igraca\n");
        printf("4. Obrisi igraca\n");
        printf("5. Pretrazi igraca\n");
        printf("6. Izlaz\n");

        int choice;
        if (!safeScanInt("Izaberite opciju: ", &choice)) {
            printf("Neispravan unos, pokusajte ponovno.\n");
            continue;
        }

        switch ((MenuOption)choice) {
        case MENU_ADD_PLAYER:
            addPlayer();
            break;
        case MENU_DISPLAY_PLAYERS:
            displayPlayers();
            break;
        case MENU_UPDATE_PLAYER:
            updatePlayer();
            break;
        case MENU_DELETE_PLAYER:
            deletePlayer();
            break;
        case MENU_SEARCH_PLAYER:
            searchPlayer();
            break;
        case MENU_EXIT:
            running = 0;
            printf("Izlaz iz programa.\n");
            break;
        default:
            printf("Nepostojeca opcija, pokusajte ponovno.\n");
            break;
        }
    }
}
