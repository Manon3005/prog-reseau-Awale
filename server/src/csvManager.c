#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

    fprintf(file, "%s,%s,{},\n", username, pwd); 
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
        char *file_friends = strtok(NULL, ",");

        if (file_username && strcmp(file_username, username) == 0) {
            size_t len= strlen(bio);
            char* result = malloc(len + 3); 
            if (!result) {
                printf("Erreur d'allocation mémoire\n");
                return NULL;
            }
            result[0] = '{';                    
            strcpy(result + 1, bio);      
            result[len + 1] = '}';      
            result[len + 2] = '\0'; 
            strcpy(bio, result);
            free(result);
            fprintf(temp, "%s,%s,%s,%s\n",
                file_username, 
                file_pwd ? file_pwd : "", 
                bio, 
                file_friends ? file_friends : "");
            user_found = 1;
        } else {
            // Copier la ligne inchangée
            fprintf(temp, "%s,%s,%s,%s\n", 
                file_username ? file_username : "", 
                file_pwd ? file_pwd : "", 
                file_bio ? file_bio : "", 
                file_friends ? file_friends : "");
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

    static char bio[1024]; 
    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; // Nettoyage des fins de ligne

        char *file_username = strtok(line, ",");
        char *file_pwd = strtok(NULL, ",");
        char *file_bio = strtok(NULL, ",");
        char *file_friends = strtok(NULL, ",");

        if (file_username && strcmp(file_username, username) == 0) {
            if (file_bio) {
                strncpy(bio, file_bio, sizeof(bio) - 1); 
                bio[sizeof(bio) - 1] = '\0';
            } else {
                bio[0] = '\0'; 
            }
            fclose(file);
            return bio;
        }
    }

    fclose(file);
    return NULL; // Utilisateur non trouvé
}

int addGameToCsv(csvManager* csvManager, const char* player1, const char* player2, const char* moves, const char* date) {
    FILE *file = fopen("data/games.csv", "a");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    fprintf(file, "%s,%s,%s,%s\n", player1, player2, moves, date);
    fclose(file);
    return 1;
}

void getCurrentDateTime(char* dateBuffer, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(dateBuffer, size, "%04d-%02d-%02d %02d:%02d", 
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
             tm.tm_hour, tm.tm_min);
}

int getMovesFromPlayersAndDate(csvManager* csvManager, const char* player1, const char* player2, const char* date, char* moves) {
    FILE *file = fopen("data/games.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; 

        char *file_player1 = strtok(line, ",");
        char *file_player2 = strtok(NULL, ",");
        char *file_moves = strtok(NULL, ",");
        char *file_date = strtok(NULL, ",");

        if (file_player1 && file_player2 && file_date && 
            strcmp(file_player1, player1) == 0 && 
            strcmp(file_player2, player2) == 0 && 
            strcmp(file_date, date) == 0) {
            // Partie trouvée, copie les coups
            strncpy(moves, file_moves, 1024);
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0; // Partie non trouvée
}

int getGamesByPlayer(const char* username, SavedGame** games, int* gameCount) {
    FILE *file = fopen("data/games.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    char line[1024];
    *gameCount = 0;
    *games = NULL;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; 

        char* file_player1 = strtok(line, ",");
        char* file_player2 = strtok(NULL, ",");
        char* file_moves = strtok(NULL, ",");
        char* file_date = strtok(NULL, ",");

        if (!file_player1 || !file_player2 || !file_moves || !file_date) {
            continue; 
        }

        if (strcmp(file_player1, username) == 0 || strcmp(file_player2, username) == 0) {
            *games = realloc(*games, (*gameCount + 1) * sizeof(SavedGame));
            if (*games == NULL) {
                printf("Erreur d'allocation mémoire\n");
                fclose(file);
                return 0;
            }

            // Copie des infos dans la struc SavedGame
            strncpy((*games)[*gameCount].player1, file_player1, 256);
            strncpy((*games)[*gameCount].player2, file_player2, 256);
            strncpy((*games)[*gameCount].moves, file_moves, 1024);
            strncpy((*games)[*gameCount].date, file_date, 20);

            (*gameCount)++;
        }
    }

    fclose(file);
    return 1; // Succès
}

int addFriendToCsv(csvManager* csvManager, char* username, char* friend_username) {
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
        char *file_friends = strtok(NULL, ",");

        if (file_username && strcmp(file_username, username) == 0) {
            // Vérifie si l'ami est déjà présent
            if (file_friends && strstr(file_friends, friend_username) == NULL) {
                // Ajoute l'ami
                strcat(file_friends, " ");
                strcat(file_friends, friend_username);
            } else if (!file_friends) {
                file_friends = friend_username; // Si aucun ami, initialise la liste
            }
            fprintf(temp, "%s,%s,%s,%s\n", 
                file_username, 
                file_pwd ? file_pwd : "", 
                file_bio ? file_bio : "", 
                file_friends ? file_friends : "");
            user_found = 1;
        } else {
            // Copie la ligne inchangée
            fprintf(temp, "%s,%s,%s,%s\n",
                file_username ? file_username : "",
                file_pwd ? file_pwd : "",
                file_bio ? file_bio : "",
                file_friends ? file_friends : "");
        }
    }

    fclose(file);
    fclose(temp);

    // Remplace le fichier original par le temporaire
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

int removeFriendFromCsv(csvManager* csvManager, char* username, char* friend_username) {
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
        char *file_friends = strtok(NULL, ",");

        if (file_username && strcmp(file_username, username) == 0) {
            // Supprime l'ami si présent
            char updated_friends[1024] = "";
            if (file_friends) {
                char *token = strtok(file_friends, " ");
                while (token) {
                    if (strcmp(token, friend_username) != 0) {
                        if (strlen(updated_friends) > 0) strcat(updated_friends, " ");
                        strcat(updated_friends, token);
                    }
                    token = strtok(NULL, " ");
                }
            }
            fprintf(temp, "%s,%s,%s,%s\n", file_username, file_pwd ? file_pwd : "", file_bio ? file_bio : "", updated_friends);
            user_found = 1;
        } else {
            // Copie la ligne inchangée
            fprintf(temp, "%s,%s,%s,%s\n",
                file_username ? file_username : "",
                file_pwd ? file_pwd : "",
                file_bio ? file_bio : "",
                file_friends ? file_friends : "");
        }
    }

    fclose(file);
    fclose(temp);

    // Remplace le fichier original par le temporaire
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

int areFriendsInCsv(csvManager* csvManager, char* username, char* friend_username) {
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
        char *file_bio = strtok(NULL, ",");
        char *file_friends = strtok(NULL, ",");

        if (file_username && strcmp(file_username, username) == 0) {
            fclose(file);
            return file_friends && strstr(file_friends, friend_username) != NULL;
        }
    }

    fclose(file);
    return 0; // Utilisateur non trouvé ou pas ami
}

char** getFriendsAsArrayFromCsv(csvManager* csvManager, char* username, int* friend_count) {
    FILE *file = fopen("data/clients.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        *friend_count = 0;
        return NULL;
    }

    char line[1024];
    char *friends_line = NULL;
    *friend_count = 0;

    // Lire chaque ligne pour trouver l'utilisateur
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; // Nettoyer les fins de ligne

        char *file_username = strtok(line, ",");
        char *file_pwd = strtok(NULL, ",");
        char *file_bio = strtok(NULL, ",");
        char *file_friends = strtok(NULL, ",");

        if (file_username && strcmp(file_username, username) == 0) {
            friends_line = file_friends;
            break;
        }
    }

    fclose(file);

    if (!friends_line || strlen(friends_line) == 0) {
        return NULL; // Aucun ami trouvé ou ligne vide
    }

    // Compter le nombre d'amis
    char *temp = strdup(friends_line);
    char *token = strtok(temp, " ");
    while (token) {
        (*friend_count)++;
        token = strtok(NULL, " ");
    }
    free(temp);

    // Allouer un tableau pour les amis
    char **friends_array = malloc((*friend_count) * sizeof(char*));
    if (!friends_array) {
        printf("Erreur d'allocation mémoire\n");
        *friend_count = 0;
        return NULL;
    }

    // Remplir le tableau avec les noms d'amis
    int index = 0;
    token = strtok(friends_line, " ");
    while (token) {
        friends_array[index++] = strdup(token); // Allouer et copier chaque ami
        token = strtok(NULL, " ");
    }

    return friends_array;
}

int playerExistsInCsv(csvManager* csvManager, char* username) {
    FILE *file = fopen("data/clients.csv", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0; // Erreur d'ouverture, supposons que l'utilisateur n'existe pas
    }

    char line[1024];

    // Parcourir chaque ligne du fichier
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0; // Nettoyer les fins de ligne

        char *file_username = strtok(line, ","); // Lire le champ `username`

        if (file_username && strcmp(file_username, username) == 0) {
            fclose(file);
            return 1; // Joueur trouvé
        }
    }

    fclose(file);
    return 0; // Joueur non trouvé
}
