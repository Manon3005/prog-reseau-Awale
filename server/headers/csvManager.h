typedef struct csvManager {
    FILE* csvClient;
} csvManager;

int isCLientInCsv(csvManager* csvManager, char* username);
int authenticateClient(csvManager* csvManager, char* username, char* pwd);
int addClientCsv(csvManager* csvManager, char* username, char* pwd);