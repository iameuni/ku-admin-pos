#include "pos.h"

PaymentContext currentContext;

// 7.10 결제 처리 프롬프트
void makePayment() {
    int tablesWithOrders[MAX_TABLE_NUMBER];
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n결제 가능한 테이블 번호");

    currentContext.tableCount = 0;  
    int primarySelectedTable = -1;

    // 결제할 테이블 선택 (기존 코드와 동일)
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
            printf("\n결제가 종료됩니다.\n");
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
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);

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
    int originalRemainingBalance = remainingBalance;  // 원래 남은 금액 저장

    printf("총 주문액: %d원\n", totalOrderAmount);
    printf("기존 부분 결제액: %d원\n", totalPartialPayments);
    printf("남은 결제액: %d원\n\n", remainingBalance);

    // 이번 세션의 부분 결제 기록을 임시로 저장
    typedef struct TempPayment {
        int amount;
        struct TempPayment* next;
    } TempPayment;
    
    TempPayment* tempPayments = NULL;

    while (true) {
        int payment = inputPaymentAmount(remainingBalance);
        
        if (payment == -1) {  // 결제 취소 (0 입력)
            // 임시 저장된 모든 부분 결제 내역 삭제
            TempPayment* current = tempPayments;
            tempPayments = NULL;
            while (current != NULL) {
                TempPayment* next = current->next;
                free(current);
                current = next;
            }
            printf("이번 결제 세션의 모든 결제가 취소되었습니다.\n");
            break;
        }
        
        if (payment == -2) {  // 부분 결제 (. 입력)
            // 임시 저장된 모든 부분 결제를 실제로 적용
            TempPayment* current = tempPayments;
            while (current != NULL) {
                updatePaymentRecord(primarySelectedTable, current->amount);
                current = current->next;
            }
            updatePaymentUnit(primarySelectedTable, currentContext.tableNumbers, currentContext.tableCount);
            
            // 메모리 해제
            current = tempPayments;
            while (current != NULL) {
                TempPayment* next = current->next;
                free(current);
                current = next;
            }
            break;
        }

        // 실제 결제가 발생한 경우
        if (remainingBalance != payment) {
            // 새로운 부분 결제를 임시 저장
            TempPayment* newPayment = (TempPayment*)malloc(sizeof(TempPayment));
            newPayment->amount = remainingBalance - payment;
            newPayment->next = tempPayments;
            tempPayments = newPayment;
        }
        remainingBalance = payment;

        if (remainingBalance == 0) {
            while (1) {
                printf("테이블을 비우시겠습니까?: ");
                char input[10];
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';  // 개행 문자 제거

                // 입력이 '.' 인 경우
                if (strcmp(input, ".") == 0) {
                    printf("테이블을 비우지 않습니다.\n");
                    break;
                }
                // 엔터만 입력한 경우
                else if (strlen(input) == 0) {
                    // 모든 선택된 테이블 비우기
                    for (int i = 0; i < currentContext.tableCount; i++) {
                        char tablePath[256];
                        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, currentContext.tableNumbers[i]);
                        FILE* file = fopen(tablePath, "w");
                        if (file) fclose(file);
                    }
                    printf("전액 결제된 테이블을 비웠습니다.\n");
                    break;
                }
                // 그 외의 입력
                else {
                    printf("오류: \".\" 혹은 엔터를 입력하시오\n");
                    continue;
                }
            }

            // 임시 저장된 모든 부분 결제를 실제로 적용
            TempPayment* current = tempPayments;
            while (current != NULL) {
                updatePaymentRecord(primarySelectedTable, current->amount);
                current = current->next;
            }

            // 메모리 해제
            current = tempPayments;
            while (current != NULL) {
                TempPayment* next = current->next;
                free(current);
                current = next;
            }
            break;
        }
    }
}
// 7.13 결제 취소 프롬프트
void cancelPayment() {
    int tablesWithPayments[MAX_TABLE_NUMBER];
    int paymentCount = 0;
    listTablesWithPartialPayments(tablesWithPayments, &paymentCount, 
        "\n부분결제 항목이 있는 테이블 번호");
    
    if (paymentCount == 0) {
        printf("부분 결제 항목이 있는 테이블이 없습니다.\n");
        return;
    }

    while (1) {
        int tableNumber = inputTableNumber(false);
        if (tableNumber < 0) continue;

        PaymentUnit* unit = getPaymentUnit(tableNumber);
        
        // 결제 단위 내의 모든 테이블에서 부분 결제 내역 확인
        bool hasPartialPayments = false;
        if (unit->tableCount > 0) {
            for (int i = 0; i < unit->tableCount; i++) {
                char tablePath[256];
                snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unit->tables[i]);
                FILE* file = fopen(tablePath, "r");
                if (!file) continue;

                char line[256];
                while (fgets(line, sizeof(line), file)) {
                    if (line[0] == '#' && line[1] == '#') {
                        hasPartialPayments = true;
                        break;
                    }
                }
                fclose(file);
                if (hasPartialPayments) break;
            }
        }

        if (!hasPartialPayments) {
            printf("취소할 부분결제 내역이 없습니다.\n");
            free(unit->partialPayments);
            free(unit);
            continue;
        }

        int selectedPayments[100] = {0};
        int selectedCount = 0;

        // 부분 결제 금액 정보를 저장할 구조체 배열
        typedef struct {
            int amount;
            int remainingCount;  // 아직 선택되지 않은 해당 금액의 개수
            int totalCount;      // 해당 금액의 전체 개수
        } PaymentInfo;
        
        PaymentInfo payments[100] = {0};
        int uniquePaymentCount = 0;

        // 모든 부분 결제 내역을 한 번에 수집
        for (int t = 0; t < unit->tableCount; t++) {
            char tablePath[256];
            snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unit->tables[t]);
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
                            payments[i].totalCount++;
                            payments[i].remainingCount++;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        payments[uniquePaymentCount].amount = amount;
                        payments[uniquePaymentCount].totalCount = 1;
                        payments[uniquePaymentCount].remainingCount = 1;
                        uniquePaymentCount++;
                    }
                }
            }
            fclose(tableFile);
        }

        while (1) {
            // 남은 부분 결제 항목 출력
            printf("결제 취소할 부분결제 항목을 고르시오 [");
            bool first = true;
            for (int i = 0; i < uniquePaymentCount; i++) {
                if (payments[i].remainingCount > 0) {
                    for (int j = 0; j < payments[i].remainingCount; j++) {
                        if (!first) printf(", ");
                        printf("%d", payments[i].amount);
                        first = false;
                    }
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
                if (payments[i].amount == amount && payments[i].remainingCount > 0) {
                    valid = true;
                    selectedPayments[selectedCount++] = amount;
                    payments[i].remainingCount--;
                    break;
                }
            }
            
            if (!valid) {
                printf("해당하는 부분 결제 금액이 없습니다.\n");
            }
        }

        free(unit->partialPayments);
        free(unit);
        return;
    }
}
