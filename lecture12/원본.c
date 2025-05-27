#include <GLFW/glfw3.h>

#pragma comment(lib, "Opengl32.lib")

//
//0 -> 0b1111110
//1 -> 0b0110000
//2 -> 0b1101101
//3 -> 0b1111001
//4 -> 0b0110011
//5 -> 0b1011011
//6 -> 0b1011111
//7 -> 0b1110000
//8 -> 0b1111111
//9 -> 0b1111011


// 직선을 그리기 위한 함수
void drawLine(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

int main(void) {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Cross in Center", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // 2D 직교 투영

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // 화면 중앙에 십자 모양 그리기
        // 좌상
        drawLine(-0.25f, 0.5f, -0.25f, 0.0f);
        // 상
        drawLine(-0.25f, 0.5f, 0.25f, 0.5f);
        // 우상
        drawLine(0.25f, 0.5f, 0.25f, 0.0f);

        // 중
        drawLine(-0.25f, 0.0f, 0.25f, 0.0f);

        // 좌하
        drawLine(-0.25f, 0.0f, -0.25f, -0.5f);
        // 하
        drawLine(-0.25f, -0.5f, 0.25f, -0.5f);
        // 우하
        drawLine(0.25f, 0.0f, 0.25f, -0.5f);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}



/*
코드 설명
이 C++ 코드는 GLFW 라이브러리를 사용하여 창을 만들고, OpenGL을 이용해 해당 창의 중앙에 7세그먼트 디스플레이 스타일의 숫자 **'8'**을 그리는 프로그램입니다.

주요 기능 분석
1. 헤더 및 라이브러리 설정
C++

#include <GLFW/glfw3.h>
#pragma comment(lib, "Opengl32.lib")
#include <GLFW/glfw3.h>: 창 생성, 사용자 입력 처리 등을 관리하는 GLFW 라이브러리의 기능을 사용하기 위해 헤더 파일을 포함합니다.
#pragma comment(lib, "Opengl32.lib"): Windows 환경에서 OpenGL 라이브러리(Opengl32.lib)를 링크하도록 지시합니다. 이를 통해 OpenGL 함수를 사용할 수 있습니다.
2. 7세그먼트 주석
C++

// 0 -> 0b1111110
// 1 -> 0b0110000
// ...
// 8 -> 0b1111111
// ...
이 주석들은 7세그먼트 디스플레이에서 각 숫자를 어떻게 표현하는지를 이진수(binary)로 나타냅니다.
각 비트(bit)는 7개의 선분(segment) 중 하나를 켜거나(1) 끄는(0) 것을 의미합니다.
예를 들어 8 -> 0b1111111은 숫자 8을 표현하기 위해 7개의 모든 선분을 켠다는 뜻입니다. 현재 코드에서 실제로 그려지는 모양이 바로 이 '8'입니다.
3. drawLine 함수
C++

void drawLine(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}
두 개의 2차원 좌표 (x
1
​
 ,y
1
​
 )와 (x
2
​
 ,y
2
​
 )를 인자로 받아 그 사이에 직선을 그리는 간단한 함수입니다.
glBegin(GL_LINES)와 glEnd()는 구형(legacy) OpenGL 방식으로, 이 블록 안에서 glVertex2f로 지정된 정점들을 2개씩 묶어 선분으로 그립니다.
4. main 함수 (프로그램 실행 흐름)
초기화:

glfwInit(): GLFW 라이브러리를 초기화합니다.
glfwCreateWindow(): "Cross in Center"라는 제목의 800x600 크기 창을 생성합니다.
glfwMakeContextCurrent(): 생성된 창을 현재 OpenGL 렌더링 컨텍스트로 지정합니다.
좌표계 설정:

glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0): 2D 렌더링을 위한 좌표계를 설정합니다. 창의 가로(x축)와 세로(y축) 범위를 모두 -1.0에서 1.0으로 지정합니다. 따라서 창의 정중앙이 (0, 0)이 됩니다.
메인 루프 (while):

!glfwWindowShouldClose(window): 사용자가 창을 닫으려고 하기 전까지 계속 반복됩니다.
glClear(GL_COLOR_BUFFER_BIT): 매 프레임마다 이전 그림을 지우고 깨끗한 상태에서 다시 그리기 위해 화면을 초기화합니다.
숫자 '8' 그리기: drawLine 함수를 7번 호출하여 7개의 선분을 그립니다.
// 좌상, // 상, // 우상: 숫자 '8'의 위쪽 머리 부분과 양옆 세로선
// 중: 가운데 가로선
// 좌하, // 하, // 우하: 아래쪽 몸통 부분과 양옆 세로선
이 7개의 선분이 모여 숫자 '8' 형태를 만듭니다. (창 제목인 "Cross(십자)"와는 다소 다릅니다.)
glfwSwapBuffers(window): 백그라운드 버퍼(그림이 그려진 곳)와 프론트 버퍼(화면에 보이는 곳)를 교체하여 완성된 그림을 화면에 표시합니다.
glfwPollEvents(): 키보드, 마우스 입력이나 창 닫기 같은 이벤트를 처리합니다.
종료:

glfwTerminate(): 루프가 끝나면(창이 닫히면) GLFW 관련 모든 리소스를 해제하고 프로그램을 종료합니다.

*/