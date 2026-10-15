#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "triangles.hpp"
#include "graphic.hpp"

const GLuint WIDTH = 800, HEIGHT = 600;

struct RenderData {
    glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 5.0f);
    float     yaw        = -90.0f;
    float     pitch      =   0.0f;
    glm::vec3 forward    = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right      = glm::vec3(1.0f, 0.0f, 0.0f);
};

namespace {

const char* kVertexSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
uniform mat4 uMVP;
out vec3 vColor;
void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vColor = aColor;
}
)";

const char* kFragmentSrc = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() { FragColor = vec4(vColor, 1.0); }
)";

GLuint g_program = 0;
GLuint g_vao = 0, g_vbo = 0;
GLsizei g_vertexCount = 0;

GLuint CompileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024]; glGetShaderInfoLog(s, 1024, nullptr, log);
        std::cerr << "Shader error: " << log << "\n";
    }
    return s;
}

glm::vec3 ForwardFromYawPitch(float yawDeg, float pitchDeg) {
    float y = glm::radians(yawDeg);
    float p = glm::radians(pitchDeg);
    return glm::normalize(glm::vec3(
        std::cos(p) * std::cos(y),
        std::sin(p),
        std::cos(p) * std::sin(y)
    ));
}

void InitOnce(TrianglesData& t_data) {
    GLuint vs = CompileShader(GL_VERTEX_SHADER, kVertexSrc);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, kFragmentSrc);
    g_program = glCreateProgram();
    glAttachShader(g_program, vs);
    glAttachShader(g_program, fs);
    glLinkProgram(g_program);
    glDeleteShader(vs); glDeleteShader(fs);

    std::vector<float> verts;
    verts.reserve(t_data.GetTriangles().size() * 3 * 6);
    for (const auto& tri : t_data.GetTriangles()) {
        for (int i = 0; i < 3; ++i) {
            const float r = tri.interception ? 1.0f : 1.0f;
            const float g = tri.interception ? 0.0f : 1.0f;
            const float b = tri.interception ? 0.0f : 1.0f;

            verts.push_back(tri.vertices[i].x);
            verts.push_back(tri.vertices[i].y);
            verts.push_back(tri.vertices[i].z);
            verts.push_back(r);
            verts.push_back(g);
            verts.push_back(b);
        }
    }
    g_vertexCount = (GLsizei)(verts.size() / 6);

    glGenVertexArrays(1, &g_vao);
    glGenBuffers(1, &g_vbo);
    glBindVertexArray(g_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float),
                 verts.data(), GL_STATIC_DRAW);

    const GLsizei stride = 6 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void Render(TrianglesData& t_data, RenderData& r_data) {
    static bool inited = false;
    if (!inited) { InitOnce(t_data); inited = true; }

    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 camPos = r_data.camera_pos;
    glm::vec3 target = r_data.camera_pos + r_data.forward;
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::mat4 proj  = glm::perspective(glm::radians(60.0f),
                                       (float)WIDTH / (float)HEIGHT,
                                       0.1f, 1000.0f);
    glm::mat4 view  = glm::lookAt(camPos, target, up);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp   = proj * view * model;

    glUseProgram(g_program);
    GLint loc = glGetUniformLocation(g_program, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(g_vao);
    glDrawArrays(GL_TRIANGLES, 0, g_vertexCount);
    glBindVertexArray(0);
}

struct InputState {
    bool w = false, a = false, s = false, d = false;
    bool shift = false;
    bool up = false, down = false, left = false, right = false;
    double lastMouseX = 0, lastMouseY = 0;
    bool firstMouse = true;
};

void KeyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    auto* st = static_cast<InputState*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_W:          st->w     = true; break;
            case GLFW_KEY_A:          st->a     = true; break;
            case GLFW_KEY_S:          st->s     = true; break;
            case GLFW_KEY_D:          st->d     = true; break;
            case GLFW_KEY_LEFT_SHIFT: st->shift = true; break;
            case GLFW_KEY_UP:         st->up    = true; break;
            case GLFW_KEY_DOWN:       st->down  = true; break;
            case GLFW_KEY_LEFT:       st->left  = true; break;
            case GLFW_KEY_RIGHT:      st->right = true; break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W:          st->w     = false; break;
            case GLFW_KEY_A:          st->a     = false; break;
            case GLFW_KEY_S:          st->s     = false; break;
            case GLFW_KEY_D:          st->d     = false; break;
            case GLFW_KEY_LEFT_SHIFT: st->shift = false; break;
            case GLFW_KEY_UP:         st->up    = false; break;
            case GLFW_KEY_DOWN:       st->down  = false; break;
            case GLFW_KEY_LEFT:       st->left  = false; break;
            case GLFW_KEY_RIGHT:      st->right = false; break;
        }
    }
}

} // namespace

void Graphic(TrianglesData& t_data) {

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Triangles", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    RenderData r_data;

    InputState input;
    glfwSetWindowUserPointer(window, &input);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    double prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = static_cast<float>(now - prevTime);
        if (dt > 0.1f) dt = 0.1f;
        prevTime = now;

        glfwPollEvents();

        const float turnSpeed = 90.0f;

        if (input.left)  r_data.yaw   -= turnSpeed * dt;
        if (input.right) r_data.yaw   += turnSpeed * dt;
        if (input.up)    r_data.pitch += turnSpeed * dt;
        if (input.down)  r_data.pitch -= turnSpeed * dt;

        r_data.pitch = glm::clamp(r_data.pitch, -89.0f, 89.0f);

        r_data.forward = ForwardFromYawPitch(r_data.yaw, r_data.pitch);
        r_data.right   = glm::normalize(
            glm::cross(r_data.forward, glm::vec3(0.0f, 1.0f, 0.0f)));

        float speed = 5.0f * (input.shift ? 3.0f : 1.0f);
        float step  = speed * dt;

        glm::vec3 move(0.0f);
        if (input.w) move += r_data.forward;
        if (input.s) move -= r_data.forward;
        if (input.d) move += r_data.right;
        if (input.a) move -= r_data.right;

        if (glm::length(move) > 0.0f)
            r_data.camera_pos += glm::normalize(move) * step;

        Render(t_data, r_data);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}