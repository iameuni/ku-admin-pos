#include "pos.h"

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
    // 1. 모든 목적지 테이블의 결제 단위 정보 수집
    int allDestTables[MAX_TABLE_NUMBER];
    int totalDestCount = 0;
    bool processed[MAX_TABLE_NUMBER + 1] = {false};

    for (int i = 0; i < destCount; i++) {
        if (processed[destTables[i]]) continue;
        
        PaymentUnit* destUnit = getPaymentUnit(destTables[i]);
        if (destUnit->tableCount > 0) {
            // 결제 단위에 속한 모든 테이블 추가
            for (int j = 0; j < destUnit->tableCount; j++) {
                if (!processed[destUnit->tables[j]]) {
                    allDestTables[totalDestCount++] = destUnit->tables[j];
                    processed[destUnit->tables[j]] = true;
                }
            }
        } else {
            // 단독 테이블인 경우
            allDestTables[totalDestCount++] = destTables[i];
            processed[destTables[i]] = true;
        }
        free(destUnit->partialPayments);
        free(destUnit);
    }

    // 2. 테이블 데이터 저장 구조체
    typedef struct {
        char orders[5000];      // 주문 내역
        char payments[5000];    // 부분 결제 내역
        bool hasContent;
    } TableContent;
    
    TableContent contents[MAX_TABLE_NUMBER + 1] = {0};
    
    // 2-1. 출발 테이블들의 내용 저장
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        int tableNum = sourceUnit->tables[i];
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNum);
        
        FILE* file = fopen(tablePath, "r");
        if (file) {
            char line[256];
            contents[tableNum].orders[0] = '\0';
            contents[tableNum].payments[0] = '\0';
            contents[tableNum].hasContent = true;
            
            while (fgets(line, sizeof(line), file)) {
                if (line[0] != '#') {
                    strcat(contents[tableNum].orders, line);
                } else if (line[0] == '#' && line[1] == '#') {
                    strcat(contents[tableNum].payments, line);
                }
            }
            fclose(file);
        }
    }

    // 2-2. 목적지 테이블들의 내용 저장
    for (int i = 0; i < totalDestCount; i++) {
        int tableNum = allDestTables[i];
        if (contents[tableNum].hasContent) continue;
        
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNum);
        
        FILE* file = fopen(tablePath, "r");
        if (file) {
            char line[256];
            contents[tableNum].orders[0] = '\0';
            contents[tableNum].payments[0] = '\0';
            contents[tableNum].hasContent = true;
            
            while (fgets(line, sizeof(line), file)) {
                if (line[0] != '#') {
                    strcat(contents[tableNum].orders, line);
                } else if (line[0] == '#' && line[1] == '#') {
                    strcat(contents[tableNum].payments, line);
                }
            }
            fclose(file);
        }
    }
    
    // 3. 모든 목적지 테이블 업데이트
    for (int i = 0; i < totalDestCount; i++) {
        int tableNum = allDestTables[i];
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNum);
        
        FILE* file = fopen(tablePath, "w");
        if (!file) continue;

        // 3-1. 기존 주문 내역 유지
        if (contents[tableNum].hasContent) {
            fprintf(file, "%s", contents[tableNum].orders);
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

        // 3-2. 새로운 결제 단위 정보 추가
        for (int j = 0; j < totalDestCount; j++) {
            fprintf(file, "#%d\n", allDestTables[j]);
        }

        // 3-3. 부분 결제 내역 처리
        if (contents[tableNum].hasContent) {
            fprintf(file, "%s", contents[tableNum].payments);
        }
        
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
    
    // 4. 출발 테이블 비우기 (목적지에 포함되지 않은 것만)
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        int tableNum = sourceUnit->tables[i];
        bool isDestination = false;
        for (int j = 0; j < totalDestCount; j++) {
            if (tableNum == allDestTables[j]) {
                isDestination = true;
                break;
            }
        }
        
        if (!isDestination) {
            char tablePath[256];
            snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, tableNum);
            remove(tablePath);  // 파일 완전히 삭제
            FILE* file = fopen(tablePath, "w");  // 빈 파일로 새로 생성
            if (file) fclose(file);
        }
    }
    
    // 5. 결과 출력
    printf("\n");
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        printf("%d", sourceUnit->tables[i]);
        if (i < sourceUnit->tableCount - 1) printf(", ");
    }
    printf("번 테이블이 ");
    for (int i = 0; i < totalDestCount; i++) {
        printf("%d", allDestTables[i]);
        if (i < totalDestCount - 1) printf(", ");
    }
    printf("번 테이블로 이동되었습니다.\n");
}