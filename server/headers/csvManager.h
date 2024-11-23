typedef struct csvManager {
    FILE* csvClient;
} csvManager;

typedef struct {
    char player1[256];
    char player2[256];
    char date[20];
    char winner[256];
} SavedGame;

int isCLientInCsv(csvManager* csvManager, char* username);
int authenticateClient(csvManager* csvManager, char* username, char* pwd);
int addClientCsv(csvManager* csvManager, char* username, char* pwd);
int changeBioCsv(csvManager* csvManager, char* username, char* bio);
char* getBioFromCsv(csvManager* csvManager, char* username);
int addGameToCsv(csvManager* csvManager, const char* player1, const char* player2, const char* moves, const char* date);
void getCurrentDateTime(char* dateBuffer, size_t size);
