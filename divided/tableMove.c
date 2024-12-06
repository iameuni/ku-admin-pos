#include "pos.h"

bool isValidDestinationTable(int destTable, int* destTables, int destCount, PaymentUnit* sourceUnit) {
    // �������� �ʴ� ���̺� ��ȣ üũ
    if (!isTableExist(destTable)) {
        printf("�������� �ʴ� ���̺� ��ȣ�Դϴ�.\n");
        return false;
    }

    // �̹� ���õ� ���̺����� üũ
    for (int i = 0; i < destCount; i++) {
        if (destTables[i] == destTable) {
            printf("�̹� ���õ� ���̺��Դϴ�.\n");
            return false;
        }
    }

    // ��� ���̺�� ������ ���̺����� üũ
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        if (sourceUnit->tables[i] == destTable) {
            return true; // ��� ���̺��� ���� ������ ���� ���̺��� ���
        }
    }

    return true;
}


// ���̺��� ������ ���̺� ��Ͽ� �ִ��� Ȯ���ϴ� ���� �Լ�
bool isInDestTables(int tableNum, int* destTables, int destCount) {
    for (int i = 0; i < destCount; i++) {
        if (destTables[i] == tableNum) {
            return true;
        }
    }
    return false;
}

void executeTableMove(PaymentUnit* sourceUnit, int* destTables, int destCount) {
    // 1. ��� ���̺��� ������ �ӽ� ���Ͽ� ������� ���
    FILE* tempFile = fopen("temp.txt", "w");
    if (!tempFile) return;

    // 1.1. ��� ���̺��� �ֹ� ������ ���� ����
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        char sourcePath[256];
        snprintf(sourcePath, sizeof(sourcePath), "%s/%d.txt", TABLE_FILE_PATH, sourceUnit->tables[i]);
        
        FILE* sourceFile = fopen(sourcePath, "r");
        if (sourceFile) {
            char line[256];
            while (fgets(line, sizeof(line), sourceFile)) {
                if (line[0] != '#') {  // �ֹ� ������ ����
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(sourceFile);
        }
    }

    // 1.2. ��� ���̺��� �κ� ���� ���� ����
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        char sourcePath[256];
        snprintf(sourcePath, sizeof(sourcePath), "%s/%d.txt", TABLE_FILE_PATH, sourceUnit->tables[i]);
        
        FILE* sourceFile = fopen(sourcePath, "r");
        if (sourceFile) {
            char line[256];
            while (fgets(line, sizeof(line), sourceFile)) {
                if (line[0] == '#' && line[1] == '#') {  // �κ� ���� ������ ����
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(sourceFile);
        }
    }

    fclose(tempFile);

    // 2. ��� ���̺���� ��� ���
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, sourceUnit->tables[i]);
        FILE* file = fopen(tablePath, "w");
        if (file) fclose(file);
    }

    // 3. �Էµ� ������ ���̺���� ���� �������� ��� ã�Ƽ� �ϳ��� ��ħ
    int newUnit[MAX_TABLE_NUMBER];
    int newUnitCount = 0;

    // 3.1 ���� �Էµ� ������ ���̺���� newUnit�� �߰�
    for (int i = 0; i < destCount; i++) {
        bool exists = false;
        for (int j = 0; j < newUnitCount; j++) {
            if (newUnit[j] == destTables[i]) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            newUnit[newUnitCount++] = destTables[i];
        }
    }

    // 3.2 �� ������ ���̺��� ���� ���� ���� ���̺�鵵 newUnit�� �߰�
    for (int i = 0; i < destCount; i++) {
        PaymentUnit* destUnit = getPaymentUnit(destTables[i]);
        if (destUnit->tableCount > 0) {
            for (int j = 0; j < destUnit->tableCount; j++) {
                bool exists = false;
                for (int k = 0; k < newUnitCount; k++) {
                    if (newUnit[k] == destUnit->tables[j]) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    newUnit[newUnitCount++] = destUnit->tables[j];
                }
            }
        }
        free(destUnit->partialPayments);
        free(destUnit);
    }

    // 4. ��� ���� ���� ���̺� ���� ������Ʈ
    for (int i = 0; i < newUnitCount; i++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, newUnit[i]);
        
        FILE* tempFile = fopen("temp_update.txt", "w");
        if (!tempFile) continue;

        // 4.1. ���� �ֹ� ���� ����
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

        // 4.2. ù ��° ������ ���̺��̸� ����� �ֹ� ������ �߰�
        if (newUnit[i] == destTables[0]) {
            FILE* backupFile = fopen("temp.txt", "r");
            if (backupFile) {
                char line[256];
                while (fgets(line, sizeof(line), backupFile)) {
                    if (line[0] != '#') {  // �ֹ� ������ ����
                        fprintf(tempFile, "%s", line);
                    }
                }
                fclose(backupFile);
            }
        }

        // 4.3. ���ο� ���� ���� ���� �߰�
        for (int j = 0; j < newUnitCount; j++) {
            fprintf(tempFile, "#%d\n", newUnit[j]);
        }

        // 4.4. ���� �κ� ���� ���� ����
        tableFile = fopen(tablePath, "r");
        if (tableFile) {
            char line[256];
            while (fgets(line, sizeof(line), tableFile)) {
                if (line[0] == '#' && line[1] == '#') {  // �κ� ���� ������ ����
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(tableFile);
        }

        // 4.5. ù ��° ������ ���̺��̸� ����� �κ� ���� ������ �߰�
        if (newUnit[i] == destTables[0]) {
            FILE* backupFile = fopen("temp.txt", "r");
            if (backupFile) {
                char line[256];
                while (fgets(line, sizeof(line), backupFile)) {
                    if (line[0] == '#' && line[1] == '#') {  // �κ� ���� ������ ����
                        fprintf(tempFile, "%s", line);
                    }
                }
                fclose(backupFile);
            }
        }

        fclose(tempFile);

        // 4.6. �ӽ� ���Ϸ� ���� ���� ��ü
        remove(tablePath);
        rename("temp_update.txt", tablePath);
    }

    remove("temp.txt");

    // 5. ��� �޽��� ���
    printf("\n");
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        printf("%d", sourceUnit->tables[i]);
        if (i < sourceUnit->tableCount - 1) printf(", ");
    }
    printf("�� ���̺��� ");
    // ���ݱ��� ���õ� �� ���̺� ���� ���� ���� ��ü�� ǥ��
    bool first = true;
        PaymentUnit* unit = getPaymentUnit(destTables[0]);
        if (unit->tableCount > 0) {
            if (!first) printf(", ");
            for (int j = 0; j < unit->tableCount; j++) {
                printf("%d", unit->tables[j]);
                if (j < unit->tableCount - 1) printf(", ");
            }
            first = false;
        }
        free(unit->partialPayments);
        free(unit);
    printf("�� ���̺�� �̵��Ǿ����ϴ�.\n");
}