#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_INPUT 100
#define MAX_TABLE_NUMBER 10
#define MAX_TABLE_ADJUST 9 // ���̺� ���� �ִ� ����
#define FILE_PATH "foodlist.txt" // ���� ��� ����
#define TABLE_FILE_PATH "table" //���̺� ���� ��� ����
#define MAX_INT 2,147,483,647

//////////////////// ����ü ���� ////////////////////
typedef struct OrderItem {
    int itemID;
    int quantity;
    struct OrderItem* next;
} OrderItem;



//////////////////// ��Ÿ �Լ� ////////////////////

// ���α׷� ���� ���
void exitProgram() {
    printf("���α׷��� �����մϴ�.\n");
    system("PAUSE");
    exit(0);
}

// ������ �Ǹ� �׸� ���� ��ȣ�� �������� �Լ� (7.6 �Ǹ� �׸� �߰� ������Ʈ���� ���)
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

// �ؽ�Ʈ ���� Ư�� ������ ���� �����ϴ� �Լ� (7.10 ���� ó�� ������Ʈ���� ���)
int deleteLines(const char* filePath, int startLine, int endLine) {
    FILE* fp_read, * fp_write;
    char line[1024];
    int currentLine = 1;

    // ���� ����
    fp_read = fopen(filePath, "r");
    if (fp_read == NULL) {
        perror("fopen");
        return -1;
    }
    fp_write = fopen("temp.txt", "w");
    if (fp_write == NULL) {
        perror("fopen");
        fclose(fp_read);
        return -1;
    }

    // ���� �а� ����
    while (fgets(line, sizeof(line), fp_read) != NULL) {
        if (currentLine < startLine || (endLine != -1 && currentLine > endLine)) {
            fputs(line, fp_write);
        }
        currentLine++;
    }

    // ���� �ݱ�
    fclose(fp_read);
    fclose(fp_write);

    remove(filePath);
    rename("temp.txt", filePath);
    return 0;
}

// ���� �Է� �Լ�
int inputInt(const char* prompt, bool allowZero, bool allowMinus) {
    char n[MAX_INPUT + 2];
    char* endptr;
    long num;

    if (prompt != NULL) printf("%s", prompt);
    if (fgets(n, sizeof(n), stdin) == NULL) {
        printf("�Է� ������ �߻��߽��ϴ�. �ٽ� �õ����ּ���.\n");
    }
    else {
        n[strcspn(n, "\n")] = '\0';
        if (strlen(n) > MAX_INPUT) {
            printf("���: %d�� ���Ϸ� ���ڸ� �Է����ּ���.\n", MAX_INPUT);
            return -MAX_INT;
        }
        else {
            char* start = n;
            char* end = n + strlen(n) - 1;

            n[strcspn(n, "\n")] = '\0'; // ���� ���� ����

            // �����̽��ٴ� ���, ���� ���� ó��
            while (*start == ' ') start++;  // �����̽��ٴ� ���
            if (strchr(n, '\t') != NULL || strchr(n, '\v') != NULL || strchr(n, '\f') != NULL || strchr(n, '\r') != NULL) { // �Է¿� ���� ���ԵǸ� ���� ��ȯ
                printf("����: �Է¿� ������ �ʴ� ���� ���ڰ� ���ԵǾ� �ֽ��ϴ�.\n");
                return -MAX_INT+1; // �� ���� ����
            }

            // �� �Է� ó��
            if (*start == '\0'&&!allowMinus) {
                return -1; // ���Ͱ� ������ ��� -1 ��ȯ
            } 
            if (*start == '\0' && allowMinus) return -30;

            if (start[0] == '0' && strlen(start) > 1) {
                printf("����: 0���� �����ϴ� ���� �Է��� �� �����ϴ�.\n");
                return -MAX_INT+2;
            }

            if (start[0] == '-' && start[1] == '0') {
                printf("����: 0���� �����ϴ� ���� �Է��� �� �����ϴ�.\n");
                return -MAX_INT+3;
            }

            else if (*start == '\0'&&!allowMinus) {
                printf("����: �Է°��� ����ֽ��ϴ�.\n");
                return -MAX_INT+4;
            }
            else {
                while (end > start && *end == ' ') end--; // �����̽��ٴ� ���
                *(end + 1) = '\0';

                num = strtol(start, &endptr, 10);

                if (*endptr != '\0') {
                    printf("����: ���� �ƴ� ������ �Է����ּ���.\n");
                    return -MAX_INT+6;
                }
                else {
                    if ((!allowMinus && num < 0) || num > INT_MAX) {
                        printf("����: ���� �ƴ� ������ �Է����ּ���.\n");
                        return -MAX_INT + 7;
                    }
                    else {
                        if (num == 0 && !allowZero) {
                            printf("����: 0�� �Է��� �� �����ϴ�.\n");
                            return -MAX_INT + 8;
                        }
                        return (int)num;
                    }
                }
            }
        }
    }
}

// ���̺� ���� ���� Ȯ�� �Լ�
bool isTableExist(int tableNumber) {
    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);
    FILE* tableFile = fopen(tableFilePath, "r");
    if (tableFile != NULL) {
        fclose(tableFile);
        return true;
    }
    return false;
}

// ������ (���� ū) ���̺� ��ȣ ��ȯ�ϴ� �Լ�
int getLastTableNumber() {
    int maxNumber = 0;
    for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
        if (isTableExist(i) && i > maxNumber) {
            maxNumber = i;
        }
    }
    return maxNumber;
}

// Ư�� ���̺��� �ֹ� ���� ���� ���� Ȯ�� �Լ�
bool hasOrders(int tableNumber) {
    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);

    FILE* tableFile = fopen(tableFilePath, "r");
    if (tableFile == NULL) {
        return false;
    }

    // ������ ù ��° non-whitespace ���ڸ� ã��
    int c;
    while ((c = fgetc(tableFile)) != EOF) {
        if (!isspace(c)) {
            fclose(tableFile);
            return true;
        }
    }

    fclose(tableFile);
    return false;
}

// �ֹ� ������ �ִ� ���̺� �Ǵ� ���� ������ ���̺� ��� �Լ�
void listTablesWithOrders(int* tablesWithOrders, int* orderCount, const char* message) {
    *orderCount = 0;

    printf("%s: { ", message); // �޽��� ���

    bool first = true;
    int maxTableNum = getLastTableNumber();
    for (int table = 1; table <= maxTableNum; table++) {
        if (hasOrders(table)) { // Check if there are orders using hasOrders
            if (!first) {
                printf(", ");
            }
            printf("%d", table);
            first = false;

            tablesWithOrders[*orderCount] = table; // Store table number
            (*orderCount)++; // Increment order count
        }
    }
    printf(" }\n\n");
}

// ���� �����ϴ� ���̺� ���� ���
int getCurrentTableCount() {
    int count = 0;
    for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
        if (isTableExist(i)) {
            count++;
        }
    }
    return count;
}



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

    // ��� ������ ���̺� ���� �˻�
    for (int table = 1; table <= MAX_TABLE_NUMBER; table++) {
        char tableFileName[256];
        snprintf(tableFileName, sizeof(tableFileName), "%s/%d.txt", TABLE_FILE_PATH, table);

        FILE* tableFile = fopen(tableFileName, "r");
        if (tableFile) {
            tableCount++;
            existingTables[table] = 1;

            // ���̺� ������ ���� �˻�
            int tableLineNumber = 0;
            while (fgets(line, sizeof(line), tableFile)) {
                tableLineNumber++;
                int saleItemId;
                if (sscanf(line, "%d", &saleItemId) == 1) {
                    bool found = false;
                    for (int i = 0; i < itemCount; i++) {
                        if (itemIds[i] == saleItemId) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        printf("���̺� ������ ������ %d��° ���� �Ǹ� �׸� ���� ��ȣ %d�� �ùٸ� �Ǹ� �׸� ���� ��ȣ�� �ƴմϴ�. ���α׷��� �����մϴ�.\n",
                            tableLineNumber, saleItemId);
                        fclose(tableFile);
                        return false;
                    }
                }
                else {
                    printf("���̺� ������ ������ %d��° �ٿ��� �ùٸ� ������ �Ǹ� �׸� ���� ��ȣ�� ã�� �� �����ϴ�. ���α׷��� �����մϴ�.\n",
                        tableLineNumber);
                    fclose(tableFile);
                    return false;
                }
            }
            fclose(tableFile);
        }
    }

    // ���̺� ���� ���� �˻�
    if (tableCount < 1) {
        printf("���̺��� �ϳ��� �������� �ʽ��ϴ�. ���α׷��� �����մϴ�.\n");
        return false;
    }
    if (tableCount > MAX_TABLE_NUMBER) {
        printf("���̺��� %d���� �ʰ��մϴ�. ���α׷��� �����մϴ�.\n", MAX_TABLE_NUMBER);
        return false;
    }

    return true;
}

// 7.2 �Ǹ� �׸� ���� �Է� *����* �� �Լ��� �ǹ� ��Ģ�� �˻����� �ʽ��ϴ�.
int inputFoodNumber() {
    int foodNumber;
    while (1) {
        foodNumber = inputInt("�Ǹ� �׸� ��ȣ�� �Է��ϼ���: ", true,false);
        return foodNumber;
    }
}

// 7.3.1 �Ǹ� �׸�� �Է�
char* inputFoodName() {
    char* s = (char*)malloc(sizeof(char) * (MAX_INPUT + 2));
    if (s == NULL) {
        fprintf(stderr, "�޸� �Ҵ� ����\n");
        return NULL;
    }

    while (1) {
        printf("�Ǹ� �׸��: ");
        if (fgets(s, MAX_INPUT + 2, stdin) == NULL) {
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
    while (1)
    {
        printf("������ �ݾ��� �Է��ϼ��� [%d��]: ", remainingBalance);
        int paymentAmount = inputInt(NULL, true, false); // ���� �Է� �Լ� ���

        // �Է��� ����ִٸ� ��ü �ݾ� ����
        if (paymentAmount == -1) { // ����Ű�� �Էµ� ���
            printf("��ü �ݾ��� ���� �Ϸ�Ǿ����ϴ�.\n");
            remainingBalance = 0; // ���� ���� �Ϸ�
            return remainingBalance;
        }

        // ��ȿ�� �˻�
        if (paymentAmount < -1) {
            continue;
        }
        else if (paymentAmount == 0) { // 0 �Է� �� ���� �ߴ�
            printf("���� �ߴ�.\n");
            return -1;
        }
        else if (paymentAmount > remainingBalance) {
            printf("����: ������ �ݾ׺��� Ů�ϴ�.\n");
            continue;
        }

        remainingBalance -= paymentAmount;
        if (remainingBalance == 0) {
            printf("��ü �ݾ��� ���� �Ϸ�Ǿ����ϴ�.\n");
            remainingBalance = 0; // ���� ���� �Ϸ�
            return remainingBalance;
        }
        else {
            printf("%d�� ���� ���� �Ϸ�Ǿ����ϴ�. ���� �ݾ�: %d��\n", paymentAmount, remainingBalance);
            return remainingBalance;
        }
    }
}

// 7.5 �Ǹ� �׸� ��ȸ ������Ʈ
void printFoodList() {

    ///// �Ǹ� �׸� ������ ���� ���� /////
    FILE* foodFile = fopen(FILE_PATH, "r"); // �б�
    if (foodFile == NULL) {
        printf("������ �� �� �����ϴ�.\n");
    }
    checkDataIntegrity(foodFile);
    rewind(foodFile);  // ���� �����͸� ó������ �ǵ���
    /////////////////////////////////////

    char line[100]; // �� ���� ������ ���ڿ�
    int line_count = 0;
    int firstNum, secondNum, price;
    char food[50];  // ���� �̸� ����

    printf("\n===== �Ǹ� �׸� ��� =====\n");

    while (fgets(line, sizeof(line), foodFile)) {
        sscanf(line, "%d  %d    %s  %d", &firstNum, &secondNum, food, &price);  // ���� 2ĭ, 4ĭ, 2ĭ ����

        if (firstNum == 0) {
            printf("%d. %s - %d\n", ++line_count, food, price);
        }
    }

    if (line_count == 0) {
        printf("�Ǹ� ���� �׸��� �����ϴ�.\n");
    }

    fclose(foodFile);
}

// 7.6 �Ǹ� �׸� �߰� ������Ʈ
void addToFoodList() {

    ///// �Ǹ� �׸� ������ ���� ���� /////
    FILE* foodFile = fopen(FILE_PATH, "r+"); // �б� �� ����
    if (foodFile == NULL) {
        printf("������ �� �� �����ϴ�.\n");
    }
    checkDataIntegrity(foodFile);
    rewind(foodFile);  // ���� �����͸� ó������ �ǵ���
    /////////////////////////////////////

    int firstNum = 0;
    int secondNum = getLastSecondNumber(foodFile) + 1;

    printf("\n�߰��� �Ǹ� �׸� ������ �Է��ϼ���(�׸���� ���ĺ��� ���)\n");
    char* foodName = inputFoodName();

    if (foodName != NULL) {
        int price = inputPrice();

        fseek(foodFile, 0, SEEK_END);  // ������ ������ �̵�
        if (fseek(foodFile, -1, SEEK_END) == 0 && fgetc(foodFile) != '\n') {
            fprintf(foodFile, "\n");
        }
        fprintf(foodFile, "%d  %d    %s  %d\n", firstNum, secondNum, foodName, price);  // ���� 2ĭ, 4ĭ, 2ĭ ���� ����
        printf("%s ��/�� �߰��Ǿ����ϴ�.\n", foodName);

        free(foodName);
    }
    else {
        printf("�޸� �Ҵ� ����\n");
    }

    fclose(foodFile);
}

// 7.7 �Ǹ� �׸� ���� ������Ʈ
void removeFoodItem() {

    ///// �Ǹ� �׸� ������ ���� ���� /////
    FILE* foodFile = fopen(FILE_PATH, "r+"); // �б� �� ����
    if (foodFile == NULL) {
        printf("������ �� �� �����ϴ�.\n");
    }
    checkDataIntegrity(foodFile);
    rewind(foodFile);  // ���� �����͸� ó������ �ǵ���
    /////////////////////////////////////

    printFoodList(); // ���� �Ǹ� �׸��� ���

    while (1) {
        rewind(foodFile);  // ���� �����͸� ó������ �ǵ���
        FILE* tempFile = fopen("temp.txt", "w");
        if (tempFile == NULL) {
            printf("�ӽ� ������ �� �� �����ϴ�.\n");
            return;
        }
        int userChoice = inputFoodNumber();  // ����ڰ� ������ �׸� ��ȣ (��� ����)

        char line[256];
        int firstNum, secondNum, price;
        char foodName[50];
        int currentZeroIndex = 0;  // 0�� �׸� ī��Ʈ
        int found = 0;

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
            fclose(tempFile);
            continue;
        }
        else {
            fclose(tempFile);
            fclose(foodFile); // �ӽ� ���Ϸ� ��ü �� �ݾƾ� ��.

            // ���� ������ �ӽ� ���Ϸ� ��ü
            remove(FILE_PATH);
            rename("temp.txt", FILE_PATH);
            printf("�޴� ��ȣ %d�� ���ŵǾ����ϴ�.\n", userChoice);
            break;
        }
    }
}

// 7.8 �ֹ� ���� ������Ʈ
void createOrder() {

    // �ֹ� ������ ���̺� ��� ǥ��
    int tablesWithOrders[MAX_TABLE_NUMBER];  // �ֹ� ������ �ִ� ���̺��� ������ �迭
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n�ֹ� ������ �ִ� ���̺� ��ȣ");  // �ֹ��� �ִ� ���̺��� Ȯ���ϰ� ǥ��

    FILE* foodFile = fopen(FILE_PATH, "r+"); // �б� �� ����
    if (foodFile == NULL) {
        printf("������ �� �� �����ϴ�.\n");
        return;
    }
    rewind(foodFile);  // ���� �����͸� ó������ �ǵ���

    int tableNumber = 0;

    while (1) {
        tableNumber = inputTableNumber(false);//���̺� ��ȣ �Է¹ޱ�
        if (tableNumber < -1) continue;
        else break;
    }

    // ���̺� ���� ��� ����
    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);

    // ���̺� ���� ����
    FILE* tableFile = fopen(tableFilePath, "a");
    if (tableFile == NULL) {
        printf("���̺� ������ �� �� �����ϴ�.\n");
        fclose(foodFile);
        return;
    }

    printFoodList();  // �Ǹ� ��� ���

    int selection = -1;  // �Ǹ� �׸� ���� ����
    OrderItem* orderList = NULL;

    int firstNum, secondNum, price;
    char foodName[50];

    while (selection != 0) {  // 0�� �Է��ϸ� �ֹ��� ����
        printf("\n<�ֹ��� �������� 0�� �Է��ϼ���>\n\n");
        selection = inputFoodNumber();

        if (selection == 0) {
            break;  // 0 �Է� �� �ֹ� ����
        }

        int currentMenuIndex = 0;
        int validSelection = 0;

        // ������ �ٽ� �о� ������ �޴��� ID ã��
        rewind(foodFile);
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0) {
                currentMenuIndex++;
                if (currentMenuIndex == selection) {
                    validSelection = 1;
                    int quantity = inputQuantity(); // ���� �Է¹ޱ�

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

    // �ֹ� ����Ʈ�� ���̺� ���Ͽ� ����
    OrderItem* current = orderList;
    while (current != NULL) {
        for (int i = 0; i < current->quantity; i++) {
            fprintf(tableFile, "%d\n", current->itemID);  // �޴� ID ����
        }
        current = current->next;
    }

    // ���� �ֹ� ��� ��� (OrderItem ���)
    printf("\n%d�� ���̺� ", tableNumber);

    current = orderList;

    int itemCount = 0;
    while (current != NULL) {
        // �޴� ���� ã��
        rewind(foodFile);  // ���� �����͸� ó������ �ǵ���
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0 && secondNum == current->itemID) {
                if (itemCount > 0) {
                    printf(" ");  // �� �׸� ���̿� ���� �߰�
                }
                printf("%s %d��", foodName, current->quantity);
                itemCount++;
                break;
            }
        }
        current = current->next;
    }

    printf(" �ֹ��Ϸ�Ǿ����ϴ�.\n");

    // �޸� ����
    freeOrderItems(orderList);
    freeOrderItems(current);

    fclose(tableFile);
    fclose(foodFile);
}

// 7.9 �ֹ� ��ȸ ������Ʈ
void printOrder() {
    // �ֹ� ������ ���̺� ��� ǥ��
    int tablesWithOrders[MAX_TABLE_NUMBER];  // �ֹ� ������ �ִ� ���̺��� ������ �迭
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n�ֹ� ������ �ִ� ���̺� ��ȣ");  // �ֹ��� �ִ� ���̺��� Ȯ���ϰ� ǥ��

    int tableNumber = 0;

    while (1) {
        tableNumber = inputTableNumber(false);
        if (tableNumber < -1) continue;
        else break;
    }

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
    printf("\n\t%d�� ���̺� �ֹ� ��ȸ\t\n\n", tableNumber);
    printf("%-20s %-10s %-10s\n", "�޴�", "����", "�ݾ�");

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
                printf("%-20s %-10d %-10d\n", foodName, quantity, quantity * price);
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

// 7.10 ���� ó�� ������Ʈ
void makePayment() {
    int tablesWithOrders[MAX_TABLE_NUMBER];  // �ֹ� ������ �ִ� ���̺� ��ȣ�� ������ �迭
    int orderCount = 0;       // �ֹ� ������ �ִ� ���̺� ���� ������ ����

    listTablesWithOrders(tablesWithOrders, &orderCount, "\n���� ������ ���̺� ��ȣ");  // �ֹ� ������ �ִ� ���̺��� ǥ���ϰ�, ����� ������

    int selectedTables[MAX_TABLE_NUMBER];
    int selectedCount = 0;

    // inputMultipleTablesForPayment ������ ���̺� ��ȣ�� �Է�
    while (true) {
        // ���� ���õ� ���̺� ��ȣ ���
        printf("���̺� ��ȣ�� �Է��ϼ��� {");
        for (int i = 0; i < selectedCount; i++) {
            printf("%d", selectedTables[i]);
            if (i < selectedCount - 1) {
                printf(", ");
            }
        }
        printf("}: ");

        int input = inputTableNumber(true); // ���� ó�������� ȣ��

        // -1�� ��ȯ�ϸ� ���� �Է�
        if (input == -1) {
            if (selectedCount > 0) break; // �̹� ���õ� ���̺��� �ִٸ� ����
            continue;
        }

        if (input < -1) {
            continue;
        }

        // 0 �Է� �� ���� ���
        if (input == 0) {
            printf("������ ����ϰ� ���� �޴��� ���ư��ϴ�.\n");
            selectedCount = 0; // ���õ� ���̺� ���� �ʱ�ȭ
            return;
        }

        // ��ȿ�� ���̺� ��ȣ���� Ȯ��
        bool validOrder = false;
        for (int i = 0; i < orderCount; i++) {
            if (tablesWithOrders[i] == input) {
                validOrder = true;
                break;
            }
        }
        if (!validOrder) {
            printf("���: %d�� ���̺��� �ֹ� ������ �����ϴ�.\n", input);
            continue;
        }
        else {
            // �̹� ���õ� ���̺� ��ȣ���� Ȯ��
            bool alreadySelected = false;
            for (int i = 0; i < selectedCount; i++) {
                if (selectedTables[i] == input) {
                    alreadySelected = true;
                    break;
                }
            }
            if (alreadySelected) {
                printf("���: �̹� ���õ� ���̺� ��ȣ�Դϴ�.\n");
                continue;
            }
            else {
                selectedTables[selectedCount] = input;
                selectedCount++;
            }
        }
    }

    int combinedTotal = 0;  // ���õ� ���̺���� ��ü ���� �ݾ�
    int counters[MAX_TABLE_NUMBER];        // �� ���õ� ���̺��� �� ���� ������ �迭

    printf("\n");

    // �� ���õ� ���̺��� �� �ֹ����� ���
    for (int i = 0; i < selectedCount; i++) {
        int tableNumber = selectedTables[i];
        char tableFilePath[256];
        snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);

        FILE* tableFile = fopen(tableFilePath, "r");
        if (!tableFile) {
            printf("������ �� �� �����ϴ�: %d�� ���̺�\n", tableNumber);
            continue;
        }

        // foodlist ������ ���� �� �׸��� ������ Ȯ��
        FILE* foodFile = fopen(FILE_PATH, "r");
        if (!foodFile) {
            printf("�Ǹ� �׸� ������ �� �� �����ϴ�.\n");
            fclose(tableFile);
            continue;
        }

        // �ֹ� �׸��� ���� ���
        int itemID;
        OrderItem* orderList = NULL;

        while (fscanf(tableFile, "%d", &itemID) == 1) {
            orderList = addOrderItem(orderList, itemID);
        }

        // �� �� ���
        counters[i] = 0; // �� ���̺��� �� ���� �ʱ�ȭ
        char c;
        FILE* tableFileForLineCount = fopen(tableFilePath, "r");
        while ((c = fgetc(tableFileForLineCount)) != EOF) {
            if (c == '\n') {
                counters[i]++; // �� ���̺��� �� ���� ����
            }
        }
        fclose(tableFileForLineCount);

        // �ֹ� �հ� ���
        int foundItems = 0;
        int firstNum, secondNum, price;
        char foodName[50];
        int totalPrice = 0;

        OrderItem* current = orderList;
        while (current != NULL) {
            rewind(foodFile);  // ���� �����͸� ó������ �ǵ���
            while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
                if (firstNum == 0 && secondNum == current->itemID) {
                    int quantity = current->quantity;
                    totalPrice += quantity * price;
                    foundItems++;
                    break;
                }
            }
            current = current->next;
        }

        // �޸� ����
        freeOrderItems(orderList);
        fclose(foodFile);
        fclose(tableFile);

        if (foundItems == 0) {
            printf("\n%d�� ���̺��� ������ �Ұ����մϴ�.\n", tableNumber);
        }
        else {
            combinedTotal += totalPrice;  // ���� �ݾ��� �ջ�
            printf("%d�� ���̺� �ֹ���: %d\n", tableNumber, totalPrice);
        }
    }

    printf("\n");

    // ���� ó��
    int remainingBalance = combinedTotal; // ���� �����ݾ�
    int paymentSuccess = 0; // ���� ���� ���� üũ ����
    int paidAmount = 0;

    // ���� ���� �ݾ��� ���� ��� �ݺ�
    while (remainingBalance > 0)
    {
        paidAmount = combinedTotal - remainingBalance;
        // ����ڷκ��� ���� �ݾ� �Է� �ޱ�
        remainingBalance = inputPaymentAmount(remainingBalance);

        // ���� �ߴ� ��ȣ�� �ԷµǸ� �ݺ� Ż��
        if (remainingBalance == -1) {
            break;
        }
        // ������ �Ϸ�Ǹ� ���� ǥ�� �� �ݺ� Ż��
        else if (remainingBalance == 0) {
            paymentSuccess = 1;
            break;
        }
        
    }

    // ��ü ������ �Ϸ�� �� �� ���̺��� �ֹ� ���� ����
    if (paymentSuccess) {
        for (int i = 0; i < selectedCount; i++) {
            char tableFilePath[256];
            snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, selectedTables[i]);

            deleteLines(tableFilePath, 0, -1);
        }
    }


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

// 7.12 ���� �޴� ������Ʈ
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
        printf("8. ����\n");
        s = inputInt("�޴� ����: ", false, false);
        if (s > 8) {
            printf("1~8 ������ ���� �Է����ּ���.\n");
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
            // 7.12 ���� �޴� ������Ʈ�� ���� ���
            exitProgram();
            break;
        default:
            break;
        }
    }

    return 0;
}