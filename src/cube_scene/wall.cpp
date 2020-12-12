#include <auxiliary/wall.h>

//render parallax-mapped wall
Wall::Wall(glm::vec3 wallPosition, Shader shader, const char* diffuseMap, const char* normalMap, const char* heightMap)
        : position(wallPosition), parallaxShader(shader) {
    wallDiffuseMap = loadTexture(FileSystem::getPath(diffuseMap).c_str());
    wallNormalMap = loadTexture(FileSystem::getPath(normalMap).c_str());
    wallHeightMap = loadTexture(FileSystem::getPath(heightMap).c_str());
}

void Wall::configure(glm::mat4 projection, glm::mat4 view, glm::vec3 cameraPos, float heightScale, glm::vec3 lightPosition) {
    parallaxShader.use();
    parallaxShader.setMat4("projection", projection);
    parallaxShader.setMat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
//        float angle = glm::radians((float)glfwGetTime() * -5.0f);
//        model = glm::rotate(model, angle,
//                            glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    auto time = (float)glfwGetTime();
    float angle;
    float norm = 60.0f;
    if (angleShift){
        angle = glm::radians(time / 5.0f / norm);
    } else {
        angle = glm::radians(time * -5.0f / norm);
    }

    if (angle < glm::radians(-30.0f)) {
        angleShift = true;
        angle = glm::radians(time / 5.0f / norm);
    } else if (angle >= glm::radians(30.0f)) {
        angleShift = false;
        angle = glm::radians(time * -5.0f / norm);
    }
    model = glm::rotate(model, angle,glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    parallaxShader.setMat4("model", model);
    parallaxShader.setVec3("viewPos", cameraPos);
    parallaxShader.setVec3("lightPos", lightPosition);
    parallaxShader.setFloat("heightScale", heightScale); // configure with Q and E keys
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wallDiffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wallNormalMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, wallHeightMap);
}

// renders a 1x1 wall with  tangent vectors
void Wall::render(glm::mat4 projection, glm::mat4 view, glm::vec3 cameraPos, float heightScale, glm::vec3 lightPosition) {
    configure(projection, view, cameraPos, heightScale, lightPosition);
    if (wallVAO == 0) {
        // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // triangle 2
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);

        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);

        float wallVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &wallVAO);
        glGenBuffers(1, &wallVBO);
        glBindVertexArray(wallVAO);
        glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), &wallVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(wallVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}