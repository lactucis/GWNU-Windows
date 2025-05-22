#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>     // GLFW 라이브러리(윈도우 생성 및 입력, OpenGL context 제공)
#include <math.h>           // 수학 함수(sin, cos, fmod, floor 등) 사용

#define PI 3.1415926535f    // 원주율(파이) 상수 정의, f는 float형

// 2차원 벡터 구조체
typedef struct
{
    float x, y;             // x, y 좌표값
} Vec2;

// 변환(Transformation) 정보 구조체
typedef struct
{
    Vec2 position;          // 위치 (x, y)
    float rotation;         // 회전 각도(라디안)
    Vec2 scale;             // 크기(x, y 스케일)
} Transform;

// 원 구조체: 변환 정보와 세그먼트 개수 가짐
typedef struct
{
    Transform transform;    // 원의 변환 정보
    int segments;           // 원을 몇 개의 점(삼각형)으로 그릴지
} Circle;

// --- 직선 그리기 함수 (from your second snippet) ---
void drawLine(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

// --- 디지털 숫자 표시를 위한 데이터 및 함수 ---
// 7-segment patterns (a,b,c,d,e,f,g - MSB to LSB)
// gfedcba
// 0b1111110 -> 0 (a,b,c,d,e,f)
// 0b0110000 -> 1 (b,c)
// 0b1101101 -> 2 (a,b,d,e,g)
// 0b1111001 -> 3 (a,b,c,d,g)
// ...and so on
const int segment_patterns[10] = {
    0b1111110, // 0 (abcdef)
    0b0110000, // 1 (bc)
    0b1101101, // 2 (abdeg)
    0b1111001, // 3 (abcdg)
    0b0110011, // 4 (bcfg)
    0b1011011, // 5 (acdfg)
    0b1011111, // 6 (acdefg)
    0b1110000, // 7 (abc)
    0b1111111, // 8 (abcdefg)
    0b1111011  // 9 (abcdfg)
};

// 디지털 숫자 하나를 그리는 함수
void drawDigitalDigit(int digit, Vec2 pos, float char_width, float char_height)
{
    if (digit < 0 || digit > 9) return;
    int pattern = segment_patterns[digit];

    float x = pos.x;
    float y = pos.y;

    // Segment definitions (relative to pos being bottom-left)
    //   .--a--.
    //   f     b
    //   .--g--.
    //   e     c
    //   .--d--.

    // a (top)
    if (pattern & 0x40) drawLine(x, y + char_height, x + char_width, y + char_height);
    // b (top-right)
    if (pattern & 0x20) drawLine(x + char_width, y + char_height / 2.0f, x + char_width, y + char_height);
    // c (bottom-right)
    if (pattern & 0x10) drawLine(x + char_width, y, x + char_width, y + char_height / 2.0f);
    // d (bottom)
    if (pattern & 0x08) drawLine(x, y, x + char_width, y);
    // e (bottom-left)
    if (pattern & 0x04) drawLine(x, y, x, y + char_height / 2.0f);
    // f (top-left)
    if (pattern & 0x02) drawLine(x, y + char_height / 2.0f, x, y + char_height);
    // g (middle)
    if (pattern & 0x01) drawLine(x, y + char_height / 2.0f, x + char_width, y + char_height / 2.0f);
}

// 디지털 콜론(:)을 그리는 함수
void drawDigitalColon(Vec2 pos, float char_height, float colon_char_width, float dot_actual_size)
{
    float center_x_of_colon_cell = pos.x + colon_char_width / 2.0f;
    float y_dot_top_center = pos.y + char_height * 0.62f;
    float y_dot_bottom_center = pos.y + char_height * 0.38f;
    float dot_half = dot_actual_size / 2.0f;

    glBegin(GL_QUADS);
    // Top dot
    glVertex2f(center_x_of_colon_cell - dot_half, y_dot_top_center - dot_half);
    glVertex2f(center_x_of_colon_cell + dot_half, y_dot_top_center - dot_half);
    glVertex2f(center_x_of_colon_cell + dot_half, y_dot_top_center + dot_half);
    glVertex2f(center_x_of_colon_cell - dot_half, y_dot_top_center + dot_half);
    // Bottom dot
    glVertex2f(center_x_of_colon_cell - dot_half, y_dot_bottom_center - dot_half);
    glVertex2f(center_x_of_colon_cell + dot_half, y_dot_bottom_center - dot_half);
    glVertex2f(center_x_of_colon_cell + dot_half, y_dot_bottom_center + dot_half);
    glVertex2f(center_x_of_colon_cell - dot_half, y_dot_bottom_center + dot_half);
    glEnd();
}

// "13:00" 디지털 시간을 그리는 함수
void draw_digital_time_display(float center_x, float bottom_y, float char_h, float r, float g, float b)
{
    glColor3f(r, g, b);
    glLineWidth(2.0f); // Set line width for digital display

    float char_w = char_h * 0.6f;      // Width of each digit character
    float spacing = char_w * 0.35f;    // Spacing between characters
    float colon_w = char_w * 0.4f;     // Width allocated for the colon character
    float colon_dot_size = char_w * 0.3f; // Actual size of the colon dots

    // Calculate total width of "13:00"
    // 1 (char_w) + space + 3 (char_w) + space + : (colon_w) + space + 0 (char_w) + space + 0 (char_w)
    float total_width = char_w + spacing + char_w + spacing + colon_w + spacing + char_w + spacing + char_w;

    float current_x_start = center_x - total_width / 2.0f;
    Vec2 current_pos;
    current_pos.y = bottom_y;

    // Draw '1'
    current_pos.x = current_x_start;
    drawDigitalDigit(1, current_pos, char_w, char_h);
    current_x_start += char_w + spacing;

    // Draw '3'
    current_pos.x = current_x_start;
    drawDigitalDigit(3, current_pos, char_w, char_h);
    current_x_start += char_w + spacing;

    // Draw ':'
    current_pos.x = current_x_start;
    drawDigitalColon(current_pos, char_h, colon_w, colon_dot_size);
    current_x_start += colon_w + spacing;

    // Draw '0'
    current_pos.x = current_x_start;
    drawDigitalDigit(0, current_pos, char_w, char_h);
    current_x_start += char_w + spacing;

    // Draw '0'
    current_pos.x = current_x_start;
    drawDigitalDigit(0, current_pos, char_w, char_h);
}


// --- 기존 아날로그 시계 함수들 ---
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
    glVertex2f(0.0f, 0.0f);     // 침의 시작점 (현재 원점)
    glVertex2f(0.0f, length);   // 침의 끝점 (Y축 양의 방향으로 그림)
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

    GLFWwindow* window = glfwCreateWindow(500, 500, "Analog Clock with Digital Display", NULL, NULL);
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
    Vec2 subDialPosition = { -0.35f, 0.0f }; // 원래 위치 유지
    float subDialScaleValue = 0.4f;

    Circle secondsSubDialFace = { {subDialPosition, 0.0f, {subDialScaleValue, subDialScaleValue}}, 32 };
    Circle secondsSubDialBorder = { {subDialPosition, 0.0f, {subDialScaleValue + 0.02f, subDialScaleValue + 0.02f}}, 32 };

    // 디지털 시간 표시 파라미터
    float digital_display_center_x = 0.0f;
    float digital_display_bottom_y = -0.40f; // 시계판 아래쪽에 위치
    float digital_display_char_height = 0.13f;


    const float start_time_offset_seconds = 1.0f * 3600.0f; // 1시 0분 0초부터 시작

    while (!glfwWindowShouldClose(window))
    {
        float elapsed_seconds_float = (float)glfwGetTime();
        float total_simulated_seconds_float = start_time_offset_seconds + elapsed_seconds_float;

        float sim_s_continuous = fmodf(total_simulated_seconds_float, 60.0f);
        int total_simulated_seconds_int = (int)floorf(total_simulated_seconds_float);
        int current_discrete_minute = (total_simulated_seconds_int / 60) % 60;
        int current_discrete_hour_of_day = (total_simulated_seconds_int / 3600);
        int current_discrete_hour = current_discrete_hour_of_day % 12;

        float sec_angle = -(sim_s_continuous * 6.0f);
        float min_angle = -((float)current_discrete_minute * 6.0f);
        // 시침은 분의 영향도 받도록 수정 (선택 사항, 원래 코드에서는 정시마다 이동)
        // float hour_angle = -((current_discrete_hour + current_discrete_minute / 60.0f) * 30.0f);
        // 원래 코드대로 정시마다 이동
        float hour_angle = -((float)current_discrete_hour * 30.0f);


        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 아날로그 시계 그리기
        draw_circle(&clockBorder, 0.4f, 0.4f, 0.4f);
        draw_circle(&clockFace, 1.0f, 1.0f, 1.0f);
        draw_clock_markers(0.75f); // clockFace scale is 1.5, internal radius 0.5 -> actual radius 0.75

        draw_hand(0.4f, 6.0f, 0.0f, 0.0f, 0.0f, hour_angle);
        draw_hand(0.6f, 4.0f, 0.2f, 0.2f, 0.2f, min_angle);

        // 초침 다이얼 그리기
        draw_circle(&secondsSubDialBorder, 0.3f, 0.3f, 0.3f);
        draw_circle(&secondsSubDialFace, 0.85f, 0.85f, 0.85f);

        glPushMatrix();
        glTranslatef(secondsSubDialFace.transform.position.x, secondsSubDialFace.transform.position.y, 0.0f);
        float seconds_hand_length = (subDialScaleValue * 0.5f) * 0.9f;
        draw_hand(seconds_hand_length, 1.5f, 1.0f, 0.0f, 0.0f, sec_angle);
        glPopMatrix();

        // --- 디지털 시간 "13:00" 그리기 ---
        // 검은색으로 (0,0,0)
        draw_digital_time_display(digital_display_center_x,
            digital_display_bottom_y,
            digital_display_char_height,
            0.0f, 0.0f, 0.0f);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}