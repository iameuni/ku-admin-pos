#include "pos.h"

// 7.2 판매 항목 선택 입력 *주의* 이 함수는 의미 규칙을 검사하지 않습니다.
int inputFoodNumber() {
    int foodNumber;
    while (1) {
        foodNumber = inputInt("판매 항목 번호를 입력하세요: ", true,false);
        return foodNumber;
    }
}
// 7.5 판매 항목 조회 프롬프트
void printFoodList() {

    ///// 판매 항목 데이터 파일 열기 /////
    FILE* foodFile = fopen(FILE_PATH, "r"); // 읽기
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
    }
    checkDataIntegrity();
    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
    /////////////////////////////////////

    char line[100]; // 각 행을 저장할 문자열
    int line_count = 0;
    int firstNum, secondNum, price;
    char food[50];  // 음식 이름 저장

    printf("\n===== 판매 항목 목록 =====\n");

    while (fgets(line, sizeof(line), foodFile)) {
        sscanf(line, "%d  %d    %s  %d", &firstNum, &secondNum, food, &price);  // 공백 2칸, 4칸, 2칸 유지

        if (firstNum == 0) {
            printf("%d. %s - %d\n", ++line_count, food, price);
        }
    }

    if (line_count == 0) {
        printf("판매 중인 항목이 없습니다.\n");
    }

    fclose(foodFile);
}

// 7.6 판매 항목 추가 프롬프트
void addToFoodList() {

    ///// 판매 항목 데이터 파일 열기 /////
    FILE* foodFile = fopen(FILE_PATH, "r+"); // 읽기 및 편집
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
    }
    checkDataIntegrity();
    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
    /////////////////////////////////////

    int firstNum = 0;
    int secondNum = getLastSecondNumber(foodFile) + 1;

    printf("\n추가할 판매 항목 정보를 입력하세요(항목명은 알파벳만 허용)\n");
    char* foodName = inputFoodName();

    if (foodName != NULL) {
        int price = inputPrice();

        fseek(foodFile, 0, SEEK_END);  // 파일의 끝으로 이동
        if (fseek(foodFile, -1, SEEK_END) == 0 && fgetc(foodFile) != '\n') {
            fprintf(foodFile, "\n");
        }
        fprintf(foodFile, "%d  %d    %s  %d\n", firstNum, secondNum, foodName, price);  // 공백 2칸, 4칸, 2칸 공백 적용
        printf("%s 이/가 추가되었습니다.\n", foodName);

        free(foodName);
    }
    else {
        printf("메모리 할당 실패\n");
    }

    fclose(foodFile);
}

// 7.7 판매 항목 제거 프롬프트
void removeFoodItem() {

    ///// 판매 항목 데이터 파일 열기 /////
    FILE* foodFile = fopen(FILE_PATH, "r+"); // 읽기 및 편집
    if (foodFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
    }
    checkDataIntegrity();
    rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
    /////////////////////////////////////

    printFoodList(); // 현재 판매 항목을 출력

    while (1) {
        rewind(foodFile);  // 파일 포인터를 처음으로 되돌림
        FILE* tempFile = fopen("temp.txt", "w");
        if (tempFile == NULL) {
            printf("임시 파일을 열 수 없습니다.\n");
            return;
        }
        int userChoice = inputFoodNumber();  // 사용자가 선택한 항목 번호 (출력 순서)

        char line[256];
        int firstNum, secondNum, price;
        char foodName[50];
        int currentZeroIndex = 0;  // 0인 항목만 카운트
        int found = 0;

        // 원본 파일에서 데이터를 읽고, 임시 파일로 복사하면서 상태를 변경
        while (fgets(line, sizeof(line), foodFile)) {
            // 정확한 공백에 맞추어 데이터를 파싱 (2칸, 4칸, 2칸의 공백)
            sscanf(line, "%d  %d    %s  %d", &firstNum, &secondNum, foodName, &price);

            // 첫 번째 숫자가 0인 항목만 카운트 (활성 상태인 항목만)
            if (firstNum == 0) {
                currentZeroIndex++;  // 첫 번째 숫자가 0인 항목에 대해 인덱스 증가

                // 사용자가 선택한 순서와 현재 항목이 일치하면 해당 항목 제거 (상태 1로 변경)
                if (currentZeroIndex == userChoice) {
                    firstNum = 1;  // 첫 번째 숫자를 1로 변경하여 비활성화
                    found = 1;
                }

            }

            // 변경된 내용을 임시 파일에 기록 (공백 형식을 유지하여 기록)
            fprintf(tempFile, "%d  %d    %s  %d\n", firstNum, secondNum, foodName, price);
        }

        // 모든 항목을 확인한 후에 항목을 찾지 못했을 경우 처리
        if (!found) {
            printf("해당 번호의 항목이 없습니다.\n");
            fclose(tempFile);
            continue;
        }
        else {
            fclose(tempFile);
            fclose(foodFile); // 임시 파일로 교체 전 닫아야 함.

            // 원본 파일을 임시 파일로 대체
            remove(FILE_PATH);
            rename("temp.txt", FILE_PATH);
            printf("메뉴 번호 %d가 제거되었습니다.\n", userChoice);
            break;
        }
    }
}
