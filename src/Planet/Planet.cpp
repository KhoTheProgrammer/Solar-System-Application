#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Planet.h"
using namespace std;

Planet::Planet(float radius, float orbitRadius, float orbitSpeed, float rotationSpeed, const char *texturePath, bool isSun) : sphere(radius, 64, 32, true)
{
    // Initialize variables
    this->texturePath = texturePath;
    this->radius = radius;
    this->OrbitRadius = orbitRadius;
    this->orbitSpeed = orbitSpeed;
    this->RotationSpeed = rotationSpeed;
    this->isSun = isSun;
    this->hasParent = false;
    this->OrbitAngle = 0.0f;
    this->rotationAngle = 0.0f;
    loadTexture();
}

void Planet::loadTexture()
{
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Setting texture filtering and wrapping properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(texturePath, &width, &height,
                                    &nrChannels, 0);
    if (data)
    {
        // moon texture has 4 channels while the rest have 3 channels
        GLenum format;
        if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void Planet::update(float deltaTime)
{
    // sun does not orbit, just rotate on its axis
    if (!isSun)
        OrbitAngle += orbitSpeed * deltaTime;
    rotationAngle += RotationSpeed * deltaTime;
}

void Planet::drawPlanet(Shader &shader)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setInt("ourTexture", 0);
    glm::mat4 model = glm::mat4(1.0f);

    // Moon should take the model matrix of earth inorder to orbit around it
    if (hasParent)
    {
        model = parentModelMatrix;
    }

    // Sun doesnt orbit, but stays at the center
    if (!isSun)
    {
        model = glm::translate(model, glm::vec3(sin(OrbitAngle) * OrbitRadius, 0.0f, cos(OrbitAngle) * OrbitRadius));
    }
    parentModelMatrix = model;
    // Rotate around own axis
    model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", model);
    sphere.draw(shader);
}