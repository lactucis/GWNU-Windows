/*
#include <windows.h>
#include <stdio.h>

int main() {
    while (1) {
        // 왼쪽 마우스 버튼 눌렸는지 확인 (key down)
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            printf("왼쪽 마우스 버튼 눌림 (Key Down)\n");
        }

        // 오른쪽 마우스 버튼 눌렸는지 확인 (key down)
        if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
            printf("오른쪽 마우스 버튼 눌림 (Key Down)\n");
        }

        // 왼쪽 마우스 버튼 떼어졌는지 확인 (key up)
        if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            printf("왼쪽 마우스 버튼 떼어짐 (Key Up)\n");
        }

        // 오른쪽 마우스 버튼 떼어졌는지 확인 (key up)
        if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
            printf("오른쪽 마우스 버튼 떼어짐 (Key Up)\n");
        }

        // 마우스의 X, Y 좌표 가져오기
        POINT p;
        GetCursorPos(&p);  // 마우스의 현재 화면 좌표를 p에 저장
        printf("마우스 위치: X=%d, Y=%d\n", p.x, p.y);

        // 프로그램 종료 조건
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            printf("게임 종료\n");
            break;  // Q 키를 누르면 종료
        }

        // 잠시 대기 (CPU 사용 최적화)
        Sleep(50);
        system("cls");
    }

    return 0;
}*/