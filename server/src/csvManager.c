#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../headers/csvManager.h"

int isCLientInCsv(csvManager* csvManager, char* username){
    if (strcmp(username, "man") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int authenticateClient(csvManager* csvManager, char* username, char* pwd){
    if (strcmp(pwd, "pwd") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int addClientCsv(csvManager* csvManager, char* username, char* pwd){
    printf("New client: %s - %s\n", username, pwd);
    return 1;
}
