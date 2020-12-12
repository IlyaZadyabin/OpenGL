#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Camera {
public:
    Camera(unsigned int WIDTH, unsigned int HEIGHT, glm::vec3 position);

    void processInput(GLFWwindow *window);

    void scroll_callback(double yoffset);

    void mouse_callback(double xpos, double ypos);

    glm::mat4 getCameraView() const;
    float getScreenWidth() const;
    float getScreenHeight() const;

    void updateTimeFrames();

    float getFov() const;
    glm::vec3 getCameraPos();

private:
    const unsigned int SCR_WIDTH, SCR_HEIGHT;
    bool firstMouse = true;
    float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    float lastX = SCR_WIDTH / 2.0;
    float lastY = SCR_HEIGHT / 2.0;
    float fov = 45.0f;

    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
};

#endif