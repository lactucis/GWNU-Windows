//연습문제!! (과제물)

#include <windows.h>
#include <stdio.h>

int main() {
    char letter = 'O';  // 알파벳 'O' 고정
    POINT mousePos;     // 마우스 위치
    int isDragging = 0;  // 드래그 상태 (0: 드래그 중 아님, 1: 드래그 중)
    int letterX = 10;    // 알파벳 'O'의 초기 X 좌표
    int letterY = 5;     // 알파벳 'O'의 초기 Y 좌표

    // 콘솔 화면 초기화
    system("cls");

    while (1) {
        GetCursorPos(&mousePos);


        // 드래그 시작 조건: 마우스 클릭 && 알파벳 위에서 시작
        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && isDragging == 0) {
            if ((mousePos.x / 9) == letterX && (mousePos.y / 18) == letterY) {
                isDragging = 1;
            }
        }

        // 드래그 중이면 마우스를 따라 이동
        if (isDragging == 1) {
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                letterX = mousePos.x / 9;
                letterY = mousePos.y / 18;
            }
            else {
                isDragging = 0; // 버튼 놓으면 드래그 종료
            }
        }


        // 화면 갱신 (드래그 중일 때만 위치 변경)
        system("cls");  // 화면 초기화
        printf("현재 마우스 위치: (%d, %d)\n", mousePos.x / 9, mousePos.y / 18);
        printf("알파벳 '%c'을 마우스로 드래그하세요.\n", letter);
        printf("현재 알파벳 '%c'의 위치: (%d, %d)\n", letter, letterX, letterY);

        // 알파벳 O 출력
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { letterX, letterY });
        printf("%c", letter);
       
        // 마우스 위치 표시
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { mousePos.x / 9, mousePos.y / 18 });
        printf("+");


        // 종료 조건: Q 키를 누르면 프로그램 종료
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            printf("\n프로그램 종료\n");
            break;
        }

        // 잠시 대기 (CPU 사용 최적화)
        Sleep(10);
    }

    return 0;
}