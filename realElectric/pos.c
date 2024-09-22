#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 100
#define MAX_TABLES 100

typedef struct FoodNode {
} FoodNode;
typedef struct {
} Menu;
typedef struct {
} Ordered;
typedef struct {
} Table;

static int getInt(void);
static char* getString(void);
static int getTableNum(void);

static int fileClose(FILE* f);
static void saveFoodList(Menu* m);
static void loadFoodList(Menu* m);
static void saveTables(Table* tables);
static void loadTables(Table* tables);

static int printMain(void);

static void initFoodList(Menu* m);
static void addToFoodList(Menu* m, const char* name, int price);
static void removeFromFoodList(Menu* m, int index);
static void printFoodList(Menu* m);
static void freeFoodList(Menu* m);

static void initTable(Table* t);
static void addOrder(Table* t, int foodIndex, int quantity, Menu* m);
static void clearTable(Table* t);
static void freeTable(Table* t);

static int getInt() {
}
static char* getString() {
}

static int printMain(void) {
}
static void printFoodList(Menu* m) {
}

static int getTableNum() {
}

static void initFoodList(Menu* m) {
}
static void addToFoodList(Menu* m, const char* name, int price) {
}
static void removeFromFoodList(Menu* m, int index) {
}
static void freeFoodList(Menu* m) {
}

static void initTable(Table* t) {
}
static void addOrder(Table* t, int index, int quantity, Menu* m) {
}
static void clearTable(Table* t) {
}
static void freeTable(Table* t) {
}

static int fileClose(FILE* f) {
}
static void saveFoodList(Menu* m) {
}
static void loadFoodList(Menu* m) {
}
static void saveTables(Table* tables) {
}
static void loadTables(Table* tables) {
}

int main(void) {
    return 0;
}
