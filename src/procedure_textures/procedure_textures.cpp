#define _USE_MATH_DEFINES
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <auxiliary/camera.h>
#include <iostream>
#include <auxiliary/shader.h>
#include <auxiliary/filesystem.h>
#include <auxiliary/loadTexture.h>
#include <auxiliary/positions.h>
//#include <auxiliary/sphere.h>
#include <auxiliary/torus.h>
// constants
const unsigned SCR_WIDTH = 1900;
const unsigned SCR_HEIGHT = 1000;

Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f));

//callbacks
void processInput(GLFWwindow *window) {
    camera.processInput(window);
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    camera.mouse_callback(xpos, ypos);
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.scroll_callback(yoffset);
}

GLFWwindow* initAndConfigure() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Graphics", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwWindowHint(GLFW_CURSOR_DISABLED, GL_TRUE);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    return window;
}

int main() {
    GLFWwindow *window = initAndConfigure();
    if (!window)
        return -1;

    Shader specialLightShader("specialLight_vert.glsl", "specialLight_frag.glsl");

    specialLightShader.use();
    specialLightShader.setInt("albedoMap", 0);
    specialLightShader.setInt("normalMap", 1);
    specialLightShader.setInt("metallicMap", 2);
    specialLightShader.setInt("roughnessMap", 3);
    specialLightShader.setInt("aoMap", 4);

    // static shader uniforms
    glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    specialLightShader.use();
    specialLightShader.setMat4("projection", projection);

    Torus torus("textures/procedure_textures/metal/albedo.png",
                "textures/procedure_textures/metal/normal.png",
                "textures/procedure_textures/metal/metallic.png",
                "textures/procedure_textures/metal/roughness.png",
                "textures/procedure_textures/metal/ao.png");
    // render loop
    while (!glfwWindowShouldClose(window)) {
        camera.updateTimeFrames();
        camera.processInput(window);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        specialLightShader.use();
        glm::mat4 view = camera.getCameraView();
        specialLightShader.setMat4("view", view);
        specialLightShader.setVec3("camPos", camera.getCameraPos());

        glm::mat4 model = glm::mat4(1.0f);

        torus.bindTextures();

        model = glm::translate(model, glm::vec3(1.0,1.0,1.0));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
        specialLightShader.setMat4("model", model);
        torus.render(0.1, 0.25, 64, 32);

        model = glm::translate(model, glm::vec3(1.0,1.0,5.0));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0, 0.0, 0.0));
        specialLightShader.setMat4("model", model);
        torus.render(0.1, 0.25, 64, 32);

        model = glm::translate(model, glm::vec3(1.0,1.0,-5.0));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 1.0));
        specialLightShader.setMat4("model", model);
        torus.render(0.1, 0.25, 64, 32);

        // render light sources
        for (unsigned int i = 0; i < sizeof(specialLightPositions) / sizeof(specialLightPositions[0]); ++i) {
            glm::vec3 newPos = specialLightPositions[i] + glm::vec3(std::sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = specialLightPositions[i];
            specialLightShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            specialLightShader.setVec3("lightColors[" + std::to_string(i) + "]", specialLightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            specialLightShader.setMat4("model", model);
            //torus.render(0.1, 0.25, 64, 32);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
