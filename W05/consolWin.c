#include <windows.h>
#include <stdio.h>

typedef struct
{
    int x;
    int y;
    int width;
    int height;
    char name[30];
    int bgColor;
} drawWindow;

void gotoxy(int x, int y)
{
    COORD Cur;
    Cur.X = x;
    Cur.Y = y;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, Cur);
}

void DrawWindow(drawWindow win)
{
    printf("\033[%dm", win.bgColor);
    // 윗변
    gotoxy(win.x, win.y);
    printf("┌");
    for (int i = 0; i < win.width - 2; i++) printf("─");
    printf("┐");

    // 세로 줄
    for (int i = 1; i < win.height - 1; i++) {
        gotoxy(win.x, win.y + i);
        printf("│");
        for (int j = 0; j < win.width - 2; j++) printf(" ");
        printf("│");
    }

    // 아랫변
    gotoxy(win.x, win.y + win.height - 1);
    printf("└");
    for (int i = 0; i < win.width - 2; i++) printf("─");
    printf("┘");

    // 제목 넣기
    gotoxy(win.x + 2, win.y + 1);  // 윗줄 안쪽에 텍스트
    printf("%s", win.name);

    //주소창 줄
    gotoxy(win.x, win.y + 2);  // 윗줄 안쪽에 텍스트
    printf("├");
    for (int i = 0; i < win.width - 2; i++) printf("─");
    printf("┤");

    printf("\033[0m");

    //디버그 메시지 아래로 내리기
    for (int i = 0; i < 9; i++) puts("");
}

int main()
{
    for (int i = 0; i < 45; i++)
    {
        for (int j = 0; j < 40; j++) 
        {
            printf("\033[42m \033[0m");
        }
    }

    drawWindow win1 = { 10, 3, 20, 7, "1번창", 44 };
    drawWindow win2 = { 15, 5, 20, 7, "2번창", 41 };

    DrawWindow(win1);
    DrawWindow(win2);

    return 0;
}



// 바탕화면에 해당하는 영역만을 녹색으로 가득 채울 것(공백 x) -------------------
// 바탕화면 위에 만든 윈도우 창을 20*7크기로 10.3에 찍을 것 ------------------
// 윈도우의 기본 크기, 기본 위치, 이름 속성을 struct 형태로 관리할 것 ------------------
// 다른 색상으로 15.5에 동일한 윈도우를 출력할 것 --------------------------
// 깃에 업로드하고 제출