#ifndef PLATFORM_H
#define PLATFORM_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <auxiliary/filesystem.h>
#include <auxiliary/loadTexture.h>
class Platform {
private:
    unsigned platformVAO = 0;
    unsigned platformTexture, platformSpecularMap;
public:
    Platform(const char* texture, const char* specular);
    void render();
    unsigned getPlatformTexture() const;
    unsigned getPlatformSpecularMap() const;
    static float platformVertices[];
};

#endif