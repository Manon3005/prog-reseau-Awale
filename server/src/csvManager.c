#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../headers/csvManager.h"

int isCLientInCsv(csvManager* csvManager, char* username) {
    FILE *file = fopen("data/clients.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    char buf[1024];
    while (fgets(buf, sizeof(buf), file)) {
        buf[strcspn(buf, "\r\n")] = 0; // Nettoyage des fins de ligne
        char *field = strtok(buf, ",");
        if (field && strcmp(username, field) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int authenticateClient(csvManager* csvManager, char* username, char* pwd) {
    FILE *file = fopen("data/clients.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; 

        char *file_username = strtok(line, ",");
        char *file_pwd = strtok(NULL, ",");
        strtok(NULL, ","); // Ignore la colonne bio

        if (file_username && strcmp(file_username, username) == 0) {
            if (file_pwd && strcmp(file_pwd, pwd) == 0) {
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

int addClientCsv(csvManager* csvManager, char* username, char* pwd) {
    FILE *file = fopen("data/clients.csv", "a");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    fprintf(file, "%s,%s,\n", username, pwd); 
    fclose(file);
    return 1;
}

int changeBioCsv(csvManager* csvManager, char* username, char* bio) {
    FILE *file = fopen("data/clients.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    FILE *temp = fopen("data/clients_temp.csv", "w");
    if (temp == NULL) {
        printf("Impossible de créer un fichier temporaire\n");
        fclose(file);
        return 0;
    }

    char line[1024];
    int user_found = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; // Nettoyage des fins de ligne

        char *file_username = strtok(line, ",");
        char *file_pwd = strtok(NULL, ",");
        char *file_bio = strtok(NULL, ",");

        if (file_username && strcmp(file_username, username) == 0) {
            // Mettre à jour la bio pour cet utilisateur
            fprintf(temp, "%s,%s,%s\n", file_username, file_pwd ? file_pwd : "", bio);
            user_found = 1;
        } else {
            // Copier la ligne inchangée
            fprintf(temp, "%s,%s,%s\n", 
                file_username ? file_username : "", 
                file_pwd ? file_pwd : "", 
                file_bio ? file_bio : "");
        }
    }

    fclose(file);
    fclose(temp);

    // Remplacer l'ancien fichier par le fichier temporaire
    if (remove("data/clients.csv") != 0) {
        printf("Erreur lors de la suppression de l'ancien fichier\n");
        return 0;
    }
    if (rename("data/clients_temp.csv", "data/clients.csv") != 0) {
        printf("Erreur lors du renommage du fichier temporaire\n");
        return 0;
    }

    return user_found;
}

char* getBioFromCsv(csvManager* csvManager, char* username) {
    FILE *file = fopen("data/clients.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return NULL;
    }

    static char bio[1024]; // Mémoire statique pour retourner la bio
    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; // Nettoyage des fins de ligne

        char *file_username = strtok(line, ",");
        char *file_pwd = strtok(NULL, ",");
        char *file_bio = strtok(NULL, ",");

        if (file_username && strcmp(file_username, username) == 0) {
            if (file_bio) {
                strncpy(bio, file_bio, sizeof(bio) - 1); // Copie la bio dans la variable statique
                bio[sizeof(bio) - 1] = '\0'; // Assure la terminaison de la chaîne
            } else {
                bio[0] = '\0'; // Pas de bio, retourne une chaîne vide
            }
            fclose(file);
            return bio;
        }
    }

    fclose(file);
    return NULL; // Utilisateur non trouvé
}