#include "pos.h"

// 7.3.1 �Ǹ� �׸�� �Է�
char* inputFoodName() {
    char* s = (char*)malloc(sizeof(char) * (MAX_INPUT_NUM + 2));
    if (s == NULL) {
        fprintf(stderr, "�޸� �Ҵ� ����\n");
        return NULL;
    }

    while (1) {
        printf("�Ǹ� �׸��: ");
        if (fgets(s, MAX_INPUT_NUM + 2, stdin) == NULL) {
            printf("�Է� ���� �߻�\n");
        }
        else {
            s[strcspn(s, "\n")] = '\0'; // \n�� ����

            // �� �� ���� ����
            char* start = s;
            char* end = s + strlen(s) - 1;
            while (isspace((unsigned char)*start)) start++;
            while (end > start && isspace((unsigned char)*end)) end--;
            memmove(s, start, end - start + 1);
            s[end - start + 1] = '\0';

            // ���� �˻� (���� ���� ��)
            int len = strlen(s);
            if (len < 1 || len > 20) {
                printf("���: �Է��� 1�̻� 20�����̾�� �մϴ�. �ٽ� �Է����ּ���.\n");
            }
            else {
                // ���ĺ� �˻�
                int valid = 1;
                for (int i = 0; s[i] != '\0'; i++) {
                    if (!isalpha((unsigned char)s[i])) {
                        valid = 0;
                        break;
                    }
                }
                if (!valid) {
                    printf("���: �׸���� ���ĺ��� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���.\n");
                }
                else {
                    return s;
                }
            }
        }
    }
}

// 7.3.2 �Ǹ� �׸� �Է�
int inputPrice() {
    int price;
    while (1) {
        price = inputInt("�Ǹ� �׸�: ", false,false);
        if (price >= 1 && price <= 9999999) {
            return price;
        }
        else {
            printf("����: 1~9999999������ ���� �Է��ϼ���.\n");
        }
    }
}