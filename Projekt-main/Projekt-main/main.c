#include <stdio.h>
#include <stdlib.h>
#include "header.h"

int main() {
    FILE* file;

    // Otvoriti datoteku za čitanje i pisanje (ako ne postoji, kreira se)
    file = fopen("players.txt", "r+");
    if (file == NULL) {
        file = fopen("players.txt", "w+");
        if (file == NULL) {
            perror("Ne mogu otvoriti datoteku");
            return 1;
        }
    }

    menu(file);

    fclose(file);
    return 0;
}
