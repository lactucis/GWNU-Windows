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

// 시계침 그리기 함수 (끝에 화살촉 모양 추가)
void draw_hand(float length, float thickness, float r, float g, float b, float angle_degrees) {
    glPushMatrix();
    glRotatef(angle_degrees, 0.0f, 0.0f, 1.0f);
    glLineWidth(thickness);
    glColor3f(r, g, b);

    // 주된 침 라인
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);    // 침의 시작점 (중심)
    glVertex2f(0.0f, length); // 침의 끝점 (화살촉의 뾰족한 부분)
    glEnd();

    // 화살촉 ('>' 모양)
    if (length > 0.01f) { // 매우 짧은 침에는 화살촉을 그리지 않도록 조건 추가 (선택적)
        float barb_len_offset = length * 0.12f; // 화살촉의 뒤로 뻗어나가는 길이 (침 길이의 12%)
        float barb_half_width = length * 0.06f; // 화살촉의 절반 너비 (침 길이의 6%)

        // 화살촉의 뾰족한 부분은 (0, length)
        // 왼쪽 갈고리 끝점: (-barb_half_width, length - barb_len_offset)
        // 오른쪽 갈고리 끝점: (barb_half_width, length - barb_len_offset)

        glBegin(GL_LINES);
        // 뾰족한 끝점에서 왼쪽 갈고리 밑부분으로 이어지는 선
        glVertex2f(0.0f, length);
        glVertex2f(-barb_half_width, length - barb_len_offset);

        // 뾰족한 끝점에서 오른쪽 갈고리 밑부분으로 이어지는 선
        glVertex2f(0.0f, length);
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
    GLFWwindow* window = glfwCreateWindow(500, 500, "Arrow Hands Pastel Clock (500x500)", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glOrtho(-1, 1, -1, 1, -1, 1);

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

    const float start_time_offset_seconds = 1.0f * 3600.0f;

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

    while (!glfwWindowShouldClose(window)) {
        float elapsed_seconds_float = (float)glfwGetTime();
        float total_simulated_seconds_float = start_time_offset_seconds + elapsed_seconds_float;

        float sim_s_continuous = fmod(total_simulated_seconds_float, 60.0f);
        int total_simulated_seconds_int = (int)floor(total_simulated_seconds_float);
        int current_discrete_minute = (total_simulated_seconds_int / 60) % 60;
        int current_discrete_hour_of_day = (total_simulated_seconds_int / 3600);
        int current_discrete_hour_for_analog = current_discrete_hour_of_day % 12;

        float sec_angle = -(sim_s_continuous * 6.0f);
        float min_angle = -((float)current_discrete_minute * 6.0f + sim_s_continuous * 0.1f); // 분침도 초에 따라 부드럽게
        float hour_angle = -((float)current_discrete_hour_for_analog * 30.0f + (float)current_discrete_minute * 0.5f + sim_s_continuous * (0.5f / 60.0f)); // 시침도 초에 따라 더 부드럽게

        glClearColor(0.88f, 0.85f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_circle(&clockBorder, 0.65f, 0.63f, 0.68f, 1.0f, 1.0f, 1.0f);
        draw_circle(&clockFace, 0.95f, 0.93f, 0.90f, 1.05f, 1.05f, 1.05f);

        draw_clock_markers(main_dial_radius);

        draw_filled_rectangle(0.0f, digital_bg_center_y, digital_bg_width, digital_bg_height,
            0.80f, 0.78f, 0.82f);

        int display_hour = current_discrete_hour_of_day % 24;
        draw_digital_display(display_hour, current_discrete_minute,
            0.0f, digital_base_y,
            digit_char_width, digit_char_height, digit_spacing, digit_line_thickness);

        draw_circle(&centerDot, 0.40f, 0.40f, 0.45f, 1.0f, 1.0f, 1.0f);

        // 시침, 분침 그리기 (화살촉 적용됨)
        draw_hand(main_dial_radius * hour_hand_rel_len, 7.0f, 0.45f, 0.60f, 0.73f, hour_angle);
        draw_hand(main_dial_radius * minute_hand_rel_len, 5.0f, 0.45f, 0.73f, 0.63f, min_angle);

        draw_circle(&secondsSubDialBorder, 0.60f, 0.58f, 0.62f, 1.0f, 1.0f, 1.0f);
        draw_circle(&secondsSubDialFace, 0.90f, 0.86f, 0.87f, 1.05f, 1.05f, 1.05f);
        draw_circle(&subCenterDot, 0.35f, 0.35f, 0.40f, 1.0f, 1.0f, 1.0f);

        glPushMatrix();
        glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
        // 초침 그리기 (화살촉 적용됨)
        draw_hand(sub_dial_radius * second_sub_hand_rel_len, 2.5f, 0.85f, 0.55f, 0.50f, sec_angle);
        glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}