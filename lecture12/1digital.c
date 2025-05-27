#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h> // bool, true, false 사용
#include <time.h>

#define PI 3.1415926535f

// 7-segment patterns for digits 0-9
const bool SEVEN_SEGMENT_PATTERNS[10][7] = {
    {true,true,true,true,true,true,false}, // 0
    {false,true,true,false,false,false,false}, // 1
    {true,true,false,true,true,false,true}, // 2
    {true,true,true,true,false,false,true}, // 3
    {false,true,true,false,false,true,true}, // 4
    {true,false,true,true,false,true,true}, // 5
    {true,false,true,true,true,true,true}, // 6
    {true,true,true,false,false,false,false}, // 7
    {true,true,true,true,true,true,true}, // 8
    {true,true,true,true,false,true,true}  // 9
};

typedef struct { float x, y; } Vec2;
typedef struct { Vec2 position; float rotation; Vec2 scale; } Transform;
typedef struct { Transform transform; int segments; } Circle;

// 원 그리기 함수 (그라데이션 옵션 추가)
void draw_circle(Circle* c, float r_edge, float g_edge, float b_edge, float r_center_mult, float g_center_mult, float b_center_mult) {
    glPushMatrix();
    glTranslatef(c->transform.position.x, c->transform.position.y, 0.0f);
    glRotatef(c->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(c->transform.scale.x, c->transform.scale.y, 1.0f);

    glBegin(GL_TRIANGLE_FAN);

    float cr = r_edge * r_center_mult; if (cr > 1.0f) cr = 1.0f; if (cr < 0.0f) cr = 0.0f;
    float cg = g_edge * g_center_mult; if (cg > 1.0f) cg = 1.0f; if (cg < 0.0f) cg = 0.0f;
    float cb = b_edge * b_center_mult; if (cb > 1.0f) cb = 1.0f; if (cb < 0.0f) cb = 0.0f;
    glColor3f(cr, cg, cb);
    glVertex2f(0.0f, 0.0f);

    glColor3f(r_edge, g_edge, b_edge);
    for (int i = 0; i <= c->segments; ++i) {
        float angle = 2.0f * PI * i / c->segments;
        glVertex2f(cosf(angle) * 0.5f, sinf(angle) * 0.5f);
    }
    glEnd();
    glPopMatrix();
}

void draw_hand(float length, float thickness, float r, float g, float b, float angle_degrees) {
    glPushMatrix();
    glRotatef(angle_degrees, 0.0f, 0.0f, 1.0f);
    glLineWidth(thickness);
    glColor3f(r, g, b);

    // 주된 침 라인
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);   // 침의 시작점 (중심)
    glVertex2f(0.0f, length); // 침의 끝점 (화살촉의 뾰족한 부분)
    glEnd();

    // 화살촉 (속이 채워진 삼각형 '>' 모양)
    if (length > 0.01f) {
        // 화살촉의 크기를 시침/분침/초침에 따라 다르게 조절할 수 있습니다.
        // 예를 들어 시침은 더 뭉툭하게 만들 수 있습니다.
        float barb_len_offset = length * 0.15f; // 화살촉의 길이 (침 길이의 15%)
        float barb_half_width = length * 0.10f; // 화살촉의 절반 너비 (침 길이의 10%)

        glBegin(GL_TRIANGLES); // ★ 핵심: GL_LINES 대신 GL_TRIANGLES 사용

        // 삼각형을 구성하는 세 꼭짓점
        // 1. 뾰족한 끝점
        glVertex2f(0.0f, length + 0.03f);
        // 2. 왼쪽 갈고리 밑부분
        glVertex2f(-barb_half_width, length - barb_len_offset);
        // 3. 오른쪽 갈고리 밑부분
        glVertex2f(barb_half_width, length - barb_len_offset);

        glEnd();
    }
    glPopMatrix();
}


void draw_clock_markers(float radius) {
    glColor3f(0.50f, 0.45f, 0.55f);
    for (int i = 0; i < 60; ++i) {
        glPushMatrix();
        glRotatef(-(float)i * 6.0f, 0.0f, 0.0f, 1.0f);
        glLineWidth((i % 5 == 0) ? 2.8f : 1.2f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, radius * ((i % 5 == 0) ? 0.87f : 0.91f));
        glVertex2f(0.0f, radius * 0.97f);
        glEnd();
        glPopMatrix();
    }
}

void draw_seven_segment_digit(int digit, float x, float y, float w, float h, float thickness) {
    if (digit < 0 || digit > 9) return;
    glLineWidth(thickness);
    glBegin(GL_LINES);
    const bool* segments = SEVEN_SEGMENT_PATTERNS[digit];
    float l = x, r = x + w, b = y, t = y + h, my = y + h / 2.0f;
    if (segments[0]) { glVertex2f(l, t); glVertex2f(r, t); }
    if (segments[1]) { glVertex2f(r, my); glVertex2f(r, t); }
    if (segments[2]) { glVertex2f(r, b); glVertex2f(r, my); }
    if (segments[3]) { glVertex2f(l, b); glVertex2f(r, b); }
    if (segments[4]) { glVertex2f(l, b); glVertex2f(l, my); }
    if (segments[5]) { glVertex2f(l, my); glVertex2f(l, t); }
    if (segments[6]) { glVertex2f(l, my); glVertex2f(r, my); }
    glEnd();
}

void draw_digital_colon(float x, float y, float w, float h, float thickness) {
    glLineWidth(thickness);
    glBegin(GL_LINES);
    float dot_h = h * 0.20f;
    float center_x = x + w / 2.0f;
    glVertex2f(center_x, y + h * 0.65f + dot_h / 2.0f);
    glVertex2f(center_x, y + h * 0.65f - dot_h / 2.0f);
    glVertex2f(center_x, y + h * 0.35f + dot_h / 2.0f);
    glVertex2f(center_x, y + h * 0.35f - dot_h / 2.0f);
    glEnd();
}

void draw_filled_rectangle(float center_x, float center_y, float width, float height, float r_col, float g_col, float b_col) {
    glColor3f(r_col, g_col, b_col);
    glBegin(GL_QUADS);
    glVertex2f(center_x - width / 2.0f, center_y - height / 2.0f);
    glVertex2f(center_x + width / 2.0f, center_y - height / 2.0f);
    glVertex2f(center_x + width / 2.0f, center_y + height / 2.0f);
    glVertex2f(center_x - width / 2.0f, center_y + height / 2.0f);
    glEnd();
}

void draw_digital_display(int hour_24, int minute, float base_x, float base_y, float digit_w, float digit_h, float spacing, float thickness) {
    glColor3f(0.35f, 0.40f, 0.50f);
    int h1 = hour_24 / 10;
    int h2 = hour_24 % 10;
    int m1 = minute / 10;
    int m2 = minute % 10;

    float total_width = 4 * digit_w + (digit_w * 0.5f) + 2 * spacing + 2 * (spacing * 0.75f);
    float start_x = base_x - total_width / 2.0f;
    float current_x = start_x;

    draw_seven_segment_digit(h1, current_x, base_y, digit_w, digit_h, thickness);
    current_x += digit_w + spacing;
    draw_seven_segment_digit(h2, current_x, base_y, digit_w, digit_h, thickness);
    current_x += digit_w + spacing * 0.75f;
    draw_digital_colon(current_x, base_y, digit_w * 0.5f, digit_h, thickness);
    current_x += digit_w * 0.5f + spacing * 0.75f;
    draw_seven_segment_digit(m1, current_x, base_y, digit_w, digit_h, thickness);
    current_x += digit_w + spacing;
    draw_seven_segment_digit(m2, current_x, base_y, digit_w, digit_h, thickness);
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(500, 500, "Arrow Hands Pastel Clock (500x500) - Real Time", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glOrtho(-1, 1, -1, 1, -1, 1);

    // --- 시계 요소들의 크기 및 위치 설정 (기존과 동일) ---
    float main_dial_radius = 0.75f;
    Circle clockBorder = { {{0.0f, 0.0f}, 0.0f, {(main_dial_radius + 0.035f) * 2.0f, (main_dial_radius + 0.035f) * 2.0f}}, 64 };
    Circle clockFace = { {{0.0f, 0.0f}, 0.0f, {main_dial_radius * 2.0f, main_dial_radius * 2.0f}}, 64 };
    Circle centerDot = { {{0.0f, 0.0f}, 0.0f, {0.04f, 0.04f}}, 16 };

    float sub_dial_scale_factor = 0.25f;
    float sub_dial_radius = main_dial_radius * sub_dial_scale_factor;
    Vec2 subDialPosition = { -main_dial_radius * 0.5f, main_dial_radius * 0.05f };

    Circle secondsSubDialBorder = { {subDialPosition, 0.0f, {(sub_dial_radius + 0.015f) * 2.0f, (sub_dial_radius + 0.015f) * 2.0f}}, 32 };
    Circle secondsSubDialFace = { {subDialPosition, 0.0f, {sub_dial_radius * 2.0f, sub_dial_radius * 2.0f}}, 32 };
    Circle subCenterDot = { {subDialPosition, 0.0f, {0.02f, 0.02f}}, 16 };

    float hour_hand_rel_len = 0.45f;
    float minute_hand_rel_len = 0.65f;
    float second_sub_hand_rel_len = 0.8f;

    float digit_char_height = main_dial_radius * 0.18f;
    float digit_char_width = digit_char_height * 0.6f;
    float digit_spacing = digit_char_width * 0.3f;
    float digit_line_thickness = 2.5f;
    float digital_base_y = -main_dial_radius * 0.60f;

    float digital_display_total_width = 4 * digit_char_width + (digit_char_width * 0.5f) + 2 * digit_spacing + 2 * (digit_spacing * 0.75f);
    float digital_bg_padding_x = digit_char_width * 0.3f;
    float digital_bg_padding_y = digit_char_height * 0.25f;
    float digital_bg_width = digital_display_total_width + digital_bg_padding_x;
    float digital_bg_height = digit_char_height + digital_bg_padding_y;
    float digital_bg_center_y = digital_base_y + digit_char_height / 2.0f;

    // main 함수 내부의 while 루프 전체를 이 코드로 교체하세요.
    while (!glfwWindowShouldClose(window)) {
        // --- 실시간 정보 가져오기 ---
        time_t now = time(NULL);
        struct tm* local_time = localtime(&now);

        int current_hour_24 = local_time->tm_hour;
        int current_minute = local_time->tm_min;
        int current_second = local_time->tm_sec;

        // --- 각도 계산 ---
        float sec_angle = -((float)current_second * 6.0f); // 초침: 1초에 한 칸씩 이동

        // ★★★ 분침 각도 계산 수정 ★★★
        // 분침은 '분'이 바뀔 때만 한 칸씩 움직이도록 정수 값인 current_minute만 사용합니다.
        float min_angle = -((float)current_minute * 6.0f);

        // 시침은 '분'의 흐름에 따라 계속 부드럽게 움직입니다.
        float hour_angle = -((float)(current_hour_24 % 12) * 30.0f + (float)current_minute * 0.5f);

        // --- 렌더링 (그리기) ---
        glClearColor(0.88f, 0.85f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 1. 시계의 가장 기본 배경 요소들을 먼저 그립니다.
        draw_circle(&clockBorder, 0.65f, 0.63f, 0.68f, 1.0f, 1.0f, 1.0f);
        draw_circle(&clockFace, 0.95f, 0.93f, 0.90f, 1.05f, 1.05f, 1.05f);
        draw_clock_markers(main_dial_radius);
        draw_filled_rectangle(0.0f, digital_bg_center_y, digital_bg_width, digital_bg_height,
            0.80f, 0.78f, 0.82f);

        // 2. 초침 서브 다이얼을 시침/분침보다 먼저 그립니다.
        draw_circle(&secondsSubDialBorder, 0.60f, 0.58f, 0.62f, 1.0f, 1.0f, 1.0f);
        draw_circle(&secondsSubDialFace, 0.90f, 0.86f, 0.87f, 1.05f, 1.05f, 1.05f);
        draw_circle(&subCenterDot, 0.35f, 0.35f, 0.40f, 1.0f, 1.0f, 1.0f);

        glPushMatrix();
        glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
        draw_hand(sub_dial_radius * second_sub_hand_rel_len, 2.5f, 0.85f, 0.55f, 0.50f, sec_angle);
        glPopMatrix();

        // 3. 디지털 숫자와 메인 중심점을 그립니다.
        draw_digital_display(current_hour_24, current_minute,
            0.0f, digital_base_y,
            digit_char_width, digit_char_height, digit_spacing, digit_line_thickness);
        draw_circle(&centerDot, 0.40f, 0.40f, 0.45f, 1.0f, 1.0f, 1.0f);

        // 4. 가장 마지막에 시침과 분침을 그려서 모든 요소들 위로 올라오게 합니다.
        draw_hand(main_dial_radius * hour_hand_rel_len, 7.0f, 0.45f, 0.60f, 0.73f, hour_angle);
        draw_hand(main_dial_radius * minute_hand_rel_len, 5.0f, 0.45f, 0.73f, 0.63f, min_angle);

        // --- 버퍼 교체 및 이벤트 처리 ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

/*
코드 설명
이 C++ 코드는 이전 예제들의 개념을 모두 종합하여, 실시간으로 동작하는 정교한 하이브리드(아날로그 + 디지털) 시계를 구현한 매우 완성도 높은 프로그램입니다.

GLFW와 구형 OpenGL을 사용하여, 현재 컴퓨터의 시스템 시간을 받아와 파스텔 톤의 미려한 시계 화면에 표시합니다.

전체적인 기능 및 특징
실시간 동작: <time.h> 라이브러리를 사용해 현재 시간을 실시간으로 가져와 시계에 반영합니다.
하이브리드 디스플레이: 아날로그 시계(시침, 분침)와 디지털 시계(HH:MM 형식)를 한 화면에 모두 표시합니다.
정교한 시각적 요소:
화살촉 모양 시계침: draw_hand 함수는 단순한 선이 아닌, 끝이 채워진 삼각형(화살촉) 모양의 시계침을 그립니다.
초침 서브 다이얼: 초침은 메인 시계판이 아닌, 별도의 작은 원(서브 다이얼) 안에서 움직입니다.
7세그먼트 디지털 폰트: SEVEN_SEGMENT_PATTERNS 배열을 기반으로 디지털 숫자를 직접 그려냅니다.
계층적 렌더링: 배경, 다이얼, 시계침 등을 순서대로 그려 겹치는 부분을 자연스럽게 표현합니다.
파스텔 톤과 그라데이션: draw_circle 함수는 중심과 가장자리의 색을 다르게 지정하여 은은한 그라데이션 효과를 줍니다.
코드 상세 분석
1. 핵심 데이터 및 구조체
SEVEN_SEGMENT_PATTERNS: 0부터 9까지의 숫자를 7세그먼트 디스플레이로 표현하기 위한 true/false 패턴을 정의한 2차원 배열입니다. 디지털 시간 표시에 사용되는 '폰트' 역할을 합니다.
구조체 (Vec2, Transform, Circle): 이전 예제와 동일하게 도형의 위치, 회전, 크기 등 기하학적 정보를 관리하는 데 사용됩니다.
2. 주요 그리기 함수 (Building Blocks)
draw_circle(...): 원을 그리는 함수지만, 중심 색상과 가장자리 색상을 다르게 설정할 수 있어 입체감 있는 그라데이션 효과를 줍니다. 시계판, 서브 다이얼 등에 사용됩니다.
draw_hand(...): 이 시계의 핵심적인 시각 요소를 담당합니다.
glRotatef로 각도에 맞게 좌표계를 회전시킵니다.
glBegin(GL_LINES)로 침의 중심선을 그립니다.
glBegin(GL_TRIANGLES)로 침의 끝부분에 속이 채워진 삼각형을 그려 화살촉 모양을 완성합니다.
draw_clock_markers(): 60개의 눈금을 그립니다. i % 5 == 0 조건을 사용해 5분 단위의 눈금을 더 굵고 길게 그려 가독성을 높입니다.
draw_seven_segment_digit(...): 숫자 하나와 위치, 크기 등을 인자로 받아 SEVEN_SEGMENT_PATTERNS를 참조하여 선분들을 그려 숫자를 표시합니다.
draw_digital_display(...): 관리자(Manager) 역할의 함수입니다. 시(hour)와 분(minute) 값을 받아 각 자릿수를 계산하고, draw_seven_segment_digit와 draw_digital_colon 함수를 적절히 호출하여 HH:MM 형식의 완전한 디지털 시간을 화면에 그립니다.
draw_filled_rectangle(...): 디지털 시계의 배경이 되는 사각형을 그립니다.
3. main 함수 (프로그램 실행 흐름)
초기화 및 설정:

GLFW 창을 생성하고 glOrtho로 2D 좌표계를 설정합니다.
시계의 각 구성 요소(시계판, 서브 다이얼, 시계침 길이, 디지털 숫자 크기 등)의 크기와 위치를 변수로 미리 계산하고 설정합니다. 모든 크기가 main_dial_radius에 상대적으로 정의되어 있어 전체적인 크기 조절이 용이합니다.
메인 루프 (while):

시간 정보 획득:

C

time_t now = time(NULL);
struct tm* local_time = localtime(&now);
int current_hour_24 = local_time->tm_hour;
// ... 분, 초 정보 획득
루프가 돌 때마다 현재 시스템 시간을 가져옵니다.

각도 계산:

sec_angle, min_angle, hour_angle을 계산합니다.
특히 시침(hour_angle)은 분의 흐름(current_minute)에 따라 부드럽게 움직이도록 계산되어 실제 아날로그 시계처럼 동작합니다.
분침(min_angle)은 current_minute 정수 값만 사용하여 1분에 한 칸씩 명확하게 움직입니다.
렌더링 (그리기 순서):
glClear로 화면을 파스텔 톤의 배경색으로 지운 뒤, 뒤에 있는 요소부터 순서대로 그려 올바른 레이어를 만듭니다. (화가 알고리즘)

배경 요소: 메인 시계 테두리, 시계판, 눈금, 디지털 시계 배경 사각형.
서브 다이얼: 초침이 그려질 작은 시계판과 그 중심점, 그리고 회전하는 초침.
전경 요소 (중간): 디지털 숫자들과 메인 시계의 중심점.
최상위 요소: 시침과 분침을 가장 마지막에 그려 다른 모든 요소들 위에 표시되도록 합니다.
화면 업데이트: glfwSwapBuffers와 glfwPollEvents로 최종 이미지를 화면에 표시하고 이벤트를 처리합니다.

결론 📝
이 코드는 단순한 도형 그리기를 넘어, 여러 모듈화된 함수와 실시간 데이터를 결합하여 하나의 완성된 애플리케이션(하이브리드 시계)을 만드는 과정을 잘 보여줍니다. 각 부분을 독립적인 함수로 구현하고, main 함수에서는 이들을 조립하여 원하는 결과를 만들어내는 체계적인 프로그래밍 방식을 배울 수 있는 훌륭한 예제입니다.
*/