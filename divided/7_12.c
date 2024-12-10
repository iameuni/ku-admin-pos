#include "pos.h"

// 7.12 테이블 이동 프롬프트
int inputSourceTableNumber() {
    while (1) {
        int tableNumber = inputInt("이동시킬 테이블을 입력하세요: ", false, false);
        
        // inputInt의 오류값 처리
        if (tableNumber == -1) {  // 엔터 입력
            printf("테이블 번호를 입력해주세요.\n");
            continue;
        }
        if (tableNumber <= -MAX_INT) {  // 그 외 입력 오류
            continue;  // inputInt 함수가 이미 오류 메시지를 출력했으므로 계속 진행
        }

        // 테이블 번호 유효성 검사
        if (tableNumber > MAX_TABLE_NUMBER || !isTableExist(tableNumber)) {
            printf("오류: 1~%d 사이의 존재하는 테이블 번호를 입력하세요.\n", MAX_TABLE_NUMBER);
            continue;
        }

        // 주문 내역이 있는지 확인
        if (!hasOrders(tableNumber)) {
            printf("\n주문 내역이 없는 테이블입니다.\n\n");
            continue;
        }

        return tableNumber;  // 유효한 테이블 번호만 반환
    }
}

void moveTable() {
    int tablesWithOrders[MAX_TABLE_NUMBER];
    int orderCount = 0;
    listTablesWithOrders(tablesWithOrders, &orderCount, "\n주문이 있는 테이블 번호");

    // 출발 테이블 선택
    int sourceTable = inputSourceTableNumber();
    if (sourceTable < 0) return;

    // 결제 단위 확인
    PaymentUnit* sourceUnit = getPaymentUnit(sourceTable);
    if (sourceUnit->tableCount == 0) {
        sourceUnit->tables[0] = sourceTable;
        sourceUnit->tableCount = 1;
    }

    // 목적지 테이블들 선택
    int destTables[MAX_TABLE_NUMBER];
    int destCount = 0;
    
    while (1) {
        // 현재 상태 출력
        printf("{");
        for (int i = 0; i < sourceUnit->tableCount; i++) {
            printf("%d", sourceUnit->tables[i]);
            if (i < sourceUnit->tableCount - 1) printf(", ");
        }
        printf("}번 테이블을 이동할 테이블을 입력하세요{");
        
        // 지금까지 선택된 각 테이블에 대해 결제 단위 전체를 표시
        bool first = true;
        for (int i = 0; i < destCount; i++) {
            bool isSourceTable = false;
            // 출발 테이블의 결제 단위에 속한 테이블인지 확인
            for (int j = 0; j < sourceUnit->tableCount; j++) {
                if (sourceUnit->tables[j] == destTables[i]) {
                    isSourceTable = true;
                    break;
                }
            }
            
            if (isSourceTable) {
                // 출발 테이블 결제 단위에 속한 테이블은 단독으로 표시
                if (!first) printf(", ");
                printf("%d", destTables[i]);
                first = false;
            } else {
                // 다른 테이블은 결제 단위 전체 표시
                PaymentUnit* unit = getPaymentUnit(destTables[i]);
                if (unit->tableCount > 0) {
                    if (!first) printf(", ");
                    for (int j = 0; j < unit->tableCount; j++) {
                        printf("%d", unit->tables[j]);
                        if (j < unit->tableCount - 1) printf(", ");
                    }
                    first = false;
                } else {
                    if (!first) printf(", ");
                    printf("%d", destTables[i]);
                    first = false;
                }
                free(unit->partialPayments);
                free(unit);
            }
        }
        printf("}: ");

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
            continue;  // 반복문 처음으로 돌아가서 프롬프트 다시 출력
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
        if (alreadySelected) continue;  // 반복문 처음으로 돌아가서 프롬프트 다시 출력

        // 출발 테이블과 동일한 테이블 허용
        bool isSourceTable = false;
        for (int i = 0; i < sourceUnit->tableCount; i++) {
            if (sourceUnit->tables[i] == destTable) {
                isSourceTable = true;
                break;
            }
        }

        // 선택된 테이블 추가
        destTables[destCount++] = destTable;
    }

    // 테이블 이동 실행
    if (destCount > 0) {
        executeTableMove(sourceUnit, destTables, destCount);
    }

    free(sourceUnit->partialPayments);
    free(sourceUnit);
}