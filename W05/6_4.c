/*
#include <windows.h>
#include <stdio.h>

int main() {

    POINT mousePos;     // 마우스 위치

    // 콘솔 화면 초기화
    system("cls");

    while (1) {
        // 마우스 위치 얻기
        GetCursorPos(&mousePos);

        // 화면 갱신 (드래그 중일 때만 위치 변경)
        system("cls");  // 화면 초기화
        printf("현재 마우스 위치: (%d, %d)\n", mousePos.x / 9, mousePos.y / 19);



        // 마우스 위치 표시
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { mousePos.x / 9, mousePos.y / 18 });
        printf("<");

        // 종료 조건: Q 키를 누르면 프로그램 종료
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            printf("\n프로그램 종료\n");
            break;
        }

        // 잠시 대기 (CPU 사용 최적화)
        Sleep(10);
    }

    return 0;
}*/