#include "pos.h"

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
    snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);
    
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
    int tablesWithOrders[MAX_TABLE_NUMBER];  
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n주문 내역이 있는 테이블 번호");

    int tableNumber = 0;
    while (1) {
        tableNumber = inputTableNumber(false);
        if (tableNumber < -1) continue;
        else break;
    }

    // 결제 단위 정보 가져오기
    PaymentUnit* unit = getPaymentUnit(tableNumber);
    if (unit->tableCount == 0) {
        // 결제 단위가 없는 경우 선택된 테이블만 처리
        unit->tables[0] = tableNumber;
        unit->tableCount = 1;
    }

    // 제목 출력 - 결제 단위에 속한 모든 테이블 번호 표시
    printf("\n[");
    for (int i = 0; i < unit->tableCount; i++) {
        printf("%d", unit->tables[i]);
        if (i < unit->tableCount - 1) printf(", ");
    }
    printf("]번 테이블 주문 조회\n\n");

    printf("%-20s %-10s %-10s\n", "메뉴", "수량", "금액");

    // 판매항목목록 파일 열기
    FILE* foodFile = fopen(FILE_PATH, "r");
    if (foodFile == NULL) {
        printf("판매 항목 파일을 열 수 없습니다.\n");
        free(unit->partialPayments);
        free(unit);
        return;
    }

    // 모든 주문 정보를 저장할 연결 리스트
    OrderItem* orderList = NULL;
    int totalAmount = 0;
    int totalPartialPayments = 0;

    // 결제 단위에 속한 모든 테이블의 주문 내역 수집
    for (int t = 0; t < unit->tableCount; t++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unit->tables[t]);
        FILE* tableFile = fopen(tablePath, "r");
        if (!tableFile) continue;

        char line[256];
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] != '#') {
                int itemID;
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
        fclose(tableFile);
    }

    // 주문 정보 출력
    int foundItems = 0;
    OrderItem* current = orderList;
    while (current != NULL) {
        rewind(foodFile);
        int firstNum, secondNum, price;
        char foodName[50];
        
        while (fscanf(foodFile, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price) == 4) {
            if (firstNum == 0 && secondNum == current->itemID) {
                int quantity = current->quantity;
                int itemTotal = quantity * price;
                printf("%-20s %-10d %-10d\n", foodName, quantity, itemTotal);
                totalAmount += itemTotal;
                foundItems++;
                break;
            }
        }
        current = current->next;
    }

    if (foundItems == 0) {
        printf("주문한 판매 항목이 없습니다.\n");
    } else {
        printf("\n합계: %d원\n", totalAmount);
        if (totalPartialPayments > 0) {
            printf("부분 결제: ");
            bool first = true;
            for (int t = 0; t < unit->tableCount; t++) {
                char tablePath[256];
                snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unit->tables[t]);
                FILE* tableFile = fopen(tablePath, "r");
                if (!tableFile) continue;

                char line[256];
                while (fgets(line, sizeof(line), tableFile)) {
                    if (line[0] == '#' && line[1] == '#') {
                        int payment;
                        if (sscanf(line + 2, "%d", &payment) == 1) {
                            if (!first) printf(", ");
                            printf("%d원", payment);
                            first = false;
                        }
                    }
                }
                fclose(tableFile);
            }
            printf("\n");
        }
    }

    // 메모리 해제
    freeOrderItems(orderList);
    fclose(foodFile);
    free(unit->partialPayments);
    free(unit);
}
