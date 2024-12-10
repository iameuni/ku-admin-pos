#include "pos.h"

// 테이블 존재 여부 확인 함수
bool isTableExist(int tableNumber) {
    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);
    FILE* tableFile = fopen(tableFilePath, "r");
    if (tableFile != NULL) {
        fclose(tableFile);
        return true;
    }
    return false;
}

// 마지막 (가장 큰) 테이블 번호 반환하는 함수
int getLastTableNumber() {
    int maxNumber = 0;
    for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
        if (isTableExist(i) && i > maxNumber) {
            maxNumber = i;
        }
    }
    return maxNumber;
}

// 특정 테이블의 주문 내역 존재 여부 확인 함수
bool hasOrders(int tableNumber) {
    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);

    FILE* tableFile = fopen(tableFilePath, "r");
    if (tableFile == NULL) {
        return false;
    }

    int c;
    while ((c = fgetc(tableFile)) != EOF) {
        if (!isspace(c)) {
            fclose(tableFile);
            return true;
        }
    }

    fclose(tableFile);
    return false;
}

// 주문 내역이 있는 테이블 또는 결제 가능한 테이블 출력 함수
void listTablesWithOrders(int* tablesWithOrders, int* orderCount, const char* message) {
    *orderCount = 0;
    printf("%s: { ", message);

    bool first = true;
    int maxTableNum = getLastTableNumber();
    for (int table = 1; table <= maxTableNum; table++) {
        if (hasOrders(table)) {
            if (!first) {
                printf(", ");
            }
            printf("%d", table);
            first = false;

            tablesWithOrders[*orderCount] = table;
            (*orderCount)++;
        }
    }
    printf(" }\n\n");
}

// 현재 존재하는 테이블 개수 계산
int getCurrentTableCount() {
    int count = 0;
    for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
        if (isTableExist(i)) {
            count++;
        }
    }
    return count;
}

// 테이블 변경 함수 (테이블 변경이 가능한 테이블 고유 번호인지는 사전에 검사해서 넘겨주어야 한다.)
void changeTable(int prevTableNum, int newTableNum) {
    char oldFilePath[256];
    snprintf(oldFilePath, sizeof(oldFilePath), "%s/%d.txt", TABLE_FILE_PATH, prevTableNum);
    char newFilePath[256];
    snprintf(newFilePath, sizeof(newFilePath), "%s/%d.txt", TABLE_FILE_PATH, newTableNum);

    if (rename(oldFilePath, newFilePath) != 0) {
        perror("테이블 변경 실패");
    } else {
        printf("%d번 테이블이 %d번 테이블로 변경되었습니다.\n", prevTableNum, newTableNum);
    }
}