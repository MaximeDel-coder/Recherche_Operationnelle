CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -O3 -g

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
TEST_DIR = tests

# Objets communs (utilisés par tous les exécutables)
COMMON_OBJS = $(OBJ_DIR)/solver.o $(OBJ_DIR)/io.o $(OBJ_DIR)/utils.o

all: transports benchmark test_solver

# Cible principale de l'application interactive
transports: $(COMMON_OBJS) $(OBJ_DIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^

# Cible pour le benchmark (mesures de complexité)
benchmark: $(COMMON_OBJS) $(OBJ_DIR)/benchmark.o
	$(CC) $(CFLAGS) -o $@ $^

# Cible pour les tests unitaires
test_solver: $(COMMON_OBJS) $(OBJ_DIR)/test_solver.o
	$(CC) $(CFLAGS) -o $@ $^

# Règle générique pour les fichiers source dans src/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour les tests
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/test_%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Création du dossier obj/ s'il n'existe pas
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) transports benchmark test_solver trace*.txt trace*.png

.PHONY: all clean tests
