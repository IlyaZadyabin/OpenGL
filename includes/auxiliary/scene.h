class Scene {
private:
    //shaders
    Shader lightingShader{"basic_vert.glsl", "lights_frag.glsl"};
    Shader lampShader{"basic_vert.glsl", "lamp_frag.glsl"};
    Shader shadowShader{"shadow_mapping_vert.glsl", "shadow_mapping_frag.glsl"};
    Shader shadowDepthShader{"shadow_mapping_depth_vert.glsl", "shadow_mapping_depth_frag.glsl", "shadow_mapping_depth_geom.glsl"};
    Shader parallaxShader{"parallax_mapping_vert.glsl", "parallax_mapping_frag.glsl"};

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

public:
    explicit Scene(Camera cam) : camera(cam){
        // configure depth map FBO
        glGenFramebuffers(1, &depthMapFBO);
        // create depth cubemap texture
        glGenTextures(1, &depthCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        for (unsigned i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
                         GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //shader configuration
        shadowShader.use();
        shadowShader.setInt("diffuseTexture", 0);
        shadowShader.setInt("shadowMap", 1);

        // shader configuration
        shadowShader.use();
        shadowShader.setInt("diffuseTexture", 0);
        shadowShader.setInt("depthMap", 1);

        lightingShader.use();
        lightingShader.setInt("material.diffuse", 0);
        lightingShader.setInt("material.specular", 1);
        lightingShader.setInt("material.emission", 2);

        parallaxShader.use();
        parallaxShader.setInt("diffuseMap", 0);
        parallaxShader.setInt("normalMap", 1);
        parallaxShader.setInt("depthMap", 2);

        lampShader.use();
        lampShader.setInt("lightColor", 0);

        wall = Wall(glm::vec3( 15.0f,  1.5f, 0.0f),parallaxShader,
                    "textures/wall/basecolor.jpg",
                    "textures/wall/normal.jpg",
                    "textures/wall/height.png");
    }

    void renderWithShadows(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        // create depth cubemap transformation matrices
        // only one light source used for shadow
        glm::vec3 lightPos(3.0, 1.0, sin(glfwGetTime() * 0.5) * 3.0);
        float near_plane = 1.0f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // render scene to depth cubemap
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        shadowDepthShader.use();
        for (unsigned i = 0; i < 6; ++i) {
            shadowDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        }
        shadowDepthShader.setFloat("far_plane", far_plane);
        shadowDepthShader.setVec3("lightPos", lightPos);
        renderScene(shadowDepthShader, platform.getPlatformTexture(), platform.getPlatformSpecularMap(),
                    boxDiffuseMap, boxSpecularMap, boxEmissionMap);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //render scene using the generated depth/shadow map
        glViewport(0, 0, camera.getScreenWidth() * 2, camera.getScreenHeight() * 2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shadowShader.use();
        shadowShader.setMat4("projection", projection);
        shadowShader.setMat4("view", view);
        shadowShader.setVec3("lightPos", lightPos);
        shadowShader.setVec3("viewPos", camera.getCameraPos());
        shadowShader.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, platform.getPlatformTexture());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        //render platform
        model = glm::mat4(1.0f);
        shadowShader.setMat4("model", model);
        platform.render();

        // bind cubes diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, boxDiffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        renderCubes(shadowShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void renderWithLights(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float heightScale) {
        //render scene with other lights
        glViewport(0, 0, camera.getScreenWidth() * 2, camera.getScreenHeight() * 2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setVec3("viewPos", camera.getCameraPos());
        lightingShader.setFloat("material.shininess", 64.0f);
        lightingShader.setFloat("time", glfwGetTime());
        //point lights
        for (int i = 0; i < 5; i++) {
            lightingShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            lightingShader.setVec3("pointLights["  + std::to_string(i) + "].ambient", pointLightColors[i].x * 0.1,  pointLightColors[i].y  * 0.1,  pointLightColors[i].z * 0.1);
            lightingShader.setVec3("pointLights["  + std::to_string(i) + "].diffuse", pointLightColors[i].x,  pointLightColors[i].y,  pointLightColors[i].z);
            lightingShader.setVec3("pointLights["  + std::to_string(i) + "].specular", pointLightColors[i].x,  pointLightColors[i].y,  pointLightColors[i].z);
            lightingShader.setFloat("pointLights["  + std::to_string(i) + "].constant", 1.0f);
            lightingShader.setFloat("pointLights["  + std::to_string(i) + "].linear", 0.09);
            lightingShader.setFloat("pointLights["  + std::to_string(i) + "].quadratic", 0.032);
        }
        renderScene(lightingShader, platform.getPlatformTexture(), platform.getPlatformSpecularMap(),
                    boxDiffuseMap, boxSpecularMap, boxEmissionMap);

        //render lights
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        for (int i = 0; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lampShader.setVec3("lightColor", pointLightColors[i]);
            lampShader.setMat4("model", model);
            //cube.render();
        }
        //rotating light
        model = glm::mat4(1.0f);
        float lightX = 2.5f * sin(glfwGetTime()) + 15.0f;
        float lightY = 1.5f;
        float lightZ = 2.5f * cos(glfwGetTime()) + 0.0f;
        model = glm::translate(model, glm::vec3(lightX, lightY, lightZ));
        model = glm::scale(model, glm::vec3(0.2f));
        lampShader.setVec3("lightColor", pointLightColors[4]);
        lampShader.setMat4("model", model);
        //cube.render(); // looks mysterious
        pointLightPositions[4] = glm::vec3(lightX, lightY, lightZ);

        wall.render(projection, view, camera.getCameraPos(), heightScale, pointLightPositions[4]);
    }

    void renderScene(const Shader &shader, unsigned flDiffuse, unsigned flSpecular,
                     unsigned cDiffuse, unsigned cSpecular, unsigned cEmission) {
        //bind platform diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, flDiffuse);
        // bind platform specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, flSpecular);
        //render platform
        shader.setMat4("model", glm::mat4(1.0f));
        shader.setBool("withEmission", false);
        platform.render();

        // bind cubes diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cDiffuse);
        // bind cubes specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cSpecular);
        // bind cubes emission map
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cEmission);
        renderCubes(shader);
    }
    void renderCubes(const Shader &shader) {
        for (unsigned i = 0; i < 4; i++) {
            float angle = 13.0f;
            glm::mat4 model = glm::mat4(1.0f);
            if (i == 0) {
                float lightX = 2.5f * sin(glfwGetTime()) + cubePositions[i].x;
                float lightY = cubePositions[i].y;
                float lightZ = cubePositions[i].z;
                model = glm::translate(model, glm::vec3(lightX, lightY, lightZ));
            } else {
                model = glm::translate(model, cubePositions[i]);
            }
            if (i == 0) {
                angle = 0;
            } else if (i == 1) {
                angle = (float)glfwGetTime();
            } else if (i == 2) {
                model = glm::translate(model, glm::vec3(std::sin(glfwGetTime()), 0.0, std::cos(glfwGetTime())));
                model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
                angle = 0;
            } else if (i == 3) {
                angle = (float)glfwGetTime() * 3.0;
            }
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            shader.setBool("withEmission", i == 3);
            cube.render();
        }
    }
};