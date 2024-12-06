#include "pos.h"

// 7.3.1 판매 항목명 입력
char* inputFoodName() {
    char* s = (char*)malloc(sizeof(char) * (MAX_INPUT_NUM + 2));
    if (s == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return NULL;
    }

    while (1) {
        printf("판매 항목명: ");
        if (fgets(s, MAX_INPUT_NUM + 2, stdin) == NULL) {
            printf("입력 오류 발생\n");
        }
        else {
            s[strcspn(s, "\n")] = '\0'; // \n을 제거

            // 앞 뒤 공백 제거
            char* start = s;
            char* end = s + strlen(s) - 1;
            while (isspace((unsigned char)*start)) start++;
            while (end > start && isspace((unsigned char)*end)) end--;
            memmove(s, start, end - start + 1);
            s[end - start + 1] = '\0';

            // 길이 검사 (공백 제거 후)
            int len = strlen(s);
            if (len < 1 || len > 20) {
                printf("경고: 입력은 1이상 20이하이어야 합니다. 다시 입력해주세요.\n");
            }
            else {
                // 알파벳 검사
                int valid = 1;
                for (int i = 0; s[i] != '\0'; i++) {
                    if (!isalpha((unsigned char)s[i])) {
                        valid = 0;
                        break;
                    }
                }
                if (!valid) {
                    printf("경고: 항목명은 알파벳만 입력해야 합니다. 다시 입력해주세요.\n");
                }
                else {
                    return s;
                }
            }
        }
    }
}

// 7.3.2 판매 항목가 입력
int inputPrice() {
    int price;
    while (1) {
        price = inputInt("판매 항목가: ", false,false);
        if (price >= 1 && price <= 9999999) {
            return price;
        }
        else {
            printf("오류: 1~9999999사이의 수를 입력하세요.\n");
        }
    }
}