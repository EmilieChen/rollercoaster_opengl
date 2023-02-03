#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/glm.hpp>
#include <vector>

using namespace glimac;
// using namespace std;

int window_width  = 1280;
int window_height = 1280;

static void key_callback(GLFWwindow* /*window*/, int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/)
{
}

static void mouse_button_callback(GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/)
{
}

static void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double /*yoffset*/)
{
}

static void cursor_position_callback(GLFWwindow* /*window*/, double /*xpos*/, double /*ypos*/)
{
}

static void size_callback(GLFWwindow* /*window*/, int width, int height)
{
    window_width  = width;
    window_height = height;
}

class Vertex2DColor {
    public:
        glm::vec2 position;

    public:
        glm::vec3 color;

    public:
        Vertex2DColor()
        {
        }

    public:
        Vertex2DColor(glm::vec2 pos, glm::vec3 col)
        {
            position = pos;
            color    = col;
        }
};


class Vertex2DUV {
    public:
        glm::vec2 position;

    public:
        glm::vec2 texture_coord;

    public:
        Vertex2DUV()
        {
        }

    public:
        Vertex2DUV(glm::vec2 pos, glm::vec2 uv)
        {
            position      = pos;
            texture_coord = uv;
        }
};

glm::mat3 translate(float tx, float ty)
{
    return glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(tx, ty, 1));
}

glm::mat3 scale(float sx, float sy)
{
    return glm::mat3(glm::vec3(sx, 0, 0), glm::vec3(0, sy, 0), glm::vec3(0, 0, 1));
}

glm::mat3 rotate(float a)
{
    a = glm::radians(a);
    return glm::mat3(glm::vec3(cos(a), sin(a), 0), glm::vec3(-sin(a), cos(a), 0), glm::vec3(0, 0, 1));
}

int main(int argc, char* argv[])
{
    int a = argc;
    a++;
    // cout << "You have entered " << argc << " arguments:" << "\n";
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a window and its OpenGL context */
#ifdef __APPLE__
    /* We need to explicitly ask for a 3.3 context on Mac */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP3", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Intialize glad (loads the OpenGL functions) */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);

    // -----------------------------------------------------------------------------------------
    //
    // 
    std::unique_ptr<Image> imagePtr = loadImage("C:/Users/emili/Documents/E5/workspace/OpenGL/GLImac-Template_09_12/GLImac-Template/TP4/assets/textures/triforce.png");

    
    // Création d'une texture:
    GLuint texture;
    glGenTextures(1, &texture);

    // Binding d'une texture
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imagePtr->getWidth(), imagePtr->getHeight(), 0, GL_RGBA, GL_FLOAT, imagePtr->getPixels());
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Unbinding de la texture
    glBindTexture(GL_TEXTURE_2D, 0);


    // Appel shaders
    FilePath applicationPath(argv[0]);
    Program  program = loadProgram(applicationPath.dirPath() + "TP4/shaders/tex2D.vs.glsl", applicationPath.dirPath() + "TP4/shaders/tex2D.fs.glsl");
    program.use();
    //const GLuint location_uTime = glGetUniformLocation(program.getGLId(), "uTime");
    const GLuint location_uModelMatrix = glGetUniformLocation(program.getGLId(), "uModelMatrix");
    const GLuint location_uColor       = glGetUniformLocation(program.getGLId(), "uColor");
    const GLuint location_uTexture     = glGetUniformLocation(program.getGLId(), "uTexture");

    // Création d'un VBO:
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Binding d'un VBO sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Remplir le VBO en utilisant une structure de vertex
    Vertex2DUV vertices[] = {
        Vertex2DUV(glm::vec2(-1, -1), glm::vec2(0, 1)),
        Vertex2DUV(glm::vec2(1, -1), glm::vec2(1, 1)),
        Vertex2DUV(glm::vec2(0, 1), glm::vec2(0.5, 0))};

    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DUV), vertices, GL_STATIC_DRAW);

    // Unbinding d'un VBO sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Création d'un VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Binding du VAO et VBO
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Activation des attributs de vertex
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_TEXTURE_COORD = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE_COORD);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (GLvoid*)offsetof(Vertex2DUV, position));
    glVertexAttribPointer(VERTEX_ATTR_TEXTURE_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (GLvoid*)offsetof(Vertex2DUV, texture_coord));

    // Unbinding du VAO et VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    float i = 0;
    float scaleValue = 1 / (double)4;
    glm::mat3 M;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Binding du VAO
        glBindVertexArray(vao);

        // Binding d'une texture
        glBindTexture(GL_TEXTURE_2D, texture);

        glUniform1i(location_uTexture, 0);

        M = rotate(i) * translate(float(-0.5), float(2 / (double)3)) * scale(scaleValue, scaleValue);
        //glUniform1f(location_uTime, i);
        glUniformMatrix3fv(location_uModelMatrix, 1, GL_FALSE, glm::value_ptr(M));
        glUniform3fv(location_uColor, 1, glm::value_ptr(glm::vec3(1, 0, 0)));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        M = rotate(i) * translate(float(0.5), float(2 / (double)3)) * scale(scaleValue, scaleValue) * rotate(i);
        glUniformMatrix3fv(location_uModelMatrix, 1, GL_FALSE, glm::value_ptr(M));
        glUniform3fv(location_uColor, 1, glm::value_ptr(glm::vec3(0, 1, 0)));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        M = rotate(i) * translate(float(-0.5), float(-1 / (double)3)) * scale(scaleValue, scaleValue) * rotate(i);
        glUniformMatrix3fv(location_uModelMatrix, 1, GL_FALSE, glm::value_ptr(M));
        glUniform3fv(location_uColor, 1, glm::value_ptr(glm::vec3(0, 0, 1)));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        M = rotate(i) * translate(float(0.5), float(-1 / (double)3)) * scale(scaleValue, scaleValue) * rotate(i);
        glUniformMatrix3fv(location_uModelMatrix, 1, GL_FALSE, glm::value_ptr(M));
        glUniform3fv(location_uColor, 1, glm::value_ptr(glm::vec3(1, 0, 1)));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // Unbinding d'une texture
        glBindTexture(GL_TEXTURE_2D, 0);

        i++;

        // Unbinding du VAO
        glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    // Libération des ressources
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &texture);

    glfwTerminate();
    return 0;
}
