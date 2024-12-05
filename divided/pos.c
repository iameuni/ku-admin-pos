#include "pos.h"

//////////////////// 기획서 기반 프롬프트 ////////////////////

// 7.1 데이터 파일 무결성 검사
bool checkDataIntegrity() {
    ///// 판매 항목 데이터 파일 검사 (기존 코드와 동일) /////
    FILE* foodFile = fopen(FILE_PATH, "r+");
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return false;
    }

    int itemIds[100];
    int itemCount = 0;
    char line[256];

    // 판매 항목 데이터 읽기
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

        // 고유 번호 중복 검사
        for (int i = 0; i < itemCount; i++) {
            if (itemIds[i] == id) {
                printf("판매 항목 데이터 파일 %d번째 줄과 %d번째 줄에서 고유 번호 중복이 발생했습니다. 프로그램을 종료합니다.\n",
                    i + 1, lineNumber);
                fclose(foodFile);
                return false;
            }
        }

        // 고유 번호 순차적 증가 검사
        if (id != expectedId) {
            printf("판매 항목 데이터 파일의 고유 번호가 올바른 순서로 증가하지 않습니다. 프로그램을 종료합니다.\n");
            fclose(foodFile);
            return false;
        }

        itemIds[itemCount++] = id;
        expectedId++;
    }
    fclose(foodFile);

    ///// 테이블 데이터 파일 검사 (수정된 부분) /////
    int tableCount = 0;
    int existingTables[11] = { 0 };  // 존재하는 테이블 번호 체크용 (0번 인덱스는 사용안함)

    // 테이블 데이터 파일 검사 부분 수정
    for (int table = 1; table <= MAX_TABLE_NUMBER; table++) {
        char tableFileName[256];
        snprintf(tableFileName, sizeof(tableFileName), "%s\\%d.txt", TABLE_FILE_PATH, table);

        FILE* tableFile = fopen(tableFileName, "r");
        if (!tableFile) continue;

        int tableLineNumber = 0;
        char line[256];
        int paymentUnitTables[MAX_TABLE_NUMBER] = {0};
        int paymentUnitCount = 0;
        int totalPartialPayments = 0;
        
        while (fgets(line, sizeof(line), tableFile)) {
            tableLineNumber++;
            line[strcspn(line, "\n")] = 0;  // 개행 문자 제거

            if (line[0] == '#') {
                if (line[1] == '#') {
                    // ## 부분 결제 금액 검사
                    char* paymentStr = line + 2;
                    while (*paymentStr == ' ') paymentStr++;  // 앞쪽 공백 제거
                    
                    // 0으로 시작하는지 검사
                    if (paymentStr[0] == '0' && strlen(paymentStr) > 1) {
                        printf("테이블 데이터 파일 %d번째 줄: 부분 결제 금액이 0으로 시작할 수 없습니다. 프로그램을 종료합니다.\n", tableLineNumber);
                        fclose(tableFile);
                        return false;
                    }

                    // 숫자만 포함하는지 검사
                    for (int i = 0; paymentStr[i]; i++) {
                        if (!isdigit(paymentStr[i])) {
                            printf("테이블 데이터 파일 %d번째 줄: 부분 결제 금액이 올바르지 않습니다. 프로그램을 종료합니다.\n", tableLineNumber);
                            fclose(tableFile);
                            return false;
                        }
                    }

                    int payment = atoi(paymentStr);
                    if (payment <= 0) {
                        printf("테이블 데이터 파일 %d번째 줄: 부분 결제 금액이 양의 정수가 아닙니다. 프로그램을 종료합니다.\n", tableLineNumber);
                        fclose(tableFile);
                        return false;
                    }
                    totalPartialPayments += payment;
                }
                else {
                    // # 결제 단위 테이블 번호 검사
                    char* tableStr = line + 1;
                    while (*tableStr == ' ') tableStr++;  // 앞쪽 공백 제거
                    
                    // 숫자만 포함하는지 검사
                    for (int i = 0; tableStr[i]; i++) {
                        if (!isdigit(tableStr[i])) {
                            printf("테이블 데이터 파일 %d번째 줄: 결제 단위 테이블 번호가 올바르지 않습니다. 프로그램을 종료합니다.\n", tableLineNumber);
                            fclose(tableFile);
                            return false;
                        }
                    }

                    int unitTable = atoi(tableStr);
                    if (unitTable < 1 || unitTable > MAX_TABLE_NUMBER || !isTableExist(unitTable)) {
                        printf("테이블 데이터 파일 %d번째 줄: 올바르지 않은 결제 단위 테이블 번호입니다. 프로그램을 종료합니다.\n", tableLineNumber);
                        fclose(tableFile);
                        return false;
                    }

                    // 결제 단위에 추가
                    paymentUnitTables[paymentUnitCount++] = unitTable;
                }
            }
            else {
                int saleItemId;
                if (sscanf(line, "%d", &saleItemId) != 1) {
                    printf("테이블 데이터 파일의 %d번째 줄에서 올바른 형식의 판매 항목 고유 번호를 찾을 수 없습니다. 프로그램을 종료합니다.\n", tableLineNumber);
                    fclose(tableFile);
                    return false;
                }
                
                // 판매 항목 데이터 파일에 존재하는 번호인지 확인
                bool found = false;
                for (int i = 0; i < itemCount; i++) {
                    if (itemIds[i] == saleItemId) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    printf("테이블 데이터 파일의 %d번째 줄의 판매 항목 고유 번호 %d는 올바른 판매 항목 고유 번호가 아닙니다. 프로그램을 종료합니다.\n", tableLineNumber, saleItemId);
                    fclose(tableFile);
                    return false;
                }
            }
        }

        // 결제 단위 상호 참조 검사
        if (paymentUnitCount > 0) {
            for (int i = 0; i < paymentUnitCount; i++) {
                int unitTable = paymentUnitTables[i];
                char unitFileName[256];
                snprintf(unitFileName, sizeof(unitFileName), "%s\\%d.txt", TABLE_FILE_PATH, unitTable);
                
                FILE* unitFile = fopen(unitFileName, "r");
                if (!unitFile) {
                    printf("결제 단위로 묶인 테이블 %d의 파일을 찾을 수 없습니다. 프로그램을 종료합니다.\n", unitTable);
                    fclose(tableFile);
                    return false;
                }

                // 결제 단위 테이블들이 서로를 참조하는지 검사
                bool foundAllReferences = true;
                for (int j = 0; j < paymentUnitCount; j++) {
                    bool foundReference = false;
                    rewind(unitFile);
                    
                    while (fgets(line, sizeof(line), unitFile)) {
                        if (line[0] == '#' && line[1] != '#') {
                            int refTable = atoi(line + 1);
                            if (refTable == paymentUnitTables[j]) {
                                foundReference = true;
                                break;
                            }
                        }
                    }

                    if (!foundReference) {
                        foundAllReferences = false;
                        break;
                    }
                }

                fclose(unitFile);
                
                if (!foundAllReferences) {
                    printf("결제 단위로 묶인 테이블들의 상호 참조가 올바르지 않습니다. 프로그램을 종료합니다.\n");
                    fclose(tableFile);
                    return false;
                }
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
        foodNumber = inputInt("판매 항목 번호를 입력하세요: ", true,false);
        return foodNumber;
    }
}

// 7.5 판매 항목 조회 프롬프트
void printFoodList() {

    ///// 판매 항목 데이터 파일 열기 /////
    FILE* foodFile = fopen(FILE_PATH, "r"); // 읽기
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
    }
    checkDataIntegrity();
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
    checkDataIntegrity();
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
    checkDataIntegrity();
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

// 7.8 주문 생성 프롬프트
void createOrder() {
    int tablesWithOrders[MAX_TABLE_NUMBER];  
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n주문 내역이 있는 테이블 번호");  

    FILE* foodFile = fopen(FILE_PATH, "r+");
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return;
    }
    rewind(foodFile);

    int tableNumber = 0;
    while (1) {
        tableNumber = inputTableNumber(false);
        if (tableNumber < -1) continue;
        else break;
    }

    char tableFilePath[256];
    snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);
    
    // 빈 테이블인지 확인
    bool isEmptyTable = true;
    FILE* checkFile = fopen(tableFilePath, "r");
    if (checkFile) {
        char line[256];
        while (fgets(line, sizeof(line), checkFile)) {
            if (!isspace((unsigned char)line[0])) {
                isEmptyTable = false;
                break;
            }
        }
        fclose(checkFile);
    }
    
    printFoodList();  // 판매 목록 출력

    int selection = -1;  
    OrderItem* orderList = NULL;
    int firstNum, secondNum, price;
    char foodName[50];
    bool orderMade = false;  // 실제 주문이 발생했는지 추적하는 플래그

    while (selection != 0) {  
        printf("\n<주문을 끝내려면 0을 입력하세요>\n\n");
        selection = inputFoodNumber();

        if (selection == 0) {
            break;  
        }

        int currentMenuIndex = 0;
        int validSelection = 0;

        rewind(foodFile);
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0) {
                currentMenuIndex++;
                if (currentMenuIndex == selection) {
                    validSelection = 1;
                    int quantity = inputQuantity();

                    // 최초 주문인 경우, 결제 단위 정보는 나중에 쓸 것임
                    if (isEmptyTable) {
                        orderMade = true;
                    }

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

    // 실제 주문이 발생한 경우에만 파일에 저장
    if (orderMade || orderList != NULL) {
        FILE* tempFile = fopen("temp.txt", "w");
        if (tempFile) {
            // 1. 기존 주문 내역 복사
            if (!isEmptyTable) {
                FILE* existingFile = fopen(tableFilePath, "r");
                if (existingFile) {
                    char line[256];
                    while (fgets(line, sizeof(line), existingFile)) {
                        if (line[0] != '#') {  // 순수 주문 내역만 복사
                            fprintf(tempFile, "%s", line);
                        }
                    }
                    fclose(existingFile);
                }
            }

            // 2. 새로운 주문 추가
            OrderItem* current = orderList;
            while (current != NULL) {
                for (int i = 0; i < current->quantity; i++) {
                    fprintf(tempFile, "%d\n", current->itemID);
                }
                current = current->next;
            }

            // 3. 결제 단위 정보 (#) 추가
            if (isEmptyTable && orderMade) {
                // 새로운 테이블의 경우 결제 단위 설정
                fprintf(tempFile, "#%d\n", tableNumber);
            } else if (!isEmptyTable) {
                // 기존 결제 단위 정보 복사
                FILE* existingFile = fopen(tableFilePath, "r");
                if (existingFile) {
                    char line[256];
                    while (fgets(line, sizeof(line), existingFile)) {
                        if (line[0] == '#' && line[1] != '#') {
                            fprintf(tempFile, "%s", line);
                        }
                    }
                    fclose(existingFile);
                }
            }

            // 4. 부분 결제 정보 (##) 복사
            if (!isEmptyTable) {
                FILE* existingFile = fopen(tableFilePath, "r");
                if (existingFile) {
                    char line[256];
                    while (fgets(line, sizeof(line), existingFile)) {
                        if (line[0] == '#' && line[1] == '#') {
                            fprintf(tempFile, "%s", line);
                        }
                    }
                    fclose(existingFile);
                }
            }

            fclose(tempFile);

            // 임시 파일을 원래 파일로 교체
            remove(tableFilePath);
            rename("temp.txt", tableFilePath);

            // 주문 결과 출력
            printf("\n%d번 테이블 ", tableNumber);
            current = orderList;
            int itemCount = 0;
            while (current != NULL) {
                rewind(foodFile);
                while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
                    if (firstNum == 0 && secondNum == current->itemID) {
                        if (itemCount > 0) {
                            printf(" ");
                        }
                        printf("%s %d개", foodName, current->quantity);
                        itemCount++;
                        break;
                    }
                }
                current = current->next;
            }
            printf(" 주문완료되었습니다.\n");
        }
    }

    // 메모리 해제
    freeOrderItems(orderList);
    fclose(foodFile);
}
// 7.9 주문 조회 프롬프트
void printOrder() {
    // 주문 가능한 테이블 목록 표시
    int tablesWithOrders[MAX_TABLE_NUMBER];  // 주문 내역이 있는 테이블을 저장할 배열
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n주문 내역이 있는 테이블 번호");  // 주문이 있는 테이블을 확인하고 표시

    int tableNumber = 0;

    while (1) {
        tableNumber = inputTableNumber(false);
        if (tableNumber < -1) continue;
        else break;
    }

    // 테이블 파일 경로 
    char tableFilePath[256];
    // snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);
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
    printf("\n\t%d번 테이블 주문 조회\t\n\n", tableNumber);
    printf("%-20s %-10s %-10s\n", "메뉴", "수량", "금액");

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
                printf("%-20s %-10d %-10d\n", foodName, quantity, quantity * price);
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

int calculateTotalAmount(PaymentContext* context) {
    int total = 0;
    FILE* foodFile = fopen(FILE_PATH, "r");
    if (!foodFile) return 0;

    for (int i = 0; i < context->tableCount; i++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, context->tableNumbers[i]);
        FILE* tableFile = fopen(tablePath, "r");
        if (!tableFile) continue;

        char line[256];
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] != '#') {  // 주문 내역만 처리
                int itemId;
                if (sscanf(line, "%d", &itemId) == 1) {
                    // 해당 아이템의 가격 찾기
                    rewind(foodFile);
                    int firstNum, secondNum, price;
                    char foodName[50];
                    while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
                        if (firstNum == 0 && secondNum == itemId) {
                            total += price;
                            break;
                        }
                    }
                }
            }
            else if (line[0] == '#' && line[1] == '#') {  // 부분 결제 내역 처리
                int payment;
                if (sscanf(line + 2, "%d", &payment) == 1) {
                    total -= payment;
                }
            }
        }
        fclose(tableFile);
    }
    fclose(foodFile);
    return total;
}

// 7.10 결제 처리 프롬프트
void makePayment() {
    int tablesWithOrders[MAX_TABLE_NUMBER];
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n결제 가능한 테이블 번호");

    currentContext.tableCount = 0;  // 전역 PaymentContext 초기화
    int primarySelectedTable = -1;

    // 결제할 테이블 선택
    while (true) {
        printf("테이블 번호를 입력하세요 {");
        for (int i = 0; i < currentContext.tableCount; i++) {
            printf("%d", currentContext.tableNumbers[i]);
            if (i < currentContext.tableCount - 1) printf(", ");
        }
        printf("}: ");

        int input = inputTableNumber(true);
        if (input == -1) {
            if (currentContext.tableCount > 0) break;
            continue;
        }
        if (input == 0) {
            printf("결제가 종료됩니다.\n");
            return;
        }

        // 주문 내역 있는지 확인
        bool validOrder = false;
        for (int i = 0; i < orderCount; i++) {
            if (tablesWithOrders[i] == input) {
                validOrder = true;
                break;
            }
        }
        if (!validOrder) {
            printf("주문 내역이 없는 테이블입니다.\n");
            continue;
        }

        // 이미 선택된 테이블인지 확인
        bool alreadySelected = false;
        for (int i = 0; i < currentContext.tableCount; i++) {
            if (currentContext.tableNumbers[i] == input) {
                alreadySelected = true;
                break;
            }
        }
        if (alreadySelected) {
            printf("이미 입력한 테이블 번호입니다.\n");
            continue;
        }

        if (primarySelectedTable == -1) {
            primarySelectedTable = input;
        }
        
        PaymentUnit* unit = getPaymentUnit(input);
        if (unit->tableCount > 0) {
            // 결제 단위에 속한 테이블들 추가
            for (int i = 0; i < unit->tableCount; i++) {
                bool exists = false;
                for (int j = 0; j < currentContext.tableCount; j++) {
                    if (currentContext.tableNumbers[j] == unit->tables[i]) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    currentContext.tableNumbers[currentContext.tableCount++] = unit->tables[i];
                }
            }
        } else {
            currentContext.tableNumbers[currentContext.tableCount++] = input;
        }
        free(unit->partialPayments);
        free(unit);
    }

    // 각 테이블의 주문액 계산 및 출력
    printf("\n");
    int totalOrderAmount = 0;
    int totalPartialPayments = 0;

    for (int i = 0; i < currentContext.tableCount; i++) {
        int tableNumber = currentContext.tableNumbers[i];
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);

        FILE* tableFile = fopen(tablePath, "r");
        if (!tableFile) continue;

        FILE* foodFile = fopen(FILE_PATH, "r");
        if (!foodFile) {
            fclose(tableFile);
            continue;
        }

        // 주문 내역 집계
        OrderItem* orderList = NULL;
        int itemID;
        char line[256];
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] != '#') {
                if (sscanf(line, "%d", &itemID) == 1) {
                    orderList = addOrderItem(orderList, itemID);
                }
            }
            else if (line[0] == '#' && line[1] == '#') {
                int payment;
                if (sscanf(line + 2, "%d", &payment) == 1) {
                    totalPartialPayments += payment;
                }
            }
        }

        // 주문 금액 계산
        int tableTotal = 0;
        OrderItem* current = orderList;
        while (current != NULL) {
            rewind(foodFile);
            int firstNum, secondNum, price;
            char foodName[50];
            while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
                if (firstNum == 0 && secondNum == current->itemID) {
                    tableTotal += price * current->quantity;
                    break;
                }
            }
            current = current->next;
        }

        printf("%d번 테이블 주문액: %d\n", tableNumber, tableTotal);
        totalOrderAmount += tableTotal;

        freeOrderItems(orderList);
        fclose(foodFile);
        fclose(tableFile);
    }

    // 남은 결제 금액 계산
    int remainingBalance = totalOrderAmount - totalPartialPayments;
    if (remainingBalance <= 0) {
        printf("이미 전액 결제되었습니다.\n");
        return;
    }

    printf("총 주문액: %d원\n", totalOrderAmount);
    printf("기존 부분 결제액: %d원\n", totalPartialPayments);
    printf("남은 결제액: %d원\n\n", remainingBalance);

    // 결제 처리
    while (remainingBalance > 0) {
        int payment = inputPaymentAmount(remainingBalance);
        
        if (payment == -1) {  // 결제 취소 (0 입력)
            break;
        }
        
        if (payment == -2) {  // 부분 결제 (. 입력)
            updatePaymentUnit(primarySelectedTable, currentContext.tableNumbers, currentContext.tableCount);
            break;
        }

        // 결제 기록 업데이트
        updatePaymentRecord(primarySelectedTable, remainingBalance - payment);
        remainingBalance = payment;

        if (remainingBalance == 0) {
            printf("테이블을 비우시겠습니까?: ");
            char input[10];
            fgets(input, sizeof(input), stdin);
            if (input[0] != '.') {
                // 모든 선택된 테이블 비우기
                for (int i = 0; i < currentContext.tableCount; i++) {
                    char tablePath[256];
                    snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, currentContext.tableNumbers[i]);
                    FILE* file = fopen(tablePath, "w");
                    if (file) fclose(file);
                }
                printf("전액 결제된 테이블을 비웠습니다.\n");
            } else {
                printf("테이블을 비우지 않습니다.\n");
            }
            break;
        }
    }
}
// 7.11 테이블 증감 프롬프트
void adjustTables() {
    while (1) {
        int currentTableCount = getCurrentTableCount();

        printf("\n증감시킬 테이블 수를 입력하세요 (-%d ~ %d): ",MAX_TABLE_ADJUST, MAX_TABLE_ADJUST);

       
        int adjustment = inputInt(NULL, true, true);

        // '0' 입력 시 취소
        if (adjustment == 0) {
            printf("테이블 증감 작업이 취소되었습니다.\n");
            return;
        }

        // 범위 검증 (-MAX_TABLE_ADJUST ~ MAX_TABLE_ADJUST)
        if (adjustment < -MAX_TABLE_ADJUST || adjustment > MAX_TABLE_ADJUST) {
            printf("올바르지 않은 입력입니다.\n");
            continue;
        }

        // 테이블 감소
        if (adjustment < 0) {
            // 현재 테이블 수가 감소시키려는 수보다 적거나 같은 경우
            if (currentTableCount <= -adjustment) {
                printf("오류: 현재 테이블이 %d개 뿐이라서 %d개를 감소시킬 수 없습니다.\n",
                    currentTableCount, -adjustment);
                continue;
            }

            // 감소 후 테이블 수가 1개 미만이 되는 경우
            if (currentTableCount + adjustment < 1) {
                printf("오류: 테이블은 최소 1개가 존재해야 합니다.\n");
                continue;
            }

            // 주문 내역이 없는 테이블 수 계산
            int emptyCount = 0;
            for (int i = MAX_TABLE_NUMBER; i >= 1; i--) {
                if (isTableExist(i) && !hasOrders(i)) {
                    emptyCount++;
                }
            }

            // 주문 내역이 없는 테이블이 하나도 없는 경우
            if (emptyCount == 0) {
                printf("오류: 모든 테이블에 주문 내역이 있어서 감소시킬 수 없습니다.\n");
                continue;
            }

            // 주문 내역이 없는 테이블 수가 감소시키려는 수보다 적은 경우
            if (emptyCount < -adjustment) {
                printf("오류: 주문 내역이 없는 테이블이 %d개 뿐이라서 %d개를 감소시킬 수 없습니다.\n",
                    emptyCount, -adjustment);
                continue;
            }

            // 테이블 감소 실행
            int removed = 0;
            while (removed < -adjustment) {
                // 가장 큰 번호의 빈 테이블 찾기
                int tableToRemove = -1;
                for (int i = MAX_TABLE_NUMBER; i >= 1; i--) {
                    if (isTableExist(i) && !hasOrders(i)) {
                        tableToRemove = i;
                        break;
                    }
                }

                if (tableToRemove == -1) {
                    printf("예기치 않은 오류가 발생했습니다.\n");
                    return;
                }

                char tableFilePath[256];
                snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, tableToRemove);
                remove(tableFilePath);
                printf("%d번 테이블이 제거되었습니다.\n", tableToRemove);
                removed++;
            }
        }
        // 테이블 증가
        else {
            if (currentTableCount + adjustment > MAX_TABLE_NUMBER) {
                printf("테이블은 최대 %d개까지만 존재할 수 있습니다.\n", MAX_TABLE_NUMBER);
                continue;
            }

            // 테이블 증가 실행
            int added = 0;
            while (added < adjustment) {
                // 사용 가능한 가장 작은 번호 찾기
                int newTableNumber = -1;
                for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
                    if (!isTableExist(i)) {
                        newTableNumber = i;
                        break;
                    }
                }

                if (newTableNumber == -1) {
                    printf("더 이상 추가할 수 있는 테이블 번호가 없습니다.\n");
                    return;
                }

                char tableFilePath[256];
                snprintf(tableFilePath, sizeof(tableFilePath), "%s\\%d.txt", TABLE_FILE_PATH, newTableNumber);
                FILE* tableFile = fopen(tableFilePath, "w");
                if (tableFile != NULL) {
                    fclose(tableFile);
                    printf("%d번 테이블이 추가되었습니다.\n", newTableNumber);
                    added++;
                }
            }
        }

        // 성공적으로 처리된 경우 함수 종료
        return;
    }
}

// 테이블 이동 함수 수정
void moveTable() {
    int tablesWithOrders[MAX_TABLE_NUMBER];
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n주문이 있는 테이블 번호");

    // 출발 테이블 선택
    printf("이동시킬 테이블을 입력하세요: ");
    int sourceTable = inputInt(NULL, false, false);
    if (sourceTable < 0) return;

    // 결제 단위 확인
    PaymentUnit* sourceUnit = getPaymentUnit(sourceTable);
    if (sourceUnit->tableCount == 0) {
        sourceUnit->tables[0] = sourceTable;
        sourceUnit->tableCount = 1;
    }
    
    printf("{");
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        printf("%d", sourceUnit->tables[i]);
        if (i < sourceUnit->tableCount - 1) printf(", ");
    }
    printf("}번 테이블을 이동할 테이블을 입력하세요{}: ");

    // 목적지 테이블들 선택
    int destTables[MAX_TABLE_NUMBER];
    int destCount = 0;
    
    while (1) {
        if (destCount > 0) {
            printf("{");
            for (int i = 0; i < sourceUnit->tableCount; i++) {
                printf("%d", sourceUnit->tables[i]);
                if (i < sourceUnit->tableCount - 1) printf(", ");
            }
            printf("}번 테이블을 이동할 테이블을 입력하세요{");
            for (int i = 0; i < destCount; i++) {
                printf("%d", destTables[i]);
                if (i < destCount - 1) printf(", ");
            }
            printf("}: ");
        }

        int destTable = inputTableNumber(true);
        if (destTable == -1) {
            if (destCount > 0) break;
            continue;
        }
        if (destTable == 0) {
            printf("이동이 취소되었습니다.\n");
            free(sourceUnit->partialPayments);
            free(sourceUnit);
            return;
        }

        // 유효성 검사
        if (!isTableExist(destTable)) {
            printf("존재하지 않는 테이블 번호입니다.\n");
            continue;
        }

        // 이미 선택된 테이블인지 확인
        bool alreadySelected = false;
        for (int i = 0; i < destCount; i++) {
            if (destTables[i] == destTable) {
                printf("이미 선택된 테이블입니다.\n");
                alreadySelected = true;
                break;
            }
        }
        if (alreadySelected) continue;

        // 출발 테이블과 동일한 테이블 허용
        bool isSourceTable = false;
        for (int i = 0; i < sourceUnit->tableCount; i++) {
            if (sourceUnit->tables[i] == destTable) {
                isSourceTable = true;
                break;
            }
        }

        // 선택된 테이블 추가 (결제 단위와 상관없이 선택된 테이블만)
        destTables[destCount++] = destTable;
    }

    // 테이블 이동 실행
    if (destCount > 0) {
        executeTableMove(sourceUnit, destTables, destCount);
    }

    free(sourceUnit->partialPayments);
    free(sourceUnit);
}


// 결제 취소 함수
void cancelPayment() {
    int tablesWithPayments[MAX_TABLE_NUMBER];
    int paymentCount = 0;
    listTablesWithPartialPayments(tablesWithPayments, &paymentCount, 
        "\n부분결제 항목이 있는 테이블 번호");
    
    if (paymentCount == 0) {
        printf("부분 결제 항목이 있는 테이블이 없습니다.\n");
        return;
    }

    int tableNumber = inputTableNumber(false);
    if (tableNumber < 0) return;

    PaymentUnit* unit = getPaymentUnit(tableNumber);
    if (unit->paymentCount == 0) {
        printf("취소할 부분결제 내역이 없습니다.\n");
        free(unit->partialPayments);
        free(unit);
        return;
    }

    // 모든 결제단위 테이블의 부분결제 내역 수집
    typedef struct {
        int amount;
        int count;
    } PaymentInfo;
    
    PaymentInfo payments[100] = {0};  // 충분히 큰 크기로 설정
    int uniquePaymentCount = 0;

    for (int t = 0; t < unit->tableCount; t++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, unit->tables[t]);
        FILE* tableFile = fopen(tablePath, "r");
        if (!tableFile) continue;

        char line[256];
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] == '#' && line[1] == '#') {
                int amount;
                sscanf(line + 2, "%d", &amount);
                
                // 이미 있는 금액인지 확인
                bool found = false;
                for (int i = 0; i < uniquePaymentCount; i++) {
                    if (payments[i].amount == amount) {
                        payments[i].count++;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    payments[uniquePaymentCount].amount = amount;
                    payments[uniquePaymentCount].count = 1;
                    uniquePaymentCount++;
                }
            }
        }
        fclose(tableFile);
    }

    int selectedPayments[100] = {0};
    int selectedCount = 0;

    while (1) {
        printf("결제 취소할 부분결제 항목을 고르시오 [");
        bool first = true;
        for (int i = 0; i < uniquePaymentCount; i++) {
            if (payments[i].count > 0) {
                if (!first) printf(", ");
                // 같은 금액이 여러 번 있으면 그만큼 반복 출력
                for (int j = 0; j < payments[i].count; j++) {
                    if (j > 0) printf(", ");
                    printf("%d", payments[i].amount);
                }
                first = false;
            }
        }
        printf("]: ");

        int amount = inputInt(NULL, true, false);
        if (amount == 0) {
            printf("결제 취소가 종료됩니다.\n");
            break;
        }
        if (amount == -1) {
            if (selectedCount == 0) {
                printf("취소할 액수가 없습니다. 취소 작업을 중단하려면 0을 입력하세요\n");
                continue;
            }
            executeCancelPayments(unit, selectedPayments, selectedCount);
            break;
        }

        // 입력된 금액이 유효한지 확인
        bool valid = false;
        for (int i = 0; i < uniquePaymentCount; i++) {
            if (payments[i].amount == amount && payments[i].count > 0) {
                valid = true;
                selectedPayments[selectedCount++] = amount;
                payments[i].count--;  // 선택된 금액의 남은 개수 감소
                break;
            }
        }
        
        if (!valid) {
            printf("해당하는 부분 결제 금액이 없습니다.\n");
        }
    }

    free(unit->partialPayments);
    free(unit);
}


// 7.12 메인 메뉴 프롬프트
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
        printf("7. 테이블 증감\n");
        printf("8. 테이블 이동\n");
        printf("9. 결제 취소\n");
        printf("10. 종료\n");
        s = inputInt("메뉴 선택: ", false, false);
        if (s > 10) {
            printf("1~10 사이의 값을 입력해주세요.\n");
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
            // 7.11 테이블 증감 프롬프트
            adjustTables();
            break;
        case 8:
            // 7.12 테이블 이동 프롬프트
            moveTable();
            break;
        case 9:
            // 7.13 결제 취소 프롬프트
            cancelPayment();
            break;
        case 10:
            // 7.14 메인 메뉴 프롬프트의 종료 기능
            exitProgram();
            break;
        default:
            break;
        }
    }

    return 0;
}