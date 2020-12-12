#define _USE_MATH_DEFINES
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <auxiliary/filesystem.h>
#include <glm/glm.hpp>
#include <auxiliary/loadTexture.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Torus {
private:
    unsigned torusVAO = 0;
    unsigned torusIndexCount = 0;

    unsigned torusAlbedo;
    unsigned torusNormal;
    unsigned torusMetallic;
    unsigned torusRoughness;
    unsigned torusAo;
public:
    Torus(const char* albedo, const char* normal, const char* metallic, const char* roughness, const char* ao);
    void render(double r, double c, int rSeg, int cSeg);
    void bindTextures() const;
};
