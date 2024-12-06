#include "pos.h"

PaymentUnit* getPaymentUnit(int tableNumber) {
    PaymentUnit* unit = malloc(sizeof(PaymentUnit));
    unit->tableCount = 0;
    unit->paymentCount = 0;
    unit->partialPayments = NULL;
    
    char tablePath[256];
    snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);
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
// updatePaymentUnit 함수 수정
void updatePaymentUnit(int primaryTable, int* unitTables, int unitCount) {
    // 결제 단위에 속한 모든 테이블에 대해 처리
    for (int i = 0; i < unitCount; i++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, unitTables[i]);
        
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
    snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNumber);
    
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