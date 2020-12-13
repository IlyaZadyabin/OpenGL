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
#include <auxiliary/shader.h>
#include <auxiliary/camera.h>
#include <auxiliary/wall.h>
#include <auxiliary/cube.h>
#include <auxiliary/torus.h>

class Scene {
public:
    //shaders
    Shader lightingShader{"basic_vert.glsl", "lights_frag.glsl"};
    Shader lampShader{"basic_vert.glsl", "lamp_frag.glsl"};
    Shader shadowShader{"shadow_mapping_vert.glsl", "shadow_mapping_frag.glsl"};
    Shader shadowDepthShader{"shadow_mapping_depth_vert.glsl", "shadow_mapping_depth_frag.glsl", "shadow_mapping_depth_geom.glsl"};
    Shader parallaxShader{"parallax_mapping_vert.glsl", "parallax_mapping_frag.glsl"};
    Shader specialLightShader{"specialLight_vert.glsl", "specialLight_frag.glsl"};

    unsigned boxDiffuseMap  = loadTexture(FileSystem::getPath("textures/basecolor.jpg").c_str());
    unsigned boxSpecularMap = loadTexture(FileSystem::getPath("textures/metallic.jpg").c_str());
    unsigned boxEmissionMap = loadTexture(FileSystem::getPath("textures/acid.jpg").c_str());

    const unsigned SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned depthMapFBO{};
    unsigned depthCubemap{};

    Cube cube;
    Wall wall;
    Platform platform{"textures/whiteSquare.png", "textures/whiteSquare_specular.png"};
    Camera camera;
    Torus torus{"textures/procedure_textures/metal/albedo.png",
    "textures/procedure_textures/metal/normal.png",
    "textures/procedure_textures/metal/metallic.png",
    "textures/procedure_textures/metal/roughness.png",
    "textures/procedure_textures/metal/ao.png"};
    void configureTorus() const;


public:
    explicit Scene(Camera cam);

    void renderWithShadows(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void renderWithLights(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float heightScale);

    void renderScene(const Shader &shader, unsigned flDiffuse, unsigned flSpecular,
                     unsigned cDiffuse, unsigned cSpecular, unsigned cEmission);
    void renderCubes(const Shader &shader);

    void renderWithTorus(Camera camera);

    static  glm::vec3 cubePositions[];

    static  glm::vec3 pointLightPositions[];
    static  glm::vec3 pointLightColors[];
    static  glm::vec3 specialLightPositions[4];
    static  glm::vec3 specialLightColors[];
};