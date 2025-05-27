#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>
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

typedef struct
{
    Transform transform;
    int segments;
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

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}




int main()
{
    // 1. �ʱ�ȭ
    if (!glfwInit())
        return -1;

    // 2. ������ ����
    GLFWwindow* window = glfwCreateWindow(800, 700, "Window Title", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // 3. ���ؽ�Ʈ ����
    glfwMakeContextCurrent(window);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // 2D ��ǥ�� ���� (���� �ʼ�)

    Circle face = { {{-0.7f, 0.0f}, 0, {0.5, 0.5}}, 32 };          // ��: (0.5,0.5), ȸ��0, ������1, 32���׸�Ʈ

    const float orbit_radius = 0.7f;
    const float circle_radius = 0.1f;

    // 4. ���� ����
    while (!glfwWindowShouldClose(window))
    {
        // 5. ȭ�� �����
        glClear(GL_COLOR_BUFFER_BIT);

        float time = (float)glfwGetTime();

        

        // --- ���⿡ �׸� �׸��� �ڵ� �ۼ� ---
        draw_circle(&face, 1.0f, 1.0f, 0.0f);
        draw_line(-0.8f, 0.1f, -0.8f, 0.0f);
        draw_line(-0.6f, 0.1f, -0.6f, 0.0f);


        // 6. ���� ��ü �� �̺�Ʈ ó��
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7. ����
    glfwTerminate();
    return 0;
}



/*
�׽�Ʈ ����

1. ���׶�̸� �ϳ� �׸���, ���� ���� (��), �� (�� ������ �ݿ� �׸���), ������ 0.2
2. ���μ��� ���� 1:1(ũ��� ��� ����)
3. �� ����� �׸� �׸���, �� ���� 0.02, �밢�� �׸� �ȿ� �׸���.
4. �׸�� ���� �Ÿ� 0.5
5. ���� �� ��ó�� ����, ���� �߾��� �������� ����, ���� ���⵵ �ؾ� ��.
6. ���� ���, ���� ����


�˾ƾ��Ұ�
����, ���� �ʼ��� ����
��ο� ����, ��ο� Ʈ���̿��� ��ο� ����Ʈ
�̵�, ȸ��, Ȯ��, ��� - ��Ȯ�ϰ� �˰� �ֱ�
����, Ǫ����
�θ�� �ڽİ��� ���̷�Ű�� ���� �ִϸ��̼��ϱ�
*/





/*
1. ���α׷��� �⺻ ��� (���� �߿�!)
�� ������ � ������ �޵� ���� ���� �ۼ��ؾ� �ϴ� �����Դϴ�. ��°�� �ܿ켼��.

C

#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>

int main()
{
    // 1. �ʱ�ȭ
    if (!glfwInit())
        return -1;

    // 2. ������ ����
    GLFWwindow* window = glfwCreateWindow(800, 600, "Window Title", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // 3. ���ؽ�Ʈ ����
    glfwMakeContextCurrent(window);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // 2D ��ǥ�� ���� (���� �ʼ�)

    // 4. ���� ����
    while (!glfwWindowShouldClose(window))
    {
        // 5. ȭ�� �����
        glClear(GL_COLOR_BUFFER_BIT);

        // --- ���⿡ �׸� �׸��� �ڵ� �ۼ� ---

        // 6. ���� ��ü �� �̺�Ʈ ó��
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7. ����
    glfwTerminate();
    return 0;
}
2. OpenGL �ٽ� �ϱ���� (�׸��� ���)
��. �׸��� �⺻ (glBegin / glEnd)
glBegin(MODE); / glEnd();: ��� �׸����� ���۰� ��. MODE�� ������ �� �˴ϴ�.
GL_LINES: �� �׸��� (�� 2����)
GL_TRIANGLES: �ﰢ�� �׸��� (�� 3����)
GL_QUADS: �簢�� �׸��� (�� 4����)
GL_TRIANGLE_FAN: ä���� �� �׸��� (�߽��� 1�� + ������ ����)
glVertex2f(x, y);: 2D ����(��)�� ��� �Լ�. glBegin/glEnd �ȿ����� �ǹ̰� �ֽ��ϴ�.
glColor3f(r, g, b);: ������ �����ϴ� �Լ� (0.0 ~ 1.0 ������ float ��). glVertex ���� ȣ���ؾ� �ش� ������ ���� ����˴ϴ�.
glLineWidth(�β�);: ���� �β��� �����մϴ�.
��. ��ǥ ��ȯ (���� �򰥸��� ���� �κ�)
���� ��ü�� ���������� �����̰� �Ϸ��� �� ������ �ݵ�� ����ؾ� �մϴ�.

C

glPushMatrix(); // ���� ��ǥ�� ���� ���� (�ڡڡڡڡ�)
{
    // �� ��� ���� ��ȯ�� ���� ��ü���� �����
    glTranslatef(x, y, 0.0f);                      // ���ϴ� ��ġ�� ��ǥ�� �̵�
    glRotatef(angle_in_degrees, 0.0f, 0.0f, 1.0f); // Z�� �������� ȸ�� (�ڡ�����: ����(degree) ���ڡ�)
    glScalef(sx, sy, 1.0f);                        // ũ�� ����

    // ���⿡ ��ü �׸��� �ڵ� (��: draw_circle)
}
glPopMatrix(); // �����ߴ� ��ǥ�� ���·� ���� (�ڡڡڡڡ�)
glPushMatrix()�� glPopMatrix()�� ������ ����ؼ� �ٸ� �׸��� ������ ���� �ʵ��� �ݸ��ϴ� ������ �մϴ�.
glRotatef�� ����(radian)�� �ƴ� ����(degree)�� ����մϴ�. ���� PI�� �̿��� �������� ������ ����ߴٸ� * (180.0f / PI)�� ���ؼ� ��ȯ����� �մϴ�.
��. ��ǥ�� ����
glOrtho(left, right, bottom, top, near, far);: 2D �������� ���� ��ǥ�踦 ����ϴ�. glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);���� �����ϸ� â�� ���� ���� -1, ������ ���� 1�� �˴ϴ�.
3. C��� �� ���� �ٽ�
��. ����ü (struct)
������ �Ӽ�(��ġ, ȸ��, ũ��)�� ��� �����ϴ� �� �ſ� �����մϴ�. Transform ����ü�� �� ����صμ���.
C

typedef struct { float x, y; } Vec2;
typedef struct { Vec2 position; float rotation; Vec2 scale; } Transform;
typedef struct { Transform transform; /* �߰� ���� *//* } Circle;
�Լ��� ����ü�� �ѱ� ���� ������(*)�� ����ϰ�, ����� ������ ���� ȭ��ǥ ������(->)�� ����մϴ�. (my_circle->transform.position.x)
��.�� �׸���(�ﰢ�Լ�)
���� �׸��� �ڵ�� ���迡 ���� Ȯ���� �ſ� �����ϴ�. for ������ �ﰢ�Լ� ������ �ܿ켼��.

C

// draw_circle �Լ��� for ����
for (int i = 0; i <= segments; i++) {
    float angle = 2.0f * PI * i / segments; // ���� ���� (����)
    float x = r * cosf(angle);              // ���� ������ (x��ǥ)
    float y = r * sinf(angle);              // ���� ������ (y��ǥ)
    glVertex2f(cx + x, cy + y);
}
4. ���� ���� : �� �� ��ó��(���ͳ� ����!)
�⺻ ��ݺ��� �ۼ� : � ������ ���͵� main �Լ��� ������� �������� �ۼ��ϰ� �����ϼ���.
�ܼ��� �ͺ��� ���� : ������ ������ �䱸�ϴ���, ���� ȭ�鿡 ���� �簢���̳� �ﰢ�� �ϳ��� ���� ���� ��ǥ�� �ϼ���.�ϴ� ���� ���̸� �ɸ������� �����˴ϴ�.
�Լ� �̸��̳� ���� ������ �򰥸� �� :
�Ϻ����� �ʾƵ� �������ϴ�.glVertex2f�� glVertex��� ���ų� glfwCreateWindow�� ���� ������ �ణ Ʋ����, �ּ����� ������ �Ϸ����� ��Ȯ�� �����ϸ� �κ� ������ ���� �� �ֽ��ϴ�.
�� : // ���⿡ �ʺ� 800, ���� 600�� â�� �����Ϸ��� ��
    �׸����� ���� ���� : ������ ĳ���ͳ� �ð� ���� �׷��� �Ѵٸ�, �ڵ� �ۼ� ���� �������� ������ � ����(��, �簢��, ��)���� �������� �׸��� �׷����� ��ǥ�� �뷫������ ���ϸ� �ڵ��� �ξ� �������ϴ�.
    �ּ� ���� Ȱ�� : ���� ���� ������ �ϰ� �ִ���, ������ ������ �� ������ �ּ����� ��� �ۼ��ϸ鼭 ������ �帧�� �����ϼ���.ä�������Ե� ���� �λ��� �ݴϴ�.
    ��� : �ּ��� �̰͸���!(ġƮ ��Ʈ)
    ����	�Լ��� / ����	����
    ���	glfwInit->CreateWindow->MakeContext -> while ����->Terminate	���α׷��� �����ֱ�
    ����	glClear->�׸���->SwapBuffers->PollEvents	�� �������� ����
    �׸���	glBegin(MODE) / glEnd()	���� �׸����� ���۰� ��
    ����	glVertex2f(x, y)	�� ���
    ����	glColor3f(r, g, b)	�� ����(0.0~1.0)
    ��ȯ	glPushMatrix() / glPopMatrix()	��ü �� ������ ����
    �̵�	glTranslatef(x, y, z)	��ġ �̵�
    ȸ��	glRotatef(angle_degrees, 0, 0, 1)	Z�� ȸ��(���� ���!)
    ��	for ���� + cosf(angle) / sinf(angle)	���� ���� ���
    ��ǥ��	glOrtho(-1, 1, -1, 1, ...)	2D �� ����

    Sheets�� ��������
    �� ����鸸 Ȯ���� �����ϰ� �����ϸ� ���ͳ� ���̵� ����� ���� ����� ������ �� ���� �̴ϴ�.����� ���ϴ�!

    */