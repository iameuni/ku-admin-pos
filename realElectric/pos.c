#include <stdio.h>

#define MAX_MENU_ITEMS 100
#define MAX_NAME_LENGTH 50  // ���ĸ��� �ִ� ����
#define MAX_TABLES 10

// ���� �׸�
typedef struct {
    char name[MAX_NAME_LENGTH];
    int price;
} MenuItem;

// �ֹ��� �׸�
typedef struct {
    int menuItemIndex;  // ������ �ε���
    int quantity;       // �ش� ���� �ֹ� ��
} OrderItem;

// ���̺�� �ֹ��� ������ �������� �Ѿ�
typedef struct {
    OrderItem items[MAX_MENU_ITEMS];
    int itemCount;
    int totalAmount;
} Order;

// ���Կ��� �Ȱ� �ִ� ��ü ���İ� ������
MenuItem menu[MAX_MENU_ITEMS];  
int menuItemCount = 0;

Order tables[MAX_TABLES];

// ��� ���̺��� �ֹ��� �ʱ�ȭ�ϴ� �Լ�
void initializeTables() {
}

// �Ÿ޴��� �߰��ϴ� �Լ�
void addMenuItem() {
}

// ������ ������Ű�� �Լ�
void removeMenuItem() {
}

// �ֹ� ���� �Լ�
void createOrder() {
}

// ���̺� �ֹ� ��ȸ �Լ�
void viewOrder() {
}

// ���� ó�� �Լ�
void processPayment() {
}

int main() {
    // ���� �޴� ���
    // ����� ���ÿ� ���� �Լ� ȣ��
    return 0;
}