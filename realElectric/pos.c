#include <stdio.h>

#define MAX_MENU_ITEMS 100
#define MAX_NAME_LENGTH 50  // 음식명의 최대 길이
#define MAX_TABLES 10

// 메뉴 항목
typedef struct {
    char name[MAX_NAME_LENGTH];
    int price;
} MenuItem;

// 주문된 항목
typedef struct {
    int menuIndex;  // 음식의 인덱스
    int count;       // 해당 음식 주문 수
} OrderItem;

// 테이블당 주문된 음식의 가짓수와 총액
typedef struct {
    OrderItem orderItems[MAX_MENU_ITEMS];
    int foodType;
    int totalPrice;
} Receipt;

// 가게에서 팔고 있는 전체 음식과 가짓수
MenuItem menuItems[MAX_MENU_ITEMS];  
int menuType = 0;

Receipt receipts[MAX_TABLES];

// 모든 테이블의 주문을 초기화하는 함수
void resetOrders() {
}

// 신메뉴를 추가하는 함수
void getMenu() {
}

// 음식을 단종시키는 함수
void removeMenu() {
}

// 주문 생성 함수
void getOrder() {
}

// 테이블 주문 조회 함수
void showReceipt() {
}

// 결제 처리 함수
void getPay() {
}

int main() {
    // 메인 메뉴 출력
    // 사용자 선택에 따른 함수 호출
    return 0;
}
