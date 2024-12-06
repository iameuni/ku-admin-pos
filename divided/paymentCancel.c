#include "pos.h"

void executeCancelPayments(PaymentUnit* unit, int* selectedPayments, int selectedCount) {
    for (int t = 0; t < unit->tableCount; t++) {
        int tableNum = unit->tables[t];
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNum);
        
        FILE* tempFile = fopen("temp.txt", "w");
        FILE* tableFile = fopen(tablePath, "r");
        
        if (!tempFile || !tableFile) continue;

        char line[256];
        // ���õ� �ݾ׺��� ��� ���� ���� ����
        bool* cancelled = (bool*)calloc(selectedCount, sizeof(bool));
        
        // ���� �ֹ� ������ ���� ���� ���� ����
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] != '#') {
                fprintf(tempFile, "%s", line);
            } else if (line[0] == '#' && line[1] != '#') {
                fprintf(tempFile, "%s", line);
            }
        }

        // �κ� ���� ���� ó��
        rewind(tableFile);
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] == '#' && line[1] == '#') {
                int payment;
                sscanf(line + 2, "%d", &payment);
                
                bool shouldKeep = true;
                // ���� ��ҵ��� ���� ���õ� �ݾװ� ��ġ�ϴ��� Ȯ��
                for (int i = 0; i < selectedCount; i++) {
                    if (!cancelled[i] && payment == selectedPayments[i]) {
                        cancelled[i] = true;
                        shouldKeep = false;
                        break;
                    }
                }
                
                if (shouldKeep) {
                    fprintf(tempFile, "%s", line);
                }
            }
        }

        free(cancelled);
        fclose(tableFile);
        fclose(tempFile);
        remove(tablePath);
        rename("temp.txt", tablePath);
    }

    // ��ҵ� �κ� ���� �ݾ׵� ���
    for (int i = 0; i < selectedCount; i++) {
        printf("%d��", selectedPayments[i]);
        if (i < selectedCount - 1) {
            printf(" ");
        }
    }
    printf(" �κа����� ���� ��ҵǾ����ϴ�.\n");
}

void listTablesWithPartialPayments(int* tables, int* count, const char* message) {
    *count = 0;
    printf("%s: { ", message);
    bool first = true;

    // ���� �κ� ������ �ִ� ���̺���� ã�Ƽ� ���� ���� ������ ����
    PaymentUnit* units[MAX_TABLE_NUMBER] = {NULL};
    int unitCount = 0;

    for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
        if (!isTableExist(i)) continue;

        // �̹� �ٸ� ���� ������ ���Ե� ���̺����� Ȯ��
        bool alreadyIncluded = false;
        for (int j = 0; j < unitCount; j++) {
            for (int k = 0; k < units[j]->tableCount; k++) {
                if (units[j]->tables[k] == i) {
                    alreadyIncluded = true;
                    break;
                }
            }
            if (alreadyIncluded) break;
        }
        if (alreadyIncluded) continue;

        // ���̺��� ���� ���� ���� Ȯ��
        PaymentUnit* unit = getPaymentUnit(i);
        bool hasPartialPayment = false;

        // ���� ���� �� ��� ���̺��� �κ� ���� Ȯ��
        if (unit->tableCount > 0) {
            for (int j = 0; j < unit->tableCount; j++) {
                int tableNum = unit->tables[j];
                char tablePath[256];
                snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNum);
                FILE* file = fopen(tablePath, "r");
                if (!file) continue;

                char line[256];
                while (fgets(line, sizeof(line), file)) {
                    if (line[0] == '#' && line[1] == '#') {
                        hasPartialPayment = true;
                        break;
                    }
                }
                fclose(file);
                if (hasPartialPayment) break;
            }

            if (hasPartialPayment) {
                // ���� ������ ��� ���̺� �߰�
                for (int j = 0; j < unit->tableCount; j++) {
                    tables[(*count)++] = unit->tables[j];
                }
                units[unitCount++] = unit;
                if (!first) printf(", ");
                first = false;

                // ���� ������ ��� ���̺� ���
                printf("{");
                for (int j = 0; j < unit->tableCount; j++) {
                    printf("%d", unit->tables[j]);
                    if (j < unit->tableCount - 1) printf(", ");
                }
                printf("}");
            } else {
                free(unit->partialPayments);
                free(unit);
            }
        } else {
            char tablePath[256];
            snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, i);
            FILE* file = fopen(tablePath, "r");
            if (!file) continue;

            char line[256];
            while (fgets(line, sizeof(line), file)) {
                if (line[0] == '#' && line[1] == '#') {
                    hasPartialPayment = true;
                    break;
                }
            }
            fclose(file);

            if (hasPartialPayment) {
                tables[(*count)++] = i;
                if (!first) printf(", ");
                printf("%d", i);
                first = false;
            }
            free(unit->partialPayments);
            free(unit);
        }
    }
    printf(" }\n\n");

    // �޸� ����
    for (int i = 0; i < unitCount; i++) {
        free(units[i]->partialPayments);
        free(units[i]);
    }
}


bool isPaymentSelected(int payment, int* selectedPayments, int selectedCount) {
    for (int i = 0; i < selectedCount; i++) {
        if (selectedPayments[i] == payment) return true;
    }
    return false;
}

bool isValidPartialPayment(int amount, PaymentUnit* unit, int* selectedPayments, int selectedCount) {
    // �̹� ���õ� �ݾ����� Ȯ��
    if (isPaymentSelected(amount, selectedPayments, selectedCount)) {
        printf("�̹� ���õ� �κ� ���� �ݾ��Դϴ�.\n");
        return false;
    }

    // ��ȿ�� �κ� ���� �ݾ����� Ȯ��
    for (int i = 0; i < unit->paymentCount; i++) {
        if (unit->partialPayments[i] == amount) {
            return true;
        }
    }

    printf("�ش��ϴ� �κ� ���� �ݾ��� �����ϴ�.\n");
    return false;
}
