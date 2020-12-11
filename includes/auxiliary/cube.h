#include <glad/glad.h>
#include <glm/glm.hpp>

class Cube {
private:
    unsigned int cubeVAO = 0;
public:
    Cube() = default;
    static float cubeVertices[];

    void render();
};