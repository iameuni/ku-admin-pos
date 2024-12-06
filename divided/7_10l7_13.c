#include "pos.h"

PaymentContext currentContext;

// 7.10 ���� ó�� ������Ʈ
void makePayment() {
    int tablesWithOrders[MAX_TABLE_NUMBER];
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n���� ������ ���̺� ��ȣ");

    currentContext.tableCount = 0;  
    int primarySelectedTable = -1;

    // ������ ���̺� ����
    while (true) {
        printf("���̺� ��ȣ�� �Է��ϼ��� {");
        for (int i = 0; i < currentContext.tableCount; i++) {
            printf("%d", currentContext.tableNumbers[i]);
            if (i < currentContext.tableCount - 1) printf(", ");
        }
        printf("}: ");

        int input = inputTableNumber(true);
        if (input == -1) {
            if (currentContext.tableCount > 0) break;
            continue;
        }
        if (input == 0) {
            printf("\n������ ����˴ϴ�.\n");
            return;
        }

        // �ֹ� ���� �ִ��� Ȯ��
        bool validOrder = false;
        for (int i = 0; i < orderCount; i++) {
            if (tablesWithOrders[i] == input) {
                validOrder = true;
                break;
            }
        }
        if (!validOrder) {
            printf("�ֹ� ������ ���� ���̺��Դϴ�.\n");
            continue;
        }

        // �̹� ���õ� ���̺����� Ȯ��
        bool alreadySelected = false;
        for (int i = 0; i < currentContext.tableCount; i++) {
            if (currentContext.tableNumbers[i] == input) {
                alreadySelected = true;
                break;
            }
        }
        if (alreadySelected) {
            printf("�̹� �Է��� ���̺� ��ȣ�Դϴ�.\n");
            continue;
        }

        if (primarySelectedTable == -1) {
            primarySelectedTable = input;
        }
        
        PaymentUnit* unit = getPaymentUnit(input);
        if (unit->tableCount > 0) {
            for (int i = 0; i < unit->tableCount; i++) {
                bool exists = false;
                for (int j = 0; j < currentContext.tableCount; j++) {
                    if (currentContext.tableNumbers[j] == unit->tables[i]) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    currentContext.tableNumbers[currentContext.tableCount++] = unit->tables[i];
                }
            }
        } else {
            currentContext.tableNumbers[currentContext.tableCount++] = input;
        }
        free(unit->partialPayments);
        free(unit);
    }

    // �� ���̺��� �ֹ��� ��� �� ���
    printf("\n");
    int totalOrderAmount = 0;
    int totalPartialPayments = 0;

    for (int i = 0; i < currentContext.tableCount; i++) {
        int tableNumber = currentContext.tableNumbers[i];
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);

        FILE* tableFile = fopen(tablePath, "r");
        if (!tableFile) continue;

        FILE* foodFile = fopen(FILE_PATH, "r");
        if (!foodFile) {
            fclose(tableFile);
            continue;
        }

        // �ֹ� ���� ����
        OrderItem* orderList = NULL;
        int itemID;
        char line[256];
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] != '#') {
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

        // �ֹ� �ݾ� ���
        int tableTotal = 0;
        OrderItem* current = orderList;
        while (current != NULL) {
            rewind(foodFile);
            int firstNum, secondNum, price;
            char foodName[50];
            while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
                if (firstNum == 0 && secondNum == current->itemID) {
                    tableTotal += price * current->quantity;
                    break;
                }
            }
            current = current->next;
        }

        printf("%d�� ���̺� �ֹ���: %d\n", tableNumber, tableTotal);
        totalOrderAmount += tableTotal;

        freeOrderItems(orderList);
        fclose(foodFile);
        fclose(tableFile);
    }

    // ���� ���� �ݾ� ���
    int remainingBalance = totalOrderAmount - totalPartialPayments;


    printf("�� �ֹ���: %d��\n", totalOrderAmount);
    printf("���� �κ� ������: %d��\n", totalPartialPayments);
    printf("���� ������: %d��\n\n", remainingBalance);

    // ������ ���� �ݾ� �Է� �ޱ�
// makePayment �Լ� ���� ���� ó�� �κ�
    while (true) {
        int payment = inputPaymentAmount(remainingBalance);
        
        if (payment == -1) {  // ���� ��� (0 �Է�)
            break;
        }
        
        if (payment == -2) {  // �κ� ���� (. �Է�)
            updatePaymentUnit(primarySelectedTable, currentContext.tableNumbers, currentContext.tableCount);
            break;
        }

        // ���� ������ �߻��� ��쿡�� (payment�� 0�� �ƴ� ����) ���� ��� ������Ʈ
        if (remainingBalance != payment) {  // ��, ������ ������ �ݾ��� ���� ����
            updatePaymentRecord(primarySelectedTable, remainingBalance - payment);
        }
        remainingBalance = payment;

        if (remainingBalance == 0) {
            printf("���̺��� ���ðڽ��ϱ�?: ");
            char input[10];
            fgets(input, sizeof(input), stdin);
            if (input[0] != '.') {
                // ��� ���õ� ���̺� ����
                for (int i = 0; i < currentContext.tableCount; i++) {
                    char tablePath[256];
                    snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, currentContext.tableNumbers[i]);
                    FILE* file = fopen(tablePath, "w");
                    if (file) fclose(file);
                }
                printf("���� ������ ���̺��� ������ϴ�.\n");
            } else {
                printf("���̺��� ����� �ʽ��ϴ�.\n");
            }
            break;
        }
    }
}

// 7.13 ���� ��� ������Ʈ
void cancelPayment() {
    int tablesWithPayments[MAX_TABLE_NUMBER];
    int paymentCount = 0;
    listTablesWithPartialPayments(tablesWithPayments, &paymentCount, 
        "\n�κа��� �׸��� �ִ� ���̺� ��ȣ");
    
    if (paymentCount == 0) {
        printf("�κ� ���� �׸��� �ִ� ���̺��� �����ϴ�.\n");
        return;
    }

    while (1) {
        int tableNumber = inputTableNumber(false);
        if (tableNumber < 0) continue;

        PaymentUnit* unit = getPaymentUnit(tableNumber);
        
        // ���� ���� ���� ��� ���̺��� �κ� ���� ���� Ȯ��
        bool hasPartialPayments = false;
        if (unit->tableCount > 0) {
            for (int i = 0; i < unit->tableCount; i++) {
                char tablePath[256];
                snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unit->tables[i]);
                FILE* file = fopen(tablePath, "r");
                if (!file) continue;

                char line[256];
                while (fgets(line, sizeof(line), file)) {
                    if (line[0] == '#' && line[1] == '#') {
                        hasPartialPayments = true;
                        break;
                    }
                }
                fclose(file);
                if (hasPartialPayments) break;
            }
        }

        if (!hasPartialPayments) {
            printf("����� �κа��� ������ �����ϴ�.\n");
            free(unit->partialPayments);
            free(unit);
            continue;
        }

        int selectedPayments[100] = {0};
        int selectedCount = 0;

        // �κ� ���� �ݾ� ������ ������ ����ü �迭
        typedef struct {
            int amount;
            int remainingCount;  // ���� ���õ��� ���� �ش� �ݾ��� ����
            int totalCount;      // �ش� �ݾ��� ��ü ����
        } PaymentInfo;
        
        PaymentInfo payments[100] = {0};
        int uniquePaymentCount = 0;

        // ��� �κ� ���� ������ �� ���� ����
        for (int t = 0; t < unit->tableCount; t++) {
            char tablePath[256];
            snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unit->tables[t]);
            FILE* tableFile = fopen(tablePath, "r");
            if (!tableFile) continue;

            char line[256];
            while (fgets(line, sizeof(line), tableFile)) {
                if (line[0] == '#' && line[1] == '#') {
                    int amount;
                    sscanf(line + 2, "%d", &amount);
                    
                    // �̹� �ִ� �ݾ����� Ȯ��
                    bool found = false;
                    for (int i = 0; i < uniquePaymentCount; i++) {
                        if (payments[i].amount == amount) {
                            payments[i].totalCount++;
                            payments[i].remainingCount++;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        payments[uniquePaymentCount].amount = amount;
                        payments[uniquePaymentCount].totalCount = 1;
                        payments[uniquePaymentCount].remainingCount = 1;
                        uniquePaymentCount++;
                    }
                }
            }
            fclose(tableFile);
        }

        while (1) {
            // ���� �κ� ���� �׸� ���
            printf("���� ����� �κа��� �׸��� ���ÿ� [");
            bool first = true;
            for (int i = 0; i < uniquePaymentCount; i++) {
                if (payments[i].remainingCount > 0) {
                    for (int j = 0; j < payments[i].remainingCount; j++) {
                        if (!first) printf(", ");
                        printf("%d", payments[i].amount);
                        first = false;
                    }
                }
            }
            printf("]: ");

            int amount = inputInt(NULL, true, false);
            if (amount == 0) {
                printf("���� ��Ұ� ����˴ϴ�.\n");
                break;
            }
            if (amount == -1) {
                if (selectedCount == 0) {
                    printf("����� �׼��� �����ϴ�. ��� �۾��� �ߴ��Ϸ��� 0�� �Է��ϼ���\n");
                    continue;
                }
                executeCancelPayments(unit, selectedPayments, selectedCount);
                break;
            }

            // �Էµ� �ݾ��� ��ȿ���� Ȯ��
            bool valid = false;
            for (int i = 0; i < uniquePaymentCount; i++) {
                if (payments[i].amount == amount && payments[i].remainingCount > 0) {
                    valid = true;
                    selectedPayments[selectedCount++] = amount;
                    payments[i].remainingCount--;
                    break;
                }
            }
            
            if (!valid) {
                printf("�ش��ϴ� �κ� ���� �ݾ��� �����ϴ�.\n");
            }
        }

        free(unit->partialPayments);
        free(unit);
        return;
    }
}
