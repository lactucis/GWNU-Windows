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
    int x = 0; // ���� x��ǥ (����)
    int y = 0;  // ���� y��ǥ (����)

    // ����
    gotoxy(x, y);
    printf("��");
    for (int i = 0; i < width - 2; i++) printf("��");
    printf("��");

    // ���� ��
    for (int i = 1; i < height - 1; i++) {
        gotoxy(x, y + i);
        printf("��");
        for (int j = 0; j < width - 2; j++) printf(" ");
        printf("��");
    }

    // �Ʒ���
    gotoxy(x, y + height - 1);
    printf("��");
    for (int i = 0; i < width - 2; i++) printf("��");
    printf("��");

    // ���� �ֱ�
    gotoxy(x + 2, y + 1);  // ���� ���ʿ� �ؽ�Ʈ
    printf("���߾�");

    //�ּ�â ��
    gotoxy(x, y + 2);  // ���� ���ʿ� �ؽ�Ʈ
    printf("��");
    for (int i = 0; i < width - 2; i++) printf("��");
    printf("��");

    //����� �޽��� �Ʒ��� ������
    for (int i = 0; i < 9; i++) puts("");
}


//4 ���� 2

int main()
{
    DrawWindow(30, 10);  // ���� 30ĭ, ���� 10��¥�� â ����
    return 0;
}