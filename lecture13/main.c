#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>
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
    int segments;
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
    // 1. 초기화
    if (!glfwInit())
        return -1;

    // 2. 윈도우 생성
    GLFWwindow* window = glfwCreateWindow(800, 700, "Window Title", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // 3. 컨텍스트 설정
    glfwMakeContextCurrent(window);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // 2D 좌표계 설정 (거의 필수)

    Circle face = { {{-0.7f, 0.0f}, 0, {0.5, 0.5}}, 32 };          // 원: (0.5,0.5), 회전0, 스케일1, 32세그먼트

    const float orbit_radius = 0.7f;
    const float circle_radius = 0.1f;

    // 4. 메인 루프
    while (!glfwWindowShouldClose(window))
    {
        // 5. 화면 지우기
        glClear(GL_COLOR_BUFFER_BIT);

        float time = (float)glfwGetTime();

        

        // --- 여기에 그림 그리는 코드 작성 ---
        draw_circle(&face, 1.0f, 1.0f, 0.0f);
        draw_line(-0.8f, 0.1f, -0.8f, 0.0f);
        draw_line(-0.6f, 0.1f, -0.6f, 0.0f);


        // 6. 버퍼 교체 및 이벤트 처리
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7. 종료
    glfwTerminate();
    return 0;
}



/*
테스트 문제

1. 동그라미를 하나 그리고, 직선 직선 (눈), 입 (눈 사이의 반원 그리기), 반지름 0.2
2. 가로세로 비율 1:1(크기는 상관 없음)
3. 정 가운데에 네모 그리기, 한 변은 0.02, 대각선 네모 안에 그리기.
4. 네모와 얼굴은 거리 0.5
5. 별은 얼굴 근처를 자전, 얼굴은 중앙을 기준으로 공전, 얼굴은 돌기도 해야 함.
6. 얼굴은 노랑, 입은 검정


알아야할거
공전, 자전 필수로 나옴
드로우 라인, 드로우 트라이엥글 드로우 포인트
이동, 회전, 확대, 축소 - 명확하게 알고 있기
스택, 푸쉬팝
부모와 자식관계 하이러키를 만들어서 애니메이션하기
*/





/*
1. 프로그램의 기본 골격 (가장 중요!)
이 구조는 어떤 문제를 받든 가장 먼저 작성해야 하는 뼈대입니다. 통째로 외우세요.

C

#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>

int main()
{
    // 1. 초기화
    if (!glfwInit())
        return -1;

    // 2. 윈도우 생성
    GLFWwindow* window = glfwCreateWindow(800, 600, "Window Title", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // 3. 컨텍스트 설정
    glfwMakeContextCurrent(window);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // 2D 좌표계 설정 (거의 필수)

    // 4. 메인 루프
    while (!glfwWindowShouldClose(window))
    {
        // 5. 화면 지우기
        glClear(GL_COLOR_BUFFER_BIT);

        // --- 여기에 그림 그리는 코드 작성 ---

        // 6. 버퍼 교체 및 이벤트 처리
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7. 종료
    glfwTerminate();
    return 0;
}
2. OpenGL 핵심 암기사항 (그리기 기술)
가. 그리기 기본 (glBegin / glEnd)
glBegin(MODE); / glEnd();: 모든 그리기의 시작과 끝. MODE를 잊으면 안 됩니다.
GL_LINES: 선 그리기 (점 2개씩)
GL_TRIANGLES: 삼각형 그리기 (점 3개씩)
GL_QUADS: 사각형 그리기 (점 4개씩)
GL_TRIANGLE_FAN: 채워진 원 그리기 (중심점 1개 + 나머지 점들)
glVertex2f(x, y);: 2D 정점(점)을 찍는 함수. glBegin/glEnd 안에서만 의미가 있습니다.
glColor3f(r, g, b);: 색상을 지정하는 함수 (0.0 ~ 1.0 사이의 float 값). glVertex 전에 호출해야 해당 정점에 색이 적용됩니다.
glLineWidth(두께);: 선의 두께를 지정합니다.
나. 좌표 변환 (가장 헷갈리기 쉬운 부분)
여러 객체를 독립적으로 움직이게 하려면 이 패턴을 반드시 기억해야 합니다.

C

glPushMatrix(); // 현재 좌표계 상태 저장 (★★★★★)
{
    // 이 블록 안의 변환은 현재 객체에만 적용됨
    glTranslatef(x, y, 0.0f);                      // 원하는 위치로 좌표계 이동
    glRotatef(angle_in_degrees, 0.0f, 0.0f, 1.0f); // Z축 기준으로 회전 (★★주의: 각도(degree) 사용★★)
    glScalef(sx, sy, 1.0f);                        // 크기 조절

    // 여기에 객체 그리기 코드 (예: draw_circle)
}
glPopMatrix(); // 저장했던 좌표계 상태로 복원 (★★★★★)
glPushMatrix()와 glPopMatrix()는 쌍으로 사용해서 다른 그림에 영향이 가지 않도록 격리하는 역할을 합니다.
glRotatef는 라디안(radian)이 아닌 각도(degree)를 사용합니다. 만약 PI를 이용해 라디안으로 각도를 계산했다면 * (180.0f / PI)를 곱해서 변환해줘야 합니다.
다. 좌표계 설정
glOrtho(left, right, bottom, top, near, far);: 2D 렌더링을 위한 좌표계를 만듭니다. glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);으로 설정하면 창의 왼쪽 끝이 -1, 오른쪽 끝이 1이 됩니다.
3. C언어 및 수학 핵심
가. 구조체 (struct)
도형의 속성(위치, 회전, 크기)을 묶어서 관리하는 데 매우 유용합니다. Transform 구조체는 꼭 기억해두세요.
C

typedef struct { float x, y; } Vec2;
typedef struct { Vec2 position; float rotation; Vec2 scale; } Transform;
typedef struct { Transform transform; /* 추가 정보 *//* } Circle;
함수에 구조체를 넘길 때는 포인터(*)를 사용하고, 멤버에 접근할 때는 화살표 연산자(->)를 사용합니다. (my_circle->transform.position.x)
나.원 그리기(삼각함수)
원을 그리는 코드는 시험에 나올 확률이 매우 높습니다. for 루프와 삼각함수 공식을 외우세요.

C

// draw_circle 함수의 for 루프
for (int i = 0; i <= segments; i++) {
    float angle = 2.0f * PI * i / segments; // 현재 각도 (라디안)
    float x = r * cosf(angle);              // 원의 방정식 (x좌표)
    float y = r * sinf(angle);              // 원의 방정식 (y좌표)
    glVertex2f(cx + x, cy + y);
}
4. 시험 전략 : 모를 때 대처법(인터넷 없이!)
기본 골격부터 작성 : 어떤 문제가 나와도 main 함수의 뼈대부터 막힘없이 작성하고 시작하세요.
단순한 것부터 시작 : 복잡한 도형을 요구하더라도, 먼저 화면에 작은 사각형이나 삼각형 하나라도 띄우는 것을 목표로 하세요.일단 뭐라도 보이면 심리적으로 안정됩니다.
함수 이름이나 인자 순서가 헷갈릴 때 :
완벽하지 않아도 괜찮습니다.glVertex2f를 glVertex라고 쓰거나 glfwCreateWindow의 인자 순서가 약간 틀려도, 주석으로 무엇을 하려는지 명확히 설명하면 부분 점수를 받을 수 있습니다.
예 : // 여기에 너비 800, 높이 600의 창을 생성하려고 함
    그림으로 먼저 설계 : 복잡한 캐릭터나 시계 등을 그려야 한다면, 코드 작성 전에 시험지에 간단히 어떤 도형(원, 사각형, 선)으로 구성될지 그림을 그려보고 좌표를 대략적으로 정하면 코딩이 훨씬 쉬워집니다.
    주석 적극 활용 : 내가 지금 무엇을 하고 있는지, 다음엔 무엇을 할 것인지 주석으로 계속 작성하면서 생각의 흐름을 정리하세요.채점관에게도 좋은 인상을 줍니다.
    요약 : 최소한 이것만은!(치트 시트)
    구분	함수명 / 개념	역할
    골격	glfwInit->CreateWindow->MakeContext -> while 루프->Terminate	프로그램의 생명주기
    루프	glClear->그리기->SwapBuffers->PollEvents	매 프레임의 순서
    그리기	glBegin(MODE) / glEnd()	도형 그리기의 시작과 끝
    정점	glVertex2f(x, y)	점 찍기
    색상	glColor3f(r, g, b)	색 지정(0.0~1.0)
    변환	glPushMatrix() / glPopMatrix()	객체 간 독립성 보장
    이동	glTranslatef(x, y, z)	위치 이동
    회전	glRotatef(angle_degrees, 0, 0, 1)	Z축 회전(각도 사용!)
    원	for 루프 + cosf(angle) / sinf(angle)	원의 정점 계산
    좌표계	glOrtho(-1, 1, -1, 1, ...)	2D 뷰 설정

    Sheets로 내보내기
    이 내용들만 확실히 숙지하고 이해하면 인터넷 없이도 충분히 좋은 결과를 얻으실 수 있을 겁니다.행운을 빕니다!

    */