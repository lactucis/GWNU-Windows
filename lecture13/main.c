﻿/*
 * GLFW + OpenGL 1.1 애니메이션 데모
 * - 오른쪽에 스마일맨(원 + 눈 + 웃는 입) 배치
 * - 스마일맨은 화면 중심을 기준으로 공전
 * - 얼굴(눈/입)은 자전 공전에 반대 방향으로 회전해 항상 수평 유지
 * - 스마일맨 주변에 육망성(별)이 공전하며 자체적으로 빠르게 자전
 */

#include <GLFW/glfw3.h>
#include <math.h>

#pragma comment(lib, "opengl32.lib")

#define PI 3.1415926535f

 // 원 그리기: 반지름 r, 세그먼트 수 seg
void drawCircle(float r, int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);            // 중심
    for (int i = 0; i <= seg; ++i) {
        float a = 2.0f * PI * i / seg;
        glVertex2f(cosf(a) * r, sinf(a) * r);
    }
    glEnd();
}

// 스마일맨 얼굴(눈 + 입) 그리기
void drawFace(float eyeR, float eyeOffsetX, float eyeOffsetY) {
    // 왼쪽 눈
    glPushMatrix();
    glTranslatef(-eyeOffsetX, eyeOffsetY, 0.0f);
    drawCircle(eyeR, 16);
    glPopMatrix();

    // 오른쪽 눈
    glPushMatrix();
    glTranslatef(eyeOffsetX, eyeOffsetY, 0.0f);
    drawCircle(eyeR, 16);
    glPopMatrix();

    // 웃는 입 (반원 아크)
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; ++i) {
        // -30° .. -150° 사이
        float a = (-30.0f - 120.0f * i / 20.0f) * (PI / 180.0f);
        glVertex2f(cosf(a) * 0.5f * eyeOffsetX, sinf(a) * 0.5f * eyeOffsetY);
    }
    glEnd();
}

// 육망성(별) 그리기: 반지름 r
void drawHexagram(float r) {
    float h = r * 0.866f; // sqrt(3)/2 * r
    glBegin(GL_TRIANGLES);
    // 윗 삼각형
    glVertex2f(0.0f, r);
    glVertex2f(-h, -r * 0.5f);
    glVertex2f(h, -r * 0.5f);
    // 아랫 삼각형 (역방향)
    glVertex2f(0.0f, -r);
    glVertex2f(-h, r * 0.5f);
    glVertex2f(h, r * 0.5f);
    glEnd();
}

int main(void) {
    // 1) GLFW 초기화
    if (!glfwInit()) return -1;

    // 2) 윈도우 생성 & 컨텍스트 바인딩
    GLFWwindow* window = glfwCreateWindow(800, 600, "Smiley Orbit & Hexagram", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // 3) 2D 직교 투영 설정 (좌표계 -1..1)
    glOrtho(-1, 1, -1, 1, -1, 1);

    // 애니메이션 파라미터
    const float smileOrbitR = 0.6f;   // 스마일맨 공전 반경
    const float smileRadius = 0.15f;  // 스마일맨 얼굴 반경
    const float eyeR = 0.02f;         // 눈 반경
    const float eyeOffX = 0.05f,
        eyeOffY = 0.07f;      // 눈 위치 오프셋
    const float hexOrbitR = 0.3f;     // 육망성 공전 반경
    const float hexRadius = 0.07f;    // 육망성 크기

    // 메인 루프
    while (!glfwWindowShouldClose(window)) {
        float t = (float)glfwGetTime();

        // 공전/자전 각도 계산
        float smileOrbitAng = t * 20.0f;     // 스마일맨 공전 속도
        float hexOrbitAng = t * 60.0f;     // 별 공전 속도 (빠름)
        float hexSelfAng = t * 200.0f;    // 별 자체 자전 속도 (더 빠름)

        // 4) 화면 클리어
        glClear(GL_COLOR_BUFFER_BIT);

        // 5) 스마일맨 공전 궤도
        glPushMatrix();
        glRotatef(smileOrbitAng, 0, 0, 1);                // 중심 기준 공전
        glTranslatef(smileOrbitR, 0.0f, 0.0f);          // 궤도 반경만큼 이동

        // 얼굴 그리기
        glColor3f(1.0f, 1.0f, 0.0f);                    // 노랑 얼굴
        drawCircle(smileRadius, 64);

        // 얼굴 특징(눈/입)은 역회전으로 수평 유지
        glPushMatrix();
        glRotatef(-smileOrbitAng, 0, 0, 1);
        glColor3f(0.0f, 0.0f, 0.0f);                  // 눈/입 검정
        drawFace(eyeR, eyeOffX, eyeOffY);
        glPopMatrix();

        // 6) 육망성 공전 & 자전
        glPushMatrix();
        glRotatef(hexOrbitAng, 0, 0, 1);                // 스마일맨 기준 공전
        glTranslatef(hexOrbitR, 0.0f, 0.0f);          // 궤도 반경만큼 이동
        glRotatef(hexSelfAng, 0, 0, 1);                 // 별 자체 자전
        glColor3f(0.2f, 0.6f, 1.0f);                  // 파랑 별
        drawHexagram(hexRadius);
        glPopMatrix();

        glPopMatrix();  // 스마일맨 공전 행렬 복원

        // 7) 버퍼 스왑 & 이벤트 처리
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 8) 정리
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
겟어싱크스테이트, 색의 3원색 알기
(색 변화, 어떤 키를 누르면 표정이 바뀌는 등)
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