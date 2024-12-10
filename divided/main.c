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
        snprintf(tableFileName, sizeof(tableFileName), "%s/%d.txt", TABLE_FILE_PATH, table);

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
                snprintf(unitFileName, sizeof(unitFileName), "%s/%d.txt", TABLE_FILE_PATH, unitTable);
                
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
                snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, tableToRemove);
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
                snprintf(tableFilePath, sizeof(tableFilePath), "%s/%d.txt", TABLE_FILE_PATH, newTableNumber);
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

// 7.14 메인 메뉴 프롬프트
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