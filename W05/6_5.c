//��������!! (������)

#include <windows.h>
#include <stdio.h>

int main() {
    char letter = 'O';  // ���ĺ� 'O' ����
    POINT mousePos;     // ���콺 ��ġ
    int isDragging = 0;  // �巡�� ���� (0: �巡�� �� �ƴ�, 1: �巡�� ��)
    int letterX = 10;    // ���ĺ� 'O'�� �ʱ� X ��ǥ
    int letterY = 5;     // ���ĺ� 'O'�� �ʱ� Y ��ǥ

    // �ܼ� ȭ�� �ʱ�ȭ
    system("cls");

    while (1) {
        GetCursorPos(&mousePos);


        // �巡�� ���� ����: ���콺 Ŭ�� && ���ĺ� ������ ����
        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && isDragging == 0) {
            if ((mousePos.x / 9) == letterX && (mousePos.y / 18) == letterY) {
                isDragging = 1;
            }
        }

        // �巡�� ���̸� ���콺�� ���� �̵�
        if (isDragging == 1) {
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                letterX = mousePos.x / 9;
                letterY = mousePos.y / 18;
            }
            else {
                isDragging = 0; // ��ư ������ �巡�� ����
            }
        }


        // ȭ�� ���� (�巡�� ���� ���� ��ġ ����)
        system("cls");  // ȭ�� �ʱ�ȭ
        printf("���� ���콺 ��ġ: (%d, %d)\n", mousePos.x / 9, mousePos.y / 18);
        printf("���ĺ� '%c'�� ���콺�� �巡���ϼ���.\n", letter);
        printf("���� ���ĺ� '%c'�� ��ġ: (%d, %d)\n", letter, letterX, letterY);

        // ���ĺ� O ���
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { letterX, letterY });
        printf("%c", letter);
       
        // ���콺 ��ġ ǥ��
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) { mousePos.x / 9, mousePos.y / 18 });
        printf("+");


        // ���� ����: Q Ű�� ������ ���α׷� ����
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            printf("\n���α׷� ����\n");
            break;
        }

        // ��� ��� (CPU ��� ����ȭ)
        Sleep(10);
    }

    return 0;
}