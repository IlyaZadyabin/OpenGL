#include <auxiliary/torus.h>

Torus::Torus(const char* albedo, const char* normal, const char* metallic, const char* roughness, const char* ao) {
    torusAlbedo    = loadTexture(FileSystem::getPath(albedo).c_str());
    torusNormal    = loadTexture(FileSystem::getPath(normal).c_str());
    torusMetallic  = loadTexture(FileSystem::getPath(metallic).c_str());
    torusRoughness = loadTexture(FileSystem::getPath(roughness).c_str());
    torusAo        = loadTexture(FileSystem::getPath(ao).c_str());
}

void Torus::bindTextures() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, torusAlbedo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, torusNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, torusMetallic);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, torusRoughness);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, torusAo);
}

void Torus::render(double r, double c, int rSeg, int cSeg) {
    if (torusVAO == 0) {
        glGenVertexArrays(1, &torusVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i <= rSeg; ++i) {
            for (unsigned int j = 0; j <= cSeg; ++j) {
                for (int k = 0; k < 2; k++) {
                    double s = (i + k) % rSeg + 0.5;
                    double t = j % (cSeg + 1);

                    double xPos = (c + r * std::cos(s * 2.0f * M_PI / rSeg)) * std::cos(t * 2.0f * M_PI / cSeg);
                    double yPos = (c + r * std::cos(s * 2.0f * M_PI / rSeg)) * std::sin(t * 2.0f * M_PI / cSeg);
                    double zPos = r * std::sin(s * 2.0f * M_PI / rSeg);

                    double u = (i + k) / (float) rSeg;
                    double v = t / (float) cSeg;

                    positions.emplace_back(2 * xPos, 2 * yPos, 2 * zPos);
                    normals.emplace_back(2 * xPos, 2 * yPos, 2 * zPos);
                    uv.emplace_back(u, v);
                }
            }
        }

        bool oddRow = false;
        for (int j = 0; j < cSeg; ++j) {
            if (!oddRow) {
                for (int i = 0; i <= rSeg; ++i) {
                    indices.push_back(j * (rSeg + 1) + i);
                    indices.push_back((j + 1) * (rSeg + 1) + i);
                }
            } else {
                for (int i = rSeg; i >= 0; --i) {
                    indices.push_back((j + 1) * (rSeg + 1) + i);
                    indices.push_back(j * (rSeg + 1) + i);
                }
            }
            oddRow = !oddRow;
        }
        torusIndexCount = indices.size();

        std::vector<float> data;
        for (int i = 0; i < positions.size(); ++i) {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (!normals.empty()) {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (!uv.empty()) {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(torusVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 3 + 2) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }
    glBindVertexArray(torusVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, torusIndexCount);
}