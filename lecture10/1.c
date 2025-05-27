#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>
#include <math.h>

void draw_circle(float cx, float cy, float r, int segments) {
    glBegin(GL_TRIANGLE_FAN);

    glColor3f(0.2f, 0.6f, 1.0f);
    glVertex2f(cx, cy);

    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * 3.1415926f * i / segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }

    glEnd();
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(500, 500, "GLFW: Circle", NULL, NULL);
    if (!window) {
        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        draw_circle(0.0f, 0.0f, 0.5f, 60);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


/*
코드 설명
이 C++ 코드는 GLFW 라이브러리를 사용하여 창을 만들고, 구형(Legacy) OpenGL을 이용해 화면 중앙에 파란색 원 하나를 그리는 간단한 프로그램입니다.

이전의 구조체를 사용한 예제들과 달리, 그리기 함수에 직접 위치와 크기를 인자로 넘겨주는 방식을 사용합니다.

코드 상세 분석
1. draw_circle 함수
C

void draw_circle(float cx, float cy, float r, int segments) {
    glBegin(GL_TRIANGLE_FAN);

    glColor3f(0.2f, 0.6f, 1.0f); // 원의 색상을 하늘색 계열로 지정
    glVertex2f(cx, cy); // 1. 원의 중심점을 첫 번째 정점으로 지정

    // 2. 원의 둘레를 따라 여러 개의 정점을 찍음
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * 3.1415926f * i / segments; // 0부터 2π까지 각도를 쪼갬
        float x = r * cosf(theta); // 원의 방정식 x = r * cos(θ)
        float y = r * sinf(theta); // 원의 방정식 y = r * sin(θ)
        glVertex2f(cx + x, cy + y); // 중심점에서 x, y만큼 떨어진 위치에 정점 추가
    }

    glEnd();
}
이 함수는 원의 중심 좌표 (cx, cy), 반지름 r, 그리고 원을 얼마나 부드럽게 그릴지를 결정하는 선분(segments)의 수를 인자로 받습니다.
glBegin(GL_TRIANGLE_FAN): 삼각형 부채꼴을 그리는 방식입니다. 첫 번째 정점(중심점)을 기준으로, 이후에 추가되는 모든 정점들이 순서대로 연결되어 작은 삼각형들을 만듭니다. 이 삼각형들이 모여 속이 채워진 원을 형성합니다.
for 루프는 삼각함수(cosf, sinf)를 이용해 원의 둘레에 있는 점들의 좌표를 계산하고, 이 점들을 glVertex2f를 통해 정점으로 추가합니다.
segments 값이 클수록 더 많은 삼각형으로 원을 표현하므로 더 부드러운 원이 그려집니다.
2. main 함수
main 함수의 전체적인 흐름은 이전 예제들과 동일합니다.

초기화: glfwInit()로 GLFW 라이브러리를 초기화합니다.
창 생성: 500x500 크기의 "GLFW: Circle"이라는 제목을 가진 창을 생성합니다.
컨텍스트 설정: glfwMakeContextCurrent()로 생성된 창을 현재 OpenGL 렌더링 대상으로 지정합니다.
메인 루프 (while): 사용자가 창을 닫기 전까지 계속 반복됩니다.
glClear(GL_COLOR_BUFFER_BIT): 매 프레임마다 이전 그림을 지우고 화면을 깨끗하게 합니다.
원 그리기 호출:
C

draw_circle(0.0f, 0.0f, 0.5f, 60);
draw_circle 함수를 호출하여 화면의 정중앙(0.0f, 0.0f)에 반지름이 0.5이고, 60개의 선분으로 이루어진 부드러운 원을 그리도록 명령합니다.
glfwSwapBuffers()와 glfwPollEvents(): 그려진 그림을 화면에 표시하고 사용자 입력을 처리합니다.
종료: 루프가 끝나면 glfwDestroyWindow()와 glfwTerminate()를 통해 사용된 모든 자원을 해제하고 프로그램을 종료합니다.
결론 📝
이 코드는 OpenGL의 GL_TRIANGLE_FAN과 삼각함수를 이용하여 속이 채워진 원을 그리는 가장 기본적인 방법을 보여주는 예제입니다. 구조체를 사용하지 않고 그리기 함수에 직접 필요한 정보를 전달하여 단일 객체를 간단하게 그리는 방법을 배울 수 있습니다.
*/