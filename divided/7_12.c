#include "pos.h"

// 7.12 ���̺� �̵� ������Ʈ
int inputSourceTableNumber() {
    while (1) {
        int tableNumber = inputInt("�̵���ų ���̺��� �Է��ϼ���: ", false, false);
        if (tableNumber < 0) return tableNumber;  // ���� �ڵ� ��ȯ

        // ���̺��� �����ϴ��� Ȯ��
        if (tableNumber > MAX_TABLE_NUMBER || !isTableExist(tableNumber)) {
            printf("����: 1~%d ������ �����ϴ� ���̺� ��ȣ�� �Է��ϼ���.\n", MAX_TABLE_NUMBER);
            continue;
        }

        // �ֹ� ������ �ִ��� Ȯ��
        if (!hasOrders(tableNumber)) {
            printf("\n�ֹ� ������ ���� ���̺��Դϴ�.\n\n");
            continue;
        }

        return tableNumber;
    }
}
void moveTable() {
    int tablesWithOrders[MAX_TABLE_NUMBER];
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n�ֹ��� �ִ� ���̺� ��ȣ");

    // ��� ���̺� ����
    int sourceTable = inputSourceTableNumber();
    if (sourceTable < 0) return;

    // ���� ���� Ȯ�� (���� ����)
    PaymentUnit* sourceUnit = getPaymentUnit(sourceTable);
    if (sourceUnit->tableCount == 0) {
        sourceUnit->tables[0] = sourceTable;
        sourceUnit->tableCount = 1;
    }
    
    printf("{");
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        printf("%d", sourceUnit->tables[i]);
        if (i < sourceUnit->tableCount - 1) printf(", ");
    }
    printf("}�� ���̺��� �̵��� ���̺��� �Է��ϼ���{}: ");

    // ������ ���̺�� ����
    int destTables[MAX_TABLE_NUMBER];
    int destCount = 0;
    
    while (1) {
        if (destCount > 0) {
            printf("{");
            for (int i = 0; i < sourceUnit->tableCount; i++) {
                printf("%d", sourceUnit->tables[i]);
                if (i < sourceUnit->tableCount - 1) printf(", ");
            }
            printf("}�� ���̺��� �̵��� ���̺��� �Է��ϼ���{");
            
            // ���ݱ��� ���õ� �� ���̺� ���� ���� ���� ��ü�� ǥ��
            bool first = true;
            for (int i = 0; i < destCount; i++) {
                bool isSourceTable = false;
                // ��� ���̺��� ���� ������ ���� ���̺����� Ȯ��
                for (int j = 0; j < sourceUnit->tableCount; j++) {
                    if (sourceUnit->tables[j] == destTables[i]) {
                        isSourceTable = true;
                        break;
                    }
                }
                
                if (isSourceTable) {
                    // ��� ���̺� ���� ������ ���� ���̺��� �ܵ����� ǥ��
                    if (!first) printf(", ");
                    printf("%d", destTables[i]);
                    first = false;
                } else {
                    // �ٸ� ���̺��� ���� ���� ��ü ǥ��
                    PaymentUnit* unit = getPaymentUnit(destTables[i]);
                    if (unit->tableCount > 0) {
                        if (!first) printf(", ");
                        for (int j = 0; j < unit->tableCount; j++) {
                            printf("%d", unit->tables[j]);
                            if (j < unit->tableCount - 1) printf(", ");
                        }
                        first = false;
                    } else {
                        if (!first) printf(", ");
                        printf("%d", destTables[i]);
                        first = false;
                    }
                    free(unit->partialPayments);
                    free(unit);
                }
            }
            printf("}: ");
        }

        int destTable = inputTableNumber(true);
        if (destTable == -1) {
            if (destCount > 0) break;
            continue;
        }
        if (destTable == 0) {
            printf("�̵��� ��ҵǾ����ϴ�.\n");
            free(sourceUnit->partialPayments);
            free(sourceUnit);
            return;
        }

        // ��ȿ�� �˻�
        if (!isTableExist(destTable)) {
            printf("�������� �ʴ� ���̺� ��ȣ�Դϴ�.\n");
            continue;
        }

        // �̹� ���õ� ���̺����� Ȯ��
        bool alreadySelected = false;
        for (int i = 0; i < destCount; i++) {
            if (destTables[i] == destTable) {
                printf("�̹� ���õ� ���̺��Դϴ�.\n");
                alreadySelected = true;
                break;
            }
        }
        if (alreadySelected) continue;

        // ��� ���̺�� ������ ���̺� ���
        bool isSourceTable = false;
        for (int i = 0; i < sourceUnit->tableCount; i++) {
            if (sourceUnit->tables[i] == destTable) {
                isSourceTable = true;
                break;
            }
        }

        // ���õ� ���̺� �߰� (���� ������ ������� ���õ� ���̺�)
        destTables[destCount++] = destTable;
    }

    // ���̺� �̵� ����
    if (destCount > 0) {
        executeTableMove(sourceUnit, destTables, destCount);
    }

    free(sourceUnit->partialPayments);
    free(sourceUnit);
}
