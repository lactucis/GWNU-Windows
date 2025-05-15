#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>             // GLFW ���̺귯��(������ ���� �� �Է�, OpenGL context ����)
#include <math.h>                   // ���� �Լ�(sin, cos ��) ���

#define PI 3.1415926535f            // ������(����) ��� ����, f�� float��

// 2���� ���� ����ü
typedef struct
{
    float x, y;                     // x, y ��ǥ��
} Vec2;

// ��ȯ(Transformation) ���� ����ü
typedef struct
{
    Vec2 position;                  // ��ġ (x, y)
    float rotation;                 // ȸ�� ����(����)
    Vec2 scale;                     // ũ��(x, y ������)
} Transform;


// �� ����ü: ��ȯ ������ ���׸�Ʈ ���� ����
typedef struct
{
    Transform transform;            // ���� ��ȯ ����
    int segments;                   // ���� �� ���� ��(�ﰢ��)���� �׸���
} Circle;

// �� �׸��� �Լ�
void draw_circle(Circle* c, float r, float g, float b)
{
    glPushMatrix(); // ���� ��ȯ��� ����

    // ��ġ �̵�, ȸ��, ũ�� ��ȯ ����
    glTranslatef(c->transform.position.x, c->transform.position.y, 0.0f);             // ��ġ �̵�
    glRotatef(c->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);               // ȸ��
    glScalef(c->transform.scale.x, c->transform.scale.y, 1.0f);                      // ũ�� ��ȯ

    glBegin(GL_TRIANGLE_FAN);                   // �ﰢ�� ��ä�÷� �� �׸��� ����
    glColor3f(r, g, b);
    glVertex2f(0.0f, 0.0f);                     // �߽���

    for (int i = 0; i <= c->segments; ++i)      // ���׸�Ʈ(��) ������ŭ �ݺ�
    {
        float angle = 2.0f * PI * i / c->segments;     // ���� ����(����)
        glVertex2f(cosf(angle) * 0.5f, sinf(angle) * 0.5f); // �� ���� ��(������ 0.5)
    }

    glEnd();                                    // �� �׸��� ��

    glPopMatrix(); // ��ȯ��� ����
}

void draw_line(float x1, float y1, float x2, float y2) {
    glLineWidth(3.0f);

    glBegin(GL_LINE);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}



int main()
{
    if (!glfwInit())                            // GLFW �ʱ�ȭ �����ϸ�
        return -1;                              // ����

    GLFWwindow* window = glfwCreateWindow(537, 445, "OpenGL C Demo", NULL, NULL);
    if (!window)                               // ������ ���� �����ϸ�
    {
        glfwTerminate();                       // GLFW ����
        return -1;                             // ���α׷� ����
    }

    glfwMakeContextCurrent(window);             // ���� �����忡 OpenGL context ����
    glOrtho(-1, 1, -1, 1, -1, 1);              // 2D ��ǥ��(-1~1, ���ϴ�~����)

    // ���� �ʱ�ȭ: (��ġ, ȸ��, ũ��)

    Circle face = { {{0.0f, 0.0f}, 0, {1.5, 1.5}}, 32 };          // ��: (0.5,0.5), ȸ��0, ������1, 32���׸�Ʈ
    Circle nose = { {{0.0f, -0.2f}, 0, {0.2, 0.2}}, 32 };          // ��: (0.5,0.5), ȸ��0, ������1, 32���׸�Ʈ
    Circle mouth_white1 = { {{0.1f, -0.33f}, 0, {0.3, 0.3}}, 32 }; // �Ͼ� ��
    Circle mouth_white2 = { {{-0.1f, -0.33f}, 0, {0.3, 0.3}}, 32 }; // �Ͼ� ��
    Circle left_eye = { {{-0.63f, 0.19f}, 0, {0.11f, 0.11f}}, 32 };   // ���� ��
    Circle right_eye = { {{0.64f, 0.19f}, 0, {0.11f, 0.11f}}, 32 };   // ������ ��

    while (!glfwWindowShouldClose(window))      // �����찡 ���� ������ �ݺ�
    {
        float time = (float)glfwGetTime();      // ���α׷� ���� �� �帥 �ð�(��)

        

        glClear(GL_COLOR_BUFFER_BIT);                               // ȭ�� �ʱ�ȭ(����)

        draw_circle(&face, 0.0f, 0.44f, 0.75f);
        
        draw_circle(&left_eye, 0.0f, 0.0f, 0.0f);
        draw_circle(&right_eye, 0.0f, 0.0f, 0.0f);
        draw_circle(&mouth_white1, 1.0f, 1.0f, 1.0f);
        draw_circle(&mouth_white2, 1.0f, 1.0f, 1.0f);
        draw_circle(&nose, 0.0f, 0.0f, 0.0f);

        draw_line(-0.35f, -0.11f, -0.85f, -0.15f);
        draw_line(-0.32f, -0.19f, -0.85f, -0.31f);
        draw_line(-0.30f, -0.29f, -0.77f, -0.48f);

        draw_line(0.35f, -0.11f, 0.85f, -0.15f);
        draw_line(0.32f, -0.19f, 0.85f, -0.31f);
        draw_line(0.30f, -0.29f, 0.77f, -0.48f);

        glfwSwapBuffers(window);                                    // ���� ����(ȭ�� ����)
        glfwPollEvents();                                           // �̺�Ʈ(�Է� ��) ó��
    }

    glfwTerminate();                        // GLFW ����(�ڿ� ����)
    return 0;                               // ���α׷� ����
}
