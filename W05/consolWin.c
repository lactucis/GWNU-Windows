#include <windows.h>
#include <stdio.h>

void gotoxy(int x, int y)
{
    COORD Cur;
    Cur.X = x;
    Cur.Y = y;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, Cur);
}

void DrawWindow(int width, int height)
{
    int x = 0; // 시작 x좌표 (가로)
    int y = 0;  // 시작 y좌표 (세로)

    // 윗변
    gotoxy(x, y);
    printf("┌");
    for (int i = 0; i < width - 2; i++) printf("─");
    printf("┐");

    // 세로 줄
    for (int i = 1; i < height - 1; i++) {
        gotoxy(x, y + i);
        printf("│");
        for (int j = 0; j < width - 2; j++) printf(" ");
        printf("│");
    }

    // 아랫변
    gotoxy(x, y + height - 1);
    printf("└");
    for (int i = 0; i < width - 2; i++) printf("─");
    printf("┘");

    // 제목 넣기
    gotoxy(x + 2, y + 1);  // 윗줄 안쪽에 텍스트
    printf("끼야악");

    //주소창 줄
    gotoxy(x, y + 2);  // 윗줄 안쪽에 텍스트
    printf("├");
    for (int i = 0; i < width - 2; i++) printf("─");
    printf("┤");

    //디버그 메시지 아래로 내리기
    for (int i = 0; i < 9; i++) puts("");
}


//4 가로 2

int main()
{
    DrawWindow(30, 10);  // 가로 30칸, 세로 10줄짜리 창 생성
    return 0;
}