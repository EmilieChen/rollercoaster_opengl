#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/common.hpp>
//#include <glimac/TrackballCamera.hpp>
#include <glimac/FreelyCamera.hpp>

using namespace glimac;

int window_width  = 1280;
int window_height = 720;
bool   firstMouse    = true;
float lastX = 0;
float lastY = 0;

// SDLWindowManager windowManager(window_width, window_height, "GLImac");
FreelyCamera camera;


 static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
     if (key == GLFW_KEY_W && action == GLFW_PRESS)
         camera.rotateUp(10.f);
     if (key == GLFW_KEY_S && action == GLFW_PRESS)
         camera.rotateUp(-10.f);
     if(key == GLFW_KEY_A && action == GLFW_PRESS)
         camera.rotateLeft(10.f);
     if(key == GLFW_KEY_D && action == GLFW_PRESS)
         camera.rotateLeft(-10.f);
 }

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.moveFront((float)yoffset);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX      = (float) xpos;
        lastY      = (float) ypos;
        firstMouse = false;
    }

    float xoffset = (float) (xpos - lastX);

    float yoffset = (float) (lastY - ypos);
    lastX         = (float) xpos;
    lastY         = (float) ypos;

    float sensitivity = 0.2f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;


    if (yoffset > 89.0f)
        yoffset = 89.0f;
    if (yoffset < -89.0f)
        yoffset = -89.0f;
    if (xoffset > 89.0f)
        xoffset = 89.0f;
    if (xoffset < -89.0f)
        xoffset = -89.0f;

    camera.rotateUp(yoffset);

    camera.rotateLeft(-xoffset);
    camera.computeDirectionVectors();
}

static void size_callback(GLFWwindow* /*window*/, int width, int height)
{
    window_width  = width;
    window_height = height;
}

glm::mat4 rotate(float a)
{
    a = glm::radians(a);
    return glm::mat4(glm::vec4(cos(a), sin(a), 0, 0), glm::vec4(-sin(a), cos(a), 0, 0), glm::vec4(0, 0, 0, 1), glm::vec4(0, 0, 0, 1));
}


struct EarthProgram {
    Program program;

    GLint location_uMVPMatrix;
    GLint location_uMVMatrix;
    GLint location_uNormalMatrix;
    GLint location_uEarthTexture;
    GLint location_uCloudTexture;
    EarthProgram(const FilePath& applicationPath)
        : program(loadProgram(applicationPath.dirPath() + "TP7/shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "TP7/shaders/multiTex3D.fs.glsl"))
    {
        location_uMVPMatrix    = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
        location_uMVMatrix     = glGetUniformLocation(program.getGLId(), "uMVMatrix");
        location_uNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
        location_uEarthTexture = glGetUniformLocation(program.getGLId(), "uEarthTexture");
        location_uCloudTexture = glGetUniformLocation(program.getGLId(), "uCloudTexture");
    }
};

struct MoonProgram {
    Program program;

    GLint location_uMVPMatrix;
    GLint location_uMVMatrix;
    GLint location_uNormalMatrix;
    GLint location_uTexture;

    MoonProgram(const FilePath& applicationPath)
        : program(loadProgram(applicationPath.dirPath() + "TP7/shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "TP7/shaders/tex3D.fs.glsl"))
    {
        location_uMVPMatrix    = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
        location_uMVMatrix     = glGetUniformLocation(program.getGLId(), "uMVMatrix");
        location_uNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
        location_uTexture      = glGetUniformLocation(program.getGLId(), "uTexture");
    }
};

int main(int argc, char* argv[])
{
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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP6", nullptr, nullptr);
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
    std::unique_ptr<Image> earthPtr = loadImage("C:/Users/emili/Documents/E5/workspace/OpenGL/GLImac-Template_09_12/GLImac-Template/TP7/textures/EarthMap.jpg");
    std::unique_ptr<Image> moonPtr  = loadImage("C:/Users/emili/Documents/E5/workspace/OpenGL/GLImac-Template_09_12/GLImac-Template/TP7/textures/MoonMap.jpg");
    std::unique_ptr<Image> cloudPtr = loadImage("C:/Users/emili/Documents/E5/workspace/OpenGL/GLImac-Template_09_12/GLImac-Template/TP7/textures/CloudMap.jpg");

    // Création d'une texture:
    GLuint texture;
    glGenTextures(1, &texture);

    // Création d'une texture:
    GLuint cloud;
    glGenTextures(1, &cloud);

    // Binding de textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture); // la texture earthTexture est bindée sur l'unité GL_TEXTURE0

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, earthPtr->getWidth(), earthPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, earthPtr->getPixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cloud); // la texture cloudTexture est bindée sur l'unité GL_TEXTURE1

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cloudPtr->getWidth(), cloudPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, cloudPtr->getPixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbinding de textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0); // débind sur l'unité GL_TEXTURE0
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0); // débind sur l'unité GL_TEXTURE1

    glEnable(GL_DEPTH_TEST);

    glm::mat4 ProjMatrix   = glm::perspective(glm::radians(70.f), window_width / (float)window_height, 0.1f, 100.f);
    glm::mat4 MVMatrix     = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0f));
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    // Appel shaders
    FilePath     applicationPath(argv[0]);
    EarthProgram earthProgram(applicationPath);
    MoonProgram  moonProgram(applicationPath);

    Sphere sphere(1, 32, 16);

    // Création d'un VBO:
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Binding d'un VBO sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);

    // Unbinding d'un VBO sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Création du VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Binding du VAO et VBO
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Activation des attributs de vertex
    const GLuint VERTEX_ATTR_POSITION  = 0;
    const GLuint VERTEX_ATTR_NORMAL    = 1;
    const GLuint VERTEX_ATTR_TEXCOORDS = 2;

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, texCoords));

    // Unbinding du VAO et VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::vector<glm::vec3> randomVectorArray;

    for (int i = 0; i < 32; i++) {
        randomVectorArray.push_back(glm::sphericalRand(1.0f));
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Binding du VAO
        glBindVertexArray(vao);

        // emi
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

        //glfwSetScrollCallback(window, scroll_callback);
        //glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);  


        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.moveFront(0.2f);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.moveFront(-0.2f);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.moveLeft(0.2f);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.moveLeft(-0.2f);

        earthProgram.program.use();

         glm::mat4 MVMatrix = camera.getViewMatrix();
        //glm::mat4 MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0f));
        MVMatrix           = glm::rotate(MVMatrix, (float)glfwGetTime(), glm::vec3(1, 0, 0));
        
        glUniformMatrix4fv(earthProgram.location_uMVMatrix, 1, GL_FALSE,
                           glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(earthProgram.location_uNormalMatrix, 1, GL_FALSE,
                           glm::value_ptr(NormalMatrix));
        glUniformMatrix4fv(earthProgram.location_uMVPMatrix, 1, GL_FALSE,
                           glm::value_ptr(ProjMatrix * MVMatrix));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, earthPtr->getWidth(), earthPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, earthPtr->getPixels());

        glUniform1i(earthProgram.location_uEarthTexture, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cloud);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cloudPtr->getWidth(), cloudPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, cloudPtr->getPixels());

        glUniform1i(earthProgram.location_uCloudTexture, 1);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

        // emi

        moonProgram.program.use();

        glUniform1i(moonProgram.location_uTexture, 0);

        // Binding d'une texture
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, moonPtr->getWidth(), moonPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, moonPtr->getPixels());

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        for (int i = 0; i < 32; i++) {
            MVMatrix = camera.getViewMatrix();
            //MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5 - i / 4));         // Translation
            MVMatrix = glm::rotate(MVMatrix, (float)glfwGetTime(), randomVectorArray[i]); // Translation * Rotation
            MVMatrix = glm::translate(MVMatrix, glm::vec3(-2, 0, 0));                     // Translation * Rotation * Translation
            MVMatrix = glm::scale(MVMatrix, glm::vec3(0.2, 0.2, 0.2));

            glUniformMatrix4fv(moonProgram.location_uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
            glUniformMatrix4fv(moonProgram.location_uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));

            glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        }

        // Unbinding de la texture
        glBindTexture(GL_TEXTURE_2D, 0);

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
    glDeleteTextures(1, &cloud);

    glfwTerminate();
    return 0;
}