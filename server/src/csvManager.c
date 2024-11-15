#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../headers/csvManager.h"

int isCLientInCsv(csvManager* csvManager, char* username){

    FILE * file = fopen("data/clients.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    char buf[1024];
    while (fgets(buf, 1024, file)) {
        char *field = strtok(buf, ",");
        if(strcmp(username,field)==0){
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int authenticateClient(csvManager* csvManager, char* username, char* pwd){
    FILE * file = fopen("data/clients.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    char line[1024];
    char *file_username, *file_pwd;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        file_username = strtok(line, ",");
        file_pwd = strtok(NULL, "\r\n");

        if (strcmp(file_username, username) == 0){
            if(strcmp(file_pwd, pwd) == 0) {
                return 1;
            }
        }
        
    }


    fclose(file);
    return 0;
}

int addClientCsv(csvManager* csvManager, char* username, char* pwd){
    FILE * file = fopen("data/clients.csv", "r+");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    fseek(file, 0, SEEK_END);

    fprintf(file, "\r\n%s,%s", username, pwd);

    fclose(file);
    return 0;
}
