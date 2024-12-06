#include "pos.h"

// ���̺� ���� ���� Ȯ�� �Լ�
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

// ������ (���� ū) ���̺� ��ȣ ��ȯ�ϴ� �Լ�
int getLastTableNumber() {
    int maxNumber = 0;
    for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
        if (isTableExist(i) && i > maxNumber) {
            maxNumber = i;
        }
    }
    return maxNumber;
}

// Ư�� ���̺��� �ֹ� ���� ���� ���� Ȯ�� �Լ�
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

// �ֹ� ������ �ִ� ���̺� �Ǵ� ���� ������ ���̺� ��� �Լ�
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

// ���� �����ϴ� ���̺� ���� ���
int getCurrentTableCount() {
    int count = 0;
    for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
        if (isTableExist(i)) {
            count++;
        }
    }
    return count;
}

// ���̺� ���� �Լ� (���̺� ������ ������ ���̺� ���� ��ȣ������ ������ �˻��ؼ� �Ѱ��־�� �Ѵ�.)
void changeTable(int prevTableNum, int newTableNum) {
    char oldFilePath[256];
    snprintf(oldFilePath, sizeof(oldFilePath), "%s/%d.txt", TABLE_FILE_PATH, prevTableNum);
    char newFilePath[256];
    snprintf(newFilePath, sizeof(newFilePath), "%s/%d.txt", TABLE_FILE_PATH, newTableNum);

    if (rename(oldFilePath, newFilePath) != 0) {
        perror("���̺� ���� ����");
    } else {
        printf("%d�� ���̺��� %d�� ���̺�� ����Ǿ����ϴ�.\n", prevTableNum, newTableNum);
    }
}