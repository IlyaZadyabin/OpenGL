#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <auxiliary/shader.h>
#include <auxiliary/filesystem.h>
#include <auxiliary/loadTexture.h>
class Wall {
private:
    glm::vec3 position = glm::vec3( 15.0f,  1.5f, 0.0f);
    unsigned int wallVAO = 0;
    unsigned int wallVBO{};
    Shader parallaxShader{};

    unsigned int wallDiffuseMap{};
    unsigned int wallNormalMap{};
    unsigned int wallHeightMap{};
    float rotationAngle{};
    bool angleShift = false;
    glm::vec3 lightPosition{};
public:
    Wall() = default;

    //render parallax-mapped wall
    Wall(glm::vec3 wallPosition, Shader shader, const char* diffuseMap, const char* normalMap, const char* heightMap);

    void configure(glm::mat4 projection, glm::mat4 view, glm::vec3 cameraPos, float heightScale, glm::vec3 lightPosition);

    // renders a 1x1 wall with  tangent vectors
    void render(glm::mat4 projection, glm::mat4 view, glm::vec3 cameraPos, float heightScale, glm::vec3 lightPosition);

};