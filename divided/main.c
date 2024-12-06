#include "pos.h"

//////////////////// ��ȹ�� ��� ������Ʈ ////////////////////

// 7.1 ������ ���� ���Ἲ �˻�
bool checkDataIntegrity() {
    ///// �Ǹ� �׸� ������ ���� �˻� (���� �ڵ�� ����) /////
    FILE* foodFile = fopen(FILE_PATH, "r+");
    if (foodFile == NULL) {
        printf("������ �� �� �����ϴ�.\n");
        return false;
    }

    int itemIds[100];
    int itemCount = 0;
    char line[256];

    // �Ǹ� �׸� ������ �б�
    int lineNumber = 0;
    int expectedId = 1;
    while (fgets(line, sizeof(line), foodFile) && itemCount < 100) {
        lineNumber++;
        int firstNum, id;
        char name[50];
        int price;

        char* ptr = line;
        while (*ptr && isspace(*ptr)) ptr++;
        if (sscanf(ptr, "%d", &firstNum) != 1) continue;
        while (*ptr && !isspace(*ptr)) ptr++;
        while (*ptr && isspace(*ptr)) ptr++;
        if (sscanf(ptr, "%d", &id) != 1) continue;

        while (*ptr && !isspace(*ptr)) ptr++;
        while (*ptr && isspace(*ptr)) ptr++;
        if (sscanf(ptr, "%s", name) != 1) continue;
        while (*ptr && !isspace(*ptr)) ptr++;
        while (*ptr && isspace(*ptr)) ptr++;
        if (sscanf(ptr, "%d", &price) != 1) continue;

        // ���� ��ȣ �ߺ� �˻�
        for (int i = 0; i < itemCount; i++) {
            if (itemIds[i] == id) {
                printf("�Ǹ� �׸� ������ ���� %d��° �ٰ� %d��° �ٿ��� ���� ��ȣ �ߺ��� �߻��߽��ϴ�. ���α׷��� �����մϴ�.\n",
                    i + 1, lineNumber);
                fclose(foodFile);
                return false;
            }
        }

        // ���� ��ȣ ������ ���� �˻�
        if (id != expectedId) {
            printf("�Ǹ� �׸� ������ ������ ���� ��ȣ�� �ùٸ� ������ �������� �ʽ��ϴ�. ���α׷��� �����մϴ�.\n");
            fclose(foodFile);
            return false;
        }

        itemIds[itemCount++] = id;
        expectedId++;
    }
    fclose(foodFile);

    ///// ���̺� ������ ���� �˻� (������ �κ�) /////
    int tableCount = 0;
    int existingTables[11] = { 0 };  // �����ϴ� ���̺� ��ȣ üũ�� (0�� �ε����� ������)

    // ���̺� ������ ���� �˻� �κ� ����
    for (int table = 1; table <= MAX_TABLE_NUMBER; table++) {
        char tableFileName[256];
        snprintf(tableFileName, sizeof(tableFileName), "%s/%d.txt", TABLE_FILE_PATH, table);

        FILE* tableFile = fopen(tableFileName, "r");
        if (!tableFile) continue;

        int tableLineNumber = 0;
        char line[256];
        int paymentUnitTables[MAX_TABLE_NUMBER] = {0};
        int paymentUnitCount = 0;
        int totalPartialPayments = 0;
        
        while (fgets(line, sizeof(line), tableFile)) {
            tableLineNumber++;
            line[strcspn(line, "\n")] = 0;  // ���� ���� ����

            if (line[0] == '#') {
                if (line[1] == '#') {
                    // ## �κ� ���� �ݾ� �˻�
                    char* paymentStr = line + 2;
                    while (*paymentStr == ' ') paymentStr++;  // ���� ���� ����
                    
                    // 0���� �����ϴ��� �˻�
                    if (paymentStr[0] == '0' && strlen(paymentStr) > 1) {
                        printf("���̺� ������ ���� %d��° ��: �κ� ���� �ݾ��� 0���� ������ �� �����ϴ�. ���α׷��� �����մϴ�.\n", tableLineNumber);
                        fclose(tableFile);
                        return false;
                    }

                    // ���ڸ� �����ϴ��� �˻�
                    for (int i = 0; paymentStr[i]; i++) {
                        if (!isdigit(paymentStr[i])) {
                            printf("���̺� ������ ���� %d��° ��: �κ� ���� �ݾ��� �ùٸ��� �ʽ��ϴ�. ���α׷��� �����մϴ�.\n", tableLineNumber);
                            fclose(tableFile);
                            return false;
                        }
                    }

                    int payment = atoi(paymentStr);
                    if (payment <= 0) {
                        printf("���̺� ������ ���� %d��° ��: �κ� ���� �ݾ��� ���� ������ �ƴմϴ�. ���α׷��� �����մϴ�.\n", tableLineNumber);
                        fclose(tableFile);
                        return false;
                    }
                    totalPartialPayments += payment;
                }
                else {
                    // # ���� ���� ���̺� ��ȣ �˻�
                    char* tableStr = line + 1;
                    while (*tableStr == ' ') tableStr++;  // ���� ���� ����
                    
                    // ���ڸ� �����ϴ��� �˻�
                    for (int i = 0; tableStr[i]; i++) {
                        if (!isdigit(tableStr[i])) {
                            printf("���̺� ������ ���� %d��° ��: ���� ���� ���̺� ��ȣ�� �ùٸ��� �ʽ��ϴ�. ���α׷��� �����մϴ�.\n", tableLineNumber);
                            fclose(tableFile);
                            return false;
                        }
                    }

                    int unitTable = atoi(tableStr);
                    if (unitTable < 1 || unitTable > MAX_TABLE_NUMBER || !isTableExist(unitTable)) {
                        printf("���̺� ������ ���� %d��° ��: �ùٸ��� ���� ���� ���� ���̺� ��ȣ�Դϴ�. ���α׷��� �����մϴ�.\n", tableLineNumber);
                        fclose(tableFile);
                        return false;
                    }

                    // ���� ������ �߰�
                    paymentUnitTables[paymentUnitCount++] = unitTable;
                }
            }
            else {
                int saleItemId;
                if (sscanf(line, "%d", &saleItemId) != 1) {
                    printf("���̺� ������ ������ %d��° �ٿ��� �ùٸ� ������ �Ǹ� �׸� ���� ��ȣ�� ã�� �� �����ϴ�. ���α׷��� �����մϴ�.\n", tableLineNumber);
                    fclose(tableFile);
                    return false;
                }
                
                // �Ǹ� �׸� ������ ���Ͽ� �����ϴ� ��ȣ���� Ȯ��
                bool found = false;
                for (int i = 0; i < itemCount; i++) {
                    if (itemIds[i] == saleItemId) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    printf("���̺� ������ ������ %d��° ���� �Ǹ� �׸� ���� ��ȣ %d�� �ùٸ� �Ǹ� �׸� ���� ��ȣ�� �ƴմϴ�. ���α׷��� �����մϴ�.\n", tableLineNumber, saleItemId);
                    fclose(tableFile);
                    return false;
                }
            }
        }

        // ���� ���� ��ȣ ���� �˻�
        if (paymentUnitCount > 0) {
            for (int i = 0; i < paymentUnitCount; i++) {
                int unitTable = paymentUnitTables[i];
                char unitFileName[256];
                snprintf(unitFileName, sizeof(unitFileName), "%s/%d.txt", TABLE_FILE_PATH, unitTable);
                
                FILE* unitFile = fopen(unitFileName, "r");
                if (!unitFile) {
                    printf("���� ������ ���� ���̺� %d�� ������ ã�� �� �����ϴ�. ���α׷��� �����մϴ�.\n", unitTable);
                    fclose(tableFile);
                    return false;
                }

                // ���� ���� ���̺���� ���θ� �����ϴ��� �˻�
                bool foundAllReferences = true;
                for (int j = 0; j < paymentUnitCount; j++) {
                    bool foundReference = false;
                    rewind(unitFile);
                    
                    while (fgets(line, sizeof(line), unitFile)) {
                        if (line[0] == '#' && line[1] != '#') {
                            int refTable = atoi(line + 1);
                            if (refTable == paymentUnitTables[j]) {
                                foundReference = true;
                                break;
                            }
                        }
                    }

                    if (!foundReference) {
                        foundAllReferences = false;
                        break;
                    }
                }

                fclose(unitFile);
                
                if (!foundAllReferences) {
                    printf("���� ������ ���� ���̺���� ��ȣ ������ �ùٸ��� �ʽ��ϴ�. ���α׷��� �����մϴ�.\n");
                    fclose(tableFile);
                    return false;
                }
            }
        }

        fclose(tableFile);
    }

    return true;
}

// 7.11 ���̺� ���� ������Ʈ
void adjustTables() {
    while (1) {
        int currentTableCount = getCurrentTableCount();

        printf("\n������ų ���̺� ���� �Է��ϼ��� (-%d ~ %d): ",MAX_TABLE_ADJUST, MAX_TABLE_ADJUST);

       
        int adjustment = inputInt(NULL, true, true);

        // '0' �Է� �� ���
        if (adjustment == 0) {
            printf("���̺� ���� �۾��� ��ҵǾ����ϴ�.\n");
            return;
        }

        // ���� ���� (-MAX_TABLE_ADJUST ~ MAX_TABLE_ADJUST)
        if (adjustment < -MAX_TABLE_ADJUST || adjustment > MAX_TABLE_ADJUST) {
            printf("�ùٸ��� ���� �Է��Դϴ�.\n");
            continue;
        }

        // ���̺� ����
        if (adjustment < 0) {
            // ���� ���̺� ���� ���ҽ�Ű���� ������ ���ų� ���� ���
            if (currentTableCount <= -adjustment) {
                printf("����: ���� ���̺��� %d�� ���̶� %d���� ���ҽ�ų �� �����ϴ�.\n",
                    currentTableCount, -adjustment);
                continue;
            }

            // ���� �� ���̺� ���� 1�� �̸��� �Ǵ� ���
            if (currentTableCount + adjustment < 1) {
                printf("����: ���̺��� �ּ� 1���� �����ؾ� �մϴ�.\n");
                continue;
            }

            // �ֹ� ������ ���� ���̺� �� ���
            int emptyCount = 0;
            for (int i = MAX_TABLE_NUMBER; i >= 1; i--) {
                if (isTableExist(i) && !hasOrders(i)) {
                    emptyCount++;
                }
            }

            // �ֹ� ������ ���� ���̺��� �ϳ��� ���� ���
            if (emptyCount == 0) {
                printf("����: ��� ���̺� �ֹ� ������ �־ ���ҽ�ų �� �����ϴ�.\n");
                continue;
            }

            // �ֹ� ������ ���� ���̺� ���� ���ҽ�Ű���� ������ ���� ���
            if (emptyCount < -adjustment) {
                printf("����: �ֹ� ������ ���� ���̺��� %d�� ���̶� %d���� ���ҽ�ų �� �����ϴ�.\n",
                    emptyCount, -adjustment);
                continue;
            }

            // ���̺� ���� ����
            int removed = 0;
            while (removed < -adjustment) {
                // ���� ū ��ȣ�� �� ���̺� ã��
                int tableToRemove = -1;
                for (int i = MAX_TABLE_NUMBER; i >= 1; i--) {
                    if (isTableExist(i) && !hasOrders(i)) {
                        tableToRemove = i;
                        break;
                    }
                }

                if (tableToRemove == -1) {
                    printf("����ġ ���� ������ �߻��߽��ϴ�.\n");
                    return;
                }

                char tableFilePath[256];
                snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, tableToRemove);
                remove(tableFilePath);
                printf("%d�� ���̺��� ���ŵǾ����ϴ�.\n", tableToRemove);
                removed++;
            }
        }
        // ���̺� ����
        else {
            if (currentTableCount + adjustment > MAX_TABLE_NUMBER) {
                printf("���̺��� �ִ� %d�������� ������ �� �ֽ��ϴ�.\n", MAX_TABLE_NUMBER);
                continue;
            }

            // ���̺� ���� ����
            int added = 0;
            while (added < adjustment) {
                // ��� ������ ���� ���� ��ȣ ã��
                int newTableNumber = -1;
                for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
                    if (!isTableExist(i)) {
                        newTableNumber = i;
                        break;
                    }
                }

                if (newTableNumber == -1) {
                    printf("�� �̻� �߰��� �� �ִ� ���̺� ��ȣ�� �����ϴ�.\n");
                    return;
                }

                char tableFilePath[256];
                snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, newTableNumber);
                FILE* tableFile = fopen(tableFilePath, "w");
                if (tableFile != NULL) {
                    fclose(tableFile);
                    printf("%d�� ���̺��� �߰��Ǿ����ϴ�.\n", newTableNumber);
                    added++;
                }
            }
        }

        // ���������� ó���� ��� �Լ� ����
        return;
    }
}

// 7.14 ���� �޴� ������Ʈ
int printMain() {
    int s;
    while (1) {
        printf("\n���� �޴�\n");
        printf("1. �Ǹ� �׸� ��ȸ\n");
        printf("2. �Ǹ� �׸� �߰�\n");
        printf("3. �Ǹ� �׸� ����\n");
        printf("4. �ֹ� ����\n");
        printf("5. �ֹ� ��ȸ\n");
        printf("6. ���� ó��\n");
        printf("7. ���̺� ����\n");
        printf("8. ���̺� �̵�\n");
        printf("9. ���� ���\n");
        printf("10. ����\n");
        s = inputInt("�޴� ����: ", false, false);
        if (s > 10) {
            printf("1~10 ������ ���� �Է����ּ���.\n");
        }
        else {
            return s;
        }
    }
}

// ���α׷� ����
int main(void) {
    while (1) {

        if (!checkDataIntegrity()) {
            exitProgram();
        }

        int s = printMain();
        switch (s) {
        case 1:
            // 7.5 �Ǹ� �׸� ��ȸ ������Ʈ
            printFoodList();
            break;
        case 2:
            // 7.6 �Ǹ� �׸� �߰� ������Ʈ
            addToFoodList();
            break;
        case 3:
            // 7.7 �Ǹ� �׸� ���� ������Ʈ
            removeFoodItem();
            break;
        case 4:
            // 7.8 �ֹ� ���� ������Ʈ
            createOrder();
            break;
        case 5:
            // 7.9 �ֹ� ��ȸ ������Ʈ
            printOrder();
            break;
        case 6:
            // 7.10 ���� ó�� ������Ʈ
            makePayment();
            break;
        case 7:
            // 7.11 ���̺� ���� ������Ʈ
            adjustTables();
            break;
        case 8:
            // 7.12 ���̺� �̵� ������Ʈ
            moveTable();
            break;
        case 9:
            // 7.13 ���� ��� ������Ʈ
            cancelPayment();
            break;
        case 10:
            // 7.14 ���� �޴� ������Ʈ�� ���� ���
            exitProgram();
            break;
        default:
            break;
        }
    }

    return 0;
}