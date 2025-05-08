#pragma comment(lib, "opengl32.lib")
#include <GLFW/glfw3.h>
#include <math.h>

void draw_circle(float cx, float cy, float r, int segments) {
    glBegin(GL_TRIANGLE_FAN); 

    glColor3f(0.2f, 0.6f, 1.0f);
    glVertex2f(cx, cy);        

    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * 3.1415926f * i / segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }

    glEnd();
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(500, 500, "GLFW: Circle", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        draw_circle(0.0f, 0.0f, 0.5f, 60);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
