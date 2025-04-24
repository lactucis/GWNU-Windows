#include <windows.h>
#include <stdio.h>

// ────────── 구조체 정의 ──────────
typedef struct {
    int x, y, width, height;
    const char* title;
    int color;
    int visible;
} MyWindow;

// ────────── 콘솔 도우미 함수들 ──────────
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setCursorInvisible() {
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

// ────────── 배경 그리기 ──────────
void DrawBG(int width, int height) {
    int topMargin = 1, bottomMargin = 1, sideMargin = 2;
    int taskbarY = height - bottomMargin - 1;

    for (int y = 0; y < height; y++) {
        gotoxy(0, y);
        if (y < topMargin || y >= height - bottomMargin) {
            for (int i = 0; i < width; i++) printf(" ");
        }
        else if (y == taskbarY) {
            for (int i = 0; i < sideMargin; i++) printf(" ");
            for (int i = 0; i < width - sideMargin * 2; i++) printf("\033[47m \033[0m");
            for (int i = 0; i < sideMargin; i++) printf(" ");
        }
        else {
            for (int i = 0; i < sideMargin; i++) printf(" ");
            for (int i = 0; i < width - sideMargin * 2; i++) printf("\033[44m \033[0m");
            for (int i = 0; i < sideMargin; i++) printf(" ");
        }
    }
}

// ────────── 윈도우 그리기 ──────────
void drawConsoleWindow(int startX, int startY, int width, int height, const char* title, int color) {
    gotoxy(startX, startY);
    printf("\033[4%dm|", color);
    for (int i = 0; i < width - 2; i++) printf("─");
    printf("|\033[0m");

    for (int y = 1; y < height - 1; y++) {
        gotoxy(startX, startY + y);
        printf("\033[4%dm│", color);
        for (int i = 0; i < width - 2; i++) printf(" ");
        printf("│\033[0m");
    }

    gotoxy(startX, startY + height - 1);
    printf("\033[4%dm|", color);
    for (int i = 0; i < width - 2; i++) printf("─");
    printf("|\033[0m");

    gotoxy(startX + 2, startY + 1);
    printf("\033[4%dm\033[3%dm%s\033[0m", color, 0, title);

    gotoxy(startX + width - 4, startY + 1);
    printf("\033[41m\033[37mX\033[0m");
}

// ────────── 커서 애니메이션 ──────────
void drawCursor(int x, int y, int frame) {
    const char* cursorFrames[] = { "<", ">" };
    gotoxy(x, y);
    printf("\033[32m%s\033[0m", cursorFrames[frame % 2]);
}

// ────────── 메인 함수 ──────────
int main() {
    setCursorInvisible();
    POINT mousePos;
    int frame = 0;
    const int windowCount = 3;

    MyWindow windows[3] = {
        {10, 2, 40, 10, "첫 번째 창", 3, 1},   // 노랑
        {20, 5, 30, 8,  "두 번째 창", 2, 1},   // 초록
        {30, 8, 35, 9,  "세 번째 창", 5, 1}    // 자주
    };

    while (1) {
        GetCursorPos(&mousePos);
        ScreenToClient(GetConsoleWindow(), &mousePos);

        int mouseX = mousePos.x / 9;
        int mouseY = mousePos.y / 18;

        if (mouseX < 2) mouseX = 2;
        if (mouseX > 77) mouseX = 77;
        if (mouseY < 1) mouseY = 1;
        if (mouseY > 23) mouseY = 23;

        system("cls");
        DrawBG(80, 25);

        // X 버튼 클릭 처리
        for (int i = windowCount - 1; i >= 0; i--) {
            if (!windows[i].visible) continue;
            int xBtnX = windows[i].x + windows[i].width - 4;
            int xBtnY = windows[i].y + 1;
            if ((mouseX == xBtnX || mouseX == xBtnX + 1) && mouseY == xBtnY) {
                if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                    windows[i].visible = 0;
                }
            }
        }

        // 클릭한 창을 최상단으로 이동
        for (int i = windowCount - 1; i >= 0; i--) {
            if (!windows[i].visible) continue;
            if (mouseX >= windows[i].x && mouseX < windows[i].x + windows[i].width &&
                mouseY >= windows[i].y && mouseY < windows[i].y + windows[i].height) {
                if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                    MyWindow temp = windows[i];
                    for (int j = i; j < windowCount - 1; j++) {
                        windows[j] = windows[j + 1];
                    }
                    windows[windowCount - 1] = temp;
                    break;
                }
            }
        }

        // 윈도우 출력 (순서대로 → 가장 위 창이 마지막)
        for (int i = 0; i < windowCount; i++) {
            if (windows[i].visible) {
                drawConsoleWindow(
                    windows[i].x, windows[i].y,
                    windows[i].width, windows[i].height,
                    windows[i].title, windows[i].color
                );
            }
        }

        // 마우스 커서 출력
        drawCursor(mouseX, mouseY, frame++);
        Sleep(100);
    }

    return 0;
}
