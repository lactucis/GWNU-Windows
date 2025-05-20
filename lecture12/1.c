#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>      // GLFW 라이브러리(윈도우 생성 및 입력, OpenGL context 제공)
#include <math.h>          // 수학 함수(sin, cos, fmod, floor 등) 사용

#define PI 3.1415926535f      // 원주율(파이) 상수 정의, f는 float형

// 2차원 벡터 구조체
typedef struct
{
    float x, y;              // x, y 좌표값
} Vec2;

// 변환(Transformation) 정보 구조체
typedef struct
{
    Vec2 position;           // 위치 (x, y)
    float rotation;          // 회전 각도(라디안)
    Vec2 scale;              // 크기(x, y 스케일)
} Transform;

// 원 구조체: 변환 정보와 세그먼트 개수 가짐
typedef struct
{
    Transform transform;     // 원의 변환 정보
    int segments;            // 원을 몇 개의 점(삼각형)으로 그릴지
} Circle;

// 원 그리기 함수
void draw_circle(Circle* c, float r, float g, float b)
{
    glPushMatrix(); // 현재 변환행렬 저장

    glTranslatef(c->transform.position.x, c->transform.position.y, 0.0f);
    glRotatef(c->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(c->transform.scale.x, c->transform.scale.y, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(r, g, b);
    glVertex2f(0.0f, 0.0f);

    for (int i = 0; i <= c->segments; ++i)
    {
        float angle = 2.0f * PI * i / c->segments;
        glVertex2f(cosf(angle) * 0.5f, sinf(angle) * 0.5f); // 내부적으로 반지름 0.5로 그림
    }
    glEnd();
    glPopMatrix();
}

// 시계침 그리기 함수
void draw_hand(float length, float thickness, float r, float g, float b, float angle_degrees)
{
    glPushMatrix();
    glRotatef(angle_degrees, 0.0f, 0.0f, 1.0f); // Z축 기준 회전

    glLineWidth(thickness);
    glBegin(GL_LINES);
    glColor3f(r, g, b);
    glVertex2f(0.0f, 0.0f);    // 침의 시작점 (현재 원점)
    glVertex2f(0.0f, length);  // 침의 끝점 (Y축 양의 방향으로 그림)
    glEnd();

    glPopMatrix();
}

// 주 시계 눈금 그리기 함수
void draw_clock_markers(float radius)
{
    glColor3f(0.0f, 0.0f, 0.0f); // 검은색 눈금

    for (int i = 0; i < 60; ++i) {
        glPushMatrix();
        glRotatef(-(float)i * 6.0f, 0.0f, 0.0f, 1.0f); // 각 눈금 위치로 회전

        if (i % 5 == 0) { // 시간 눈금
            glLineWidth(3.0f);
            glBegin(GL_LINES);
            glVertex2f(0.0f, radius * 0.90f);
            glVertex2f(0.0f, radius * 0.98f);
            glEnd();
        }
        else { // 분 눈금
            glLineWidth(1.0f);
            glBegin(GL_LINES);
            glVertex2f(0.0f, radius * 0.94f);
            glVertex2f(0.0f, radius * 0.98f);
            glEnd();
        }
        glPopMatrix();
    }
}


int main()
{
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(500, 500, "Analog Clock - Stepped Hands", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glOrtho(-1, 1, -1, 1, -1, 1); // 2D 좌표계 설정

    // 주 시계판 정의
    Circle clockFace = { {{0.0f, 0.0f}, 0.0f, {1.5f, 1.5f}}, 64 };
    Circle clockBorder = { {{0.0f, 0.0f}, 0.0f, {1.55f, 1.55f}}, 64 };

    // 초침용 보조 다이얼 정의
    Vec2 subDialPosition = { -0.4f, 0.0f };
    float subDialScaleValue = 0.4f;

    Circle secondsSubDialFace = { {subDialPosition, 0.0f, {subDialScaleValue, subDialScaleValue}}, 32 };
    Circle secondsSubDialBorder = { {subDialPosition, 0.0f, {subDialScaleValue + 0.02f, subDialScaleValue + 0.02f}}, 32 };

    const float start_time_offset_seconds = 1.0f * 3600.0f; // 1시 0분 0초부터 시작

    while (!glfwWindowShouldClose(window))
    {
        float elapsed_seconds_float = (float)glfwGetTime();
        float total_simulated_seconds_float = start_time_offset_seconds + elapsed_seconds_float;

        // 초침은 부드럽게 움직이도록 소수점 시간 사용
        float sim_s_continuous = fmod(total_simulated_seconds_float, 60.0f);

        // 분침과 시침은 정분/정시에 움직이도록 정수 시간 사용
        int total_simulated_seconds_int = (int)floor(total_simulated_seconds_float);

        int current_discrete_minute = (total_simulated_seconds_int / 60) % 60;
        // 시(hour)는 12시간제를 기준으로 계산 (0시 ~ 11시).
        // 예를 들어, 프로그램 시작 후 0초 (시뮬레이션 시간 1시 0분 0초):
        // total_simulated_seconds_int = 3600
        // current_discrete_hour_of_day = (3600 / 3600) = 1
        // current_discrete_hour = 1 % 12 = 1 (1시를 의미)
        //
        // 프로그램 시작 후 11시간 (시뮬레이션 시간 12시 0분 0초):
        // total_simulated_seconds_int = 3600 (오프셋) + 11 * 3600 = 12 * 3600
        // current_discrete_hour_of_day = (12 * 3600 / 3600) = 12
        // current_discrete_hour = 12 % 12 = 0 (12시를 의미, 각도 계산 시 0은 12시 위치)
        int current_discrete_hour_of_day = (total_simulated_seconds_int / 3600);
        int current_discrete_hour = current_discrete_hour_of_day % 12;


        // 각도 계산
        float sec_angle = -(sim_s_continuous * 6.0f); // 초침은 부드럽게
        float min_angle = -((float)current_discrete_minute * 6.0f); // 분침은 정분마다 이동
        float hour_angle = -((float)current_discrete_hour * 30.0f); // 시침은 정시마다 이동 (분의 영향 없음)


        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_circle(&clockBorder, 0.4f, 0.4f, 0.4f);
        draw_circle(&clockFace, 1.0f, 1.0f, 1.0f);
        draw_clock_markers(0.75f);

        draw_hand(0.4f, 6.0f, 0.0f, 0.0f, 0.0f, hour_angle);
        draw_hand(0.6f, 4.0f, 0.2f, 0.2f, 0.2f, min_angle);

        draw_circle(&secondsSubDialBorder, 0.3f, 0.3f, 0.3f);
        draw_circle(&secondsSubDialFace, 0.85f, 0.85f, 0.85f);

        glPushMatrix();
        glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
        float seconds_hand_length = (subDialScaleValue * 0.5f) * 0.9f;
        draw_hand(seconds_hand_length, 1.5f, 1.0f, 0.0f, 0.0f, sec_angle);
        glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}