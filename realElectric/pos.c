#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 100
#define FILE_PATH "..\\x64\\Debug\\foodlist.txt"  // ���� ��� ����
#define TABLE_FILE_PATH "..\\x64\\Debug\\table" //���̺� ���� ��� ����

typedef struct OrderItem { //�ֹ��� �� �ִ� ����/�Ǹ� �׸� ��� ���� ������ ��� �޸� �Ҵ�
    int itemID;
    int quantity;
    struct OrderItem* next;
} OrderItem;

// �Է� ��Ģ. \n�� �����ϱ� ������ ���� �Է��� ��� ��
static int getInt() {
    char n[MAX_INPUT + 2]; 
        char* endptr;
    long num;

    while (1) {
        if (fgets(n, sizeof(n), stdin) == NULL) {
            printf("�Է� ������ �߻��߽��ϴ�. �ٽ� �õ����ּ���.\n");
        }
        else {
            n[strcspn(n, "\n")] = '\0';
            if (strlen(n) > MAX_INPUT) {
                printf("���: %d�� ���Ϸ� ���ڸ� �Է����ּ���.\n", MAX_INPUT);
            }
            else {
                char* start = n;
                char* end = n + strlen(n) - 1;
                while (isspace((unsigned char)*start)) start++;
                if (*start == '\0') {
                    printf("����: �Է°��� ����ֽ��ϴ�.\n");
                }
                else {
                    while (end > start && isspace((unsigned char)*end)) end--;
                    *(end + 1) = '\0';

                    num = strtol(start, &endptr, 10);

                    if (*endptr != '\0') {
                        printf("����: ���� �ƴ� ������ �Է����ּ���.\n");
                    }
                    else {
                        if (num < 0 || num > INT_MAX) {
                            printf("����: %d ������ ���� �ƴ� ������ �Է����ּ���.\n", INT_MAX);
                        }
                        else {
                            return (int)num;
                        }
                    }
                }
            }
        }
    }
}

static char* getString() {
    char* s = (char*)malloc(sizeof(char) * (MAX_INPUT + 2));
    if (s == NULL) {
        fprintf(stderr, "�޸� �Ҵ� ����\n");
        return NULL;
    }
    while (1) {
        if (fgets(s, MAX_INPUT + 2, stdin) == NULL) {
            printf("�Է� ���� �߻�\n");
        }
        else {
            s[strcspn(s, "\n")] = '\0'; 
            // ���ĺ��� �ƴ� ���ڰ� �ִ��� Ȯ��
            int valid = 1;
            for (int i = 0; s[i] != '\0'; i++) {
                if (!isalpha((unsigned char)s[i])) {  // ���ĺ��� �ƴϸ� ����
                    valid = 0;
                    break;
                }
            }
            if (!valid) {
                printf("���: �׸���� ���ĺ��� �Է��ؾ� �մϴ�. �ٽ� �Է����ּ���.\n");
            }
            else if (strlen(s) <= MAX_INPUT) {
                return s;  // �Է��� ��ȿ�ϸ� ��ȯ
            }
            else {
                printf("���: �Է��� �ʹ� ��ϴ�.\n");
            }
        }
    }
}


// ���� �޴� ��� �� ����� ����
static int printMain(void) {
    int s;
    while (1) {
        printf("\n���� �޴�\n");
        printf("1. �Ǹ� �׸� ��ȸ\n");
        printf("2. �Ǹ� �׸� �߰�\n");
        printf("3. �Ǹ� �׸� ����\n");
        printf("4. �ֹ� ����\n");
        printf("5. �ֹ� ��ȸ\n");
        printf("6. ���� ó��\n");
        printf("7. ����\n");
        printf("�޴� ����: ");
        s = getInt();
        if (s > 7 || s < 1) {
            printf("1~7 ������ ���� �Է����ּ���.\n");
        }
        else {
            return s;
        }
    }
}

// �Ǹ� ��� ���
static void printFoodList(FILE* foodFile) {
    char line[100]; // �� ���� ������ ���ڿ�
    int line_count = 0;
    int firstNum, secondNum, price;
    char food[50];  // ���� �̸� ����

    printf("\n===== �Ǹ� �׸� ��� =====\n");

    rewind(foodFile);  // ���� �����͸� ó������ �ǵ���
    while (fgets(line, sizeof(line), foodFile)) {
        sscanf(line, "%d  %d    %s  %d", &firstNum, &secondNum, food, &price);  // ���� 2ĭ, 4ĭ, 2ĭ ����

        if (firstNum == 0) {
            printf("%d. %s - %d\n", ++line_count, food, price);
        }
    }

    if (line_count == 0) {
        printf("�Ǹ� ���� �׸��� �����ϴ�.\n");
    }
}

// ���� ��ȣ�� �������� �Լ�
int getLastSecondNumber(FILE* file) {
    int firstNum, secondNum, price;
    char foodName[50];
    int lastSecondNum = 0;

    rewind(file);  // ���� �����͸� ó������ �ǵ���
    while (fscanf(file, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {  // ���� 2ĭ, 4ĭ, 2ĭ ����
        lastSecondNum = secondNum;
    }
    return lastSecondNum;
}

// �Ǹ� �׸� �߰�
static void addToFoodList(FILE* foodFile) {
    int firstNum = 0;
    int secondNum = getLastSecondNumber(foodFile) + 1;

    printf("\n�߰��� �Ǹ� �׸� ������ �Է��ϼ���(�׸���� ���ĺ��� ���)\n");
    printf("�Ǹ� �׸��: ");
    char* foodName = getString();

    if (foodName != NULL) {
        printf("�Ǹ� �׸�: ");
        int price = getInt();

        fseek(foodFile, 0, SEEK_END);  // ������ ������ �̵�
        fprintf(foodFile, "%d  %d    %s  %d\n", firstNum, secondNum, foodName, price);  // ���� 2ĭ, 4ĭ, 2ĭ ���� ����
        printf("%s ��/�� �߰��Ǿ����ϴ�.\n", foodName);

        free(foodName);
    }
    else {
        printf("�޸� �Ҵ� ����\n");
    }
}

// �Ǹ� �׸� ���� �Լ�
static void removeFoodItem(FILE* foodFile) {
    printFoodList(foodFile);  // ���� �׸��� ���
    printf("������ �׸� ��ȣ�� �Է��ϼ���: ");
    int userChoice = getInt();  // ����ڰ� ������ �׸� ��ȣ (��� ����)

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("�ӽ� ������ �� �� �����ϴ�.\n");
        return;
    }

    char line[256];
    int firstNum, secondNum, price;
    char foodName[50];
    int currentZeroIndex = 0;  // 0�� �׸� ī��Ʈ
    int found = 0;

    rewind(foodFile);  // ���� �����͸� ó������ �ǵ���

    // ���� ���Ͽ��� �����͸� �а�, �ӽ� ���Ϸ� �����ϸ鼭 ���¸� ����
    while (fgets(line, sizeof(line), foodFile)) {
        // ��Ȯ�� ���鿡 ���߾� �����͸� �Ľ� (2ĭ, 4ĭ, 2ĭ�� ����)
        sscanf(line, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price);

        // ù ��° ���ڰ� 0�� �׸� ī��Ʈ (Ȱ�� ������ �׸�)
        if (firstNum == 0) {
            currentZeroIndex++;  // ù ��° ���ڰ� 0�� �׸� ���� �ε��� ����

            // ����ڰ� ������ ������ ���� �׸��� ��ġ�ϸ� �ش� �׸� ���� (���� 1�� ����)
            if (currentZeroIndex == userChoice) {
                firstNum = 1;  // ù ��° ���ڸ� 1�� �����Ͽ� ��Ȱ��ȭ
                found = 1;
            }

        }

        // ����� ������ �ӽ� ���Ͽ� ��� (���� ������ �����Ͽ� ���)
        fprintf(tempFile, "%d  %d    %s  %d\n", firstNum, secondNum, foodName, price);
    }
    
    // ��� �׸��� Ȯ���� �Ŀ� �׸��� ã�� ������ ��� ó��
    if (!found) {
        printf("�ش� ��ȣ�� �׸��� �����ϴ�.\n");
    }

    fclose(tempFile);
    fclose(foodFile);

    // ���� ������ �ӽ� ���Ϸ� ��ü
    remove(FILE_PATH);
    rename("temp.txt", FILE_PATH);

    if (found) {
        printf("�޴� ��ȣ %d�� ���ŵǾ����ϴ�.\n", userChoice);
    }
    else {
        printf("�ش� ��ȣ�� �޴��� ã�� �� �����ϴ�.\n");
    }
}

// ���� ���
static void exitProgram(FILE* foodFile) {
    fclose(foodFile);
    printf("���α׷��� �����մϴ�.\n");
    exit(0);
}
// ���̺� ��ȣ �Է¹޴� �Լ�
static int getTableNumber() {
    int tableNumber;
    while (1) {
        printf("���̺� ��ȣ�� �Է��ϼ��� (1~5): ");
        tableNumber = getInt();
        if (tableNumber < 1 || tableNumber >5) {
            printf("����: 1~5������ ��ȣ�� �Է��ϼ���.\n");
        }
        else {
            return tableNumber;
        }
    }
}
// �ֹ� �׸� ����Ʈ�� �׸� �߰�
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

// �ֹ� �׸� ����Ʈ ����
void freeOrderItems(OrderItem* head) {
    while (head != NULL) {
        OrderItem* temp = head;
        head = head->next;
        free(temp);
    }
}

// �ֹ� ��ȸ ���
static void printOrder() {
    int tableNumber = getTableNumber();

    // ���̺� ���� ��� 
    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);

    // ���̺� ���� ����
    FILE* tableFile = fopen(tableFilePath, "r");
    if (tableFile == NULL) {
        printf("���̺� ������ �� �� �����ϴ�.\n");
        return;
    }
    // �Ǹ��׸��� ���� ����
    FILE* foodFile = fopen(FILE_PATH, "r");
    if (foodFile == NULL) {
        printf("�Ǹ� �׸� ������ �� �� �����ϴ�.\n");
        fclose(tableFile);
        return;
    }
    // ���̺� ���Ͽ��� ������ȣ�� �о� ���� ���
    int itemID;
    OrderItem* orderList = NULL;

    while (fscanf(tableFile, "%d", &itemID) == 1) {
        orderList = addOrderItem(orderList, itemID);
    }

    // �ֹ� ���� ���
    printf("\n     %d�� ���̺� �ֹ� ��ȸ     \n", tableNumber);
    printf("%-10s %-10s %-10s\n", "�޴�", "����", "�ݾ�");

    int foundItems = 0;
    int firstNum, secondNum, price;
    char foodName[50];

    // FILE_PATH ���Ͽ��� ������ȣ�� �ش��ϴ� �׸� ���� ã��
    OrderItem* current = orderList;
    while (current != NULL) {
        rewind(foodFile);  // ���� �����͸� ó������ �ǵ���
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0 && secondNum == current->itemID) {
                // ��ġ�ϴ� �׸� ���
                int quantity = current->quantity;
                printf("%-10s %-10d %-10d\n", foodName, quantity, quantity * price);
                foundItems++;
                break;
            }
        }
        current = current->next;
    }

    if (foundItems == 0) {
        printf("�ֹ��� �Ǹ� �׸��� �����ϴ�.\n");
    }

    // �޸� ����
    freeOrderItems(orderList);
    fclose(tableFile);
    fclose(foodFile);
}

int main(void) {
    FILE* foodFile;

    while (1) {
        foodFile = fopen(FILE_PATH, "r+");  // ������ �б� �� ���� ���� ����
        if (foodFile == NULL) {
            printf("������ �� �� �����ϴ�.\n");
            return 1;
        }

        int s = printMain();
        switch (s) {
        case 1:
            printFoodList(foodFile);
            break;
        case 2:
            addToFoodList(foodFile);
            break;
        case 3:
            removeFoodItem(foodFile);
            break;
        case 5:
            printOrder();
            break;
        case 7:
            exitProgram(foodFile);
            break;
        default:
            break;
        }

        fclose(foodFile);  // ��� case���� ������ ��� �� ����
    }

    return 0;
}