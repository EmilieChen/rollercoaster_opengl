#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/FreelyCamera.hpp>
#include <glimac/model.h>
#include <glimac/shadermanager.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/common.hpp>

using namespace glimac;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


// camera
FreelyCamera camera;

unsigned int window_width  = 1280;
unsigned int window_height = 720;
bool  firstMouse    = true;
float lastX         = 0;
float lastY         = 0;


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

int main(int argc, char* argv[])
{
    int arg = argc;
    arg++;
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
    // glfw window creation
    // --------------------
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
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    // -----------------------------------------------------------------------------------------

    FilePath               applicationPath(argv[0]);


    std::unique_ptr<Image> groundPtr = loadImage(applicationPath.dirPath() + "assets/textures/brick_ground.jpeg");
    std::unique_ptr<Image> discoPtr  = loadImage(applicationPath.dirPath() + "assets/textures/boule_disco.jpg");

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
        ShapeVertex(glm::vec3(0.5, 0.0, -0.5), glm::vec3(0, 1, 0), glm::vec2(3, 3)),  // Sommet 1
        ShapeVertex(glm::vec3(0.5, 0.0, 0.5), glm::vec3(0, 1, 0), glm::vec2(3, 0)),   // Sommet 2
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

    // Activation des attributs de vertex

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (GLvoid*)offsetof(ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (GLvoid*)offsetof(ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (GLvoid*)offsetof(ShapeVertex, texCoords));

    // Unbinding du VAO et VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);




    // build and compile shaders
    // -------------------------

    FilePath vertexShader = applicationPath.dirPath() + "TP9/shaders/3D.vs.glsl";
    FilePath fragmentShader = applicationPath.dirPath() + "TP9/shaders/multipleLight.fs.glsl";

    ShaderManager ourCommonShader(vertexShader.c_str(), fragmentShader.c_str());

    // load models
    // -----------

    FilePath rollerCoasterOBJ = applicationPath.dirPath() + "assets/models/roller_coaster/rollerCoaster.obj";
    FilePath wagonOBJ         = applicationPath.dirPath() + "assets/models/wagon/wagon.obj";
    FilePath treeOBJ          = applicationPath.dirPath() + "assets/models/low_poly_tree/low_poly_tree.obj";

    Model rollerCoaster(rollerCoasterOBJ);
    Model wagon(wagonOBJ);
    Model tree(treeOBJ);


    // render loop
    // -----------

    std::vector<glm::vec3> randomVectorArray;
    glm::vec3              randomVector;

    for (int i = 0; i < 30; i++) {
        randomVector = glm::sphericalRand(1.0f);
        randomVectorArray.push_back(glm::vec3(randomVector.x, 1, randomVector.z));
    }

    float a = 0.0f; // variable pour le sens de la translation du wagon
    int b = 0;

    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.5f, 1.f, 0.5f, 1.f);
        
        ourCommonShader.use();

        glm::mat4 ViewMatrix = camera.getViewMatrix();

        // Lumière

        // pour une lumiere directionnelle
        glm::vec3 lumiere_dir = glm::vec4(0.0, 1.0, 1.0, 1.0);

        // pour les deux lumières tournantes

        glm::mat4 lumiere_rot1 = glm::rotate(glm::mat4(1), 3 * (float)glfwGetTime(), glm::vec3(0, 1, 0));
        glm::vec3 lumiere_pos1 = glm::vec4(0.0, 0.0, 1.0, 0.0) * lumiere_rot1 * ViewMatrix;

        glm::mat4 lumiere_rot2 = glm::rotate(glm::mat4(1), 3.0f + 3 * (float)glfwGetTime(), glm::vec3(0, 1, 0));
        glm::vec3 lumiere_pos2 = glm::vec4(0.0, 0.0, 1.0, 0.0) * lumiere_rot2 * ViewMatrix;

        // dirlight properties
        glm::vec3 dirLightColor        = glm::vec3(2.0f, 2.0f, 2.0f);
        glm::vec3 dirLightDiffuseColor = dirLightColor * glm::vec3(0.5f); // decrease the influence
        glm::vec3 dirLightAmbientColor = dirLightColor * glm::vec3(0.1f); // low influence


        ourCommonShader.setVec3("uDirLight.ambient", dirLightAmbientColor.x, dirLightAmbientColor.y, dirLightAmbientColor.z);
        ourCommonShader.setVec3("uDirLight.diffuse", dirLightDiffuseColor.x, dirLightDiffuseColor.y, dirLightDiffuseColor.z);
        ourCommonShader.setVec3("uDirLight.dir_pos", lumiere_dir.x, lumiere_dir.y, lumiere_dir.z);


        // pointlight1 properties
        glm::vec3 pointLightColor1        = glm::vec3(3.0f, 0.0f, 3.0f);
        glm::vec3 pointLightDiffuseColor1 = pointLightColor1 * glm::vec3(0.9f); // decrease the influence
        glm::vec3 pointLightAmbientColor1 = pointLightColor1 * glm::vec3(0.1f); // low influence

        ourCommonShader.setVec3("uPointLights[0].ambient", pointLightAmbientColor1.x, pointLightAmbientColor1.y, pointLightAmbientColor1.z);
        ourCommonShader.setVec3("uPointLights[0].diffuse", pointLightDiffuseColor1.x, pointLightDiffuseColor1.y, pointLightDiffuseColor1.z);
        ourCommonShader.setVec3("uPointLights[0].dir_pos", lumiere_pos1.x, lumiere_pos1.y, lumiere_pos1.z);

        // pointlight2 properties
        glm::vec3 pointLightColor2        = glm::vec3(3.0f, 3.0f, 0.0f);
        glm::vec3 pointLightDiffuseColor2 = pointLightColor2 * glm::vec3(0.9f); // decrease the influence
        glm::vec3 pointLightAmbientColor2 = pointLightColor2 * glm::vec3(0.1f); // low influence

        ourCommonShader.setVec3("uPointLights[1].ambient", pointLightAmbientColor2.x, pointLightAmbientColor2.y, pointLightAmbientColor2.z);
        ourCommonShader.setVec3("uPointLights[1].diffuse", pointLightDiffuseColor2.x, pointLightDiffuseColor2.y, pointLightDiffuseColor2.z);
        ourCommonShader.setVec3("uPointLights[1].dir_pos", lumiere_pos2.x, lumiere_pos2.y, lumiere_pos2.z);



         //Quad

        // Binding du VAO
        glBindVertexArray(vao_quad);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ground);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, groundPtr->getWidth(), groundPtr->getHeight(), 0, GL_RGBA, GL_FLOAT, groundPtr->getPixels());

        ourCommonShader.setInt("texture_diffuse1", 0);

        glm::mat4 MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-18.0f, -10.0f, -5.0f));
        MVMatrix           = glm::scale(MVMatrix, glm::vec3(100, 100, 100));

        MVMatrix = ViewMatrix * MVMatrix;

        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        ourCommonShader.setMat4("uMVMatrix", MVMatrix);
        ourCommonShader.setMat4("uNormalMatrix", NormalMatrix);
        ourCommonShader.setMat4("uMVPMatrix", ProjMatrix * MVMatrix);


        // material properties

        ourCommonShader.setVec3("Kspecular", 0.2f, 0.2f, 0.2f);
        ourCommonShader.setFloat("shininess", 0.1f);


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

        ourCommonShader.setInt("texture_diffuse1", 1);

        MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 10, -5.0f));
        MVMatrix = glm::scale(MVMatrix, glm::vec3(4, 4, 4));
        MVMatrix = glm::rotate(MVMatrix, (float)glfwGetTime(), glm::vec3(0, 1, 0));

        MVMatrix = ViewMatrix * MVMatrix;

        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        ourCommonShader.setMat4("uMVMatrix", MVMatrix);
        ourCommonShader.setMat4("uNormalMatrix", NormalMatrix);
        ourCommonShader.setMat4("uMVPMatrix", ProjMatrix * MVMatrix);

        // material properties

        ourCommonShader.setVec3("Kspecular", 0.1f, 0.1f, 0.1f);
        ourCommonShader.setFloat("shininess", 0.2f);

        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());


        for (int i = 0; i < 30; i++) {
            // pour une lumiere ponctuelle
            glm::mat4 ViewMatrix = camera.getViewMatrix();
            MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 10, -5));                            // Translation
            MVMatrix = glm::rotate(MVMatrix, 0.5f * i + (float)glfwGetTime(), randomVectorArray[i]); // Translation * Rotation
            MVMatrix = glm::translate(MVMatrix, glm::vec3(-12 - i / 6, 0, 0));                        // Translation * Rotation * Translation
            MVMatrix = glm::scale(MVMatrix, glm::vec3(1, 1, 1));

            MVMatrix               = ViewMatrix * MVMatrix;
            glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

            ourCommonShader.setMat4("uMVMatrix", MVMatrix);
            ourCommonShader.setMat4("uNormalMatrix", NormalMatrix);
            ourCommonShader.setMat4("uMVPMatrix", ProjMatrix * MVMatrix);

            // material properties


            ourCommonShader.setVec3("Kspecular", 0.03f, 0.03f, 0.03f);
            ourCommonShader.setFloat("shininess", 0.05f);

            glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        }

        // Unbinding du VAO
        glBindVertexArray(0);

        

        MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0.0f, -9.8f, -25.0f));                 // Translation
        MVMatrix = glm::scale(MVMatrix, glm::vec3(1, 1, 1));

        MVMatrix               = ViewMatrix * MVMatrix;

        ourCommonShader.setMat4("uMVMatrix", MVMatrix);
        ourCommonShader.setMat4("uNormalMatrix", NormalMatrix);
        ourCommonShader.setMat4("uMVPMatrix", ProjMatrix * MVMatrix);

        
        ourCommonShader.setVec3("Kspecular", 0.2f, 0.2f, 0.2f);
        ourCommonShader.setFloat("shininess", 0.1f);

        rollerCoaster.Draw(ourCommonShader);
        
        if (a > 1.0f) {
            a = 0.0f;
            b = (b == 1) ? 0 : 1;
        }

        a = a + 0.01f;
        
        if (b == 0 ) {
            MVMatrix = glm::translate(glm::mat4(1), glm::vec3(-5.0f + 15.0f*a, -8.0f, -25.0f)); // Translation
        }
        else {
            MVMatrix = glm::translate(glm::mat4(1), glm::vec3(10.0f - 15.0f * a, -8.0f, -25.0f)); // Translation
        }
        MVMatrix = glm::scale(MVMatrix, glm::vec3(1, 1, 1));

        //std::cout << "x " << camera.getCameraPosition().x << std::endl;
        //std::cout << "y " << camera.getCameraPosition().y << std::endl;
        //std::cout << "z " << camera.getCameraPosition().z << std::endl;

        MVMatrix = ViewMatrix * MVMatrix;

        ourCommonShader.setMat4("uMVMatrix", MVMatrix);
        ourCommonShader.setMat4("uNormalMatrix", NormalMatrix);
        ourCommonShader.setMat4("uMVPMatrix", ProjMatrix * MVMatrix);

        ourCommonShader.setVec3("Kspecular", 0.2f, 0.2f, 0.2f);
        ourCommonShader.setFloat("shininess", 0.1f);
        
        wagon.Draw(ourCommonShader);

        MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0.0f, -9.8f, -25.0f)); // Translation
        MVMatrix = glm::scale(MVMatrix, glm::vec3(1, 1, 1));

        MVMatrix = ViewMatrix * MVMatrix;

        ourCommonShader.setMat4("uMVMatrix", MVMatrix);
        ourCommonShader.setMat4("uNormalMatrix", NormalMatrix);
        ourCommonShader.setMat4("uMVPMatrix", ProjMatrix * MVMatrix);

        ourCommonShader.setVec3("Kspecular", 0.2f, 0.2f, 0.2f);
        ourCommonShader.setFloat("shininess", 0.1f);

        rollerCoaster.Draw(ourCommonShader);


        for (int i = 0; i < 2; i++) {
            MVMatrix = glm::translate(glm::mat4(1), glm::vec3(-30.0f + 40.0f*i, -10.0f, 35.0f)); // Translation
            MVMatrix = glm::scale(MVMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

            MVMatrix = ViewMatrix * MVMatrix;

            ourCommonShader.setMat4("uMVMatrix", MVMatrix);
            ourCommonShader.setMat4("uNormalMatrix", NormalMatrix);
            ourCommonShader.setMat4("uMVPMatrix", ProjMatrix * MVMatrix);

            ourCommonShader.setVec3("Kspecular", 0.2f, 0.2f, 0.2f);
            ourCommonShader.setFloat("shininess", 0.1f);

            tree.Draw(ourCommonShader);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Libération des ressources
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo_quad);
    glDeleteVertexArrays(1, &vao_quad);
    glDeleteTextures(1, &ground);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveFront(0.2f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveFront(-0.2f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveLeft(0.2f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveLeft(-0.2f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
