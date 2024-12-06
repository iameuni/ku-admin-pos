#include "pos.h"

// 프로그램 종료 기능
void exitProgram() {
    printf("프로그램을 종료합니다.\n");
    system("PAUSE");
    exit(0);
}

// 마지막 판매 항목 고유 번호를 가져오는 함수 (7.6 판매 항목 추가 프롬프트에서 사용)
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
// 텍스트 파일 특정 범위의 줄을 삭제하는 함수 (7.10 결제 처리 프롬프트에서 사용)
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

// 정수 입력 함수
int inputInt(const char* prompt, bool allowZero, bool allowMinus) {
    char n[MAX_INPUT_NUM + 2];
    char* endptr;
    long num;

    if (prompt != NULL) printf("%s", prompt);
    if (fgets(n, sizeof(n), stdin) == NULL) {
        printf("입력 오류가 발생했습니다. 다시 시도해주세요.\n");
    }
    else {
        n[strcspn(n, "\n")] = '\0';
        if (strlen(n) > MAX_INPUT_NUM) {
            printf("경고: %d자 이하로 숫자를 입력해주세요.\n", MAX_INPUT_NUM);
            return -MAX_INT;
        }
        else {
            char* start = n;
            char* end = n + strlen(n) - 1;

            n[strcspn(n, "\n")] = '\0'; // 개행 문자 제거

            // 스페이스바는 허용, 탭은 오류 처리
            while (*start == ' ') start++;  // 스페이스바는 허용
            if (strchr(n, '\t') != NULL || strchr(n, '\v') != NULL || strchr(n, '\f') != NULL || strchr(n, '\r') != NULL) { // 입력에 탭이 포함되면 오류 반환
                printf("오류: 입력에 허용되지 않는 공백 문자가 포함되어 있습니다.\n");
                return -MAX_INT+1; // 탭 문자 오류
            }

            if (*start == '.') {
                return -MAX_INT + 9;
            }

            // 빈 입력 처리
            if (*start == '\0'&&!allowMinus) {
                return -1; // 엔터가 눌렸을 경우 -1 반환
            } 
            if (*start == '\0' && allowMinus) return -MAX_INT + 9;

            if (start[0] == '0' && strlen(start) > 1) {
                printf("오류: 0으로 시작하는 수는 입력할 수 없습니다.\n");
                return -MAX_INT+2;
            }

            if (start[0] == '-' && start[1] == '0') {
                printf("오류: 0으로 시작하는 수는 입력할 수 없습니다.\n");
                return -MAX_INT+3;
            }

            else if (*start == '\0'&&!allowMinus) {
                printf("오류: 입력값이 비어있습니다.\n");
                return -MAX_INT+4;
            }
            else {
                while (end > start && *end == ' ') end--; // 스페이스바는 허용
                *(end + 1) = '\0';

                num = strtol(start, &endptr, 10);

                if (*endptr != '\0') {
                    printf("오류: 음이 아닌 정수를 입력해주세요.\n");
                    return -MAX_INT+6;
                }
                else {
                    if ((!allowMinus && num < 0) || num > INT_MAX) {
                        printf("오류: 음이 아닌 정수를 입력해주세요.\n");
                        return -MAX_INT + 7;
                    }
                    else {
                        if (num == 0 && !allowZero) {
                            printf("오류: 0은 입력할 수 없습니다.\n");
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