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

int   window_width  = 1280;
int   window_height = 720;
bool  firstMouse    = true;
float lastX         = 0;
float lastY         = 0;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// SDLWindowManager windowManager(window_width, window_height, "GLImac");
FreelyCamera camera;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        camera.rotateUp(10.f);
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        camera.rotateUp(-10.f);
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        camera.rotateLeft(10.f);
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
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
        lastX      = (float)xpos;
        lastY      = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)(xpos - lastX);

    float yoffset = (float)(lastY - ypos);
    lastX         = (float)xpos;
    lastY         = (float)ypos;

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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP9", nullptr, nullptr);
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
    std::unique_ptr<Image> groundPtr = loadImage("C:/Users/emili/Documents/E5/workspace/OpenGL/GLImac-Template_09_12/GLImac-Template/assets/textures/brick_ground.jpeg");
    std::unique_ptr<Image> discoPtr = loadImage("C:/Users/emili/Documents/E5/workspace/OpenGL/GLImac-Template_09_12/GLImac-Template/assets/textures/boule_disco.jpg");

    // Création d'une texture:
    GLuint ground;
    glGenTextures(1, &ground);

    // Création d'une texture:
    GLuint disco;
    glGenTextures(1, &disco);

    // Binding de textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ground); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, groundPtr->getWidth(), groundPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, groundPtr->getPixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, disco); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, discoPtr->getWidth(), discoPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, discoPtr->getPixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbinding de textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0); 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0); 
    
    glEnable(GL_DEPTH_TEST);

    // Appel shaders
    FilePath applicationPath(argv[0]);
    Program  program = loadProgram(applicationPath.dirPath() + "TP8/shaders/3D.vs.glsl", applicationPath.dirPath() + "TP8/shaders/multipleLight.fs.glsl");
    program.use();

    const GLuint location_uMVMatrix     = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    const GLuint location_uMVPMatrix    = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    const GLuint location_uNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");

    const GLuint location_uMaterialKspecular  = glGetUniformLocation(program.getGLId(), "uMaterial.Kspecular");
    const GLuint location_uMaterialShininess = glGetUniformLocation(program.getGLId(), "uMaterial.shininess");
    const GLuint location_uMaterialKdiffuse   = glGetUniformLocation(program.getGLId(), "uMaterial.Kdiffuse");

    const GLuint location_uViewPosition = glGetUniformLocation(program.getGLId(), "uViewPosition");

    const GLuint location_uDirLightAmbient           = glGetUniformLocation(program.getGLId(), "uDirLight.ambient");
    const GLuint location_uDirLightDirection         = glGetUniformLocation(program.getGLId(), "uDirLight.dir_pos");
    const GLuint location_uDirLightDiffuse           = glGetUniformLocation(program.getGLId(), "uDirLight.diffuse");
    
    const GLuint location_uPointLightAmbient1   = glGetUniformLocation(program.getGLId(), "uPointLights[0].ambient");
    const GLuint location_uPointLightPosition1 = glGetUniformLocation(program.getGLId(), "uPointLights[0].dir_pos");
    const GLuint location_uPointLightDiffuse1    = glGetUniformLocation(program.getGLId(), "uPointLights[0].diffuse");

    const GLuint location_uPointLightAmbient2  = glGetUniformLocation(program.getGLId(), "uPointLights[1].ambient");
    const GLuint location_uPointLightPosition2 = glGetUniformLocation(program.getGLId(), "uPointLights[1].dir_pos");
    const GLuint location_uPointLightDiffuse2  = glGetUniformLocation(program.getGLId(), "uPointLights[1].diffuse");


    /*const GLuint location_uKd           = glGetUniformLocation(program.getGLId(), "uKd");
    const GLuint location_uKs           = glGetUniformLocation(program.getGLId(), "uKs");
    const GLuint location_uShininess    = glGetUniformLocation(program.getGLId(), "uShininess");
    const GLuint location_uLightPos_vs  = glGetUniformLocation(program.getGLId(), "uLightPos_vs");
    const GLuint location_uLightIntensity = glGetUniformLocation(program.getGLId(), "uLightIntensity");*/

    glEnable(GL_DEPTH_TEST);

    glm::mat4 ProjMatrix   = glm::perspective(glm::radians(70.f), window_width / (float)window_height, 0.1f, 100.f);
    glm::mat4 MVMatrix     = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0f));
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

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



    // Quad

    // Création d'un VBO:
    GLuint vbo_quad;
    glGenBuffers(1, &vbo_quad);

    // Binding d'un VBO sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, vbo_quad);

    // Remplir le VBO en utilisant une structure de vertex et ibo
    ShapeVertex vertices[] = {
        ShapeVertex(glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0, 1, 0), glm::vec2(0, 3)), // Sommet 0
        ShapeVertex(glm::vec3(0.5, 0.0, -0.5), glm::vec3(0, 1, 0), glm::vec2(3, 3)), // Sommet 1
        ShapeVertex(glm::vec3(0.5, 0.0, 0.5), glm::vec3(0, 1, 0), glm::vec2(3, 0)),  // Sommet 2
        ShapeVertex(glm::vec3(-0.5, 0.0, 0.5), glm::vec3(0, 1, 0), glm::vec2(0, 0))   // Sommet 3
    };

    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(ShapeVertex), vertices, GL_STATIC_DRAW);

    // Unbinding d'un VBO sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // => Creation du IBO
    GLuint ibo_quad;
    glGenBuffers(1, &ibo_quad);

    // => On bind sur GL_ELEMENT_ARRAY_BUFFER, cible reservée pour les IBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_quad);

    // => Tableau d'indices: ce sont les indices des sommets à dessiner
    uint32_t indices[] = {
        0, 1, 2, 0, 2, 3};

    // => On remplit l'IBO avec les indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    // Unbinding du IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Création d'un VAO
    GLuint vao_quad;
    glGenVertexArrays(1, &vao_quad);

    // Binding du VAO et VBO
    glBindVertexArray(vao_quad);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_quad);

    // => On bind l'IBO sur GL_ELEMENT_ARRAY_BUFFER; puisqu'un VAO est actuellement bindé,
    // cela a pour effet d'enregistrer l'IBO dans le VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_quad);

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (GLvoid*)offsetof(ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (GLvoid*)offsetof(ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (GLvoid*)offsetof(ShapeVertex, texCoords));

    // Unbinding du VAO et VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    std::vector<glm::vec3> randomVectorArray;
    glm::vec3 randomVector;

    for (int i = 0; i < 10; i++) {
        randomVector = glm::sphericalRand(1.0f);
        randomVectorArray.push_back(glm::vec3(randomVector.x, 1, randomVector.z));
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.5f, 1.f, 0.5f, 1.f);

        // emi
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // glfwSetScrollCallback(window, scroll_callback);
        // glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.moveFront(0.2f);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.moveFront(-0.2f);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.moveLeft(0.2f);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.moveLeft(-0.2f);


        glm::mat4 ViewMatrix = camera.getViewMatrix();

        // Lumière

        // pour une lumiere directionnelle
        glm::vec3 lumiere_dir = glm::vec4(0.0, 1.0, 1.0, 1.0);
        //*ViewMatrix;

        // pour une lumiere tournante
        // glm::mat4 ViewMatrix  = camera.getViewMatrix();

        glm::mat4 lumiere_rot1 = glm::rotate(glm::mat4(1), 3*(float)glfwGetTime(), glm::vec3(0, 1, 0));
        glm::vec3 lumiere_pos1 = glm::vec4(0.0, 0.0, 1.0, 0.0) * lumiere_rot1 * ViewMatrix;

        glm::mat4 lumiere_rot2 = glm::rotate(glm::mat4(1), 3.0f + 3 * (float)glfwGetTime(), glm::vec3(0, 1, 0));
        glm::vec3 lumiere_pos2 = glm::vec4(0.0, 0.0, 1.0, 0.0) * lumiere_rot2 * ViewMatrix;

        // pour une lumiere ponctuelle
        // glm::mat4 ViewMatrix = camera.getViewMatrix();
        // glm::mat4 lumiere_rot = glm::rotate(glm::mat4(1), (float)glfwGetTime(), glm::vec3(0, 0, 1));
        // glm::vec3 lumiere_pos1 = ViewMatrix * glm::vec4(0.0, 5.0, -5.0, 0.0);

        // dirlight properties
        glm::vec3 dirLightColor        = glm::vec3(2.0f, 2.0f, 2.0f);
        glm::vec3 dirLightDiffuseColor = dirLightColor * glm::vec3(0.3f); // decrease the influence
        glm::vec3 dirLightAmbientColor = dirLightColor * glm::vec3(0.1f); // low influence

        glUniform3f(location_uDirLightAmbient, dirLightAmbientColor.x, dirLightAmbientColor.y, dirLightAmbientColor.z);
        glUniform3f(location_uDirLightDiffuse, dirLightDiffuseColor.x, dirLightDiffuseColor.y, dirLightDiffuseColor.z);
        glUniform3f(location_uDirLightDirection, lumiere_dir.x, lumiere_dir.y, lumiere_dir.z);

        // pointlight1 properties
        glm::vec3 pointLightColor1        = glm::vec3(2.0f, 0.0f, 2.0f);
        glm::vec3 pointLightDiffuseColor1 = pointLightColor1 * glm::vec3(0.9f); // decrease the influence
        glm::vec3 pointLightAmbientColor1 = pointLightColor1 * glm::vec3(0.1f); // low influence

        glUniform3f(location_uPointLightAmbient1, pointLightAmbientColor1.x, pointLightAmbientColor1.y, pointLightAmbientColor1.z);
        glUniform3f(location_uPointLightDiffuse1, pointLightDiffuseColor1.x, pointLightDiffuseColor1.y, pointLightDiffuseColor1.z);
        glUniform3f(location_uPointLightPosition1, lumiere_pos1.x, lumiere_pos1.y, lumiere_pos1.z);

        // pointlight2 properties
        glm::vec3 pointLightColor2        = glm::vec3(2.0f, 2.0f, 0.0f);
        glm::vec3 pointLightDiffuseColor2 = pointLightColor2 * glm::vec3(0.9f); // decrease the influence
        glm::vec3 pointLightAmbientColor2 = pointLightColor2 * glm::vec3(0.1f); // low influence

        glUniform3f(location_uPointLightAmbient2, pointLightAmbientColor2.x, pointLightAmbientColor2.y, pointLightAmbientColor2.z);
        glUniform3f(location_uPointLightDiffuse2, pointLightDiffuseColor2.x, pointLightDiffuseColor2.y, pointLightDiffuseColor2.z);

        glUniform3f(location_uPointLightPosition2, lumiere_pos2.x, lumiere_pos2.y, lumiere_pos2.z);


        // Quad
        
        // Binding du VAO
        glBindVertexArray(vao_quad);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ground);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, groundPtr->getWidth(), groundPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, groundPtr->getPixels());

        glUniform1i(location_uMaterialKdiffuse, 0);

        glm::mat4 MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -5.0f, -5.0f));
        MVMatrix           = glm::scale(MVMatrix, glm::vec3(20, 20, 20));

        MVMatrix = ViewMatrix * MVMatrix;

        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(location_uMVMatrix, 1, GL_FALSE,
                           glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(location_uNormalMatrix, 1, GL_FALSE,
                           glm::value_ptr(NormalMatrix));
        glUniformMatrix4fv(location_uMVPMatrix, 1, GL_FALSE,
                           glm::value_ptr(ProjMatrix * MVMatrix));

        // material properties

        glUniform3f(location_uMaterialKspecular, 0.2f, 0.2f, 0.2f);
        glUniform1f(location_uMaterialShininess, 0.1f);


        // => On utilise glDrawElements à la place de glDrawArrays
        // Cela indique à OpenGL qu'il doit utiliser l'IBO enregistré dans le VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        
        // Unbinding du VAO
        glBindVertexArray(0);

        // Unbinding d'une texture
        glBindTexture(GL_TEXTURE_2D, 0);


        // Disco

        // Binding du VAO
        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, disco);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, discoPtr->getWidth(), discoPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, discoPtr->getPixels());

        glUniform1i(location_uMaterialKdiffuse, 1);

        

        MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0f));
        MVMatrix = glm::scale(MVMatrix, glm::vec3(2, 2, 2));
        MVMatrix = glm::rotate(MVMatrix, (float)glfwGetTime(), glm::vec3(0, 1, 0));

        MVMatrix = ViewMatrix * MVMatrix;

        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(location_uMVMatrix, 1, GL_FALSE,
                           glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(location_uNormalMatrix, 1, GL_FALSE,
                           glm::value_ptr(NormalMatrix));
        glUniformMatrix4fv(location_uMVPMatrix, 1, GL_FALSE,
                           glm::value_ptr(ProjMatrix * MVMatrix));


        // material properties

        
        glUniform3f(location_uMaterialKspecular, 0.1f, 0.1f, 0.1f);
        glUniform1f(location_uMaterialShininess, 0.2f);

       /* glUniform3f(location_uKd, 1.0, 0.0, 1.0);
        glUniform3f(location_uKs, 0.0, 1.0, 1.0);
        glUniform3f(location_uLightPos_vs, lumiere_pos.x, lumiere_pos.y, lumiere_pos.z);
        glUniform3f(location_uLightIntensity, 4.0, 4.0, 4.0);
        glUniform1f(location_uShininess, 1.0);*/
        

        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

        // Mini discos
        // emi

        for (int i = 0; i < 10; i++) {
            // pour une lumiere ponctuelle
            glm::mat4 ViewMatrix  = camera.getViewMatrix();
            //glm::vec3 lumiere_pos = glm::vec4(1.0, 1.0, 1.0, 0.0) * ViewMatrix;
            //glm::mat4 MVMatrix    = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0f));
            MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));                    // Translation
            MVMatrix = glm::rotate(MVMatrix, 0.5f * i + (float)glfwGetTime(), randomVectorArray[i]); // Translation * Rotation
            MVMatrix = glm::translate(MVMatrix, glm::vec3(-6 - i / 8, 0, 0));                              // Translation * Rotation * Translation
            MVMatrix = glm::scale(MVMatrix, glm::vec3(0.5, 0.5, 0.5));

            MVMatrix               = ViewMatrix * MVMatrix;
            glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

            glUniformMatrix4fv(location_uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
            glUniformMatrix4fv(location_uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
            glUniformMatrix4fv(location_uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));

            // material properties

            //glUniform3f(location_uMaterialKdiffuse, 10.0f, 0.0f, 0.0f);

            glUniform3f(location_uMaterialKspecular, 0.03f, 0.03f, 0.03f);
            glUniform1f(location_uMaterialShininess, 0.05f);

            //glUniform3f(location_uKd, (float)i / 10.0f, 0.0, (float)i / 10.0f);
            //glUniform3f(location_uKs, 0.0, (float)i / 10.0f, (float)i / 10.0f);
            //glUniform3f(location_uLightPos_vs, lumiere_pos.x, lumiere_pos.y, lumiere_pos.z);

            //glUniform3f(location_uLightIntensity, (float)i / 10.0f, (float)i / 10.0f, (float)i / 10.0f);
            ////glUniform3f(location_uLightIntensity, 0.5f + 0.1f * i, 0.5f + 0.1f * i, 0.5f + 0.1f * i);
            //glUniform1f(location_uShininess, 1.0);

            glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        }

        // Unbinding du VAO
        glBindVertexArray(0);
        glUniform1i(location_uMaterialKdiffuse, 1);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    // Libération des ressources
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &ground);

    glfwTerminate();
    return 0;
}