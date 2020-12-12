#ifndef SKYBOX_H
#define SKYBOX_H

#include <auxiliary/shader.h>
#include <vector>
#include <stb_image.h>
class Skybox {
private:
    Shader skyboxShader;
    unsigned int skyboxVAO = 0;
    unsigned int cubemapTexture;
    unsigned memoryForVertices;

public:
    Skybox(const char* vert_path, const char* frag_path, std::vector<std::string>& faces);

    void render(glm::mat4 view, glm::mat4 projection);
    static unsigned int loadCubemap(std::vector<std::string> faces);
    static float vertices[];
};
#endif