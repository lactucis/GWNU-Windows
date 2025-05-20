#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h> // bool, true, false 사용

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

void draw_circle(Circle* c, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(c->transform.position.x, c->transform.position.y, 0.0f);
    glRotatef(c->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(c->transform.scale.x, c->transform.scale.y, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(r, g, b);
    glVertex2f(0.0f, 0.0f);
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
    glBegin(GL_LINES);
    glColor3f(r, g, b);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, length);
    glEnd();
    glPopMatrix();
}

void draw_clock_markers(float radius) {
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 60; ++i) {
        glPushMatrix();
        glRotatef(-(float)i * 6.0f, 0.0f, 0.0f, 1.0f);
        glLineWidth((i % 5 == 0) ? 2.5f : 1.0f); // 시간 눈금 약간 두껍게
        glBegin(GL_LINES);
        glVertex2f(0.0f, radius * ((i % 5 == 0) ? 0.88f : 0.92f)); // 눈금 길이 조정
        glVertex2f(0.0f, radius * 0.98f);
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
    glVertex2f(center_x, y + h * 0.65f + dot_h / 2.0f); // 위쪽 점 Y 조정
    glVertex2f(center_x, y + h * 0.65f - dot_h / 2.0f);
    glVertex2f(center_x, y + h * 0.35f + dot_h / 2.0f); // 아래쪽 점 Y 조정
    glVertex2f(center_x, y + h * 0.35f - dot_h / 2.0f);
    glEnd();
}

void draw_digital_display(int hour_24, int minute, float base_x, float base_y, float digit_w, float digit_h, float spacing, float thickness) {
    glColor3f(0.15f, 0.15f, 0.15f); // 디지털 시계 색상
    int h1 = hour_24 / 10;
    int h2 = hour_24 % 10;
    int m1 = minute / 10;
    int m2 = minute % 10;

    // 전체 폭 계산 (HH : MM -> 숫자 4개, 콜론 1개(폭0.5), 간격 4개지만 콜론 양옆은 0.5씩)
    float total_width = 4 * digit_w + (digit_w * 0.5f) + 2 * spacing + 2 * (spacing * 0.75f);
    float start_x = base_x - total_width / 2.0f; // 기준점에서 중앙 정렬
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
    GLFWwindow* window = glfwCreateWindow(500, 500, "Analog & Digital Clock (500x500)", NULL, NULL); // 창 크기 500x500
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glOrtho(-1, 1, -1, 1, -1, 1);

    // --- 비율 재조정 ---
    float main_dial_radius = 0.75f; // 주 다이얼 반지름 (NDC 기준)
    Circle clockFace = { {{0.0f, 0.0f}, 0.0f, {main_dial_radius * 2.0f, main_dial_radius * 2.0f}}, 64 };
    Circle clockBorder = { {{0.0f, 0.0f}, 0.0f, {(main_dial_radius + 0.025f) * 2.0f, (main_dial_radius + 0.025f) * 2.0f}}, 64 };

    float sub_dial_scale_factor = 0.25f; // 주 다이얼 대비 보조 다이얼 크기 비율
    float sub_dial_radius = main_dial_radius * sub_dial_scale_factor;
    Vec2 subDialPosition = { -main_dial_radius * 0.6f, 0.0f }; // 주 다이얼 왼쪽

    Circle secondsSubDialFace = { {subDialPosition, 0.0f, {sub_dial_radius * 2.0f, sub_dial_radius * 2.0f}}, 32 };
    Circle secondsSubDialBorder = { {subDialPosition, 0.0f, {(sub_dial_radius + 0.01f) * 2.0f, (sub_dial_radius + 0.01f) * 2.0f}}, 32 };

    // 시계침 길이 (주 다이얼 반지름 기준 상대 길이)
    float hour_hand_rel_len = 0.45f;
    float minute_hand_rel_len = 0.65f;
    float second_sub_hand_rel_len = 0.8f; // 보조 다이얼 반지름 기준

    const float start_time_offset_seconds = 1.0f * 3600.0f; // 01:00:00 에서 시작

    // 디지털 디스플레이 설정 (시계 내부 하단)
    float digit_char_height = main_dial_radius * 0.18f; // 주 다이얼 크기에 비례
    float digit_char_width = digit_char_height * 0.6f;   // 너비는 높이의 60%
    float digit_spacing = digit_char_width * 0.3f;
    float digit_line_thickness = 2.0f;
    float digital_base_y = -main_dial_radius * 0.60f; // 주 다이얼 내부 6시 방향 위

    while (!glfwWindowShouldClose(window)) {
        float elapsed_seconds_float = (float)glfwGetTime();
        float total_simulated_seconds_float = start_time_offset_seconds + elapsed_seconds_float;

        float sim_s_continuous = fmod(total_simulated_seconds_float, 60.0f);
        int total_simulated_seconds_int = (int)floor(total_simulated_seconds_float);
        int current_discrete_minute = (total_simulated_seconds_int / 60) % 60;
        int current_discrete_hour_of_day = (total_simulated_seconds_int / 3600);
        int current_discrete_hour_for_analog = current_discrete_hour_of_day % 12;

        float sec_angle = -(sim_s_continuous * 6.0f);
        float min_angle = -((float)current_discrete_minute * 6.0f);
        float hour_angle = -((float)current_discrete_hour_for_analog * 30.0f);

        glClearColor(0.9f, 0.9f, 0.92f, 1.0f); // 배경색
        glClear(GL_COLOR_BUFFER_BIT);

        draw_circle(&clockBorder, 0.5f, 0.5f, 0.5f);
        draw_circle(&clockFace, 0.98f, 0.98f, 0.98f);
        draw_clock_markers(main_dial_radius);

        draw_hand(main_dial_radius * hour_hand_rel_len, 6.5f, 0.1f, 0.1f, 0.1f, hour_angle);
        draw_hand(main_dial_radius * minute_hand_rel_len, 4.5f, 0.2f, 0.2f, 0.2f, min_angle);

        draw_circle(&secondsSubDialBorder, 0.4f, 0.4f, 0.4f);
        draw_circle(&secondsSubDialFace, 0.9f, 0.9f, 0.9f);

        glPushMatrix();
        glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
        draw_hand(sub_dial_radius * second_sub_hand_rel_len, 2.0f, 0.9f, 0.1f, 0.1f, sec_angle);
        glPopMatrix();

        int display_hour = current_discrete_hour_of_day % 24;
        draw_digital_display(display_hour, current_discrete_minute,
            0.0f, digital_base_y, // 디지털 시계 기준 X는 0 (중앙), Y는 설정값
            digit_char_width, digit_char_height, digit_spacing, digit_line_thickness);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}