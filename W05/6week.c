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

int main() {
    int x = 40;
    int y = 12;

    for (int i = 0; i < 80; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            printf("\033[44m \033[0m");
        }
    }

    gotoxy(x, y);
    printf("\033[44mP\033[0m");

    while (1) {
        char ch = getch();

        gotoxy(x, y);
        printf("\033[44m \033[0m");


        if (GetAsyncKeyState('W') & 0x8000 && y > 0) y--;  // 'W'가 눌려있을 때 위로 이동
        if (GetAsyncKeyState('S') & 0x8000 && y < 24) y++;  // 'S'가 눌려있을 때 아래로 이동
        if (GetAsyncKeyState('A') & 0x8000 && x > 0) x--;  // 'A'가 눌려있을 때 왼쪽으로 이동
        if (GetAsyncKeyState('D') & 0x8000 && x < 79) x++;  // 'D'가 눌려있을 때 오른쪽으로 이동
        if (GetAsyncKeyState('Q') & 0x8000) break;           // 'Q'가 눌려있을 때 종료

        // 방향 처리
        //if (ch == 'w' && y > 0) y--;         // 위
        //else if (ch == 's' && y < 24) y++;   // 아래
        //else if (ch == 'a' && x > 0) x--;    // 왼쪽
        //else if (ch == 'd' && x < 79) x++;   // 오른쪽
        //else if (ch == 'q') break;           // 종료

        gotoxy(x, y);
        printf("\033[44mP\033[0m");

        Sleep(10);
    }

    return 0;
}