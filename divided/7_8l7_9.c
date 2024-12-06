#include "pos.h"

// 7.8 �ֹ� ���� ������Ʈ
void createOrder() {
    int tablesWithOrders[MAX_TABLE_NUMBER];  
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n�ֹ� ������ �ִ� ���̺� ��ȣ");  

    FILE* foodFile = fopen(FILE_PATH, "r+");
    if (foodFile == NULL) {
        printf("������ �� �� �����ϴ�.\n");
        return;
    }
    rewind(foodFile);

    int tableNumber = 0;
    while (1) {
        tableNumber = inputTableNumber(false);
        if (tableNumber < -1) continue;
        else break;
    }

    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);
    
    // �� ���̺����� Ȯ��
    bool isEmptyTable = true;
    FILE* checkFile = fopen(tableFilePath, "r");
    if (checkFile) {
        char line[256];
        while (fgets(line, sizeof(line), checkFile)) {
            if (!isspace((unsigned char)line[0])) {
                isEmptyTable = false;
                break;
            }
        }
        fclose(checkFile);
    }
    
    printFoodList();  // �Ǹ� ��� ���

    int selection = -1;  
    OrderItem* orderList = NULL;
    int firstNum, secondNum, price;
    char foodName[50];
    bool orderMade = false;  // ���� �ֹ��� �߻��ߴ��� �����ϴ� �÷���

    while (selection != 0) {  
        printf("\n<�ֹ��� �������� 0�� �Է��ϼ���>\n\n");
        selection = inputFoodNumber();

        if (selection == 0) {
            break;  
        }

        int currentMenuIndex = 0;
        int validSelection = 0;

        rewind(foodFile);
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0) {
                currentMenuIndex++;
                if (currentMenuIndex == selection) {
                    validSelection = 1;
                    int quantity = inputQuantity();

                    // ���� �ֹ��� ���, ���� ���� ������ ���߿� �� ����
                    if (isEmptyTable) {
                        orderMade = true;
                    }

                    // ������ŭ �ݺ��ؼ� �׸� �߰�
                    for (int i = 0; i < quantity; i++) {
                        orderList = addOrderItem(orderList, secondNum);
                    }
                    break;
                }
            }
        }

        if (!validSelection) {
            printf("�ش��ϴ� ������ �������� �����ϴ�.\n");
        }
    }

    // ���� �ֹ��� �߻��� ��쿡�� ���Ͽ� ����
    if (orderMade || orderList != NULL) {
        FILE* tempFile = fopen("temp.txt", "w");
        if (tempFile) {
            // 1. ���� �ֹ� ���� ����
            if (!isEmptyTable) {
                FILE* existingFile = fopen(tableFilePath, "r");
                if (existingFile) {
                    char line[256];
                    while (fgets(line, sizeof(line), existingFile)) {
                        if (line[0] != '#') {  // ���� �ֹ� ������ ����
                            fprintf(tempFile, "%s", line);
                        }
                    }
                    fclose(existingFile);
                }
            }

            // 2. ���ο� �ֹ� �߰�
            OrderItem* current = orderList;
            while (current != NULL) {
                for (int i = 0; i < current->quantity; i++) {
                    fprintf(tempFile, "%d\n", current->itemID);
                }
                current = current->next;
            }

            // 3. ���� ���� ���� (#) �߰�
            if (isEmptyTable && orderMade) {
                // ���ο� ���̺��� ��� ���� ���� ����
                fprintf(tempFile, "#%d\n", tableNumber);
            } else if (!isEmptyTable) {
                // ���� ���� ���� ���� ����
                FILE* existingFile = fopen(tableFilePath, "r");
                if (existingFile) {
                    char line[256];
                    while (fgets(line, sizeof(line), existingFile)) {
                        if (line[0] == '#' && line[1] != '#') {
                            fprintf(tempFile, "%s", line);
                        }
                    }
                    fclose(existingFile);
                }
            }

            // 4. �κ� ���� ���� (##) ����
            if (!isEmptyTable) {
                FILE* existingFile = fopen(tableFilePath, "r");
                if (existingFile) {
                    char line[256];
                    while (fgets(line, sizeof(line), existingFile)) {
                        if (line[0] == '#' && line[1] == '#') {
                            fprintf(tempFile, "%s", line);
                        }
                    }
                    fclose(existingFile);
                }
            }

            fclose(tempFile);

            // �ӽ� ������ ���� ���Ϸ� ��ü
            remove(tableFilePath);
            rename("temp.txt", tableFilePath);

            // �ֹ� ��� ���
            printf("\n%d�� ���̺� ", tableNumber);
            current = orderList;
            int itemCount = 0;
            while (current != NULL) {
                rewind(foodFile);
                while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
                    if (firstNum == 0 && secondNum == current->itemID) {
                        if (itemCount > 0) {
                            printf(" ");
                        }
                        printf("%s %d��", foodName, current->quantity);
                        itemCount++;
                        break;
                    }
                }
                current = current->next;
            }
            printf(" �ֹ��Ϸ�Ǿ����ϴ�.\n");
        }
    }

    // �޸� ����
    freeOrderItems(orderList);
    fclose(foodFile);
}
// 7.9 �ֹ� ��ȸ ������Ʈ
void printOrder() {
    // �ֹ� ������ ���̺� ��� ǥ��
    int tablesWithOrders[MAX_TABLE_NUMBER];  
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n�ֹ� ������ �ִ� ���̺� ��ȣ");

    int tableNumber = 0;
    while (1) {
        tableNumber = inputTableNumber(false);
        if (tableNumber < -1) continue;
        else break;
    }

    // ���� ���� ���� ��������
    PaymentUnit* unit = getPaymentUnit(tableNumber);
    if (unit->tableCount == 0) {
        // ���� ������ ���� ��� ���õ� ���̺� ó��
        unit->tables[0] = tableNumber;
        unit->tableCount = 1;
    }

    // ���� ��� - ���� ������ ���� ��� ���̺� ��ȣ ǥ��
    printf("\n[");
    for (int i = 0; i < unit->tableCount; i++) {
        printf("%d", unit->tables[i]);
        if (i < unit->tableCount - 1) printf(", ");
    }
    printf("]�� ���̺� �ֹ� ��ȸ\n\n");

    printf("%-20s %-10s %-10s\n", "�޴�", "����", "�ݾ�");

    // �Ǹ��׸��� ���� ����
    FILE* foodFile = fopen(FILE_PATH, "r");
    if (foodFile == NULL) {
        printf("�Ǹ� �׸� ������ �� �� �����ϴ�.\n");
        free(unit->partialPayments);
        free(unit);
        return;
    }

    // ��� �ֹ� ������ ������ ���� ����Ʈ
    OrderItem* orderList = NULL;
    int totalAmount = 0;
    int totalPartialPayments = 0;

    // ���� ������ ���� ��� ���̺��� �ֹ� ���� ����
    for (int t = 0; t < unit->tableCount; t++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unit->tables[t]);
        FILE* tableFile = fopen(tablePath, "r");
        if (!tableFile) continue;

        char line[256];
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] != '#') {
                int itemID;
                if (sscanf(line, "%d", &itemID) == 1) {
                    orderList = addOrderItem(orderList, itemID);
                }
            }
            else if (line[0] == '#' && line[1] == '#') {
                int payment;
                if (sscanf(line + 2, "%d", &payment) == 1) {
                    totalPartialPayments += payment;
                }
            }
        }
        fclose(tableFile);
    }

    // �ֹ� ���� ���
    int foundItems = 0;
    OrderItem* current = orderList;
    while (current != NULL) {
        rewind(foodFile);
        int firstNum, secondNum, price;
        char foodName[50];
        
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0 && secondNum == current->itemID) {
                int quantity = current->quantity;
                int itemTotal = quantity * price;
                printf("%-20s %-10d %-10d\n", foodName, quantity, itemTotal);
                totalAmount += itemTotal;
                foundItems++;
                break;
            }
        }
        current = current->next;
    }

    if (foundItems == 0) {
        printf("�ֹ��� �Ǹ� �׸��� �����ϴ�.\n");
    } else {
        printf("\n�հ�: %d��\n", totalAmount);
        if (totalPartialPayments > 0) {
            printf("�κ� ����: ");
            bool first = true;
            for (int t = 0; t < unit->tableCount; t++) {
                char tablePath[256];
                snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unit->tables[t]);
                FILE* tableFile = fopen(tablePath, "r");
                if (!tableFile) continue;

                char line[256];
                while (fgets(line, sizeof(line), tableFile)) {
                    if (line[0] == '#' && line[1] == '#') {
                        int payment;
                        if (sscanf(line + 2, "%d", &payment) == 1) {
                            if (!first) printf(", ");
                            printf("%d��", payment);
                            first = false;
                        }
                    }
                }
                fclose(tableFile);
            }
            printf("\n");
        }
    }

    // �޸� ����
    freeOrderItems(orderList);
    fclose(foodFile);
    free(unit->partialPayments);
    free(unit);
}
