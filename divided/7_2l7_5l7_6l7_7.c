#include "pos.h"

// 7.2 �Ǹ� �׸� ���� �Է� *����* �� �Լ��� �ǹ� ��Ģ�� �˻����� �ʽ��ϴ�.
int inputFoodNumber() {
    int foodNumber;
    while (1) {
        foodNumber = inputInt("�Ǹ� �׸� ��ȣ�� �Է��ϼ���: ", true,false);
        return foodNumber;
    }
}
// 7.5 �Ǹ� �׸� ��ȸ ������Ʈ
void printFoodList() {

    ///// �Ǹ� �׸� ������ ���� ���� /////
    FILE* foodFile = fopen(FILE_PATH, "r"); // �б�
    if (foodFile == NULL) {
        printf("������ �� �� �����ϴ�.\n");
    }
    checkDataIntegrity();
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
    checkDataIntegrity();
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
    checkDataIntegrity();
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
