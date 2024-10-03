#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 100
#define FILE_PATH "..\\x64\\Debug\\foodlist.txt"  // 파일 경로 설정
#define TABLE_FILE_PATH "..\\x64\\Debug\\table" //테이블 폴더 경로 설정

typedef struct OrderItem { //주문할 수 있는 개수/판매 항목 목록 개수 제한이 없어서 메모리 할당
    int itemID;
    int quantity;
    struct OrderItem* next;
} OrderItem;

// 입력 규칙. \n을 제거하기 때문에 따로 입력해 줘야 함
static int getInt() {
    char n[MAX_INPUT + 2]; 
        char* endptr;
    long num;

    while (1) {
        if (fgets(n, sizeof(n), stdin) == NULL) {
            printf("입력 오류가 발생했습니다. 다시 시도해주세요.\n");
        }
        else {
            n[strcspn(n, "\n")] = '\0';
            if (strlen(n) > MAX_INPUT) {
                printf("경고: %d자 이하로 숫자를 입력해주세요.\n", MAX_INPUT);
            }
            else {
                char* start = n;
                char* end = n + strlen(n) - 1;
                while (isspace((unsigned char)*start)) start++;
                if (*start == '\0') {
                    printf("오류: 입력값이 비어있습니다.\n");
                }
                else {
                    while (end > start && isspace((unsigned char)*end)) end--;
                    *(end + 1) = '\0';

                    num = strtol(start, &endptr, 10);

                    if (*endptr != '\0') {
                        printf("오류: 음이 아닌 정수를 입력해주세요.\n");
                    }
                    else {
                        if (num < 0 || num > INT_MAX) {
                            printf("오류: %d 이하의 음이 아닌 정수를 입력해주세요.\n", INT_MAX);
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
        fprintf(stderr, "메모리 할당 실패\n");
        return NULL;
    }
    while (1) {
        if (fgets(s, MAX_INPUT + 2, stdin) == NULL) {
            printf("입력 오류 발생\n");
        }
        else {
            s[strcspn(s, "\n")] = '\0'; 
            // 알파벳이 아닌 문자가 있는지 확인
            int valid = 1;
            for (int i = 0; s[i] != '\0'; i++) {
                if (!isalpha((unsigned char)s[i])) {  // 알파벳이 아니면 오류
                    valid = 0;
                    break;
                }
            }
            if (!valid) {
                printf("경고: 항목명은 알파벳만 입력해야 합니다. 다시 입력해주세요.\n");
            }
            else if (strlen(s) <= MAX_INPUT) {
                return s;  // 입력이 유효하면 반환
            }
            else {
                printf("경고: 입력이 너무 깁니다.\n");
            }
        }
    }
}


// 메인 메뉴 출력 및 사용자 선택
static int printMain(void) {
    int s;
    while (1) {
        printf("\n메인 메뉴\n");
        printf("1. 판매 항목 조회\n");
        printf("2. 판매 항목 추가\n");
        printf("3. 판매 항목 제거\n");
        printf("4. 주문 생성\n");
        printf("5. 주문 조회\n");
        printf("6. 결제 처리\n");
        printf("7. 종료\n");
        printf("메뉴 선택: ");
        s = getInt();
        if (s > 7 || s < 1) {
            printf("1~7 사이의 값을 입력해주세요.\n");
        }
        else {
            return s;
        }
    }
}

// 판매 목록 출력
static void printFoodList(FILE* foodFile) {
    char line[100]; // 각 행을 저장할 문자열
    int line_count = 0;
    int firstNum, secondNum, price;
    char food[50];  // 음식 이름 저장

    printf("\n===== 판매 항목 목록 =====\n");

    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
    while (fgets(line, sizeof(line), foodFile)) {
        sscanf(line, "%d  %d    %s  %d", &firstNum, &secondNum, food, &price);  // 공백 2칸, 4칸, 2칸 유지

        if (firstNum == 0) {
            printf("%d. %s - %d\n", ++line_count, food, price);
        }
    }

    if (line_count == 0) {
        printf("판매 중인 항목이 없습니다.\n");
    }
}

// 고유 번호를 가져오는 함수
int getLastSecondNumber(FILE* file) {
    int firstNum, secondNum, price;
    char foodName[50];
    int lastSecondNum = 0;

    rewind(file);  // 파일 포인터를 처음으로 되돌림
    while (fscanf(file, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {  // 공백 2칸, 4칸, 2칸 유지
        lastSecondNum = secondNum;
    }
    return lastSecondNum;
}

// 판매 항목 추가
static void addToFoodList(FILE* foodFile) {
    int firstNum = 0;
    int secondNum = getLastSecondNumber(foodFile) + 1;

    printf("\n추가할 판매 항목 정보를 입력하세요(항목명은 알파벳만 허용)\n");
    printf("판매 항목명: ");
    char* foodName = getString();

    if (foodName != NULL) {
        printf("판매 항목가: ");
        int price = getInt();

        fseek(foodFile, 0, SEEK_END);  // 파일의 끝으로 이동
        fprintf(foodFile, "%d  %d    %s  %d\n", firstNum, secondNum, foodName, price);  // 공백 2칸, 4칸, 2칸 공백 적용
        printf("%s 이/가 추가되었습니다.\n", foodName);

        free(foodName);
    }
    else {
        printf("메모리 할당 실패\n");
    }
}

// 판매 항목 제거 함수
static void removeFoodItem(FILE* foodFile) {
    printFoodList(foodFile);  // 현재 항목을 출력
    printf("제거할 항목 번호를 입력하세요: ");
    int userChoice = getInt();  // 사용자가 선택한 항목 번호 (출력 순서)

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("임시 파일을 열 수 없습니다.\n");
        return;
    }

    char line[256];
    int firstNum, secondNum, price;
    char foodName[50];
    int currentZeroIndex = 0;  // 0인 항목만 카운트
    int found = 0;

    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림

    // 원본 파일에서 데이터를 읽고, 임시 파일로 복사하면서 상태를 변경
    while (fgets(line, sizeof(line), foodFile)) {
        // 정확한 공백에 맞추어 데이터를 파싱 (2칸, 4칸, 2칸의 공백)
        sscanf(line, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price);

        // 첫 번째 숫자가 0인 항목만 카운트 (활성 상태인 항목만)
        if (firstNum == 0) {
            currentZeroIndex++;  // 첫 번째 숫자가 0인 항목에 대해 인덱스 증가

            // 사용자가 선택한 순서와 현재 항목이 일치하면 해당 항목 제거 (상태 1로 변경)
            if (currentZeroIndex == userChoice) {
                firstNum = 1;  // 첫 번째 숫자를 1로 변경하여 비활성화
                found = 1;
            }

        }

        // 변경된 내용을 임시 파일에 기록 (공백 형식을 유지하여 기록)
        fprintf(tempFile, "%d  %d    %s  %d\n", firstNum, secondNum, foodName, price);
    }
    
    // 모든 항목을 확인한 후에 항목을 찾지 못했을 경우 처리
    if (!found) {
        printf("해당 번호의 항목이 없습니다.\n");
    }

    fclose(tempFile);
    fclose(foodFile);

    // 원본 파일을 임시 파일로 대체
    remove(FILE_PATH);
    rename("temp.txt", FILE_PATH);

    if (found) {
        printf("메뉴 번호 %d가 제거되었습니다.\n", userChoice);
    }
    else {
        printf("해당 번호의 메뉴를 찾을 수 없습니다.\n");
    }
}

// 종료 기능
static void exitProgram(FILE* foodFile) {
    fclose(foodFile);
    printf("프로그램을 종료합니다.\n");
    exit(0);
}
// 테이블 번호 입력받는 함수
static int getTableNumber() {
    int tableNumber;
    while (1) {
        printf("테이블 번호를 입력하세요 (1~5): ");
        tableNumber = getInt();
        if (tableNumber < 1 || tableNumber >5) {
            printf("오류: 1~5사이의 번호를 입력하세요.\n");
        }
        else {
            return tableNumber;
        }
    }
}
// 주문 항목 리스트에 항목 추가
OrderItem* addOrderItem(OrderItem* head, int itemID) {
    OrderItem* current = head;
    // 이미 존재하는 항목이면 수량 증가
    while (current != NULL) {
        if (current->itemID == itemID) {
            current->quantity++;
            return head;
        }
        current = current->next;
    }
    // 새 항목 추가
    OrderItem* newItem = (OrderItem*)malloc(sizeof(OrderItem));
    if (newItem == NULL) {
        printf("메모리 할당 실패\n");
        return head;
    }
    newItem->itemID = itemID;
    newItem->quantity = 1;
    newItem->next = head;
    return newItem;
}

// 주문 항목 리스트 해제
void freeOrderItems(OrderItem* head) {
    while (head != NULL) {
        OrderItem* temp = head;
        head = head->next;
        free(temp);
    }
}

// 주문 조회 기능
static void printOrder() {
    int tableNumber = getTableNumber();

    // 테이블 파일 경로 
    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);

    // 테이블 파일 열기
    FILE* tableFile = fopen(tableFilePath, "r");
    if (tableFile == NULL) {
        printf("테이블 파일을 열 수 없습니다.\n");
        return;
    }
    // 판매항목목록 파일 열기
    FILE* foodFile = fopen(FILE_PATH, "r");
    if (foodFile == NULL) {
        printf("판매 항목 파일을 열 수 없습니다.\n");
        fclose(tableFile);
        return;
    }
    // 테이블 파일에서 고유번호를 읽어 수량 계산
    int itemID;
    OrderItem* orderList = NULL;

    while (fscanf(tableFile, "%d", &itemID) == 1) {
        orderList = addOrderItem(orderList, itemID);
    }

    // 주문 정보 출력
    printf("\n     %d번 테이블 주문 조회     \n", tableNumber);
    printf("%-10s %-10s %-10s\n", "메뉴", "수량", "금액");

    int foundItems = 0;
    int firstNum, secondNum, price;
    char foodName[50];

    // FILE_PATH 파일에서 고유번호에 해당하는 항목 정보 찾기
    OrderItem* current = orderList;
    while (current != NULL) {
        rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0 && secondNum == current->itemID) {
                // 일치하는 항목 출력
                int quantity = current->quantity;
                printf("%-10s %-10d %-10d\n", foodName, quantity, quantity * price);
                foundItems++;
                break;
            }
        }
        current = current->next;
    }

    if (foundItems == 0) {
        printf("주문한 판매 항목이 없습니다.\n");
    }

    // 메모리 해제
    freeOrderItems(orderList);
    fclose(tableFile);
    fclose(foodFile);
}

int main(void) {
    FILE* foodFile;

    while (1) {
        foodFile = fopen(FILE_PATH, "r+");  // 파일을 읽기 및 쓰기 모드로 열기
        if (foodFile == NULL) {
            printf("파일을 열 수 없습니다.\n");
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

        fclose(foodFile);  // 모든 case에서 파일을 사용 후 닫음
    }

    return 0;
}