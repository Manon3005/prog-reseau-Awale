# Nom de l'exécutable final
EXEC = bin/server

# Répertoires
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = headers

# Compiler et options
CC = gcc
CFLAGS = -I$(INC_DIR)

# Recherche de tous les fichiers .c dans le répertoire src
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Création des noms de fichiers .o correspondant dans obj
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Règle principale
all: $(BIN_DIR) $(OBJ_DIR) $(EXEC)

# Création de l'exécutable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation de chaque fichier source en fichier objet
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/*.h
	$(CC) $(CFLAGS) -c $< -o $@

# Création des répertoires bin et obj si n'existent pas
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -rf $(OBJ_DIR)/*.o $(EXEC)

# Nettoyage complet (fichiers objets, exécutable et répertoires)
mrproper: clean
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all clean mrproper