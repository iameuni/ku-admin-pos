#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_INPUT 100
#define FILE_PATH "foodlist.txt" // 파일 경로 설정
#define TABLE_FILE_PATH "table" //테이블 폴더 경로 설정

//////////////////// 구조체 선언 ////////////////////
typedef struct OrderItem {
    int itemID;
    int quantity;
    struct OrderItem* next;
} OrderItem;



//////////////////// 기타 함수 ////////////////////

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

    rewind(file);  // 파일 포인터를 처음으로 되돌림
    while (fscanf(file, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {  // 공백 2칸, 4칸, 2칸 유지
        lastSecondNum = secondNum;
    }
    return lastSecondNum;
}

// 주문 항목 리스트에 항목 추가 (7.9 주문 조회 프롬프트. 7.10 결제 처리 프롬프트에서 사용)
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

// 주문 항목 리스트 해제하는 함수 (7.9 주문 조회 프롬프트. 7.10 결제 처리 프롬프트에서 사용)
void freeOrderItems(OrderItem* head) {
    while (head != NULL) {
        OrderItem* temp = head;
        head = head->next;
        free(temp);
    }
}

// 텍스트 파일 특정 범위의 줄을 삭제하는 함수 (7.10 결제 처리 프롬프트에서 사용)
int deleteLines(const char* filePath, int startLine, int endLine) {
    FILE* fp_read, * fp_write;
    char line[1024];
    int currentLine = 1;

    // 파일 열기
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

    // 파일 읽고 쓰기
    while (fgets(line, sizeof(line), fp_read) != NULL) {
        if (currentLine < startLine || currentLine > endLine) {
            fputs(line, fp_write);
        }
        currentLine++;
    }

    // 파일 닫기
    fclose(fp_read);
    fclose(fp_write);

    remove(filePath);
    rename("temp.txt", filePath);

    printf("%d번째 줄부터 %d번째 줄까지 삭제되었습니다.\n", startLine, endLine);
    return 0;
}

// 정수 입력 함수
int inputInt(const char* prompt, bool allowZero) {
    char n[MAX_INPUT + 2];
    char* endptr;
    long num;

    while (1) {
        if (prompt != NULL) printf("%s", prompt);
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
                if (*start == '0' && !allowZero) {
                    printf("오류: 0으로 시작하는 수는 입력할 수 없습니다.\n");
                }
                else if (*start == '\0') {
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

// 마지막 테이블 번호 반환하는 함수
int getLastTableNumber() {
    char filename[50];
    int i = 1;

    while (1) {
        // 파일 이름 생성 (예: table/1.txt)
        sprintf(filename, "%s/%d.txt", TABLE_FILE_PATH, i);

        // 파일 오픈 시도
        FILE* fp = fopen(filename, "r");

        // 파일이 존재하지 않으면 반복 종료
        if (fp == NULL) {
            break;
        }
        else {
            fclose(fp); // 파일 닫기
        }

        i++;
    }

    // 마지막으로 존재했던 파일 번호 출력 (i는 1부터 시작하므로 1을 빼야 함)
    //printf("1.txt부터 %d.txt까지 연속적으로 존재합니다.\n", i - 1);

    return (i - 1);
}

// 테이블 추가 함수
void createNewTable() {
    int n = getLastTableNumber() + 1;
    char filename[20]; // 파일 이름을 저장할 문자 배열

    // 정수를 문자열로 변환하여 파일 이름 생성
    sprintf(filename, "%s/%d.txt", TABLE_FILE_PATH, n);

    // 파일 열기 (쓰기 모드)
    FILE* fp = fopen(filename, "w");

    // 파일 열기 실패 시 에러 메시지 출력
    if (fp == NULL) {
        printf("파일 생성 실패: %s\n", filename);
        return;
    }

    // 파일 닫기
    fclose(fp);
}

// 테이블 삭제 함수 (삭제하려는 테이블 데이터 파일이 비었는지 확인 후 삭제)
int deleteTable(int n) {
    char filename[20];
    sprintf(filename, "%s/%d.txt", TABLE_FILE_PATH, n);

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        // 파일 열기 실패
        perror("fopen");
        return -1;
    }

    int c;
    int hasNonSpaceChar = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (!isspace(c)) {
            hasNonSpaceChar = 1;
            break;
        }
    }

    fclose(fp);

    if (hasNonSpaceChar) {
        // 공백이 아닌 문자가 존재
        return -1;
    }
    else {
        // 공백만 있거나 아무 내용이 없음
        if (remove(filename) != 0) {
            // 파일 삭제 실패
            perror("remove");
            return -1;
        }
        return 0;
    }
}



//////////////////// 기획서 기반 프롬프트 ////////////////////

// 7.1 데이터 파일 무결성 검사
// 7.1 데이터 파일 무결성 검사
bool checkDataIntegrity() {

    ///// 판매 항목 데이터 파일 열기 /////
    FILE* foodFile = fopen(FILE_PATH, "r+"); // 읽기 및 편집
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
    }
    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
    /////////////////////////////////////

    int itemIds[100];
    int itemCount = 0;
    char line[256];

    // 판매 항목 데이터 읽기
    int lineNumber = 0;
    int expectedId = 1;  // 예상되는 ID 값 (1부터 시작)
    while (fgets(line, sizeof(line), foodFile) && itemCount < 100) {
        lineNumber++;
        int firstNum, id;
        char name[50];
        int price;

        // 줄의 시작부터 첫 번째와 두 번째 숫자를 찾습니다.
        char* ptr = line;
        while (*ptr && isspace(*ptr)) ptr++; // 앞쪽 공백 건너뛰기
        if (sscanf(ptr, "%d", &firstNum) != 1) continue;
        while (*ptr && !isspace(*ptr)) ptr++; // 첫 번째 숫자 건너뛰기
        while (*ptr && isspace(*ptr)) ptr++; // 중간 공백 건너뛰기
        if (sscanf(ptr, "%d", &id) != 1) continue;

        // 이름과 가격은 무시하지만, 형식 검증을 위해 파싱합니다.
        while (*ptr && !isspace(*ptr)) ptr++; // 두 번째 숫자 건너뛰기
        while (*ptr && isspace(*ptr)) ptr++; // 중간 공백 건너뛰기
        if (sscanf(ptr, "%s", name) != 1) continue;
        while (*ptr && !isspace(*ptr)) ptr++; // 이름 건너뛰기
        while (*ptr && isspace(*ptr)) ptr++; // 중간 공백 건너뛰기
        if (sscanf(ptr, "%d", &price) != 1) continue;

        // 고유 번호 중복 검사
        for (int i = 0; i < itemCount; i++) {
            if (itemIds[i] == id) {
                printf("판매 항목 데이터 파일 %d번째 줄과 %d번째 줄에서 고유 번호 중복이 발생했습니다. 프로그램을 종료합니다.\n", i + 1, lineNumber);
                return false;
            }
        }

        // 고유 번호 순차적 증가 검사
        if (id != expectedId) {
            printf("판매 항목 데이터 파일의 고유 번호가 올바른 순서로 증가하지 않습니다. 프로그램을 종료합니다.\n");
            return false;
        }

        itemIds[itemCount++] = id;
        expectedId++;  // 다음 예상 ID 값
    }
    fclose(foodFile);

    // 테이블 주문 파일들 검사
    for (int table = 1; table <= 5; table++) {
        char tableFileName[256];
        snprintf(tableFileName, sizeof(tableFileName), "%s/%d.txt", TABLE_FILE_PATH, table);

        FILE* tableFile = fopen(tableFileName, "r");
        if (!tableFile) {
            printf("%d번 테이블 주문 파일을 열 수 없습니다.\n", table);
            return false;
        }

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
                    printf("테이블 데이터 파일의 %d번째 줄의 판매 항목 고유 번호 %d는 올바른 판매 항목 고유 번호가 아닙니다. 프로그램을 종료합니다.\n",
                        tableLineNumber, saleItemId);
                    fclose(tableFile);
                    return false;
                }
            }
            else {
                printf("테이블 데이터 파일의 %d번째 줄에서 올바른 형식의 판매 항목 고유 번호를 찾을 수 없습니다. 프로그램을 종료합니다.\n", tableLineNumber);
                fclose(tableFile);
                return false;
            }
        }
        fclose(tableFile);
    }
    return true;
}

// 7.2 판매 항목 선택 입력 *주의* 이 함수는 의미 규칙을 검사하지 않습니다.
int inputFoodNumber() {
    int foodNumber;
    while (1) {
        foodNumber = inputInt("판매 항목 번호를 입력하세요: ", true);
        return foodNumber;
    }
}

// 7.3.1 판매 항목명 입력
char* inputFoodName() {
    char* s = (char*)malloc(sizeof(char) * (MAX_INPUT + 2));
    if (s == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return NULL;
    }

    while (1) {
        printf("판매 항목명: ");
        if (fgets(s, MAX_INPUT + 2, stdin) == NULL) {
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
        price = inputInt("판매 항목가: ", false);
        if (price >= 1 && price <= 9999999) {
            return price;
        }
        else {
            printf("오류: 1~9999999사이의 수를 입력하세요.\n");
        }
    }
}

// 7.4.1 테이블 번호 입력
int inputTableNumber() {
    int tableNumber;
    int maxTableNumber = getLastTableNumber();
    while (1) {
        tableNumber = inputInt("테이블 번호를 입력하세요: ", false);
        if (tableNumber < 1 || tableNumber > maxTableNumber) {
            printf("오류: 1~%d사이의 번호를 입력하세요.\n", maxTableNumber);
        }
        else {
            return tableNumber;
        }
    }
}

// 7.4.2 수량 입력
int inputQuantity() {
    int quantity;
    while (1) {
        quantity = inputInt("수량을 입력하세요: ", false);
        if (quantity < 1 || quantity >99) {
            printf("오류: 1~99사이의 수량을 입력하세요.\n");
        }
        else {
            return quantity;
        }
    }
}

// 7.5 판매 항목 조회 프롬프트
void printFoodList() {

    ///// 판매 항목 데이터 파일 열기 /////
    FILE* foodFile = fopen(FILE_PATH, "r"); // 읽기
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
    }
    checkDataIntegrity(foodFile);
    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
    /////////////////////////////////////

    char line[100]; // 각 행을 저장할 문자열
    int line_count = 0;
    int firstNum, secondNum, price;
    char food[50];  // 음식 이름 저장

    printf("\n===== 판매 항목 목록 =====\n");

    while (fgets(line, sizeof(line), foodFile)) {
        sscanf(line, "%d  %d    %s  %d", &firstNum, &secondNum, food, &price);  // 공백 2칸, 4칸, 2칸 유지

        if (firstNum == 0) {
            printf("%d. %s - %d\n", ++line_count, food, price);
        }
    }

    if (line_count == 0) {
        printf("판매 중인 항목이 없습니다.\n");
    }

    fclose(foodFile);
}

// 7.6 판매 항목 추가 프롬프트
void addToFoodList() {

    ///// 판매 항목 데이터 파일 열기 /////
    FILE* foodFile = fopen(FILE_PATH, "r+"); // 읽기 및 편집
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
    }
    checkDataIntegrity(foodFile);
    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
    /////////////////////////////////////

    int firstNum = 0;
    int secondNum = getLastSecondNumber(foodFile) + 1;

    printf("\n추가할 판매 항목 정보를 입력하세요(항목명은 알파벳만 허용)\n");
    char* foodName = inputFoodName();

    if (foodName != NULL) {
        int price = inputPrice();

        fseek(foodFile, 0, SEEK_END);  // 파일의 끝으로 이동
        if (fseek(foodFile, -1, SEEK_END) == 0 && fgetc(foodFile) != '\n') {
            fprintf(foodFile, "\n");
        }
        fprintf(foodFile, "%d  %d    %s  %d\n", firstNum, secondNum, foodName, price);  // 공백 2칸, 4칸, 2칸 공백 적용
        printf("%s 이/가 추가되었습니다.\n", foodName);

        free(foodName);
    }
    else {
        printf("메모리 할당 실패\n");
    }

    fclose(foodFile);
}

// 7.7 판매 항목 제거 프롬프트
void removeFoodItem() {

    ///// 판매 항목 데이터 파일 열기 /////
    FILE* foodFile = fopen(FILE_PATH, "r+"); // 읽기 및 편집
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
    }
    checkDataIntegrity(foodFile);
    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
    /////////////////////////////////////

    printFoodList(); // 현재 판매 항목을 출력

    while (1) {
        rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
        FILE* tempFile = fopen("temp.txt", "w");
        if (tempFile == NULL) {
            printf("임시 파일을 열 수 없습니다.\n");
            return;
        }
        int userChoice = inputFoodNumber();  // 사용자가 선택한 항목 번호 (출력 순서)

        char line[256];
        int firstNum, secondNum, price;
        char foodName[50];
        int currentZeroIndex = 0;  // 0인 항목만 카운트
        int found = 0;

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
            fclose(tempFile);
            continue;
        }
        else {
            fclose(tempFile);
            fclose(foodFile); // 임시 파일로 교체 전 닫아야 함.

            // 원본 파일을 임시 파일로 대체
            remove(FILE_PATH);
            rename("temp.txt", FILE_PATH);
            printf("메뉴 번호 %d가 제거되었습니다.\n", userChoice);
            break;
        }
    }
}



// 주문 내역이 있는 테이블 또는 결제 가능한 테이블 조회
void listTablesWithOrders(int* tablesWithOrders, int* orderCount, const char* message) {
    *orderCount = 0;

    printf("%s: {", message); // 메시지 출력

    bool first = true;
    for (int table = 1; table <= 5; table++) { // 테이블 증감함수 구현 이후 수정
        char tableFileName[256];
        snprintf(tableFileName, sizeof(tableFileName), "%s\\%d.txt", TABLE_FILE_PATH, table);

        FILE* tableFile = fopen(tableFileName, "r");
        if (tableFile != NULL) {
            int c;
            while ((c = fgetc(tableFile)) != EOF) {
                if (!isspace(c)) { // If there's an order
                    if (!first) {
                        printf(", ");
                    }
                    printf("%d", table);
                    first = false;

                    tablesWithOrders[*orderCount] = table; // Store table number
                    (*orderCount)++; // Increment order count
                    break;
                }
            }
            fclose(tableFile);
        }
    }
    printf("}\n");
}

// 결제할 테이블 번호 입력 프롬프트
void inputMultipleTablesForPayment(int* selectedTables, int* selectedCount, int* tablesWithOrders, int orderCount) {
    char input[10];
    int tableNumber;
    *selectedCount = 0;

    printf("테이블 번호를 입력하세요 {}: ");
    while (true) {
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "") == 0) {
            if (*selectedCount > 0) break;
            printf("테이블 번호를 입력하세요 {}: ");
            continue;
        }

        if (strcmp(input, "0") == 0) {
            printf("선택을 취소하고 메인 메뉴로 돌아갑니다.\n");
            *selectedCount = 0;
            return;
        }

        tableNumber = atoi(input);
        if (tableNumber < 1 || tableNumber > 5) {  // 테이블 증감 함수 구현 이후 수정
            printf("오류: 존재하지 않는 테이블 번호입니다.\n");
        }
        else {
            bool validOrder = false;
            for (int i = 0; i < orderCount; i++) {
                if (tablesWithOrders[i] == tableNumber) {
                    validOrder = true;
                    break;
                }
            }

            if (!validOrder) {
                printf("경고: %d번 테이블에는 주문 내역이 없습니다.\n", tableNumber);
            }
            else {
                bool alreadySelected = false;
                for (int i = 0; i < *selectedCount; i++) {
                    if (selectedTables[i] == tableNumber) {
                        printf("경고: 이미 입력한 테이블 번호입니다.\n");
                        alreadySelected = true;
                        break;
                    }
                }

                if (!alreadySelected) {
                    selectedTables[*selectedCount] = tableNumber;
                    (*selectedCount)++;
                }
            }
        }

        printf("테이블 번호를 입력하세요 {");
        for (int i = 0; i < *selectedCount; i++) {
            printf("%d", selectedTables[i]);
            if (i < *selectedCount - 1) {
                printf(", ");
            }
        }
        printf("}: ");
    }
}

// 7.8 주문 생성 프롬프트
void createOrder() {

    // 주문 가능한 테이블 목록 표시
    int tablesWithOrders[5];  // 주문 내역이 있는 테이블을 저장할 배열
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "주문 내역이 있는 테이블 번호");  // 주문이 있는 테이블을 확인하고 표시

    FILE* foodFile = fopen(FILE_PATH, "r+"); // 읽기 및 편집
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return;
    }
    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림

    int tableNumber = inputTableNumber();  // 테이블 번호 입력 받기

    // 테이블 파일 경로 설정
    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);

    // 테이블 파일 열기
    FILE* tableFile = fopen(tableFilePath, "a");
    if (tableFile == NULL) {
        printf("테이블 파일을 열 수 없습니다.\n");
        fclose(foodFile);
        return;
    }

    printFoodList();  // 판매 목록 출력

    int selection = -1;  // 판매 항목 선택 변수
    OrderItem* orderList = NULL;

    int firstNum, secondNum, price;
    char foodName[50];

    while (selection != 0) {  // 0을 입력하면 주문이 끝남
        printf("<주문을 끝내려면 0을 입력하세요>\n");
        selection = inputFoodNumber();

        if (selection == 0) {
            break;  // 0 입력 시 주문 종료
        }

        int currentMenuIndex = 0;
        int validSelection = 0;

        // 파일을 다시 읽어 선택한 메뉴의 ID 찾기
        rewind(foodFile);
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0) {
                currentMenuIndex++;
                if (currentMenuIndex == selection) {
                    validSelection = 1;
                    int quantity = inputQuantity(); // 수량 입력받기

                    // 수량만큼 반복해서 항목 추가
                    for (int i = 0; i < quantity; i++) {
                        orderList = addOrderItem(orderList, secondNum);
                    }

                    break;
                }
            }
        }

        if (!validSelection) {
            printf("해당하는 숫자의 선택지가 없습니다.\n");
        }
    }

    // 주문 리스트를 테이블 파일에 저장
    OrderItem* current = orderList;
    while (current != NULL) {
        for (int i = 0; i < current->quantity; i++) {
            fprintf(tableFile, "%d\n", current->itemID);  // 메뉴 ID 저장
        }
        current = current->next;
    }

    // 최종 주문 결과 출력 (OrderItem 사용)
    printf("\n%d번 테이블 ", tableNumber);

    current = orderList;

    int itemCount = 0;
    while (current != NULL) {
        // 메뉴 정보 찾기
        rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0 && secondNum == current->itemID) {
                if (itemCount > 0) {
                    printf(" ");  // 각 항목 사이에 공백 추가
                }
                printf("%s %d개", foodName, current->quantity);
                itemCount++;
                break;
            }
        }
        current = current->next;
    }

    printf(" 주문완료되었습니다.\n");

    // 메모리 해제
    freeOrderItems(orderList);
    freeOrderItems(current);

    fclose(tableFile);
    fclose(foodFile);
}

// 7.9 주문 조회 프롬프트
void printOrder() {
    // 주문 가능한 테이블 목록 표시
    int tablesWithOrders[5];  // 주문 내역이 있는 테이블을 저장할 배열
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "주문 내역이 있는 테이블 번호");  // 주문이 있는 테이블을 확인하고 표시

    int tableNumber = inputTableNumber();

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




// 7.10 결제 처리 프롬프트
void makePayment() { // 테이블 증감함수 구현 후 수정 
    int tablesWithOrders[5];  // 주문 내역이 있는 테이블 번호를 저장할 배열
    int orderCount = 0;       // 주문 내역이 있는 테이블 수를 저장할 변수

    listTablesWithOrders(tablesWithOrders, &orderCount, "결제 가능한 테이블 번호");  // 주문 내역이 있는 테이블을 표시하고, 결과를 가져옴

    int selectedTables[5];
    int selectedCount = 0;

    inputMultipleTablesForPayment(selectedTables, &selectedCount, tablesWithOrders, orderCount); // 수정된 함수 호출
    if (selectedCount == 0) {
        printf("결제할 테이블이 선택되지 않았습니다. 메인 메뉴로 돌아갑니다.\n");
        return;
    }

    int combinedTotal = 0;  // 선택된 테이블들의 전체 결제 금액
    int paymentSuccess = 0; // 결제 성공 여부 체크 변수
    int counters[5];        // 각 선택된 테이블의 줄 수를 저장할 배열

    // 각 선택된 테이블의 총 주문액을 계산
    for (int i = 0; i < selectedCount; i++) {
        int tableNumber = selectedTables[i];
        char tableFilePath[256];
        snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);

        FILE* tableFile = fopen(tableFilePath, "r");
        if (!tableFile) {
            printf("파일을 열 수 없습니다: %d번 테이블\n", tableNumber);
            continue;
        }

        // foodlist 파일을 열어 각 항목의 가격을 확인
        FILE* foodFile = fopen(FILE_PATH, "r");
        if (!foodFile) {
            printf("판매 항목 파일을 열 수 없습니다.\n");
            fclose(tableFile);
            continue;
        }

        // 주문 항목의 수량 계산
        int itemID;
        OrderItem* orderList = NULL;

        while (fscanf(tableFile, "%d", &itemID) == 1) {
            orderList = addOrderItem(orderList, itemID);
        }

        // 줄 수 계산
        counters[i] = 0; // 각 테이블의 줄 수를 초기화
        char c;
        FILE* tableFileForLineCount = fopen(tableFilePath, "r");
        while ((c = fgetc(tableFileForLineCount)) != EOF) {
            if (c == '\n') {
                counters[i]++; // 각 테이블의 줄 수를 저장
            }
        }
        fclose(tableFileForLineCount);

        // 주문 합계 계산
        int foundItems = 0;
        int firstNum, secondNum, price;
        char foodName[50];
        int totalPrice = 0;

        OrderItem* current = orderList;
        while (current != NULL) {
            rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
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

        // 메모리 해제
        freeOrderItems(orderList);
        fclose(foodFile);
        fclose(tableFile);

        if (foundItems == 0) {
            printf("\n%d번 테이블은 결제가 불가능합니다.\n", tableNumber);
        }
        else {
            combinedTotal += totalPrice;  // 결제 금액을 합산
        }
    }

    printf("전체 결제 금액: %d원\n", combinedTotal);

    // 전체 금액에 대해 결제 처리
    int remainingBalance = combinedTotal; // 남은 결제금액(추후 중간 결산때 이용할 수 있는 변수)
    while (remainingBalance > 0) {
        printf("결제할 금액을 입력하세요 [%d원]: ", remainingBalance);

        char input[10];
        fgets(input, sizeof(input), stdin);
        int paymentAmount = atoi(input);

        if (paymentAmount < 0) {
            printf("오류: 음수는 입력할 수 없습니다.\n");
            continue;
        }
        else if (paymentAmount == 0) {
            printf("결제 중단.\n");
            break;
        }
        else if (paymentAmount > remainingBalance) {
            printf("오류: 결제할 금액보다 큽니다.\n");
            continue;
        }

        remainingBalance -= paymentAmount;
        if (remainingBalance == 0) {
            printf("전체 금액이 결제 완료되었습니다.\n");
            paymentSuccess = 1;  // 결제가 완료됨을 표시
            break;
        }
        else {
            printf("%d원 분할 결제 완료되었습니다. 남은 금액: %d원\n", paymentAmount, remainingBalance);
        }
        int totalPayment = combinedTotal - remainingBalance; // 누적 결제 금액
    }

    // 전체 결제가 완료된 후 각 테이블의 주문 내역 삭제
    if (paymentSuccess) {
        for (int i = 0; i < selectedCount; i++) {
            char tableFilePath[256];
            snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, selectedTables[i]);
            deleteLines(tableFilePath, 1, counters[i]);  // 각 테이블 파일의 모든 줄을 삭제
        }
    }
    
}




// 7.11 메인 메뉴 프롬프트
int printMain() {
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
        s = inputInt("메뉴 선택: ", false);
        if (s > 7 || s < 1) {
            printf("1~7 사이의 값을 입력해주세요.\n");
        }
        else {
            return s;
        }
    }
}

// 프로그램 실행
int main(void) {
    while (1) {

        if (!checkDataIntegrity()) {
            exitProgram();
        }

        int s = printMain();
        switch (s) {
        case 1:
            // 7.5 판매 항목 조회 프롬프트
            printFoodList();
            break;
        case 2:
            // 7.6 판매 항목 추가 프롬프트
            addToFoodList();
            break;
        case 3:
            // 7.7 판매 항목 제거 프롬프트
            removeFoodItem();
            break;
        case 4:
            // 7.8 주문 생성 프롬프트
            createOrder();
            break;
        case 5:
            // 7.9 주문 조회 프롬프트
            printOrder();
            break;
        case 6:
            // 7.10 결제 처리 프롬프트
            makePayment();
            break;
        case 7:
            // 7.11 메인 메뉴 프롬프트의 종료 기능
            exitProgram();
            break;
        default:
            break;
        }
    }

    return 0;
}