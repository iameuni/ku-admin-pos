#include "pos.h"

// 7.4.1 테이블 번호 입력
int inputTableNumber(bool paymentMode) {
    int tableNumber;
    int maxTableNumber = getLastTableNumber(); // 최대 테이블 번호를 가져옴

    // 사용자에게 입력 요청
    if (!paymentMode) { // 일반 모드
        printf("테이블 번호를 입력하세요 (1~%d): ", maxTableNumber);
        tableNumber = inputInt(NULL, false, false); // 0 입력 금지
    }
    else {
        tableNumber = inputInt(NULL, true, false); // 0 입력 허용
    }

    // 결제 모드 추가 조건 검사
    if (paymentMode) {
        // 엔터나 공백 입력 시 -1 반환
        if (tableNumber == -1) {
            return -1;
        }
        if (tableNumber == 0) {
            return 0;
        }
        if (tableNumber == -28) return -2;
        // 테이블 번호가 0 또는 1~maxTableNumber 범위에 있고, 존재하는지 확인
        if (tableNumber > maxTableNumber || !isTableExist(tableNumber)) {
            printf("오류: 0과 엔터, 1~%d 사이의 존재하는 테이블 번호만 입력 가능합니다.\n", maxTableNumber);
            return -2; // 유효하지 않은 번호 시도 시 -2 반환
        }
    }
    else {
        // 일반 모드에서 유효 범위와 테이블이 존재하는지 확인 (1~maxTableNumber)
        if (tableNumber > maxTableNumber || !isTableExist(tableNumber)) {
            printf("오류: 1~%d 사이의 존재하는 테이블 번호를 입력하세요.\n", maxTableNumber);
            return -2; // 유효하지 않은 번호 시도 시 -2 반환
        }
    }

    return tableNumber; // 유효한 테이블 번호 반환
}

// 7.4.2 수량 입력
int inputQuantity() {
    int quantity;
    while (1) {
        quantity = inputInt("수량을 입력하세요: ", false, false);
        if (quantity < 1 || quantity >99) {
            printf("오류: 1~99사이의 수량을 입력하세요.\n");
        }
        else {
            return quantity;
        }
    }
}

// 7.4.3 결제 금액 입력
int inputPaymentAmount(int remainingBalance) {
    while (1)
    {
        printf("결제할 금액을 입력하세요 [%d원]: ", remainingBalance);
        int paymentAmount = inputInt(NULL, true, false); // 정수 입력 함수 사용

        // 입력이 비어있다면 전체 금액 결제
        if (paymentAmount == -1) { // 엔터키만 입력된 경우
            printf("전체 금액이 결제 완료되었습니다.\n");
            remainingBalance = 0; // 전액 결제 완료
            return remainingBalance;
        }

        // 유효성 검사
        if (paymentAmount < -1) {
            continue;
        }
        else if (paymentAmount == 0) { // 0 입력 시 결제 중단
            printf("결제 중단.\n");
            return -1;
        }
        else if (paymentAmount > remainingBalance) {
            printf("오류: 결제할 금액보다 큽니다.\n");
            continue;
        }

        remainingBalance -= paymentAmount;
        if (remainingBalance == 0) {
            printf("전체 금액이 결제 완료되었습니다.\n");
            remainingBalance = 0; // 전액 결제 완료
            return remainingBalance;
        }
        else {
            printf("%d원 분할 결제 완료되었습니다. 남은 금액: %d원\n", paymentAmount, remainingBalance);
            return remainingBalance;
        }
    }
}