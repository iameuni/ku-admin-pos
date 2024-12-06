#include "pos.h"

// 7.4.1 ���̺� ��ȣ �Է�
int inputTableNumber(bool paymentMode) {
    int tableNumber;
    int maxTableNumber = getLastTableNumber(); // �ִ� ���̺� ��ȣ�� ������

    // ����ڿ��� �Է� ��û
    if (!paymentMode) { // �Ϲ� ���
        printf("���̺� ��ȣ�� �Է��ϼ��� (1~%d): ", maxTableNumber);
        tableNumber = inputInt(NULL, false, false); // 0 �Է� ����
    }
    else {
        tableNumber = inputInt(NULL, true, false); // 0 �Է� ���
    }

    // ���� ��� �߰� ���� �˻�
    if (paymentMode) {
        // ���ͳ� ���� �Է� �� -1 ��ȯ
        if (tableNumber == -1) {
            return -1;
        }
        if (tableNumber == 0) {
            return 0;
        }
        if (tableNumber == -28) return -2;
        // ���̺� ��ȣ�� 0 �Ǵ� 1~maxTableNumber ������ �ְ�, �����ϴ��� Ȯ��
        if (tableNumber > maxTableNumber || !isTableExist(tableNumber)) {
            printf("����: 0�� ����, 1~%d ������ �����ϴ� ���̺� ��ȣ�� �Է� �����մϴ�.\n", maxTableNumber);
            return -2; // ��ȿ���� ���� ��ȣ �õ� �� -2 ��ȯ
        }
    }
    else {
        // �Ϲ� ��忡�� ��ȿ ������ ���̺��� �����ϴ��� Ȯ�� (1~maxTableNumber)
        if (tableNumber > maxTableNumber || !isTableExist(tableNumber)) {
            printf("����: 1~%d ������ �����ϴ� ���̺� ��ȣ�� �Է��ϼ���.\n", maxTableNumber);
            return -2; // ��ȿ���� ���� ��ȣ �õ� �� -2 ��ȯ
        }
    }

    return tableNumber; // ��ȿ�� ���̺� ��ȣ ��ȯ
}

// 7.4.2 ���� �Է�
int inputQuantity() {
    int quantity;
    while (1) {
        quantity = inputInt("������ �Է��ϼ���: ", false, false);
        if (quantity < 1 || quantity >99) {
            printf("����: 1~99������ ������ �Է��ϼ���.\n");
        }
        else {
            return quantity;
        }
    }
}

// 7.4.3 ���� �ݾ� �Է�
int inputPaymentAmount(int remainingBalance) {
    static int hasPartialPayment = 0;  // �κ� ���� ���θ� ����

    while (1) {
        printf("������ �ݾ��� �Է��ϼ��� [%d]: ", remainingBalance);
        int paymentAmount = inputInt(NULL, true, false);

        if (paymentAmount == (-MAX_INT + 9)) {  // '.' �Է� ��
            if (!hasPartialPayment) {
                printf("�κ� ���� ���� �� ���� ������ �����߽��ϴ�.\n");
            } else {
                printf("�κа��� �Ǿ����ϴ�.\n");
            }
            hasPartialPayment = 0;  // ���� �ʱ�ȭ
            return -2;
        }

        if (paymentAmount == -1) {  // ���� �Է� ��
            hasPartialPayment = 0;  // ���� �ʱ�ȭ
            if (remainingBalance == 0) {
                printf("%d�� ���� �Ϸ�Ǿ����ϴ�.\n", remainingBalance);
            } else {
                printf("%d�� ���� �Ϸ�Ǿ����ϴ�.\n", remainingBalance);
            }
            return 0;
        }

        if (paymentAmount == 0) {  // '0' �Է� ��
            printf("���� �ߴ�.\n");
            hasPartialPayment = 0;  // ���� �ʱ�ȭ
            return -1;
        }

        if (paymentAmount < -1) continue;

        if (paymentAmount > remainingBalance) {
            printf("����: ������ �ݾ׺��� Ů�ϴ�.\n");
            continue;
        }

        hasPartialPayment = 1;  // ��ȿ�� ���� �ݾ� �Է� �� ���� ����
        remainingBalance -= paymentAmount;
        printf("%d�� �� %d�� ���� ���� �Ǿ����ϴ�.\n", remainingBalance + paymentAmount, paymentAmount);
        
        if (remainingBalance == 0) {
            printf("%d�� ���� �Ϸ�Ǿ����ϴ�.\n", paymentAmount);
            hasPartialPayment = 0;  // ���� �ʱ�ȭ
        }
        
        return remainingBalance;
    }
}