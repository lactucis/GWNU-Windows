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

typedef struct
{
    Transform transform;
} Square;

void draw_square(Square* sq)
{
    glPushMatrix();

    glTranslatef(sq->transform.position.x, sq->transform.position.y, 0.0f);
    glRotatef(sq->transform.rotation * (180.0f / PI), 0.0f, 0.0f, 1.0f);
    glScalef(sq->transform.scale.x, sq->transform.scale.y, 1.0f);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(-0.5f, -0.5f);
    glEnd();

    glPopMatrix();
}

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

    glBegin(GL_LINES);
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

    Square sq = { {{0.0f, 0.0f}, 0, {5, 5}} };

    Circle face = { {{0.0f, 0.0f}, 0, {1.5, 1.5}}, 32 };          // 원: (0.5,0.5), 회전0, 스케일1, 32세그먼트
    Circle faceline = { {{0.0f, 0.0f}, 0, {1.52, 1.52}}, 32 };          // 원: (0.5,0.5), 회전0, 스케일1, 32세그먼트
    Circle nose = { {{0.0f, -0.2f}, 0, {0.2, 0.2}}, 32 };          // 원: (0.5,0.5), 회전0, 스케일1, 32세그먼트
    Circle mouth_white1 = { {{0.13f, -0.33f}, 0, {0.3, 0.3}}, 32 }; // 하얀 원
    Circle mouth_white2 = { {{-0.13f, -0.33f}, 0, {0.3, 0.3}}, 32 }; // 하얀 원
    Circle left_eye = { {{-0.63f, 0.19f}, 0, {0.11f, 0.11f}}, 32 };   // 왼쪽 눈
    Circle right_eye = { {{0.64f, 0.19f}, 0, {0.11f, 0.11f}}, 32 };   // 오른쪽 눈

    while (!glfwWindowShouldClose(window))      // 윈도우가 닫힐 때까지 반복
    {
        float time = (float)glfwGetTime();      // 프로그램 시작 후 흐른 시간(초)


        glClear(GL_COLOR_BUFFER_BIT);                               // 화면 초기화(지움)

        draw_square(&sq);

        draw_circle(&faceline, 0.0f, 0.0f, 0.0f);
        draw_circle(&face, 0.0f, 0.44f, 0.75f);

        draw_circle(&left_eye, 0.0f, 0.0f, 0.0f);
        draw_circle(&right_eye, 0.0f, 0.0f, 0.0f);
        draw_circle(&mouth_white1, 1.0f, 1.0f, 1.0f);
        draw_circle(&mouth_white2, 1.0f, 1.0f, 1.0f);
        draw_circle(&nose, 0.0f, 0.0f, 0.0f);

        draw_line(-0.20f, -0.23f, -0.45f, -0.18f);
        draw_line(-0.22f, -0.35f, -0.45f, -0.35f);
        draw_line(-0.20f, -0.43f, -0.45f, -0.50f);

        draw_line(0.20f, -0.23f, 0.45f, -0.18f);
        draw_line(0.20f, -0.33f, 0.45f, -0.33f);
        draw_line(0.20f, -0.43f, 0.45f, -0.48f);

        glfwSwapBuffers(window);                                    // 버퍼 스왑(화면 갱신)
        glfwPollEvents();                                           // 이벤트(입력 등) 처리
    }

    glfwTerminate();                        // GLFW 종료(자원 해제)
    return 0;                               // 프로그램 종료
}

/*
이 코드는 GLFW와 구형 OpenGL을 사용하여 도라에몽 캐릭터의 얼굴을 그리는 프로그램입니다.  doraemon️

여러 개의 원, 사각형, 그리고 직선과 같은 기본 도형을 조합하여 하나의 정적인 이미지를 만듭니다.

코드 상세 분석
이 코드는 여러 개의 모듈화된 함수를 사용하여 각기 다른 도형을 그리고, main 함수에서 이들을 순서대로 호출하여 최종 이미지를 완성합니다.

주요 그리기 함수
draw_square(): 사각형을 그립니다. 코드에서는 창 전체를 덮는 흰색 사각형을 그려 배경으로 사용합니다.
draw_circle(): 색상이 채워진 원을 그립니다. 얼굴 윤곽선, 얼굴색, 눈, 코, 입 등 도라에몽 얼굴의 대부분의 요소를 그리는 데 사용됩니다.
draw_line(): 직선을 그립니다. 도라에몽의 수염 6개를 그리는 데 사용됩니다.
main 함수의 렌더링 흐름
while 루프 안에서 다음과 같은 순서로 도형을 그려 도라에몽 얼굴을 완성합니다. 이는 화가가 캔버스에 배경부터 칠하고 그 위에 그림을 그려나가는 순서와 같습니다.

draw_square(&sq): 커다란 흰색 사각형을 그려 배경을 채웁니다.
draw_circle(&faceline, ...): 얼굴 윤곽선을 그리기 위해 약간 큰 검은색 원을 먼저 그립니다.
draw_circle(&face, ...): 윤곽선 위에 약간 작은 파란색 원을 덮어 그려 도라에몽의 얼굴색을 표현합니다.
눈, 코, 입 그리기:
검은색 원 두 개로 눈을 그립니다.
하얀색 원 두 개를 겹쳐서 도라에몽 특유의 입 주변의 흰 부분을 표현합니다.
가운데에 검은색 원으로 코를 그립니다.
draw_line(...): 총 6번의 draw_line 함수 호출을 통해 얼굴 양쪽에 수염을 각각 3개씩 그립니다.
화면 업데이트: glfwSwapBuffers 함수를 호출하여 완성된 그림을 화면에 표시합니다.
결론 📝
이 코드는 여러 개의 기본 도형(Circle, Square, Line)을 정의하고, 각 도형의 위치, 크기, 색상을 정밀하게 조절하여 복잡한 캐릭터 이미지를 만들어내는 좋은 예시입니다. 각 신체 부위를 별도의 도형 객체로 선언하고 순서대로 그림으로써, 계층적인 2D 그래픽 렌더링의 기본 원리를 잘 보여줍니다.
*/