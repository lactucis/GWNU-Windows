#include <GLFW/glfw3.h>
#include <math.h>

#pragma comment(lib, "Opengl32.lib")

#define PI 3.1415926535f

typedef struct
{
    float x, y;
} Vec2;

typedef struct
{
    Vec2 position;
    float rotation;    // 라디안
    Vec2 scale;
} Transform;

typedef struct
{
    Transform transform;
} Triangle;

void draw_triangle(Triangle* tri)
{
    glPushMatrix();

    glTranslatef(tri->transform.position.x, tri->transform.position.y, 0.0f);
    glRotatef(tri->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(tri->transform.scale.x, tri->transform.scale.y, 1.0f);

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();

    glPopMatrix();
}

typedef struct
{
    Transform transform;
} Square;

void draw_square(Square* sq)
{
    glPushMatrix();

    glTranslatef(sq->transform.position.x, sq->transform.position.y, 0.0f);
    glRotatef(sq->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(sq->transform.scale.x, sq->transform.scale.y, 1.0f);

    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(-0.5f, -0.5f);
    glEnd();

    glPopMatrix();
}

typedef struct
{
    Transform transform;
    int segments;
} Circle;

void draw_circle(Circle* c)
{
    glPushMatrix();

    glTranslatef(c->transform.position.x, c->transform.position.y, 0.0f);
    glRotatef(c->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(c->transform.scale.x, c->transform.scale.y, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);

    for (int i = 0; i <= c->segments; ++i)
    {
        float angle = 2.0f * PI * i / c->segments;
        glVertex2f(cosf(angle) * 0.5f, sinf(angle) * 0.5f);
    }

    glEnd();

    glPopMatrix();
}

int main()
{
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL C Demo", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glOrtho(-1, 1, -1, 1, -1, 1); // 2D

    Triangle tri = { {{0, 0}, 0, {1, 1}} };
    Square sq = { {{-0.5f, -0.5f}, 0, {1, 1}} };
    Circle cir = { {{0.5f, 0.5f}, 0, {1, 1}}, 32 };

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();

        // 애니메이션 예시
        tri.transform.position.x = sinf(time);
        sq.transform.rotation = time;
        cir.transform.scale.x = 1.0f + 0.5f * sinf(time);

        glClear(GL_COLOR_BUFFER_BIT);

        draw_triangle(&tri);
        draw_square(&sq);
        draw_circle(&cir);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

/*
코드 설명
이 C++ 코드는 이전 예제에서 한 단계 더 나아가, 구조체(struct)를 사용해 여러 도형(삼각형, 사각형, 원)을 객체처럼 관리하고, 각 도형에 **독립적인 애니메이션(이동, 회전, 크기 조절)**을 적용하는 방법을 보여줍니다.

GLFW와 구형(legacy) OpenGL을 사용한 동적 2D 그래픽 렌더링의 좋은 예시입니다.

핵심 개념
이 코드를 이해하기 위한 두 가지 핵심 개념이 있습니다.

객체 지향적 데이터 구조 (C언어 스타일): struct를 사용해 각 도형의 속성(위치, 회전, 크기)을 하나의 묶음(Transform)으로 관리합니다. 이로 인해 코드가 더 체계적이고 확장하기 쉬워집니다.
변환 행렬 스택 (glPushMatrix, glPopMatrix): 각 도형을 그리기 전에 현재의 좌표계 상태를 저장(glPushMatrix)하고, 해당 도형에만 이동/회전/크기 변환을 적용한 후, 그리기가 끝나면 원래 좌표계 상태로 복원(glPopMatrix)합니다. 이것이 각 도형이 다른 도형에 영향을 주지 않고 독립적으로 움직일 수 있게 하는 핵심 원리입니다.
코드 상세 분석
1. 구조체 정의
Vec2: 2차원 벡터를 나타내며, x와 y 좌표를 가집니다. 위치(position)나 크기(scale)를 표현하는 데 사용됩니다.
Transform: 그래픽 객체의 기하학적 변환에 필요한 모든 정보를 담습니다.
position (Vec2): 객체의 위치.
rotation (float): 객체의 회전 각도 (단위: 라디안(radian)).
scale (Vec2): 객체의 x, y축 크기 배율.
Triangle, Square, Circle: 각각의 도형을 나타내는 구조체입니다. 모두 Transform 구조체를 멤버로 포함하여 자신의 위치, 회전, 크기 정보를 가집니다. Circle은 추가로 segments라는 멤버를 가져 원을 몇 개의 삼각형으로 쪼개어 그릴지 결정합니다.
2. 도형 그리기 함수 (draw_triangle, draw_square, draw_circle)
세 함수 모두 비슷한 구조를 가집니다. draw_triangle을 예로 들면 다음과 같습니다.

C++

void draw_triangle(Triangle* tri)
{
    glPushMatrix(); // 1. 현재 좌표계 상태 저장

    // 2. 이 삼각형에만 적용될 변환 설정
    // 위치 이동
    glTranslatef(tri->transform.position.x, tri->transform.position.y, 0.0f);
    // 회전 (라디안 -> 각도 변환)
    glRotatef(tri->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    // 크기 조절
    glScalef(tri->transform.scale.x, tri->transform.scale.y, 1.0f);

    // 3. (0,0)을 중심으로 기본 모델 그리기
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); // 빨간색
    glVertex2f(0.0f, 0.5f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();

    glPopMatrix(); // 4. 저장했던 좌표계 상태로 복원
}
draw_square는 사각형을, draw_circle은 GL_TRIANGLE_FAN을 이용해 중심점을 공유하는 여러 개의 작은 삼각형을 그려 원을 표현합니다.
3. main 함수
초기화: 이전 코드와 동일하게 GLFW 창을 생성하고 OpenGL 컨텍스트를 설정합니다.

객체 생성 및 초기화:

C++

Triangle tri = { {{0, 0}, 0, {1, 1}} };
Square sq = { {{-0.5f, -0.5f}, 0, {1, 1}} };
Circle cir = { {{0.5f, 0.5f}, 0, {1, 1}}, 32 };
삼각형, 사각형, 원 객체를 각각 생성하고 초기 위치, 회전, 크기 값을 설정합니다.
메인 루프 (while):

float time = (float)glfwGetTime();: 프로그램 시작 후 흐른 시간을 초 단위로 가져옵니다. 이 time 값이 애니메이션의 핵심 동력입니다.
애니메이션 로직:
C++

// 삼각형의 x 좌표를 sin 함수에 따라 -1과 1 사이로 계속 움직입니다.
tri.transform.position.x = sinf(time);

// 사각형을 시간에 따라 계속 회전시킵니다.
sq.transform.rotation = time;

// 원의 x축 크기를 sin 함수에 따라 0.5 ~ 1.5 사이로 부드럽게 커졌다 작아졌다 반복(맥박처럼)하게 합니다.
cir.transform.scale.x = 1.0f + 0.5f * sinf(time);
glClear(GL_COLOR_BUFFER_BIT): 화면을 지웁니다.
렌더링: draw_triangle, draw_square, draw_circle 함수를 호출하여 애니메이션으로 갱신된 Transform 정보를 바탕으로 각 도형을 화면에 그립니다.
glfwSwapBuffers와 glfwPollEvents로 화면을 갱신하고 이벤트를 처리합니다.
결론 📝
이 코드는 다음과 같은 중요한 개념을 보여주는 훌륭한 예제입니다.

데이터와 행위의 분리: struct로 도형의 데이터를 정의하고, draw_* 함수로 그리는 행위를 분리했습니다.
독립적인 객체 변환: glPushMatrix/glPopMatrix을 사용하여 각 객체의 변환(이동, 회전, 크기)이 다른 객체에 영향을 주지 않도록 격리했습니다.
시간 기반 애니메이션: glfwGetTime()을 이용해 시간에 따라 객체의 속성을 변화시켜 동적인 움직임을 만들어냈습니다.
결과적으로 빨간색 삼각형은 좌우로 움직이고, 녹색 사각형은 제자리에서 회전하며, 파란색 원은 크기가 커졌다 작아지는 것을 반복하는 애니메이션이 창에 표시됩니다.
*/