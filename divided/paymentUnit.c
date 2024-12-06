#include "pos.h"

PaymentUnit* getPaymentUnit(int tableNumber) {
    PaymentUnit* unit = malloc(sizeof(PaymentUnit));
    unit->tableCount = 0;
    unit->paymentCount = 0;
    unit->partialPayments = NULL;
    
    char tablePath[256];
    snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);
    FILE* file = fopen(tablePath, "r");
    if (!file) return unit;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') {
            if (line[1] != '#') { // ���� ���� ǥ��
                int unitTable;
                sscanf(line + 1, "%d", &unitTable);
                unit->tables[unit->tableCount++] = unitTable;
            } else { // �κ� ���� ����
                int payment;
                sscanf(line + 2, "%d", &payment);
                unit->partialPayments = realloc(unit->partialPayments, 
                    (unit->paymentCount + 1) * sizeof(int));
                unit->partialPayments[unit->paymentCount++] = payment;
            }
        }
    }
    fclose(file);
    return unit;
}
// updatePaymentUnit �Լ� ����
void updatePaymentUnit(int primaryTable, int* unitTables, int unitCount) {
    // ���� ������ ���� ��� ���̺� ���� ó��
    for (int i = 0; i < unitCount; i++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unitTables[i]);
        
        // �ӽ� ���� ����
        FILE* tempFile = fopen("temp.txt", "w");
        if (!tempFile) continue;

        // 1. ���� �ֹ� ������ ����
        FILE* tableFile = fopen(tablePath, "r");
        if (tableFile) {
            char line[256];
            while (fgets(line, sizeof(line), tableFile)) {
                if (line[0] != '#') {  // �ֹ� ������ ����
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(tableFile);
        }

        // 2. ���� ���� ���� (#) ���
        for (int j = 0; j < unitCount; j++) {
            fprintf(tempFile, "#%d\n", unitTables[j]);
        }

        // 3. �κ� ���� ���� (##) ����
        tableFile = fopen(tablePath, "r");
        if (tableFile) {
            char line[256];
            while (fgets(line, sizeof(line), tableFile)) {
                if (line[0] == '#' && line[1] == '#') {
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(tableFile);
        }

        fclose(tempFile);
        remove(tablePath);
        rename("temp.txt", tablePath);
    }
}

// updatePaymentRecord �Լ� ����
void updatePaymentRecord(int tableNumber, int paymentAmount) {
    char tablePath[256];
    snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);
    
    FILE* tempFile = fopen("temp.txt", "w");
    if (!tempFile) return;
    
    // 1. �ֹ� ���� ����
    FILE* tableFile = fopen(tablePath, "r");
    if (!tableFile) {
        fclose(tempFile);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), tableFile)) {
        if (line[0] != '#') {  // �ֹ� ������ ����
            fprintf(tempFile, "%s", line);
        }
    }

    // 2. ���� ���� ���� (#) ����
    rewind(tableFile);
    while (fgets(line, sizeof(line), tableFile)) {
        if (line[0] == '#' && line[1] != '#') {
            fprintf(tempFile, "%s", line);
        }
    }

    // 3. ���� �κ� ���� ���� (##) ����
    rewind(tableFile);
    while (fgets(line, sizeof(line), tableFile)) {
        if (line[0] == '#' && line[1] == '#') {
            fprintf(tempFile, "%s", line);
        }
    }

    // 4. ���ο� �κ� ���� ���� �߰�
    fprintf(tempFile, "##%d\n", paymentAmount);

    fclose(tableFile);
    fclose(tempFile);

    remove(tablePath);
    rename("temp.txt", tablePath);
}