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


/*
코드 설명: 단계별로 움직이는 아날로그 시뮬레이션 시계
이 C++ 코드는 GLFW와 구형 OpenGL을 사용하여, 시침과 분침은 정시/정분에 "똑딱"거리며 단계별로 움직이고, 초침은 부드럽게 움직이는 클래식한 아날로그 시계를 구현합니다.

이전 예제와 가장 큰 차이점은 실제 시스템 시간을 사용하는 대신, 프로그램 시작 후 경과된 시간을 기반으로 하는 시뮬레이션 시계라는 점과, 시계침의 움직임 유형(부드러움/단계적)을 분리하여 구현했다는 점입니다.

핵심 특징
시뮬레이션 시간: <time.h> 대신 glfwGetTime()을 사용하여 프로그램 시작 후 흐른 시간을 기반으로 시간을 시뮬레이션합니다. start_time_offset_seconds 변수를 통해 시계가 1시 정각부터 시작하도록 설정했습니다.
움직임 유형 분리 (Stepped vs. Smooth): 이 코드의 가장 중요한 특징입니다.
초침: 소수점까지 계산된 시간(float)을 사용하여 부드럽게 물 흐르듯 움직입니다.
시침/분침: 시간을 정수(int)로 변환하여 단계적으로(Stepped) 움직입니다. 분침은 1분에 한 번, 시침은 1시간에 한 번 위치가 바뀝니다.
메인 다이얼과 서브 다이얼: 시침과 분침은 큰 메인 다이얼에, 초침은 별도의 작은 서브 다이얼(보조 시계판)에 그려집니다.
모듈화된 그리기 함수: 원, 시계침, 눈금을 그리는 함수가 각각 명확하게 분리되어 있어 코드의 구조가 깔끔하고 이해하기 쉽습니다.
코드 상세 분석
1. 그리기 함수
draw_circle(): 단색으로 채워진 원을 그립니다. 시계판과 테두리를 그리는 데 사용됩니다.
draw_hand(): 주어진 각도에 맞게 단순한 선 형태의 시계침을 그립니다. 이전 예제의 화살촉 모양 대신 클래식한 디자인입니다.
draw_clock_markers(): 60개의 눈금을 그립니다. i % 5 == 0 조건을 이용해 시간과 분 눈금의 굵기와 길이를 다르게 하여 가독성을 높입니다.
2. main 함수 - 시간 계산 로직 (핵심)
이 코드의 핵심은 while 루프 안의 시간 계산 로직에 있습니다.

시뮬레이션 시간 계산:

C

float elapsed_seconds_float = (float)glfwGetTime(); // 프로그램 시작 후 경과 시간
float total_simulated_seconds_float = start_time_offset_seconds + elapsed_seconds_float; // 시작 오프셋을 더해 총 시뮬레이션 시간 계산
시계는 1시(3600초)에서 시작하여, 실제 시간이 흐른 만큼 더해진 시간을 기준으로 움직입니다.

부드러운 움직임 vs 단계적 움직임을 위한 시간 분리:

초침용 (부드러운 움직임):

C

float sim_s_continuous = fmod(total_simulated_seconds_float, 60.0f);
fmod 함수는 실수의 나머지 연산을 수행합니다. total_simulated_seconds_float 값은 소수점을 포함하므로, sim_s_continuous는 0.0 ~ 59.99... 사이를 부드럽게 순환합니다. 이 값은 초침의 각도를 계산하는 데 사용됩니다.

시침/분침용 (단계적 움직임):

C

int total_simulated_seconds_int = (int)floor(total_simulated_seconds_float);
int current_discrete_minute = (total_simulated_seconds_int / 60) % 60;
int current_discrete_hour = ((total_simulated_seconds_int / 3600)) % 12;
floor 함수로 소수점 아래를 버리고 정수(int)로 변환하여 "똑딱"거리는 효과를 만듭니다. 이 정수 시간을 기준으로 현재의 '분'과 '시'를 정확히 계산합니다.

각도 계산:

C

float sec_angle = -(sim_s_continuous * 6.0f);                  // 부드러운 시간 값 사용
float min_angle = -((float)current_discrete_minute * 6.0f);  // 정수 분 값 사용
float hour_angle = -((float)current_discrete_hour * 30.0f); // 정수 시 값 사용
각도 계산 시, 초침은 연속적인 float 값을, 분침과 시침은 이산적인 int 값을 사용하여 움직임의 특성을 분리합니다.
(참고: 이 코드의 시침은 분의 흐름을 반영하지 않고 정시에만 움직입니다.)

3. main 함수 - 렌더링 순서
배경부터 순서대로 그려 겹치는 부분을 올바르게 표현합니다.

배경색으로 화면을 지웁니다.
메인 시계의 테두리와 흰색 시계판을 그립니다.
시계 눈금을 그립니다.
시침과 분침을 그립니다.
초침용 서브 다이얼의 테두리와 시계판을 그립니다.
초침 그리기 (중요):
C

glPushMatrix();
// 좌표계를 서브 다이얼의 중심으로 이동
glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
// 이동된 좌표계의 원점을 기준으로 초침을 그림
draw_hand(seconds_hand_length, 1.5f, 1.0f, 0.0f, 0.0f, sec_angle);
glPopMatrix();
glPushMatrix / glPopMatrix 블록을 사용하여 초침이 서브 다이얼의 중심을 기준으로 회전하도록 합니다. glTranslatef를 통해 드로잉 원점을 일시적으로 서브 다이얼의 중심으로 옮겼다가, 그리기가 끝나면 glPopMatrix으로 원래 원점으로 복귀시킵니다.
결론 📝
이 코드는 서로 다른 애니메이션 스타일(부드러운 움직임과 단계적 움직임)을 하나의 객체(시계)에 동시에 적용하는 방법을 보여주는 훌륭한 예제입니다. 시간 데이터를 float과 int로 구분하여 처리함으로써 원하는 움직임을 정교하게 제어하는 기법을 배울 수 있습니다.
*/