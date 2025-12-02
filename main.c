/*
학번:
이름:
아너코드 선언: 나 OOO는 하나님과 사람 앞에서 정직하고 성실하게 코딩 테스트를 수행하겠습니다.
*/

#include <stdio.h>
#include <stdlib.h> // for malloc()
#include <string.h> // for strlen(), strstr()
#include <ctype.h> // for isalnum()

#define MAX_QUOTES 50
#define FILENAME "quotes.txt"

// 명언 구조체 정의
typedef struct st_quote{
    char author[100];
    char text[255];
    int likes;
    int word_count;
} QUOTE;

// 전역 변수: 구조체 포인터 배열 (동적 할당된 QUOTE 구조체를 가리킴)
QUOTE* quote_list[MAX_QUOTES]; 
int quote_count = 0;

// ===============================================
// 함수 원형 (Function Prototypes)
// ===============================================

// 유틸리티 및 헬퍼 함수
void removeNewLine(char* str); // 문자열 끝의 \n을 제거
void clearInputBuffer(); // 입력 버퍼를 비우는 함수
void printQuote(int index); // 명언 정보 출력
int calculateWordCount(char* text); // 명언의 단어 수 계산
void inputQuoteDetails(QUOTE* q); // 명언 상세 정보 입력
int loadData(); // 파일에서 데이터 로드
void displayMenu(); // 메뉴 출력

// 메뉴별 기능 함수
void listQuotes(); // 1. 전체 목록 출력
void searchQuotes(); // 2. 키워드로 검색
void addQuote(); // 3. 명언 추가
void editQuote(); // 4. 명언 수정
void removeQuote(); // 5. 명언 제거
void recommendQuote(); // 6. 추천수 증가
void reportQuotes(); // 7. 통계 보고서 출력
void saveData(); // 8. 파일에 저장

// ===============================================
// Main 함수
// ===============================================

int main() {
    // 파일에서 데이터 로드 및 전역 변수 quote_count 설정
    quote_count = loadData();

    int choice;

    while (1) {
        displayMenu();

        // 메뉴 번호 입력 (숫자 입력 가정)
        printf("Select a menu option (0-9): ");
        scanf("%d", &choice);
        clearInputBuffer(); // scanf로 인해 입력 버퍼에 남아있는 개행 문자를 제거

        switch (choice) {
            case 1: listQuotes(); break;
            case 2: searchQuotes(); break;
            case 3: addQuote(); break;
            case 4: editQuote(); break;
            case 5: removeQuote(); break;
            case 6: recommendQuote(); break;
            case 7: reportQuotes(); break;
            case 8: saveData(); break;
            case 0: 
                // 동적으로 할당된 모든 메모리 해제
                for (int i = 0; i < quote_count; i++) {
                    free(quote_list[i]);
                }
                printf(">> Program terminated. Thank thank you.\n");
                return 0;
            default:
                // 유효하지 않은 메뉴 번호 처리
                printf(">> [Error] Please select a valid menu number.\n");
        }
    }
}

// --- 유틸리티 및 헬퍼 함수 ---

// 문자열 끝의 개행 문자(\n)를 널 문자로 제거하는 함수
void removeNewLine(char* str) {
    if (str == NULL) return;
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';// 마지막 문자가 개행 문자라면 널 문자로 변경
    }
}

// 입력 버퍼에 남아있는 개행 문자 및 기타 문자를 모두 비우는 함수
void clearInputBuffer() {
    int c; 
    while ((c = getchar()) != '\n' && c != EOF); 
}

// 명언 상세 정보를 출력하는 함수 (파라미터: 배열 인덱스)
void printQuote(int index) { 
    // 배열 인덱스가 유효한지 확인
    if (index < 0 || index >= quote_count || quote_list[index] == NULL) {
        return;
    }

    // 포인터를 통해 데이터 접근
    QUOTE* q = quote_list[index];

    // 인덱스 번호와 상세 정보 출력
    printf("[%02d] Author: %s (Likes: %d | Word Count: %d)\n", index + 1, q->author, q->likes, q->word_count);
    printf("\"%s\"\n", q->text);
}

// 명언 텍스트의 단어 수를 계산하는 함수
int calculateWordCount(char* text) {
    int count = 0;
    int in_word = 0; // 0: 공백 상태, 1: 단어 내부 상태
    for (int i = 0; text[i] != '\0'; i++) {
        // 알파벳, 숫자, 또는 흔한 축약어/하이픈을 포함하는 경우
        if (isalnum((unsigned char)text[i]) || 
            text[i] == '-' || text[i] == '\'') {
            if (in_word == 0) {
                count++; // 새 단어 시작
                in_word = 1;
            }
        } else {
            // 공백 또는 구분자
            in_word = 0;
        }
    }
    return count;
}

// 새로운 명언 정보를 입력받는 함수 - 추가, 수정 기능에서 사용
void inputQuoteDetails(QUOTE* q) {
    // Author Name
    printf("Author Name (max 99 chars): ");
    if (fgets(q->author, sizeof(q->author), stdin) == NULL) return;
    removeNewLine(q->author); 

    // Quote Text
    printf("Quote Text (max 254 chars): ");
    if (fgets(q->text, sizeof(q->text), stdin) == NULL) return;
    removeNewLine(q->text); 

    // 단어 수 자동 계산 및 저장
    q->word_count = calculateWordCount(q->text);
}

// 파일에서 데이터를 로드하는 함수
int loadData() {
    FILE* fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf(">> [Info] File '%s' not found. Starting with an empty list.\n", FILENAME);
        return 0;
    }

    QUOTE temp_q;
    int current_count = 0;

    // 명언당 3줄씩 반복하여 읽기
    while (current_count < MAX_QUOTES) 
    {
        // 1. 저자 이름 읽기
        if (fgets(temp_q.author, sizeof(temp_q.author), fp) == NULL) {
            break; // 파일 끝(EOF) 또는 오류
        }
        // 2. 명언 텍스트 읽기
        if (fgets(temp_q.text, sizeof(temp_q.text), fp) == NULL) {
            break; // 파일 끝(EOF) 또는 오류
        }
        // 3. 추천수와 단어 수 읽기
        if (fscanf(fp, "%d %d", &temp_q.likes, &temp_q.word_count) != 2) {
            break; // 파일 끝(EOF) 또는 항목 불완전
        }
        // fscanf가 읽지 않은 잔여 개행 문자를 파일 포인터(fp)를 사용하여 제거
        int c;
        while ((c = fgetc(fp)) != '\n' && c != EOF); 

        removeNewLine(temp_q.author); 
        removeNewLine(temp_q.text); 

        // --- 동적 할당 및 저장 ---
        quote_list[current_count] = (QUOTE*)malloc(sizeof(QUOTE));
        if (quote_list[current_count] == NULL) {
            printf(">> [Error] Memory allocation failed during loading. Halting read.\n");
            break;
        }

        // 임시 구조체 내용을 동적으로 할당된 메모리에 복사
        *quote_list[current_count] = temp_q;
        current_count++;
    }

    fclose(fp);
    printf(">> Success: Loaded %d quotes from file.\n", current_count);
    return current_count;
}


// 메인 메뉴 출력 함수
void displayMenu() {
    printf("\n============================================\n");
    printf("        Famous Quote Management System\n");
    printf("============================================\n");
    printf(" 1. List: Display all quotes\n");
    printf(" 2. Search: Search quotes by keyword\n");
    printf(" 3. Add: Add a new quote\n");
    printf(" 4. Edit: Edit quote details\n");
    printf(" 5. Remove: Remove a quote\n");
    printf(" 6. Recommend: Increase quote likes\n");
    printf(" 7. Report: Generate statistics report\n");
    printf(" 8. Save: Save current list to file\n");
    printf(" 0. Exit: Terminate program\n");
    printf("--------------------------------------------\n");
}


// --- 메뉴 함수 구현 ---

// 1. 전체 명언 목록 출력
void listQuotes() {
    printf("\n--- 1. All Quotes (%d entries) ---\n", quote_count);
    if (quote_count == 0) {
        printf(">> No quotes registered.\n");
        return;
    }
    for (int i = 0; i < quote_count; i++) {
        printQuote(i); 
    }
}

// 2. 키워드로 명언 검색
void searchQuotes() {
    char keyword[100];
    int found = 0;

    printf("\n--- 2. Quote Search ---\n");
    printf("Enter keyword to search: ");
    if (fgets(keyword, sizeof(keyword), stdin) == NULL) return;
    removeNewLine(keyword); 

    if (strlen(keyword) == 0) {
        printf(">> A search keyword is required.\n");
        return;
    }

    for (int i = 0; i < quote_count; i++) {
        // 저자 이름 또는 명언 내용에 키워드가 포함되어 있는지 확인
        if (strstr(quote_list[i]->author, keyword) != NULL ||
            strstr(quote_list[i]->text, keyword) != NULL) {
            printQuote(i);
            found++;
        }
    }

    if (found == 0) {
        printf(">> No quotes found matching the keyword '%s'.\n", keyword);
    } else {
        printf(">> Found %d quotes in total.\n", found);
    }
}

// 3. 새로운 명언 추가
void addQuote() {
    printf("\n--- 3. Add New Quote ---\n");
    if (quote_count >= MAX_QUOTES) {
        printf(">> [Error] Cannot add more quotes (Max %d entries).\n");
        return;
    }

    // --- 동적 할당 ---
    quote_list[quote_count] = (QUOTE*)malloc(sizeof(QUOTE));
    if (quote_list[quote_count] == NULL) {
        printf(">> [Error] Memory allocation failed for new quote.\n");
        return;
    }

    // 명언 상세 정보 입력
    inputQuoteDetails(quote_list[quote_count]); 
    quote_list[quote_count]->likes = 0;

    quote_count++;
    printf(">> Success: New quote (Author: %s) has been added to the list.\n", quote_list[quote_count-1]->author); 
}

// 4. 명언 정보 수정
void editQuote() {
    printf("\n--- 4. Edit Quote Details ---\n");
    if (quote_count == 0) {
        printf(">> No quotes to edit.\n");
        return;
    }

    int index;
    printf("Enter list number of the quote to edit (1 - %d): ", quote_count);
    scanf("%d", &index);
    // 버퍼 비우기 (다음 fgets를 위해 필수)
    clearInputBuffer(); 

    // 여기에 코드 작성
    
}

// 5. 명언 제거
void removeQuote() {
    printf("\n--- 5. Remove Quote ---\n");
    if (quote_count == 0) {
        printf(">> No quotes to remove.\n");
        return;
    }
    int index;
    printf("Enter list number of the quote to remove (1 - %d): ", quote_count);
    scanf("%d", &index);

    // 여기에 코드 작성

}

// 6. 추천수 1 증가
void recommendQuote() {
    printf("\n--- 6. Recommend Quote ---\n");
    if (quote_count == 0) {
        printf(">> No quotes to recommend.\n");
        return;
    }
    int index;
    printf("Enter list number of the quote to recommend (1 - %d): ", quote_count);
    scanf("%d", &index);
    // 버퍼 비우기 (다음 fgets를 위해 필수)
    clearInputBuffer(); 

    // 여기에 코드 작성

}

// 7. 통계 보고서 출력
void reportQuotes() {
    printf("\n--- 7. Quote Statistics Report ---\n");
    if (quote_count == 0) {
        printf(">> No quote data available for analysis.\n");
        return;
    }

    // 평균 단어 수 계산
    long long total_word_count = 0;
    for (int i = 0; i < quote_count; i++) {
        total_word_count += quote_list[i]->word_count;
    }
    double avg_word_count = (quote_count > 0) ? (double)total_word_count / quote_count : 0.0;

    printf("\n** Overall Status:\n");
    printf("  - Total number of quotes: %d\n", quote_count);
    printf("  - Average word count per quote: %.1f words\n", avg_word_count);

    printf("\n** Top 3 Quotes by Likes:\n");

    // 여기에 코드 작성


    printf("\n** Quote with the highest word count:\n");


    // 여기에 코드 작성


}


// 8. 현재 목록을 파일에 저장
void saveData() {
    FILE* fp = fopen(FILENAME, "w");
    if (fp == NULL) {
        printf(">> [Error] Could not open file: %s\n", FILENAME);
        return;
    }

    for (int i = 0; i < quote_count; i++) {
        // 포인터를 통해 데이터 접근 및 저장
        fprintf(fp, "%s\n", quote_list[i]->author);
        fprintf(fp, "%s\n", quote_list[i]->text);
        fprintf(fp, "%d %d\n", 
                quote_list[i]->likes, 
                quote_list[i]->word_count);
    }

    fclose(fp);
    printf(">> Success: %d quotes saved to file '%s'.\n", quote_count, FILENAME);
}

