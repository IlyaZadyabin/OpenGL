#ifndef OBJECTS_H
#define OBJECTS_H

glm::vec3 cubePositions[] = {
        glm::vec3(2.0f, 1.0f, 2.0f),
        glm::vec3(4.2f, 1.6f, -1.8f),
        glm::vec3(3.5f, 1.5f, 6.0f),
        glm::vec3(1.9f, 4.5f, 5.5f)
};

glm::vec3 pointLightPositions[] = {
        glm::vec3( 1.0f,  1.0f, -4.5f),
        glm::vec3( 6.0f,  0.2f,  2.0f),
        glm::vec3( 3.3f, 6.0f, 0.0f),
        glm::vec3(4.0f, 2.0f, 9.5f),
        glm::vec3( 15.0f,  1.0f,  2.0f)
};
glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 0.6f, 1.0f),
        glm::vec3(1.0f, 1.0, 0.0),
        glm::vec3(1.0f, 0.5f, 0.5f),
        glm::vec3(0.3f, 0.3f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
};

glm::vec3 specialLightPositions[] = {
        glm::vec3(-5.0f,  5.0f, -5.0f),
        glm::vec3( 5.0f,  5.0f, 5.0f),
        glm::vec3(-5.0f, -5.0f, -5.0f),
        glm::vec3( 5.0f, -5.0f, 5.0f),
};
glm::vec3 specialLightColors[] = {
        glm::vec3(225.0f, 0.0f, 225.0f),
        glm::vec3(225.0f, 225.0f, 0.0f),
        glm::vec3(0.0f, 225.0f, 225.0f),
        glm::vec3(150.0f, 150.0f, 150.0f)
};

#endif
