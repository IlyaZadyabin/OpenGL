#include <auxiliary/platform.h>

 float Platform::platformVertices[] = {
        // positions            // normals         // texcoords
        25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

        25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
        25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
};

Platform::Platform(const char* texture, const char* specular) {
    platformTexture = loadTexture(FileSystem::getPath(texture).c_str());
    platformSpecularMap = loadTexture(FileSystem::getPath(specular).c_str());
}
void Platform::render() {
    if (platformVAO == 0) {
        unsigned int platformVBO;
        glGenVertexArrays(1, &platformVAO);
        glGenBuffers(1, &platformVBO);
        glBindVertexArray(platformVAO);
        glBindBuffer(GL_ARRAY_BUFFER, platformVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(platformVertices), platformVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
        glBindVertexArray(0);
    }
    glBindVertexArray(platformVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
unsigned Platform::getPlatformTexture() const {
    return platformTexture;
}
unsigned Platform::getPlatformSpecularMap() const {
    return platformSpecularMap;
}