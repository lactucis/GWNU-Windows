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


        if (GetAsyncKeyState('W') & 0x8000 && y > 0) y--;  // 'W'�� �������� �� ���� �̵�
        if (GetAsyncKeyState('S') & 0x8000 && y < 24) y++;  // 'S'�� �������� �� �Ʒ��� �̵�
        if (GetAsyncKeyState('A') & 0x8000 && x > 0) x--;  // 'A'�� �������� �� �������� �̵�
        if (GetAsyncKeyState('D') & 0x8000 && x < 79) x++;  // 'D'�� �������� �� ���������� �̵�
        if (GetAsyncKeyState('Q') & 0x8000) break;           // 'Q'�� �������� �� ����

        // ���� ó��
        //if (ch == 'w' && y > 0) y--;         // ��
        //else if (ch == 's' && y < 24) y++;   // �Ʒ�
        //else if (ch == 'a' && x > 0) x--;    // ����
        //else if (ch == 'd' && x < 79) x++;   // ������
        //else if (ch == 'q') break;           // ����

        gotoxy(x, y);
        printf("\033[44mP\033[0m");

        Sleep(10);
    }

    return 0;
}