#pragma comment(lib, "Opengl32.lib")

#include <GLFW/glfw3.h>



/* NuGet 패키지 매니져에서 GLFW 설치후 사용*/

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(500, 500, "GLFW: Triangle", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-0.5f, -0.5f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(0.0f, 0.5f);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

/*
코드 설명
이 C++ 코드는 GLFW 라이브러리를 사용하여 창을 만들고, 구형(Legacy) OpenGL을 이용해 각 꼭짓점의 색이 다른 삼각형 하나를 그리는 매우 기본적인 예제입니다.

코드 상세 분석
1. 라이브러리 설정 및 포함
C++

#pragma comment(lib, "Opengl32.lib")
#include <GLFW/glfw3.h>
#include <GLFW/glfw3.h>: 창 생성 및 관리를 위한 GLFW 라이브러리의 함수를 사용하기 위해 헤더 파일을 포함합니다.
#pragma comment(lib, "Opengl32.lib"): Windows 환경에서 OpenGL 라이브러리(Opengl32.lib)를 링크하여 그래픽스 함수를 사용할 수 있게 합니다.
2. main 함수 (프로그램 실행 흐름)
GLFW 초기화:

C++

if (!glfwInit()) return -1;
glfwInit(): GLFW 라이브러리를 사용하기 위해 초기화합니다. 실패하면 -1을 반환하고 프로그램을 종료합니다.
창(Window) 생성:

C++

GLFWwindow* window = glfwCreateWindow(500, 500, "GLFW: Triangle", NULL, NULL);
if (!window) {
    glfwTerminate();
    return -1;
}
glfwCreateWindow(): 가로 500, 세로 500 픽셀 크기의 창을 "GLFW: Triangle"이라는 제목으로 생성합니다.
창 생성에 실패하면 GLFW를 종료하고 프로그램을 끝냅니다.
OpenGL 컨텍스트 설정:

C++

glfwMakeContextCurrent(window);
생성된 창을 현재 OpenGL 명령이 실행될 대상으로 지정합니다. 이 함수가 호출된 이후의 모든 gl* 함수는 이 창에 그려집니다.
메인 루프 (Render Loop):

C++

while (!glfwWindowShouldClose(window)) {
    // ... 루프 내용 ...
}
glfwWindowShouldClose(window): 사용자가 창의 'X' 버튼을 누르거나 Alt+F4 등을 입력했는지 확인합니다. 창을 닫으라는 신호가 없을 때까지 루프는 계속 반복됩니다.
루프 내부 작업 (매 프레임마다 실행):

화면 지우기:

C++

glClear(GL_COLOR_BUFFER_BIT);
매 프레임을 그리기 전에 이전 프레임의 그림을 깨끗하게 지웁니다. GL_COLOR_BUFFER_BIT는 색상 버퍼를 지우라는 의미입니다.
삼각형 그리기:

C++

glBegin(GL_TRIANGLES);
// ... 정점 및 색상 설정 ...
glEnd();
glBegin(GL_TRIANGLES): 지금부터 그릴 정점(Vertex)들을 3개씩 묶어 삼각형을 만들겠다고 OpenGL에 알립니다.
첫 번째 꼭짓점 (왼쪽 아래):
C++

glColor3f(1.0f, 0.0f, 0.0f); // 색상을 빨간색(Red)으로 설정
glVertex2f(-0.5f, -0.5f);   // 좌표를 (-0.5, -0.5)로 설정
두 번째 꼭짓점 (오른쪽 아래):
C++

glColor3f(0.0f, 1.0f, 0.0f); // 색상을 녹색(Green)으로 설정
glVertex2f(0.5f, -0.5f);    // 좌표를 (0.5, -0.5)로 설정
세 번째 꼭짓점 (위쪽 중앙):
C++

glColor3f(0.0f, 0.0f, 1.0f); // 색상을 파란색(Blue)으로 설정
glVertex2f(0.0f, 0.5f);     // 좌표를 (0.0, 0.5)로 설정
glEnd(): 도형 그리기를 마칩니다.
핵심: glVertex 함수를 호출하기 직전에 glColor 함수로 설정한 색상이 해당 정점의 색상이 됩니다. OpenGL은 이 세 꼭짓점의 색상을 **자동으로 보간(interpolate)**하여 삼각형 내부를 부드러운 그라데이션으로 채웁니다.
버퍼 교체 및 이벤트 처리:

C++

glfwSwapBuffers(window);
glfwPollEvents();
glfwSwapBuffers(window): 더블 버퍼링 기법을 사용합니다. 보이지 않는 버퍼(백 버퍼)에 그림을 모두 그린 후, 이 함수를 통해 백 버퍼를 화면에 보이는 버퍼(프론트 버퍼)와 순간적으로 교체합니다. 이렇게 하면 그림이 그려지는 중간 과정이 보이지 않아 깜빡임이 없습니다.
glfwPollEvents(): 키보드 입력, 마우스 움직임, 창 크기 조절 등 운영체제로부터 받은 이벤트를 처리합니다.
종료:

C++

glfwDestroyWindow(window);
glfwTerminate();
return 0;
루프가 종료되면(창이 닫히면) 생성했던 창을 파괴하고, GLFW의 모든 리소스를 해제한 후 프로그램을 정상적으로 종료합니다.
결론 📝
이 코드는 GLFW로 창을 생성하고, OpenGL의 기본 그리기 명령을 사용하여 화면에 다채로운 색상의 삼각형 하나를 렌더링하는 가장 기본적인 골격을 보여줍니다. 왼쪽 아래는 빨간색, 오른쪽 아래는 녹색, 위쪽 중앙은 파란색인 삼각형이 그려지며 그 내부는 색상이 부드럽게 섞여 표시됩니다.
*/