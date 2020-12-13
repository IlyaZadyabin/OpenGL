#define _USE_MATH_DEFINES
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <auxiliary/platform.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <auxiliary/positions.h>
#include <auxiliary/shader.h>
#include <auxiliary/camera.h>
#include <auxiliary/wall.h>
#include <auxiliary/cube.h>
#include <auxiliary/scene.h>
#include <auxiliary/skybox.h>
#include <auxiliary/torus.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

//constants
const unsigned SCR_WIDTH = 1900;
const unsigned SCR_HEIGHT = 1000;
float heightScale = 0.05;
bool shadows = true;
bool shadowsKeyPressed = false; //press F to switch scenes

Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(10.0f, 0.0f, 5.0f));

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

    std::vector<std::string> faces {
            FileSystem::getPath("textures/skybox/px.jpg"),
            FileSystem::getPath("textures/skybox/nx.jpg"),
            FileSystem::getPath("textures/skybox/py.jpg"),
            FileSystem::getPath("textures/skybox/ny.jpg"),
            FileSystem::getPath("textures/skybox/pz.jpg"),
            FileSystem::getPath("textures/skybox/nz.jpg"),
    };

    Skybox skybox ("skybox_vert.glsl", "skybox_frag.glsl", faces);

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

    Scene scene{camera};
    // rendering loop
    while (!glfwWindowShouldClose(window)) {
        camera.updateTimeFrames();
        processInput(window);

        glClearColor(0.2f, 0.6f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //uniforms
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getCameraView();
        glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        if (shadows) {
           scene.renderWithShadows(model, view, projection);
        } else {
           scene.renderWithLights(model, view, projection, heightScale);

			specialLightShader.use();
			glm::mat4 view = camera.getCameraView();
			specialLightShader.setMat4("view", view);
			specialLightShader.setVec3("camPos", camera.getCameraPos());

			glm::mat4 model = glm::mat4(1.0f);

			torus.bindTextures();

			model = glm::translate(model, glm::vec3(-10.0, 5.0, -10.0));
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
				torus.render(0.1, 0.25, 64, 32);
			}
        }
        skybox.render(glm::mat4(glm::mat3(camera.getCameraView())), projection);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    camera.processInput(window);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !shadowsKeyPressed) {
        shadows = !shadows;
        shadowsKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
        shadowsKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (heightScale > 0.0f) {
            heightScale -= 0.0005f;
        } else {
            heightScale = 0.0f;
        }
    } else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (heightScale < 1.0f) {
            heightScale += 0.0005f;
        } else {
            heightScale = 1.0f;
        }
    }
}

//callbacks
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    camera.mouse_callback(xpos, ypos);
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.scroll_callback(yoffset);
}