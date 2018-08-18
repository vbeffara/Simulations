#include <GLFW/glfw3.h>
#include <linmath.h>
#include <stdlib.h>

struct vertex {
    float x, y, r, g, b;
};

const vertex vertices[3] = {{-.6, -.4, 1, 0, 0}, {.6, -.4, 0, 1, 0}, {0, 0.6, 0, 0, 1}};

const char * vertex_shader_text = "uniform mat4 MVP;\n"
                                  "attribute vec3 vCol;\n"
                                  "attribute vec2 vPos;\n"
                                  "varying vec3 color;\n"
                                  "void main()\n"
                                  "{\n"
                                  "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
                                  "    color = vCol;\n"
                                  "}\n";

const char * fragment_shader_text = "varying vec3 color;\n"
                                    "void main()\n"
                                    "{\n"
                                    "    gl_FragColor = vec4(color, 1.0);\n"
                                    "}\n";

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
    if (!glfwInit()) exit(EXIT_FAILURE);

    auto window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    auto mvp_location  = glGetUniformLocation(program, "MVP");
    auto vpos_location = glGetAttribLocation(program, "vPos");
    auto vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 2));

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        float  ratio = width / (float)height;
        mat4x4 m, p, mvp;
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1, 1, 1, -1);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
