#pragma once
#include "../Sphere/Sphere.h"
#include "../Shader/Shader.h"
class Planet
{
private:
    float radius;
    float orbitSpeed;
    float RotationSpeed;
    float OrbitRadius;
    float OrbitAngle;
    float rotationAngle;
    Sphere sphere;
    const char *texturePath;
    unsigned int texture;
    glm::mat4 parentModelMatrix;
    bool isSun;
    bool hasParent;

public:
    Planet(float radius, float orbitRadius, float orbitSpeed, float rotationSpeed, const char *texturePath, bool isSun = false);
    void update(float deltaTime);
    void drawPlanet(Shader &shader);
    void increaseMoonSpeed() { this->orbitSpeed += 0.01f; }
    void decreaseMoonSpeed() { this->orbitSpeed -= 0.01f; }
    void increaseSunSpeed() { this->RotationSpeed += 0.01f; }
    void decreaseSunSpeed() { this->RotationSpeed -= 0.01f; }
    
    void setRadius(float radius) { this->radius = radius; }
    void setOrbitSpeed(float orbitSpeed) { this->orbitSpeed = orbitSpeed; }
    void setRotationSpeed(float RotationSpeed) { this->RotationSpeed = RotationSpeed; }
    void setAngle(float angle) { this->OrbitAngle = angle; }
    void setParentModelMatrix(const glm::mat4 &parentModel) { parentModelMatrix = parentModel; }

    void setHasParent(bool hasParent) { this->hasParent = hasParent; }

    float getRadius() const { return radius; }
    float getOrbitSpeed() const { return orbitSpeed; }
    float getRotationSpeed() const { return RotationSpeed; }
    float getangle() const { return OrbitAngle; }
    glm::mat4 getParentModelMatrix() { return parentModelMatrix; }
    unsigned int getTexture(){return texture; }

    void loadTexture();
};