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
    // 1. 출발 테이블의 내역을 임시 파일에 순서대로 백업
    FILE* tempFile = fopen("temp.txt", "w");
    if (!tempFile) return;

    // 1.1. 모든 테이블의 주문 내역만 먼저 수집
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        char sourcePath[256];
        snprintf(sourcePath, sizeof(sourcePath), "%s/%d.txt", TABLE_FILE_PATH, sourceUnit->tables[i]);
        
        FILE* sourceFile = fopen(sourcePath, "r");
        if (sourceFile) {
            char line[256];
            while (fgets(line, sizeof(line), sourceFile)) {
                if (line[0] != '#') {  // 주문 내역만 저장
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(sourceFile);
        }
    }

    // 1.2. 모든 테이블의 부분 결제 내역 수집
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        char sourcePath[256];
        snprintf(sourcePath, sizeof(sourcePath), "%s/%d.txt", TABLE_FILE_PATH, sourceUnit->tables[i]);
        
        FILE* sourceFile = fopen(sourcePath, "r");
        if (sourceFile) {
            char line[256];
            while (fgets(line, sizeof(line), sourceFile)) {
                if (line[0] == '#' && line[1] == '#') {  // 부분 결제 내역만 저장
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(sourceFile);
        }
    }

    fclose(tempFile);

    // 2. 출발 테이블들을 모두 비움
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, sourceUnit->tables[i]);
        FILE* file = fopen(tablePath, "w");
        if (file) fclose(file);
    }

    // 3. 입력된 목적지 테이블들의 결제 단위들을 모두 찾아서 하나로 합침
    int newUnit[MAX_TABLE_NUMBER];
    int newUnitCount = 0;

    // 3.1 먼저 입력된 목적지 테이블들을 newUnit에 추가
    for (int i = 0; i < destCount; i++) {
        bool exists = false;
        for (int j = 0; j < newUnitCount; j++) {
            if (newUnit[j] == destTables[i]) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            newUnit[newUnitCount++] = destTables[i];
        }
    }

    // 3.2 각 목적지 테이블의 기존 결제 단위 테이블들도 newUnit에 추가
    for (int i = 0; i < destCount; i++) {
        PaymentUnit* destUnit = getPaymentUnit(destTables[i]);
        if (destUnit->tableCount > 0) {
            for (int j = 0; j < destUnit->tableCount; j++) {
                bool exists = false;
                for (int k = 0; k < newUnitCount; k++) {
                    if (newUnit[k] == destUnit->tables[j]) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    newUnit[newUnitCount++] = destUnit->tables[j];
                }
            }
        }
        free(destUnit->partialPayments);
        free(destUnit);
    }

    // 4. 모든 결제 단위 테이블 파일 업데이트
    for (int i = 0; i < newUnitCount; i++) {
        char tablePath[256];
        snprintf(tablePath, sizeof(tablePath), "%s/%d.txt", TABLE_FILE_PATH, newUnit[i]);
        
        FILE* tempFile = fopen("temp_update.txt", "w");
        if (!tempFile) continue;

        // 4.1. 기존 주문 내역 복사
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

        // 4.2. 첫 번째 목적지 테이블이면 백업한 주문 내역도 추가
        if (newUnit[i] == destTables[0]) {
            FILE* backupFile = fopen("temp.txt", "r");
            if (backupFile) {
                char line[256];
                while (fgets(line, sizeof(line), backupFile)) {
                    if (line[0] != '#') {  // 주문 내역만 복사
                        fprintf(tempFile, "%s", line);
                    }
                }
                fclose(backupFile);
            }
        }

        // 4.3. 새로운 결제 단위 정보 추가
        for (int j = 0; j < newUnitCount; j++) {
            fprintf(tempFile, "#%d\n", newUnit[j]);
        }

        // 4.4. 기존 부분 결제 내역 복사
        tableFile = fopen(tablePath, "r");
        if (tableFile) {
            char line[256];
            while (fgets(line, sizeof(line), tableFile)) {
                if (line[0] == '#' && line[1] == '#') {  // 부분 결제 내역만 복사
                    fprintf(tempFile, "%s", line);
                }
            }
            fclose(tableFile);
        }

        // 4.5. 첫 번째 목적지 테이블이면 백업한 부분 결제 내역도 추가
        if (newUnit[i] == destTables[0]) {
            FILE* backupFile = fopen("temp.txt", "r");
            if (backupFile) {
                char line[256];
                while (fgets(line, sizeof(line), backupFile)) {
                    if (line[0] == '#' && line[1] == '#') {  // 부분 결제 내역만 복사
                        fprintf(tempFile, "%s", line);
                    }
                }
                fclose(backupFile);
            }
        }

        fclose(tempFile);

        // 4.6. 임시 파일로 원본 파일 교체
        remove(tablePath);
        rename("temp_update.txt", tablePath);
    }

    remove("temp.txt");

    // 5. 결과 메시지 출력
    printf("\n");
    for (int i = 0; i < sourceUnit->tableCount; i++) {
        printf("%d", sourceUnit->tables[i]);
        if (i < sourceUnit->tableCount - 1) printf(", ");
    }
    printf("번 테이블이 ");
    // 지금까지 선택된 각 테이블에 대해 결제 단위 전체를 표시
    bool first = true;
        PaymentUnit* unit = getPaymentUnit(destTables[0]);
        if (unit->tableCount > 0) {
            if (!first) printf(", ");
            for (int j = 0; j < unit->tableCount; j++) {
                printf("%d", unit->tables[j]);
                if (j < unit->tableCount - 1) printf(", ");
            }
            first = false;
        }
        free(unit->partialPayments);
        free(unit);
    printf("번 테이블로 이동되었습니다.\n");
}