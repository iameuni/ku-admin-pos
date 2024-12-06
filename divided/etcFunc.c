#include "pos.h"

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

    rewind(file);
    while (fscanf(file, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
        lastSecondNum = secondNum;
    }
    return lastSecondNum;
}
// �ؽ�Ʈ ���� Ư�� ������ ���� �����ϴ� �Լ� (7.10 ���� ó�� ������Ʈ���� ���)
int deleteLines(const char* filePath, int startLine, int endLine) {
    FILE* fp_read, * fp_write;
    char line[1024];
    int currentLine = 1;

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

    while (fgets(line, sizeof(line), fp_read) != NULL) {
        if (currentLine < startLine || (endLine != -1 && currentLine > endLine)) {
            fputs(line, fp_write);
        }
        currentLine++;
    }

    fclose(fp_read);
    fclose(fp_write);

    remove(filePath);
    rename("temp.txt", filePath);
    return 0;
}

// ���� �Է� �Լ�
int inputInt(const char* prompt, bool allowZero, bool allowMinus) {
    char n[MAX_INPUT_NUM + 2];
    char* endptr;
    long num;

    if (prompt != NULL) printf("%s", prompt);
    if (fgets(n, sizeof(n), stdin) == NULL) {
        printf("�Է� ������ �߻��߽��ϴ�. �ٽ� �õ����ּ���.\n");
    }
    else {
        n[strcspn(n, "\n")] = '\0';
        if (strlen(n) > MAX_INPUT_NUM) {
            printf("���: %d�� ���Ϸ� ���ڸ� �Է����ּ���.\n", MAX_INPUT_NUM);
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

            if (*start == '.') {
                return -MAX_INT + 9;
            }

            // �� �Է� ó��
            if (*start == '\0'&&!allowMinus) {
                return -1; // ���Ͱ� ������ ��� -1 ��ȯ
            } 
            if (*start == '\0' && allowMinus) return -MAX_INT + 9;

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
    return -MAX_INT;
}