#include "pos.h"

void executeCancelPayments(PaymentUnit* unit, int* selectedPayments, int selectedCount) {
    for (int t = 0; t < unit->tableCount; t++) {
        int tableNum = unit->tables[t];
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNum);
        
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

void listTablesWithPartialPayments(int* tables, int* count, const char* message) {
    *count = 0;
    printf("%s: { ", message);
    bool first = true;

    // 먼저 부분 결제가 있는 테이블들을 찾아서 결제 단위 정보를 수집
    PaymentUnit* units[MAX_TABLE_NUMBER] = {NULL};
    int unitCount = 0;

    for (int i = 1; i <= MAX_TABLE_NUMBER; i++) {
        if (!isTableExist(i)) continue;

        // 이미 다른 결제 단위에 포함된 테이블인지 확인
        bool alreadyIncluded = false;
        for (int j = 0; j < unitCount; j++) {
            for (int k = 0; k < units[j]->tableCount; k++) {
                if (units[j]->tables[k] == i) {
                    alreadyIncluded = true;
                    break;
                }
            }
            if (alreadyIncluded) break;
        }
        if (alreadyIncluded) continue;

        // 테이블의 결제 단위 정보 확인
        PaymentUnit* unit = getPaymentUnit(i);
        bool hasPartialPayment = false;

        // 결제 단위 내 모든 테이블의 부분 결제 확인
        if (unit->tableCount > 0) {
            for (int j = 0; j < unit->tableCount; j++) {
                int tableNum = unit->tables[j];
                char tablePath[256];
                snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNum);
                FILE* file = fopen(tablePath, "r");
                if (!file) continue;

                char line[256];
                while (fgets(line, sizeof(line), file)) {
                    if (line[0] == '#' && line[1] == '#') {
                        hasPartialPayment = true;
                        break;
                    }
                }
                fclose(file);
                if (hasPartialPayment) break;
            }

            if (hasPartialPayment) {
                // 결제 단위의 모든 테이블 추가
                for (int j = 0; j < unit->tableCount; j++) {
                    tables[(*count)++] = unit->tables[j];
                }
                units[unitCount++] = unit;
                if (!first) printf(", ");
                first = false;

                // 결제 단위의 모든 테이블 출력
                printf("{");
                for (int j = 0; j < unit->tableCount; j++) {
                    printf("%d", unit->tables[j]);
                    if (j < unit->tableCount - 1) printf(", ");
                }
                printf("}");
            } else {
                free(unit->partialPayments);
                free(unit);
            }
        } else {
            char tablePath[256];
            snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, i);
            FILE* file = fopen(tablePath, "r");
            if (!file) continue;

            char line[256];
            while (fgets(line, sizeof(line), file)) {
                if (line[0] == '#' && line[1] == '#') {
                    hasPartialPayment = true;
                    break;
                }
            }
            fclose(file);

            if (hasPartialPayment) {
                tables[(*count)++] = i;
                if (!first) printf(", ");
                printf("%d", i);
                first = false;
            }
            free(unit->partialPayments);
            free(unit);
        }
    }
    printf(" }\n\n");

    // 메모리 해제
    for (int i = 0; i < unitCount; i++) {
        free(units[i]->partialPayments);
        free(units[i]);
    }
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
