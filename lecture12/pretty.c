#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h> // rand(), srand()
#include <time.h>   // time() for srand()

#define PI 3.1415926535f

// --- 별 배경 설정 ---
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

// --- 초침 잔상 설정 ---
#define TRAIL_LENGTH 8
typedef struct { Vec2 P1, P2; float alpha; } LineSegment; // 선분으로 잔상 표현
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
    glPointSize(1.5f); // 기본 별 크기
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; ++i) {
        float current_brightness = stars[i].brightness * (0.75f + 0.25f * sinf(stars[i].pulse_speed * time_val + (float)i));
        glColor4f(0.8f, 0.8f, 0.9f, current_brightness * stars[i].z); // Z로 약간의 투명도/색상 변화
        glVertex3f(stars[i].x, stars[i].y, -stars[i].z); // Z 값 사용 (약간의 parallax 효과 시도)
    }
    glEnd();
}


// 원 그리기 함수 (방사형 그라데이션 추가)
void draw_circle_gradient(Circle* c, float r_center, float g_center, float b_center, float a_center,
    float r_edge, float g_edge, float b_edge, float a_edge) {
    glPushMatrix();
    glTranslatef(c->transform.position.x, c->transform.position.y, 0.0f);
    glRotatef(c->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(c->transform.scale.x, c->transform.scale.y, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(r_center, g_center, b_center, a_center); // 중심 색상
    glVertex2f(0.0f, 0.0f); // 중심점
    for (int i = 0; i <= c->segments; ++i) {
        float angle = 2.0f * PI * i / c->segments;
        glColor4f(r_edge, g_edge, b_edge, a_edge); // 가장자리 색상
        glVertex2f(cosf(angle) * 0.5f, sinf(angle) * 0.5f);
    }
    glEnd();
    glPopMatrix();
}

// 스타일리시한 시계침 그리기 함수
void draw_stylish_hand(float length, float base_width, float tip_width,
    float r, float g, float b, float a, float angle_degrees) {
    glPushMatrix();
    glRotatef(angle_degrees, 0.0f, 0.0f, 1.0f);
    glColor4f(r, g, b, a);
    glBegin(GL_TRIANGLES);
    // 손잡이 쪽 원형 또는 사각형 장식 추가 가능
    // 예: 간단한 사다리꼴 모양의 침
    glVertex2f(-base_width / 2.0f, 0.0f);
    glVertex2f(base_width / 2.0f, 0.0f);
    glVertex2f(tip_width / 2.0f, length);

    glVertex2f(-base_width / 2.0f, 0.0f);
    glVertex2f(tip_width / 2.0f, length);
    glVertex2f(-tip_width / 2.0f, length);
    glEnd();

    // 침 중심에 작은 원 추가 (장식)
    Circle center_knob;
    center_knob.transform.position = (Vec2){ 0.0f, 0.0f };
    center_knob.transform.rotation = 0;
    center_knob.transform.scale = (Vec2){ base_width * 1.2f, base_width * 1.2f }; // 침 두께에 비례
    center_knob.segments = 16;
    draw_circle_gradient(&center_knob, r * 0.8f, g * 0.8f, b * 0.8f, a, r * 0.6f, g * 0.6f, b * 0.6f, a);

    glPopMatrix();
}


// 초침 (라인 + 끝 원) 그리기 - 보조 다이얼용, 잔상 효과와 함께 사용
void draw_second_hand_with_trail(float length, float thickness, float r, float g, float b, float a, float angle_degrees,
    Vec2 center_pos, bool record_trail) {
    // 현재 침 그리기
    glPushMatrix();
    glTranslatef(center_pos.x, center_pos.y, 0.0f);
    glRotatef(angle_degrees, 0.0f, 0.0f, 1.0f);

    // 침 라인
    glLineWidth(thickness);
    glBegin(GL_LINES);
    glColor4f(r, g, b, a);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, length * 0.85f); // 침 길이를 약간 줄여서 원이 끝에 오도록
    glEnd();

    // 침 끝에 작은 원
    Circle tip_circle;
    tip_circle.transform.position = (Vec2){ 0.0f, length * 0.85f };
    tip_circle.transform.rotation = 0;
    float tip_circle_radius = thickness * 1.5f;
    tip_circle.transform.scale = (Vec2){ tip_circle_radius, tip_circle_radius };
    tip_circle.segments = 12;
    draw_circle_gradient(&tip_circle, r, g, b, a, r * 0.7f, g * 0.7f, b * 0.7f, a * 0.8f);

    // 잔상 기록
    if (record_trail) {
        float rad_angle = (angle_degrees - 90.0f) * PI / 180.0f; // 0도가 위쪽을 향하도록 조정
        sub_second_hand_trail[trail_idx].P1 = center_pos;
        sub_second_hand_trail[trail_idx].P2.x = center_pos.x + cosf(rad_angle) * length * 0.85f;
        sub_second_hand_trail[trail_idx].P2.y = center_pos.y + sinf(rad_angle) * length * 0.85f;
        sub_second_hand_trail[trail_idx].alpha = a;
        trail_idx = (trail_idx + 1) % TRAIL_LENGTH;
    }
    glPopMatrix();

    // 잔상 그리기
    if (record_trail) { // record_trail 플래그를 재활용하여 메인 호출 시에만 그리도록 함
        glLineWidth(thickness * 0.8f); // 잔상은 약간 가늘게
        glBegin(GL_LINES);
        for (int i = 0; i < TRAIL_LENGTH; ++i) {
            int current = (trail_idx - 1 - i + TRAIL_LENGTH) % TRAIL_LENGTH;
            float trail_alpha = sub_second_hand_trail[current].alpha * (1.0f - (float)i / TRAIL_LENGTH);
            if (trail_alpha > 0.01f) { // 너무 투명하면 그리지 않음
                glColor4f(r, g * 0.7f, b * 0.7f, trail_alpha * 0.7f); // 잔상 색상 약간 변경
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
        glRotatef(-(float)i * 6.0f, 0.0f, 0.0f, 1.0f); // 각 눈금 위치로 회전

        float r = 0.7f, g = 0.85f, b = 0.9f, a = 0.8f; // 기본 눈금 색 (밝은 청록)
        float marker_len_outer = radius * 0.98f;
        float marker_len_inner;
        glLineWidth(1.5f);

        if (i % 5 == 0) { // 시간 눈금
            glLineWidth(3.0f);
            marker_len_inner = radius * 0.88f;
            if (i == 0 || i == 15 || i == 30 || i == 45) { // 12, 3, 6, 9시 눈금
                float pulse = 0.7f + 0.3f * fabsf(sinf(time_val * 1.5f + (float)i * PI / 30.0f));
                r = 0.8f * pulse; g = 0.9f * pulse; b = 1.0f * pulse; a = 0.9f * pulse;
                marker_len_inner = radius * 0.85f; // 주요 눈금 더 길게
            }
        }
        else { // 분 눈금
            marker_len_inner = radius * 0.93f;
            a = 0.6f; // 분 눈금은 약간 더 투명하게
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
    float l = x, rt = x + w, bot = y, top = y + h, my = y + h / 2.0f; // 변수명 변경 (rt, bot, top)
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
    glColor4f(0.1f, 0.15f, 0.2f, 0.6f); // 반투명 어두운 패널 색상
    glBegin(GL_QUADS);
    glVertex2f(center_x - panel_w / 2.0f, center_y - panel_h / 2.0f);
    glVertex2f(center_x + panel_w / 2.0f, center_y - panel_h / 2.0f);
    glVertex2f(center_x + panel_w / 2.0f, center_y + panel_h / 2.0f);
    glVertex2f(center_x - panel_w / 2.0f, center_y + panel_h / 2.0f);
    glEnd();
}

void draw_digital_display(int hour_24, int minute, float base_x, float base_y, float digit_w, float digit_h, float spacing, float thickness) {
    float r = 0.7f, gg = 0.85f, b = 0.95f, a = 0.9f; // 디지털 숫자 색상 (밝은 청록/하늘색)

    int h1 = hour_24 / 10;
    int h2 = hour_24 % 10;
    int m1 = minute / 10;
    int m2 = minute % 10;

    float total_panel_width = 4 * digit_w + (digit_w * 0.6f) + 2 * spacing + 2 * (spacing * 0.5f) + digit_w * 0.4f; // 패널 여유 공간
    float panel_height = digit_h + digit_h * 0.3f;
    draw_digital_display_panel(base_x, base_y, total_panel_width, panel_height);


    float total_width = 4 * digit_w + (digit_w * 0.6f) + 2 * spacing + 2 * (spacing * 0.5f);
    float start_x = base_x - total_width / 2.0f;
    float current_x = start_x;

    draw_seven_segment_digit(h1, current_x, base_y - digit_h / 2.0f, digit_w, digit_h, thickness, r, gg, b, a); // Y 위치 보정
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Fancy Analog & Digital Clock", NULL, NULL); // 창 크기 약간 키움
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync 켜기 (애니메이션 부드럽게)
    glOrtho(-1, 1, -1, 1, -1.5, 1.5); // Z축 범위 약간 늘림 (별 배경 위해)

    glEnable(GL_BLEND); // 알파 블렌딩 활성화 (잔상, 반투명 패널 등)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH); // 선 부드럽게
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    init_stars();
    for (int i = 0; i < TRAIL_LENGTH; ++i) { // 잔상 초기화
        sub_second_hand_trail[i].alpha = 0.0f;
    }


    float main_dial_radius = 0.7f;
    Circle clockFace = { {{0.0f, 0.0f}, 0.0f, {main_dial_radius * 2.0f, main_dial_radius * 2.0f}}, 128 }; // 세그먼트 늘림
    Circle clockBorder = { {{0.0f, 0.0f}, 0.0f, {(main_dial_radius + 0.02f) * 2.0f, (main_dial_radius + 0.02f) * 2.0f}}, 128 };

    float sub_dial_scale_factor = 0.28f;
    float sub_dial_radius = main_dial_radius * sub_dial_scale_factor;
    Vec2 subDialPosition = { -main_dial_radius * 0.55f, -main_dial_radius * 0.05f }; // 위치 살짝 조정

    Circle secondsSubDialFace = { {subDialPosition, 0.0f, {sub_dial_radius * 2.0f, sub_dial_radius * 2.0f}}, 64 };
    Circle secondsSubDialBorder = { {subDialPosition, 0.0f, {(sub_dial_radius + 0.01f) * 2.0f, (sub_dial_radius + 0.01f) * 2.0f}}, 64 };

    float hour_hand_rel_len = 0.5f;   float hour_hand_base_w = 0.04f;  float hour_hand_tip_w = 0.01f;
    float minute_hand_rel_len = 0.7f; float minute_hand_base_w = 0.03f; float minute_hand_tip_w = 0.008f;
    float second_sub_hand_len_factor = 0.9f; // 보조 다이얼 반지름 기준

    const float start_time_offset_seconds = 1.0f * 3600.0f;

    float digit_char_height = main_dial_radius * 0.15f;
    float digit_char_width = digit_char_height * 0.55f;
    float digit_spacing = digit_char_width * 0.25f;
    float digit_line_thickness = 3.0f;
    float digital_base_y = -main_dial_radius * 0.55f;

    double last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double current_glfw_time = glfwGetTime();
        // double delta_time = current_glfw_time - last_time; // 사용하지는 않지만 프레임 시간 계산용
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

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // 어두운 남색 배경
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 깊이 버퍼도 클리어

        draw_stars((float)current_glfw_time);

        // 주 시계판 그라데이션 (중앙: 약간 밝은 회색, 가장자리: 배경과 유사하게 어둡게)
        draw_circle_gradient(&clockBorder, 0.3f, 0.3f, 0.4f, 0.8f, 0.1f, 0.1f, 0.15f, 0.9f);
        draw_circle_gradient(&clockFace, 0.2f, 0.2f, 0.25f, 0.9f, 0.1f, 0.1f, 0.12f, 0.95f);
        draw_clock_markers(main_dial_radius, (float)current_glfw_time);

        // 스타일리시한 시계침 (시, 분)
        draw_stylish_hand(main_dial_radius * hour_hand_rel_len, hour_hand_base_w, hour_hand_tip_w, 0.8f, 0.8f, 0.85f, 0.95f, hour_angle);
        draw_stylish_hand(main_dial_radius * minute_hand_rel_len, minute_hand_base_w, minute_hand_tip_w, 0.7f, 0.7f, 0.75f, 0.9f, min_angle);

        // 초침 보조 다이얼
        draw_circle_gradient(&secondsSubDialBorder, 0.25f, 0.25f, 0.3f, 0.7f, 0.08f, 0.08f, 0.1f, 0.8f);
        draw_circle_gradient(&secondsSubDialFace, 0.15f, 0.15f, 0.2f, 0.8f, 0.08f, 0.08f, 0.1f, 0.85f);

        // 보조 다이얼 초침 (잔상 효과 포함)
        // draw_second_hand_with_trail 함수는 내부에서 center_pos를 사용하지 않으므로, 
        // 해당 함수 호출 전에 glTranslate로 이동해야 함. 또는 함수 수정.
        // 여기서는 함수를 수정하지 않고 glTranslate 사용.
        glPushMatrix();
        glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
        // draw_second_hand_with_trail 함수 내부에서 center_pos 인자는 더 이상 사용하지 않음.
        // 잔상 기록을 위한 P1, P2 계산 시 월드 좌표가 필요하므로, 현재 위치(secondsSubDialFace.transform.position)를 넘겨줘야 함.
        // draw_second_hand_with_trail의 잔상 기록 로직 수정 필요.
        // -> draw_second_hand_with_trail 함수 시그니처 변경: center_pos를 받지만 내부에서 glTranslate 후에 (0,0) 기준으로 그리고,
        //    잔상 기록 시에는 원래의 center_pos를 사용하도록 수정.
        //    더 간단하게는, draw_second_hand_with_trail은 항상 (0,0) 기준으로 그리고, 잔상 기록할 때만 현재 matrix의 변환값을 가져와서 월드 좌표 계산.
        //    여기서는 draw_second_hand_with_trail 함수가 이미 glTranslate후 (0,0) 기준으로 그림.
        //    잔상 기록시 (sub_second_hand_trail[trail_idx].P1 = center_pos;) 이 부분을 (0,0)을 현재 변환된 월드좌표로 바꿔야 함.
        //    가장 간단한 방법은 draw_second_hand_with_trail 호출 전에 glTranslate를 하고,
        //    잔상 효과는 이 변환된 공간 내에서 상대적으로 그리거나, 월드 좌표를 계산하여 저장.
        //    일단 현재 구조에서는, 잔상 라인들을 그릴 때도 동일한 glTranslate를 적용하면 상대 좌표로 그려짐.
        //    초기 잔상 데이터에 월드 좌표를 저장했으므로, 잔상 그릴때는 glTranslate 없이 그려야 함.

        // 초침 그리기 (현재)
        // 이 push/pop은 초침 자체를 그리기 위함.
        glPushMatrix();
        glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
        draw_stylish_hand(sub_dial_radius * second_sub_hand_len_factor, 0.02f, 0.005f, 0.9f, 0.3f, 0.3f, 0.9f, sec_angle_subdial);
        glPopMatrix();

        // 잔상 기록 및 그리기 (월드 좌표 기준)
        // 1. 현재 초침 끝 월드 좌표 계산
        float rad_angle_trail = (sec_angle_subdial - 90.0f) * PI / 180.0f; // 0도가 위쪽
        Vec2 current_tip_world;
        current_tip_world.x = secondsSubDialFace.transform.position.x + cosf(rad_angle_trail) * sub_dial_radius * second_sub_hand_len_factor;
        current_tip_world.y = secondsSubDialFace.transform.position.y + sinf(rad_angle_trail) * sub_dial_radius * second_sub_hand_len_factor;

        // 2. 잔상 배열 업데이트
        sub_second_hand_trail[trail_idx].P1 = secondsSubDialFace.transform.position; // 중심점
        sub_second_hand_trail[trail_idx].P2 = current_tip_world;                     // 끝점
        sub_second_hand_trail[trail_idx].alpha = 0.9f; // 최대 알파값
        trail_idx = (trail_idx + 1) % TRAIL_LENGTH;

        // 3. 잔상 그리기 (월드 좌표 그대로 사용)
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        for (int i = 0; i < TRAIL_LENGTH; ++i) {
            int current = (trail_idx - 1 - i + TRAIL_LENGTH) % TRAIL_LENGTH;
            float trail_alpha_val = sub_second_hand_trail[current].alpha * powf((1.0f - (float)i / TRAIL_LENGTH), 2.0f); // 급격히 사라지도록 제곱
            if (trail_alpha_val > 0.01f) {
                glColor4f(0.9f, 0.3f, 0.3f, trail_alpha_val * 0.6f); // 잔상 색상 및 투명도
                glVertex2f(sub_second_hand_trail[current].P1.x, sub_second_hand_trail[current].P1.y);
                glVertex2f(sub_second_hand_trail[current].P2.x, sub_second_hand_trail[current].P2.y);
            }
        }
        glEnd();
        // --- 잔상 효과 수정 끝 ---

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