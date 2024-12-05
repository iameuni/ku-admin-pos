#include "pos.h"

PaymentUnit* getPaymentUnit(int tableNumber) {
    PaymentUnit* unit = malloc(sizeof(PaymentUnit));
    unit->tableCount = 0;
    unit->paymentCount = 0;
    unit->partialPayments = NULL;
    
    char tablePath[256];
    snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);
    FILE* file = fopen(tablePath, "r");
    if (!file) return unit;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') {
            if (line[1] != '#') { // 결제 단위 표시
                int unitTable;
                sscanf(line + 1, "%d", &unitTable);
                unit->tables[unit->tableCount++] = unitTable;
            } else { // 부분 결제 내역
                int payment;
                sscanf(line + 2, "%d", &payment);
                unit->partialPayments = realloc(unit->partialPayments, 
                    (unit->paymentCount + 1) * sizeof(int));
                unit->partialPayments[unit->paymentCount++] = payment;
            }
        }
    }
    fclose(file);
    return unit;
}

void executeCancelPayments(PaymentUnit* unit, int* selectedPayments, int selectedCount) {
    for (int t = 0; t < unit->tableCount; t++) {
        int tableNum = unit->tables[t];
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNum);
        
        FILE* tempFile = fopen("temp.txt", "w");
        FILE* tableFile = fopen(tablePath, "r");
        
        if (!tempFile || !tableFile) continue;

        char line[256];
        // 선택된 금액별로 취소 수행 여부 추적
        bool* cancelled = (bool*)calloc(selectedCount, sizeof(bool));
        
        // 먼저 주문 내역과 결제 단위 정보 복사
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] != '#') {
                fprintf(tempFile, "%s", line);
            } else if (line[0] == '#' && line[1] != '#') {
                fprintf(tempFile, "%s", line);
            }
        }

        // 부분 결제 내역 처리
        rewind(tableFile);
        while (fgets(line, sizeof(line), tableFile)) {
            if (line[0] == '#' && line[1] == '#') {
                int payment;
                sscanf(line + 2, "%d", &payment);
                
                bool shouldKeep = true;
                // 아직 취소되지 않은 선택된 금액과 일치하는지 확인
                for (int i = 0; i < selectedCount; i++) {
                    if (!cancelled[i] && payment == selectedPayments[i]) {
                        cancelled[i] = true;
                        shouldKeep = false;
                        break;
                    }
                }
                
                if (shouldKeep) {
                    fprintf(tempFile, "%s", line);
                }
            }
        }

        free(cancelled);
        fclose(tableFile);
        fclose(tempFile);
        remove(tablePath);
        rename("temp.txt", tablePath);
    }

    printf("선택한 부분결제가 취소되었습니다.\n");
}


bool isValidDestinationTable(int destTable, int* destTables, int destCount, PaymentUnit* sourceUnit) {
    // 존재하지 않는 테이블 번호 체크
    if (!isTableExist(destTable)) {
        printf("존재하지 않는 테이블 번호입니다.\n");
        return false;
    }

    // 이미 선택된 테이블인지 체크
    for (int i = 0; i < destCount; i++) {
        if (destTables[i] == destTable) {
            printf("이미 선택된 테이블입니다.\n");
            return false;
        }
    }

    // 출발 테이블과 동일한 테이블인지 체크
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        if (sourceUnit->tables[i] == destTable) {
            return true; // 출발 테이블의 결제 단위에 속한 테이블은 허용
        }
    }

    return true;
}


// 테이블이 목적지 테이블 목록에 있는지 확인하는 헬퍼 함수
bool isInDestTables(int tableNum, int* destTables, int destCount) {
    for (int i = 0; i < destCount; i++) {
        if (destTables[i] == tableNum) {
            return true;
        }
    }
    return false;
}

void executeTableMove(PaymentUnit* sourceUnit, int* destTables, int destCount) {
    // 1. 테이블 데이터 저장 구조체
    typedef struct {
        char orders[5000];      // 주문 내역
        char payments[5000];    // 부분 결제 내역
        char unitInfo[1000];    // 결제 단위 정보
        bool hasContent;
    } TableContent;
    
    TableContent contents[MAX_TABLE_NUMBER + 1] = {0};
    
    // 1-1. 출발 테이블들의 내용 저장
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        int tableNum = sourceUnit->tables[i];
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNum);
        
        FILE* file = fopen(tablePath, "r");
        if (file) {
            char line[256];
            contents[tableNum].orders[0] = '\0';
            contents[tableNum].payments[0] = '\0';
            contents[tableNum].unitInfo[0] = '\0';
            contents[tableNum].hasContent = true;
            
            while (fgets(line, sizeof(line), file)) {
                if (line[0] != '#') {
                    strcat(contents[tableNum].orders, line);
                } else if (line[0] == '#' && line[1] == '#') {
                    strcat(contents[tableNum].payments, line);
                } else {
                    strcat(contents[tableNum].unitInfo, line);
                }
            }
            fclose(file);
        }
    }

    // 1-2. 목적지 테이블들의 내용 저장
    for (int i = 0; i < destCount; i++) {
        int tableNum = destTables[i];
        if (contents[tableNum].hasContent) continue;  // 이미 저장된 경우 스킵
        
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNum);
        
        FILE* file = fopen(tablePath, "r");
        if (file) {
            char line[256];
            contents[tableNum].orders[0] = '\0';
            contents[tableNum].payments[0] = '\0';
            contents[tableNum].unitInfo[0] = '\0';
            contents[tableNum].hasContent = true;
            
            while (fgets(line, sizeof(line), file)) {
                if (line[0] != '#') {
                    strcat(contents[tableNum].orders, line);
                } else if (line[0] == '#' && line[1] == '#') {
                    strcat(contents[tableNum].payments, line);
                } else {
                    strcat(contents[tableNum].unitInfo, line);
                }
            }
            fclose(file);
        }
    }
    
    // 2. 목적지 테이블들 업데이트
    for (int i = 0; i < destCount; i++) {
        int tableNum = destTables[i];
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNum);
        
        FILE* file = fopen(tablePath, "w");
        if (!file) continue;

        // 2-1. 주문 내역
        if (contents[tableNum].hasContent) {
            fprintf(file, "%s", contents[tableNum].orders);  // 기존 주문 내역
        }
        
        // 첫 번째 목적지 테이블인 경우만 출발 테이블의 주문 내역 추가
        if (i == 0) {
            for (int j = 0; j < sourceUnit->tableCount; j++) {
                int sourceNum = sourceUnit->tables[j];
                if (sourceNum != tableNum && contents[sourceNum].hasContent) {
                    fprintf(file, "%s", contents[sourceNum].orders);
                }
            }
        }

        // 2-2. 결제 단위 정보 처리
        if (i == 0) {
            // 첫 번째 목적지 테이블: 기존 결제 단위 유지하고 나머지 목적지 테이블 추가
            if (contents[tableNum].hasContent) {
                fprintf(file, "%s", contents[tableNum].unitInfo);
            }
            // 다른 목적지 테이블 번호 중 없는 것만 추가
            for (int j = 0; j < destCount; j++) {
                char checkLine[256];
                snprintf(checkLine, sizeof(checkLine), "#%d\n", destTables[j]);
                if (strstr(contents[tableNum].unitInfo, checkLine) == NULL) {
                    fprintf(file, "%s", checkLine);
                }
            }
        } else {
            // 나머지 목적지 테이블: 기존 결제 단위 유지하고 첫 번째 목적지 테이블 번호만 추가
            if (contents[tableNum].hasContent) {
                fprintf(file, "%s", contents[tableNum].unitInfo);
            }
            // destTables[0]이 없으면 추가
            char checkLine[256];
            snprintf(checkLine, sizeof(checkLine), "#%d\n", destTables[0]);
            if (strstr(contents[tableNum].unitInfo, checkLine) == NULL) {
                fprintf(file, "%s", checkLine);
            }
        }

        // 2-3. 부분 결제 내역
        if (contents[tableNum].hasContent) {
            fprintf(file, "%s", contents[tableNum].payments);  // 기존 부분 결제 내역
        }
        // 첫 번째 목적지 테이블인 경우만 출발 테이블의 부분 결제 내역 추가
        if (i == 0) {
            for (int j = 0; j < sourceUnit->tableCount; j++) {
                int sourceNum = sourceUnit->tables[j];
                if (sourceNum != tableNum && contents[sourceNum].hasContent) {
                    fprintf(file, "%s", contents[sourceNum].payments);
                }
            }
        }
        
        fclose(file);
    }
    
    // 3. 출발 테이블 비우기 (목적지에 포함되지 않은 것만)
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        int tableNum = sourceUnit->tables[i];
        bool isDestination = false;
        for (int j = 0; j < destCount; j++) {
            if (tableNum == destTables[j]) {
                isDestination = true;
                break;
            }
        }
        
        if (!isDestination) {
            char tablePath[256];
            snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNum);
            remove(tablePath);  // 파일 완전히 삭제
            FILE* file = fopen(tablePath, "w");  // 빈 파일로 새로 생성
            if (file) fclose(file);
        }
    }
    
    printf("\n");
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        printf("%d", sourceUnit->tables[i]);
        if (i < sourceUnit->tableCount - 1) printf(", ");
    }
    printf("번 테이블이 ");
    for (int i = 0; i < destCount; i++) {
        printf("%d", destTables[i]);
        if (i < destCount - 1) printf(", ");
    }
    printf("번 테이블로 이동되었습니다.\n");
}

void listTablesWithPartialPayments(int* tables, int* count, const char* message) {
    *count = 0;
    printf("%s: { ", message);
    bool first = true;

    for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
        if (!isTableExist(i)) continue;

        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, i);
        FILE* file = fopen(tablePath, "r");
        if (!file) continue;

        char line[256];
        bool hasPartialPayment = false;
        while (fgets(line, sizeof(line), file)) {
            if (line[0] == '#' && line[1] == '#') {
                hasPartialPayment = true;
                break;
            }
        }
        fclose(file);

        if (hasPartialPayment) {
            if (!first) printf(", ");
            printf("%d", i);
            tables[(*count)++] = i;
            first = false;
        }
    }
    printf(" }\n");
}

bool isPaymentSelected(int payment, int* selectedPayments, int selectedCount) {
    for (int i = 0; i < selectedCount; i++) {
        if (selectedPayments[i] == payment) return true;
    }
    return false;
}

bool isValidPartialPayment(int amount, PaymentUnit* unit, int* selectedPayments, int selectedCount) {
    // 이미 선택된 금액인지 확인
    if (isPaymentSelected(amount, selectedPayments, selectedCount)) {
        printf("이미 선택된 부분 결제 금액입니다.\n");
        return false;
    }

    // 유효한 부분 결제 금액인지 확인
    for (int i = 0; i < unit->paymentCount; i++) {
        if (unit->partialPayments[i] == amount) {
            return true;
        }
    }

    printf("해당하는 부분 결제 금액이 없습니다.\n");
    return false;
}

// updatePaymentUnit 함수 수정
void updatePaymentUnit(int primaryTable, int* unitTables, int unitCount) {
    // 결제 단위에 속한 모든 테이블에 대해 처리
    for (int i = 0; i < unitCount; i++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, unitTables[i]);
        
        // 임시 파일 생성
        FILE* tempFile = fopen("temp.txt", "w");
        if (!tempFile) continue;

        // 1. 먼저 주문 내역만 복사
        FILE* tableFile = fopen(tablePath, "r");
        if (tableFile) {
            char line[256];
            while (fgets(line, sizeof(line), tableFile)) {
                if (line[0] != '#') {  // 주문 내역만 복사
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(tableFile);
        }

        // 2. 결제 단위 정보 (#) 기록
        for (int j = 0; j < unitCount; j++) {
            fprintf(tempFile, "#%d\n", unitTables[j]);
        }

        // 3. 부분 결제 정보 (##) 복사
        tableFile = fopen(tablePath, "r");
        if (tableFile) {
            char line[256];
            while (fgets(line, sizeof(line), tableFile)) {
                if (line[0] == '#' && line[1] == '#') {
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(tableFile);
        }

        fclose(tempFile);
        remove(tablePath);
        rename("temp.txt", tablePath);
    }
}

// updatePaymentRecord 함수 수정
void updatePaymentRecord(int tableNumber, int paymentAmount) {
    char tablePath[256];
    snprintf(tablePath, sizeof(tablePath), "%s\\%d.txt", TABLE_FILE_PATH, tableNumber);
    
    FILE* tempFile = fopen("temp.txt", "w");
    if (!tempFile) return;
    
    // 1. 주문 내역 복사
    FILE* tableFile = fopen(tablePath, "r");
    if (!tableFile) {
        fclose(tempFile);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), tableFile)) {
        if (line[0] != '#') {  // 주문 내역만 복사
            fprintf(tempFile, "%s", line);
        }
    }

    // 2. 결제 단위 정보 (#) 복사
    rewind(tableFile);
    while (fgets(line, sizeof(line), tableFile)) {
        if (line[0] == '#' && line[1] != '#') {
            fprintf(tempFile, "%s", line);
        }
    }

    // 3. 기존 부분 결제 정보 (##) 복사
    rewind(tableFile);
    while (fgets(line, sizeof(line), tableFile)) {
        if (line[0] == '#' && line[1] == '#') {
            fprintf(tempFile, "%s", line);
        }
    }

    // 4. 새로운 부분 결제 정보 추가
    fprintf(tempFile, "##%d\n", paymentAmount);

    fclose(tableFile);
    fclose(tempFile);

    remove(tablePath);
    rename("temp.txt", tablePath);
}