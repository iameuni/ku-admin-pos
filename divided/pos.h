#ifndef POS_H
#define POS_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h> 

// 매크로 정의
#define MAX_INPUT_NUM 100
#define MAX_TABLE_NUMBER 10
#define MAX_TABLE_ADJUST 9 // 테이블 증감 최대 절댓값
#define FILE_PATH "./foodlist.txt" // 파일 경로 설정
#define TABLE_FILE_PATH "./table" //테이블 폴더 경로 설정
#define MAX_INT 2147483647

//////////////////// 구조체 선언 ////////////////////
typedef struct OrderItem {
    int itemID;
    int quantity;
    struct OrderItem* next;
} OrderItem;

// 기타 함수 선언
void exitProgram();
int getLastSecondNumber(FILE* file);
OrderItem* addOrderItem(OrderItem* head, int itemID);
void freeOrderItems(OrderItem* head);
int deleteLines(const char* filePath, int startLine, int endLine);
int inputInt(const char* prompt, bool allowZero, bool allowMinus);
bool isTableExist(int tableNumber);
int getLastTableNumber();
bool hasOrders(int tableNumber);
void listTablesWithOrders(int* tablesWithOrders, int* orderCount, const char* message);
int getCurrentTableCount();
void changeTable(int prevTableNum, int newTableNum);

// 메인 기능 함수 선언
bool checkDataIntegrity();
char* inputFoodName();
int inputFoodNumber();
int inputPrice();
int inputTableNumber(bool paymentMode);
int inputQuantity();
int inputPaymentAmount(int remainingBalance);
void printFoodList();
void addToFoodList();
void removeFoodItem();
void createOrder();
void printOrder();
void makePayment();
void adjustTables();
int printMain();

// 2차 요구 사항
// 구조체 선언에 추가
typedef struct {
    int tables[MAX_TABLE_NUMBER];
    int tableCount;
    int* partialPayments;
    int paymentCount;
} PaymentUnit;

// 함수 선언에 추가
PaymentUnit* getPaymentUnit(int tableNumber);
void moveTable();
void cancelPayment();
void executeCancelPayments(PaymentUnit* unit, int* selectedPayments, int selectedCount);
bool isValidDestinationTable(int destTable, int* destTables, int destCount, PaymentUnit* sourceUnit);
void executeTableMove(PaymentUnit* sourceUnit, int* destTables, int destCount);
void listTablesWithPartialPayments(int* tables, int* count, const char* message);
bool isPaymentSelected(int payment, int* selectedPayments, int selectedCount);
bool isValidPartialPayment(int amount, PaymentUnit* unit, int* selectedPayments, int selectedCount);

void updatePaymentRecord(int tableNumber, int paymentAmount);
void updatePaymentUnit(int primaryTable, int* unitTables, int unitCount);

typedef struct {
    int tableNumbers[MAX_TABLE_NUMBER];
    int tableCount;
} PaymentContext;

#endif // POS_H