#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>      // GLFW ���̺귯��(������ ���� �� �Է�, OpenGL context ����)
#include <math.h>          // ���� �Լ�(sin, cos, fmod, floor ��) ���

#define PI 3.1415926535f      // ������(����) ��� ����, f�� float��

// 2���� ���� ����ü
typedef struct
{
    float x, y;              // x, y ��ǥ��
} Vec2;

// ��ȯ(Transformation) ���� ����ü
typedef struct
{
    Vec2 position;           // ��ġ (x, y)
    float rotation;          // ȸ�� ����(����)
    Vec2 scale;              // ũ��(x, y ������)
} Transform;

// �� ����ü: ��ȯ ������ ���׸�Ʈ ���� ����
typedef struct
{
    Transform transform;     // ���� ��ȯ ����
    int segments;            // ���� �� ���� ��(�ﰢ��)���� �׸���
} Circle;

// �� �׸��� �Լ�
void draw_circle(Circle* c, float r, float g, float b)
{
    glPushMatrix(); // ���� ��ȯ��� ����

    glTranslatef(c->transform.position.x, c->transform.position.y, 0.0f);
    glRotatef(c->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(c->transform.scale.x, c->transform.scale.y, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(r, g, b);
    glVertex2f(0.0f, 0.0f);

    for (int i = 0; i <= c->segments; ++i)
    {
        float angle = 2.0f * PI * i / c->segments;
        glVertex2f(cosf(angle) * 0.5f, sinf(angle) * 0.5f); // ���������� ������ 0.5�� �׸�
    }
    glEnd();
    glPopMatrix();
}

// �ð�ħ �׸��� �Լ�
void draw_hand(float length, float thickness, float r, float g, float b, float angle_degrees)
{
    glPushMatrix();
    glRotatef(angle_degrees, 0.0f, 0.0f, 1.0f); // Z�� ���� ȸ��

    glLineWidth(thickness);
    glBegin(GL_LINES);
    glColor3f(r, g, b);
    glVertex2f(0.0f, 0.0f);    // ħ�� ������ (���� ����)
    glVertex2f(0.0f, length);  // ħ�� ���� (Y�� ���� �������� �׸�)
    glEnd();

    glPopMatrix();
}

// �� �ð� ���� �׸��� �Լ�
void draw_clock_markers(float radius)
{
    glColor3f(0.0f, 0.0f, 0.0f); // ������ ����

    for (int i = 0; i < 60; ++i) {
        glPushMatrix();
        glRotatef(-(float)i * 6.0f, 0.0f, 0.0f, 1.0f); // �� ���� ��ġ�� ȸ��

        if (i % 5 == 0) { // �ð� ����
            glLineWidth(3.0f);
            glBegin(GL_LINES);
            glVertex2f(0.0f, radius * 0.90f);
            glVertex2f(0.0f, radius * 0.98f);
            glEnd();
        }
        else { // �� ����
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
    glOrtho(-1, 1, -1, 1, -1, 1); // 2D ��ǥ�� ����

    // �� �ð��� ����
    Circle clockFace = { {{0.0f, 0.0f}, 0.0f, {1.5f, 1.5f}}, 64 };
    Circle clockBorder = { {{0.0f, 0.0f}, 0.0f, {1.55f, 1.55f}}, 64 };

    // ��ħ�� ���� ���̾� ����
    Vec2 subDialPosition = { -0.4f, 0.0f };
    float subDialScaleValue = 0.4f;

    Circle secondsSubDialFace = { {subDialPosition, 0.0f, {subDialScaleValue, subDialScaleValue}}, 32 };
    Circle secondsSubDialBorder = { {subDialPosition, 0.0f, {subDialScaleValue + 0.02f, subDialScaleValue + 0.02f}}, 32 };

    const float start_time_offset_seconds = 1.0f * 3600.0f; // 1�� 0�� 0�ʺ��� ����

    while (!glfwWindowShouldClose(window))
    {
        float elapsed_seconds_float = (float)glfwGetTime();
        float total_simulated_seconds_float = start_time_offset_seconds + elapsed_seconds_float;

        // ��ħ�� �ε巴�� �����̵��� �Ҽ��� �ð� ���
        float sim_s_continuous = fmod(total_simulated_seconds_float, 60.0f);

        // ��ħ�� ��ħ�� ����/���ÿ� �����̵��� ���� �ð� ���
        int total_simulated_seconds_int = (int)floor(total_simulated_seconds_float);

        int current_discrete_minute = (total_simulated_seconds_int / 60) % 60;
        // ��(hour)�� 12�ð����� �������� ��� (0�� ~ 11��).
        // ���� ���, ���α׷� ���� �� 0�� (�ùķ��̼� �ð� 1�� 0�� 0��):
        // total_simulated_seconds_int = 3600
        // current_discrete_hour_of_day = (3600 / 3600) = 1
        // current_discrete_hour = 1 % 12 = 1 (1�ø� �ǹ�)
        //
        // ���α׷� ���� �� 11�ð� (�ùķ��̼� �ð� 12�� 0�� 0��):
        // total_simulated_seconds_int = 3600 (������) + 11 * 3600 = 12 * 3600
        // current_discrete_hour_of_day = (12 * 3600 / 3600) = 12
        // current_discrete_hour = 12 % 12 = 0 (12�ø� �ǹ�, ���� ��� �� 0�� 12�� ��ġ)
        int current_discrete_hour_of_day = (total_simulated_seconds_int / 3600);
        int current_discrete_hour = current_discrete_hour_of_day % 12;


        // ���� ���
        float sec_angle = -(sim_s_continuous * 6.0f); // ��ħ�� �ε巴��
        float min_angle = -((float)current_discrete_minute * 6.0f); // ��ħ�� ���и��� �̵�
        float hour_angle = -((float)current_discrete_hour * 30.0f); // ��ħ�� ���ø��� �̵� (���� ���� ����)


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