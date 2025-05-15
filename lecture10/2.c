#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>             // GLFW 라이브러리(윈도우 생성 및 입력, OpenGL context 제공)
#include <math.h>                   // 수학 함수(sin, cos 등) 사용

#define PI 3.1415926535f            // 원주율(파이) 상수 정의, f는 float형

// 2차원 벡터 구조체
typedef struct
{
    float x, y;                     // x, y 좌표값
} Vec2;

// 변환(Transformation) 정보 구조체
typedef struct
{
    Vec2 position;                  // 위치 (x, y)
    float rotation;                 // 회전 각도(라디안)
    Vec2 scale;                     // 크기(x, y 스케일)
} Transform;


// 원 구조체: 변환 정보와 세그먼트 개수 가짐
typedef struct
{
    Transform transform;            // 원의 변환 정보
    int segments;                   // 원을 몇 개의 점(삼각형)으로 그릴지
} Circle;

// 원 그리기 함수
void draw_circle(Circle* c, float r, float g, float b)
{
    glPushMatrix(); // 현재 변환행렬 저장

    // 위치 이동, 회전, 크기 변환 적용
    glTranslatef(c->transform.position.x, c->transform.position.y, 0.0f);             // 위치 이동
    glRotatef(c->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);               // 회전
    glScalef(c->transform.scale.x, c->transform.scale.y, 1.0f);                      // 크기 변환

    glBegin(GL_TRIANGLE_FAN);                   // 삼각형 부채꼴로 원 그리기 시작
    glColor3f(r, g, b);
    glVertex2f(0.0f, 0.0f);                     // 중심점

    for (int i = 0; i <= c->segments; ++i)      // 세그먼트(점) 개수만큼 반복
    {
        float angle = 2.0f * PI * i / c->segments;     // 현재 각도(라디안)
        glVertex2f(cosf(angle) * 0.5f, sinf(angle) * 0.5f); // 원 위의 점(반지름 0.5)
    }

    glEnd();                                    // 원 그리기 끝

    glPopMatrix(); // 변환행렬 복원
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
    if (!glfwInit())                            // GLFW 초기화 실패하면
        return -1;                              // 종료

    GLFWwindow* window = glfwCreateWindow(537, 445, "OpenGL C Demo", NULL, NULL);
    if (!window)                               // 윈도우 생성 실패하면
    {
        glfwTerminate();                       // GLFW 종료
        return -1;                             // 프로그램 종료
    }

    glfwMakeContextCurrent(window);             // 현재 쓰레드에 OpenGL context 연결
    glOrtho(-1, 1, -1, 1, -1, 1);              // 2D 좌표계(-1~1, 좌하단~우상단)

    // 도형 초기화: (위치, 회전, 크기)

    Circle face = { {{0.0f, 0.0f}, 0, {1.5, 1.5}}, 32 };          // 원: (0.5,0.5), 회전0, 스케일1, 32세그먼트
    Circle nose = { {{0.0f, -0.2f}, 0, {0.2, 0.2}}, 32 };          // 원: (0.5,0.5), 회전0, 스케일1, 32세그먼트
    Circle mouth_white1 = { {{0.1f, -0.33f}, 0, {0.3, 0.3}}, 32 }; // 하얀 원
    Circle mouth_white2 = { {{-0.1f, -0.33f}, 0, {0.3, 0.3}}, 32 }; // 하얀 원
    Circle left_eye = { {{-0.63f, 0.19f}, 0, {0.11f, 0.11f}}, 32 };   // 왼쪽 눈
    Circle right_eye = { {{0.64f, 0.19f}, 0, {0.11f, 0.11f}}, 32 };   // 오른쪽 눈

    while (!glfwWindowShouldClose(window))      // 윈도우가 닫힐 때까지 반복
    {
        float time = (float)glfwGetTime();      // 프로그램 시작 후 흐른 시간(초)

        

        glClear(GL_COLOR_BUFFER_BIT);                               // 화면 초기화(지움)

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

        glfwSwapBuffers(window);                                    // 버퍼 스왑(화면 갱신)
        glfwPollEvents();                                           // 이벤트(입력 등) 처리
    }

    glfwTerminate();                        // GLFW 종료(자원 해제)
    return 0;                               // 프로그램 종료
}
