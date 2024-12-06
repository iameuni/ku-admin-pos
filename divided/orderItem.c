#include "pos.h"

// �ֹ� �׸� ����Ʈ�� �׸� �߰� (7.9 �ֹ� ��ȸ ������Ʈ. 7.10 ���� ó�� ������Ʈ���� ���)
OrderItem* addOrderItem(OrderItem* head, int itemID) {
    OrderItem* current = head;
    // �̹� �����ϴ� �׸��̸� ���� ����
    while (current != NULL) {
        if (current->itemID == itemID) {
            current->quantity++;
            return head;
        }
        current = current->next;
    }
    // �� �׸� �߰�
    OrderItem* newItem = (OrderItem*)malloc(sizeof(OrderItem));
    if (newItem == NULL) {
        printf("�޸� �Ҵ� ����\n");
        return head;
    }
    newItem->itemID = itemID;
    newItem->quantity = 1;
    newItem->next = head;
    return newItem;
}
// �ֹ� �׸� ����Ʈ �����ϴ� �Լ� (7.9 �ֹ� ��ȸ ������Ʈ. 7.10 ���� ó�� ������Ʈ���� ���)
void freeOrderItems(OrderItem* head) {
    while (head != NULL) {
        OrderItem* temp = head;
        head = head->next;
        free(temp);
    }
}