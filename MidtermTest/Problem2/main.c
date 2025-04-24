#include <stdio.h>
#include <windows.h>

void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void DrawBG(int width, int height) {
    int topMargin = 1;
    int bottomMargin = 1;
    int sideMargin = 2;

    int taskbarY = height - bottomMargin - 1;

    for (int y = 0; y < height; y++) {
        gotoxy(0, y);

        // 상단 여백
        if (y < topMargin) {
            for (int i = 0; i < width; i++) {
                printf(" ");
            }
        }
        // 작업표시줄
        else if (y == taskbarY) {
            for (int i = 0; i < sideMargin; i++) {
                printf(" ");
            }
            for (int i = 0; i < width - (sideMargin * 2); i++) {
                printf("\033[47m \033[0m"); // 회색 배경 (작업표시줄)
            }
            for (int i = 0; i < sideMargin; i++) {
                printf(" ");
            }
        }
        // 하단 여백
        else if (y >= height - bottomMargin) {
            for (int i = 0; i < width; i++) {
                printf(" ");
            }
        }
        // 일반 파란 배경
        else {
            for (int i = 0; i < sideMargin; i++) {
                printf(" ");
            }
            for (int i = 0; i < width - (sideMargin * 2); i++) {
                printf("\033[44m \033[0m");
            }
            for (int i = 0; i < sideMargin; i++) {
                printf(" ");
            }
        }
    }
}


int main() {
    DrawBG(80, 25);
    return 0;
}
