typedef struct csvManager {
    char csvClientFilePath[50];
    char csvGamesFilePath[50];
} csvManager;

typedef struct {
    char player1[256];
    char player2[256];
    char date[20];
    char winner[256];
} SavedGame;

void initCsvManager(csvManager* csvManager, char* csvClientFilePath,char* csvGamesFilePath);
int isCLientInCsv(csvManager* csvManager, char* username);
int authenticateClient(csvManager* csvManager, char* username, char* pwd);
int addClientCsv(csvManager* csvManager, char* username, char* pwd);
int changeBioCsv(csvManager* csvManager, char* username, char* bio);
char* getBioFromCsv(csvManager* csvManager, char* username);
int addGameToCsv(csvManager* csvManager, const char* player1, const char* player2, const char* moves, const char* date);
void getCurrentDateTime(char* dateBuffer, size_t size);
int addFriendToCsv(csvManager* csvManager, char* username, char* friend_username);
int removeFriendFromCsv(csvManager* csvManager, char* username, char* friend_username);
int areFriendsInCsv(csvManager* csvManager, char* username, char* friend_username);
char** getFriendsAsArrayFromCsv(csvManager* csvManager, char* username, int* friend_count);
int playerExistsInCsv(csvManager* csvManager, char* username);
int getGamesByPlayer(const char* username, SavedGame** games, int* gameCount);