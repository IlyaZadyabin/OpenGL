#include <auxiliary/global.h>

//constants
const unsigned SCR_WIDTH = 1920;
const unsigned SCR_HEIGHT = 1000;
float heightScale = 0.1;
bool shadows = true;
bool shadowsKeyPressed = false; //press F to enable/disable shadows

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
            FileSystem::getPath("textures/a/px.jpg"),
            FileSystem::getPath("textures/a/nx.jpg"),
            FileSystem::getPath("textures/a/py.jpg"),
            FileSystem::getPath("textures/a/ny.jpg"),
            FileSystem::getPath("textures/a/pz.jpg"),
            FileSystem::getPath("textures/a/nz.jpg"),

//            FileSystem::getPath("textures/px.jpg"),
//            FileSystem::getPath("textures/nx.png"),
//            FileSystem::getPath("textures/py.png"),
//            FileSystem::getPath("textures/ny.png"),
//            FileSystem::getPath("textures/pz.png"),
//            FileSystem::getPath("textures/nz.png")
    };

    Skybox skybox ("skybox_vert.glsl", "skybox_frag.glsl", faces);
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
        }
        skybox.renderSkybox(glm::mat4(glm::mat3(camera.getCameraView())), projection);
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
        if (heightScale > 0.0f)
            heightScale -= 0.0005f;
        else
            heightScale = 0.0f;
    } else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (heightScale < 1.0f)
            heightScale += 0.0005f;
        else
            heightScale = 1.0f;
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