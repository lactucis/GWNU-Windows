#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h> // rand(), srand()
#include <time.h>   // time() for srand()

#define PI 3.1415926535f

// --- �� ��� ���� ---
#define NUM_STARS 150
typedef struct { float x, y, z; float brightness; float pulse_speed; } Star;
Star stars[NUM_STARS];

// 7-segment patterns for digits 0-9
const bool SEVEN_SEGMENT_PATTERNS[10][7] = {
    {true,true,true,true,true,true,false}, {false,true,true,false,false,false,false},
    {true,true,false,true,true,false,true}, {true,true,true,true,false,false,true},
    {false,true,true,false,false,true,true}, {true,false,true,true,false,true,true},
    {true,false,true,true,true,true,true}, {true,true,true,false,false,false,false},
    {true,true,true,true,true,true,true}, {true,true,true,true,false,true,true}
};

typedef struct { float x, y; } Vec2;
typedef struct { Vec2 position; float rotation; Vec2 scale; } Transform;
typedef struct { Transform transform; int segments; } Circle;

// --- ��ħ �ܻ� ���� ---
#define TRAIL_LENGTH 8
typedef struct { Vec2 P1, P2; float alpha; } LineSegment; // �������� �ܻ� ǥ��
LineSegment sub_second_hand_trail[TRAIL_LENGTH];
int trail_idx = 0;


void init_stars() {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i].x = (rand() / (float)RAND_MAX) * 2.0f - 1.0f; // -1 to 1
        stars[i].y = (rand() / (float)RAND_MAX) * 2.0f - 1.0f; // -1 to 1
        stars[i].z = (rand() / (float)RAND_MAX) * 0.5f + 0.5f; // pseudo-depth for parallax/size
        stars[i].brightness = (rand() / (float)RAND_MAX) * 0.5f + 0.2f; // Initial brightness
        stars[i].pulse_speed = (rand() / (float)RAND_MAX) * 1.5f + 0.5f;
    }
}

void draw_stars(float time_val) {
    glEnable(GL_POINT_SMOOTH);
    glPointSize(1.5f); // �⺻ �� ũ��
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; ++i) {
        float current_brightness = stars[i].brightness * (0.75f + 0.25f * sinf(stars[i].pulse_speed * time_val + (float)i));
        glColor4f(0.8f, 0.8f, 0.9f, current_brightness * stars[i].z); // Z�� �ణ�� ����/���� ��ȭ
        glVertex3f(stars[i].x, stars[i].y, -stars[i].z); // Z �� ��� (�ణ�� parallax ȿ�� �õ�)
    }
    glEnd();
}


// �� �׸��� �Լ� (����� �׶��̼� �߰�)
void draw_circle_gradient(Circle* c, float r_center, float g_center, float b_center, float a_center,
    float r_edge, float g_edge, float b_edge, float a_edge) {
    glPushMatrix();
    glTranslatef(c->transform.position.x, c->transform.position.y, 0.0f);
    glRotatef(c->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(c->transform.scale.x, c->transform.scale.y, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(r_center, g_center, b_center, a_center); // �߽� ����
    glVertex2f(0.0f, 0.0f); // �߽���
    for (int i = 0; i <= c->segments; ++i) {
        float angle = 2.0f * PI * i / c->segments;
        glColor4f(r_edge, g_edge, b_edge, a_edge); // �����ڸ� ����
        glVertex2f(cosf(angle) * 0.5f, sinf(angle) * 0.5f);
    }
    glEnd();
    glPopMatrix();
}

// ��Ÿ�ϸ����� �ð�ħ �׸��� �Լ�
void draw_stylish_hand(float length, float base_width, float tip_width,
    float r, float g, float b, float a, float angle_degrees) {
    glPushMatrix();
    glRotatef(angle_degrees, 0.0f, 0.0f, 1.0f);
    glColor4f(r, g, b, a);
    glBegin(GL_TRIANGLES);
    // ������ �� ���� �Ǵ� �簢�� ��� �߰� ����
    // ��: ������ ��ٸ��� ����� ħ
    glVertex2f(-base_width / 2.0f, 0.0f);
    glVertex2f(base_width / 2.0f, 0.0f);
    glVertex2f(tip_width / 2.0f, length);

    glVertex2f(-base_width / 2.0f, 0.0f);
    glVertex2f(tip_width / 2.0f, length);
    glVertex2f(-tip_width / 2.0f, length);
    glEnd();

    // ħ �߽ɿ� ���� �� �߰� (���)
    Circle center_knob;
    center_knob.transform.position = (Vec2){ 0.0f, 0.0f };
    center_knob.transform.rotation = 0;
    center_knob.transform.scale = (Vec2){ base_width * 1.2f, base_width * 1.2f }; // ħ �β��� ���
    center_knob.segments = 16;
    draw_circle_gradient(&center_knob, r * 0.8f, g * 0.8f, b * 0.8f, a, r * 0.6f, g * 0.6f, b * 0.6f, a);

    glPopMatrix();
}


// ��ħ (���� + �� ��) �׸��� - ���� ���̾��, �ܻ� ȿ���� �Բ� ���
void draw_second_hand_with_trail(float length, float thickness, float r, float g, float b, float a, float angle_degrees,
    Vec2 center_pos, bool record_trail) {
    // ���� ħ �׸���
    glPushMatrix();
    glTranslatef(center_pos.x, center_pos.y, 0.0f);
    glRotatef(angle_degrees, 0.0f, 0.0f, 1.0f);

    // ħ ����
    glLineWidth(thickness);
    glBegin(GL_LINES);
    glColor4f(r, g, b, a);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, length * 0.85f); // ħ ���̸� �ణ �ٿ��� ���� ���� ������
    glEnd();

    // ħ ���� ���� ��
    Circle tip_circle;
    tip_circle.transform.position = (Vec2){ 0.0f, length * 0.85f };
    tip_circle.transform.rotation = 0;
    float tip_circle_radius = thickness * 1.5f;
    tip_circle.transform.scale = (Vec2){ tip_circle_radius, tip_circle_radius };
    tip_circle.segments = 12;
    draw_circle_gradient(&tip_circle, r, g, b, a, r * 0.7f, g * 0.7f, b * 0.7f, a * 0.8f);

    // �ܻ� ���
    if (record_trail) {
        float rad_angle = (angle_degrees - 90.0f) * PI / 180.0f; // 0���� ������ ���ϵ��� ����
        sub_second_hand_trail[trail_idx].P1 = center_pos;
        sub_second_hand_trail[trail_idx].P2.x = center_pos.x + cosf(rad_angle) * length * 0.85f;
        sub_second_hand_trail[trail_idx].P2.y = center_pos.y + sinf(rad_angle) * length * 0.85f;
        sub_second_hand_trail[trail_idx].alpha = a;
        trail_idx = (trail_idx + 1) % TRAIL_LENGTH;
    }
    glPopMatrix();

    // �ܻ� �׸���
    if (record_trail) { // record_trail �÷��׸� ��Ȱ���Ͽ� ���� ȣ�� �ÿ��� �׸����� ��
        glLineWidth(thickness * 0.8f); // �ܻ��� �ణ ���ð�
        glBegin(GL_LINES);
        for (int i = 0; i < TRAIL_LENGTH; ++i) {
            int current = (trail_idx - 1 - i + TRAIL_LENGTH) % TRAIL_LENGTH;
            float trail_alpha = sub_second_hand_trail[current].alpha * (1.0f - (float)i / TRAIL_LENGTH);
            if (trail_alpha > 0.01f) { // �ʹ� �����ϸ� �׸��� ����
                glColor4f(r, g * 0.7f, b * 0.7f, trail_alpha * 0.7f); // �ܻ� ���� �ణ ����
                glVertex2f(sub_second_hand_trail[current].P1.x, sub_second_hand_trail[current].P1.y);
                glVertex2f(sub_second_hand_trail[current].P2.x, sub_second_hand_trail[current].P2.y);
            }
        }
        glEnd();
    }
}


void draw_clock_markers(float radius, float time_val) {
    for (int i = 0; i < 60; ++i) {
        glPushMatrix();
        glRotatef(-(float)i * 6.0f, 0.0f, 0.0f, 1.0f); // �� ���� ��ġ�� ȸ��

        float r = 0.7f, g = 0.85f, b = 0.9f, a = 0.8f; // �⺻ ���� �� (���� û��)
        float marker_len_outer = radius * 0.98f;
        float marker_len_inner;
        glLineWidth(1.5f);

        if (i % 5 == 0) { // �ð� ����
            glLineWidth(3.0f);
            marker_len_inner = radius * 0.88f;
            if (i == 0 || i == 15 || i == 30 || i == 45) { // 12, 3, 6, 9�� ����
                float pulse = 0.7f + 0.3f * fabsf(sinf(time_val * 1.5f + (float)i * PI / 30.0f));
                r = 0.8f * pulse; g = 0.9f * pulse; b = 1.0f * pulse; a = 0.9f * pulse;
                marker_len_inner = radius * 0.85f; // �ֿ� ���� �� ���
            }
        }
        else { // �� ����
            marker_len_inner = radius * 0.93f;
            a = 0.6f; // �� ������ �ణ �� �����ϰ�
        }

        glColor4f(r, g, b, a);
        glBegin(GL_LINES);
        glVertex2f(0.0f, marker_len_inner);
        glVertex2f(0.0f, marker_len_outer);
        glEnd();
        glPopMatrix();
    }
}

void draw_seven_segment_digit(int digit, float x, float y, float w, float h, float thickness, float r, float g, float b, float a) {
    if (digit < 0 || digit > 9) return;
    glLineWidth(thickness);
    glColor4f(r, g, b, a);
    glBegin(GL_LINES);
    const bool* segments = SEVEN_SEGMENT_PATTERNS[digit];
    float l = x, rt = x + w, bot = y, top = y + h, my = y + h / 2.0f; // ������ ���� (rt, bot, top)
    if (segments[0]) { glVertex2f(l, top); glVertex2f(rt, top); }
    if (segments[1]) { glVertex2f(rt, my); glVertex2f(rt, top); }
    if (segments[2]) { glVertex2f(rt, bot); glVertex2f(rt, my); }
    if (segments[3]) { glVertex2f(l, bot); glVertex2f(rt, bot); }
    if (segments[4]) { glVertex2f(l, bot); glVertex2f(l, my); }
    if (segments[5]) { glVertex2f(l, my); glVertex2f(l, top); }
    if (segments[6]) { glVertex2f(l, my); glVertex2f(rt, my); }
    glEnd();
}

void draw_digital_colon(float x, float y, float w, float h, float thickness, float r, float g, float b, float a) {
    glLineWidth(thickness);
    glColor4f(r, g, b, a);
    glBegin(GL_LINES);
    float dot_h = h * 0.20f;
    float center_x = x + w / 2.0f;
    glVertex2f(center_x, y + h * 0.65f + dot_h / 2.0f);
    glVertex2f(center_x, y + h * 0.65f - dot_h / 2.0f);
    glVertex2f(center_x, y + h * 0.35f + dot_h / 2.0f);
    glVertex2f(center_x, y + h * 0.35f - dot_h / 2.0f);
    glEnd();
}

void draw_digital_display_panel(float center_x, float center_y, float panel_w, float panel_h) {
    glColor4f(0.1f, 0.15f, 0.2f, 0.6f); // ������ ��ο� �г� ����
    glBegin(GL_QUADS);
    glVertex2f(center_x - panel_w / 2.0f, center_y - panel_h / 2.0f);
    glVertex2f(center_x + panel_w / 2.0f, center_y - panel_h / 2.0f);
    glVertex2f(center_x + panel_w / 2.0f, center_y + panel_h / 2.0f);
    glVertex2f(center_x - panel_w / 2.0f, center_y + panel_h / 2.0f);
    glEnd();
}

void draw_digital_display(int hour_24, int minute, float base_x, float base_y, float digit_w, float digit_h, float spacing, float thickness) {
    float r = 0.7f, gg = 0.85f, b = 0.95f, a = 0.9f; // ������ ���� ���� (���� û��/�ϴû�)

    int h1 = hour_24 / 10;
    int h2 = hour_24 % 10;
    int m1 = minute / 10;
    int m2 = minute % 10;

    float total_panel_width = 4 * digit_w + (digit_w * 0.6f) + 2 * spacing + 2 * (spacing * 0.5f) + digit_w * 0.4f; // �г� ���� ����
    float panel_height = digit_h + digit_h * 0.3f;
    draw_digital_display_panel(base_x, base_y, total_panel_width, panel_height);


    float total_width = 4 * digit_w + (digit_w * 0.6f) + 2 * spacing + 2 * (spacing * 0.5f);
    float start_x = base_x - total_width / 2.0f;
    float current_x = start_x;

    draw_seven_segment_digit(h1, current_x, base_y - digit_h / 2.0f, digit_w, digit_h, thickness, r, gg, b, a); // Y ��ġ ����
    current_x += digit_w + spacing;
    draw_seven_segment_digit(h2, current_x, base_y - digit_h / 2.0f, digit_w, digit_h, thickness, r, gg, b, a);
    current_x += digit_w + spacing * 0.5f;
    draw_digital_colon(current_x, base_y - digit_h / 2.0f, digit_w * 0.6f, digit_h, thickness, r, gg, b, a);
    current_x += digit_w * 0.6f + spacing * 0.5f;
    draw_seven_segment_digit(m1, current_x, base_y - digit_h / 2.0f, digit_w, digit_h, thickness, r, gg, b, a);
    current_x += digit_w + spacing;
    draw_seven_segment_digit(m2, current_x, base_y - digit_h / 2.0f, digit_w, digit_h, thickness, r, gg, b, a);
}


int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(600, 600, "Fancy Analog & Digital Clock", NULL, NULL); // â ũ�� �ణ Ű��
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync �ѱ� (�ִϸ��̼� �ε巴��)
    glOrtho(-1, 1, -1, 1, -1.5, 1.5); // Z�� ���� �ణ �ø� (�� ��� ����)

    glEnable(GL_BLEND); // ���� ���� Ȱ��ȭ (�ܻ�, ������ �г� ��)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH); // �� �ε巴��
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    init_stars();
    for (int i = 0; i < TRAIL_LENGTH; ++i) { // �ܻ� �ʱ�ȭ
        sub_second_hand_trail[i].alpha = 0.0f;
    }


    float main_dial_radius = 0.7f;
    Circle clockFace = { {{0.0f, 0.0f}, 0.0f, {main_dial_radius * 2.0f, main_dial_radius * 2.0f}}, 128 }; // ���׸�Ʈ �ø�
    Circle clockBorder = { {{0.0f, 0.0f}, 0.0f, {(main_dial_radius + 0.02f) * 2.0f, (main_dial_radius + 0.02f) * 2.0f}}, 128 };

    float sub_dial_scale_factor = 0.28f;
    float sub_dial_radius = main_dial_radius * sub_dial_scale_factor;
    Vec2 subDialPosition = { -main_dial_radius * 0.55f, -main_dial_radius * 0.05f }; // ��ġ ��¦ ����

    Circle secondsSubDialFace = { {subDialPosition, 0.0f, {sub_dial_radius * 2.0f, sub_dial_radius * 2.0f}}, 64 };
    Circle secondsSubDialBorder = { {subDialPosition, 0.0f, {(sub_dial_radius + 0.01f) * 2.0f, (sub_dial_radius + 0.01f) * 2.0f}}, 64 };

    float hour_hand_rel_len = 0.5f;   float hour_hand_base_w = 0.04f;  float hour_hand_tip_w = 0.01f;
    float minute_hand_rel_len = 0.7f; float minute_hand_base_w = 0.03f; float minute_hand_tip_w = 0.008f;
    float second_sub_hand_len_factor = 0.9f; // ���� ���̾� ������ ����

    const float start_time_offset_seconds = 1.0f * 3600.0f;

    float digit_char_height = main_dial_radius * 0.15f;
    float digit_char_width = digit_char_height * 0.55f;
    float digit_spacing = digit_char_width * 0.25f;
    float digit_line_thickness = 3.0f;
    float digital_base_y = -main_dial_radius * 0.55f;

    double last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double current_glfw_time = glfwGetTime();
        // double delta_time = current_glfw_time - last_time; // ��������� ������ ������ �ð� ����
        last_time = current_glfw_time;

        float total_simulated_seconds_float = start_time_offset_seconds + (float)current_glfw_time;

        float sim_s_continuous = fmod(total_simulated_seconds_float, 60.0f);
        int total_simulated_seconds_int = (int)floor(total_simulated_seconds_float);
        int current_discrete_minute = (total_simulated_seconds_int / 60) % 60;
        int current_discrete_hour_of_day = (total_simulated_seconds_int / 3600);
        int current_discrete_hour_for_analog = current_discrete_hour_of_day % 12;

        float sec_angle_subdial = -(sim_s_continuous * 6.0f);
        float min_angle = -((float)current_discrete_minute * 6.0f);
        float hour_angle = -((float)current_discrete_hour_for_analog * 30.0f);

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // ��ο� ���� ���
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ���� ���۵� Ŭ����

        draw_stars((float)current_glfw_time);

        // �� �ð��� �׶��̼� (�߾�: �ణ ���� ȸ��, �����ڸ�: ���� �����ϰ� ��Ӱ�)
        draw_circle_gradient(&clockBorder, 0.3f, 0.3f, 0.4f, 0.8f, 0.1f, 0.1f, 0.15f, 0.9f);
        draw_circle_gradient(&clockFace, 0.2f, 0.2f, 0.25f, 0.9f, 0.1f, 0.1f, 0.12f, 0.95f);
        draw_clock_markers(main_dial_radius, (float)current_glfw_time);

        // ��Ÿ�ϸ����� �ð�ħ (��, ��)
        draw_stylish_hand(main_dial_radius * hour_hand_rel_len, hour_hand_base_w, hour_hand_tip_w, 0.8f, 0.8f, 0.85f, 0.95f, hour_angle);
        draw_stylish_hand(main_dial_radius * minute_hand_rel_len, minute_hand_base_w, minute_hand_tip_w, 0.7f, 0.7f, 0.75f, 0.9f, min_angle);

        // ��ħ ���� ���̾�
        draw_circle_gradient(&secondsSubDialBorder, 0.25f, 0.25f, 0.3f, 0.7f, 0.08f, 0.08f, 0.1f, 0.8f);
        draw_circle_gradient(&secondsSubDialFace, 0.15f, 0.15f, 0.2f, 0.8f, 0.08f, 0.08f, 0.1f, 0.85f);

        // ���� ���̾� ��ħ (�ܻ� ȿ�� ����)
        // draw_second_hand_with_trail �Լ��� ���ο��� center_pos�� ������� �����Ƿ�, 
        // �ش� �Լ� ȣ�� ���� glTranslate�� �̵��ؾ� ��. �Ǵ� �Լ� ����.
        // ���⼭�� �Լ��� �������� �ʰ� glTranslate ���.
        glPushMatrix();
        glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
        // draw_second_hand_with_trail �Լ� ���ο��� center_pos ���ڴ� �� �̻� ������� ����.
        // �ܻ� ����� ���� P1, P2 ��� �� ���� ��ǥ�� �ʿ��ϹǷ�, ���� ��ġ(secondsSubDialFace.transform.position)�� �Ѱ���� ��.
        // draw_second_hand_with_trail�� �ܻ� ��� ���� ���� �ʿ�.
        // -> draw_second_hand_with_trail �Լ� �ñ״�ó ����: center_pos�� ������ ���ο��� glTranslate �Ŀ� (0,0) �������� �׸���,
        //    �ܻ� ��� �ÿ��� ������ center_pos�� ����ϵ��� ����.
        //    �� �����ϰԴ�, draw_second_hand_with_trail�� �׻� (0,0) �������� �׸���, �ܻ� ����� ���� ���� matrix�� ��ȯ���� �����ͼ� ���� ��ǥ ���.
        //    ���⼭�� draw_second_hand_with_trail �Լ��� �̹� glTranslate�� (0,0) �������� �׸�.
        //    �ܻ� ��Ͻ� (sub_second_hand_trail[trail_idx].P1 = center_pos;) �� �κ��� (0,0)�� ���� ��ȯ�� ������ǥ�� �ٲ�� ��.
        //    ���� ������ ����� draw_second_hand_with_trail ȣ�� ���� glTranslate�� �ϰ�,
        //    �ܻ� ȿ���� �� ��ȯ�� ���� ������ ��������� �׸��ų�, ���� ��ǥ�� ����Ͽ� ����.
        //    �ϴ� ���� ����������, �ܻ� ���ε��� �׸� ���� ������ glTranslate�� �����ϸ� ��� ��ǥ�� �׷���.
        //    �ʱ� �ܻ� �����Ϳ� ���� ��ǥ�� ���������Ƿ�, �ܻ� �׸����� glTranslate ���� �׷��� ��.

        // ��ħ �׸��� (����)
        // �� push/pop�� ��ħ ��ü�� �׸��� ����.
        glPushMatrix();
        glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
        draw_stylish_hand(sub_dial_radius * second_sub_hand_len_factor, 0.02f, 0.005f, 0.9f, 0.3f, 0.3f, 0.9f, sec_angle_subdial);
        glPopMatrix();

        // �ܻ� ��� �� �׸��� (���� ��ǥ ����)
        // 1. ���� ��ħ �� ���� ��ǥ ���
        float rad_angle_trail = (sec_angle_subdial - 90.0f) * PI / 180.0f; // 0���� ����
        Vec2 current_tip_world;
        current_tip_world.x = secondsSubDialFace.transform.position.x + cosf(rad_angle_trail) * sub_dial_radius * second_sub_hand_len_factor;
        current_tip_world.y = secondsSubDialFace.transform.position.y + sinf(rad_angle_trail) * sub_dial_radius * second_sub_hand_len_factor;

        // 2. �ܻ� �迭 ������Ʈ
        sub_second_hand_trail[trail_idx].P1 = secondsSubDialFace.transform.position; // �߽���
        sub_second_hand_trail[trail_idx].P2 = current_tip_world;                     // ����
        sub_second_hand_trail[trail_idx].alpha = 0.9f; // �ִ� ���İ�
        trail_idx = (trail_idx + 1) % TRAIL_LENGTH;

        // 3. �ܻ� �׸��� (���� ��ǥ �״�� ���)
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        for (int i = 0; i < TRAIL_LENGTH; ++i) {
            int current = (trail_idx - 1 - i + TRAIL_LENGTH) % TRAIL_LENGTH;
            float trail_alpha_val = sub_second_hand_trail[current].alpha * powf((1.0f - (float)i / TRAIL_LENGTH), 2.0f); // �ް��� ��������� ����
            if (trail_alpha_val > 0.01f) {
                glColor4f(0.9f, 0.3f, 0.3f, trail_alpha_val * 0.6f); // �ܻ� ���� �� ����
                glVertex2f(sub_second_hand_trail[current].P1.x, sub_second_hand_trail[current].P1.y);
                glVertex2f(sub_second_hand_trail[current].P2.x, sub_second_hand_trail[current].P2.y);
            }
        }
        glEnd();
        // --- �ܻ� ȿ�� ���� �� ---

        int display_hour = current_discrete_hour_of_day % 24;
        draw_digital_display(display_hour, current_discrete_minute,
            0.0f, digital_base_y,
            digit_char_width, digit_char_height, digit_spacing, digit_line_thickness);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}