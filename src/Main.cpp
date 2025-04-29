#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Sphere/Sphere.h"
using namespace std;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera Settings
const float radius = 3.0f;
float camX = sin(glfwGetTime()) * radius;
float camZ = cos(glfwGetTime()) * radius;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

float moonOrbitSpeed = 2.0f;

// DeltaTime variables
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *wimdow);
unsigned int loadTexture(const char *path);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
int main()
{
    // Initialiase glfw
    if (!glfwInit())
    {
        cout << "Failed to initialize glfw" << endl;
    };

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Opengl", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Hide the cursor and capture it
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize Glad" << endl;
        glfwTerminate();
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);
    // Set callback for window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader ourShader("shader.vs", "shader.fs");
    Sphere sun(0.4, 64, 32, true);
    Sphere earth(0.2, 64, 32, true);
    Sphere moon(0.1, 64, 32, true);
    Sphere jupiter(0.3, 64, 32, true);
    unsigned int sunTexture = loadTexture("PlanetTextureMaps/sunmap.jpg");
    unsigned int earthTexture = loadTexture("PlanetTextureMaps/earthmap1k.jpg");
    unsigned int moonTexture = loadTexture("PlanetTextureMaps/venusmap.jpg");
    unsigned int jupiterTexture = loadTexture("PlanetTextureMaps/jupitermap.jpg");

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Process input
        processInput(window);

        // Clear buffers
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate new deltaTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // View Matrix
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Projecttion Matrix
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Draw Sun
        ourShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sunTexture);
        ourShader.setInt("ourTexture", 0);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
        ourShader.setMat4("model", model);
        sun.draw(ourShader);

        // Draw Earth
        ourShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earthTexture);
        ourShader.setInt("ourTexture", 0);
        float earthRadius = 1.5f;
        float earthOrbitSpeed = 0.5f;
        float earthRotationSpeed = 2.0f;
        float earthAngle = (float)glfwGetTime() * earthOrbitSpeed;
        glm::mat4 earthmodel = glm::mat4(1.0f);
        earthmodel = glm::translate(earthmodel, glm::vec3(sin(earthAngle) * earthRadius, 0.0f, cos(earthAngle) * earthRadius));
        earthmodel = glm::rotate(earthmodel, (float)glfwGetTime() * earthRotationSpeed, glm::vec3(0.0, 1.0, 0.0));
        ourShader.setMat4("model", earthmodel);
        earth.draw(ourShader);

        // Moon Draw
        ourShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, moonTexture);
        ourShader.setInt("ourTexture", 0);
        float moonRadius = 0.5f;
        float moonAngle = (float)glfwGetTime() * moonOrbitSpeed;
        glm::mat4 moonmodel = earthmodel;
        moonmodel = glm::translate(moonmodel, glm::vec3(sin(moonAngle) * moonRadius, 0.0, cos(moonAngle) * moonRadius));
        ourShader.setMat4("model", moonmodel);
        moon.draw(ourShader);

         // Jupiter Draw
         ourShader.use();
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, jupiterTexture);
         ourShader.setInt("ourTexture", 0);
         float jupiterRadius = 2.0f;
         float jupiterRotationSpeed = 1.5f;
         float jupiterAngle = (float)glfwGetTime() * jupiterRotationSpeed;
         glm::mat4 jupiterModel = glm::mat4(1.0f);
         jupiterModel = glm::translate(jupiterModel, glm::vec3(sin(jupiterAngle) * jupiterRadius, 0.0f, cos(jupiterAngle) * jupiterRadius));
         jupiterModel = glm::rotate(jupiterModel, (float)glfwGetTime() * jupiterRotationSpeed, glm::vec3(0.0, 1.0, 0.0));
         ourShader.setMat4("model", jupiterModel);
         jupiter.draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// Callback function to adjust viewport on window resize
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Process input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // Changing camera speed
    float cameraSpeed = 2.5f * deltaTime;

    // Zoom in
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    // Zoom out
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    // Move Left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) *
                     cameraSpeed;

    // Move Right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) *
                     cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        moonOrbitSpeed -= 0.01;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        moonOrbitSpeed += 0.01;
}

unsigned int loadTexture(const char *path)
{
    // Generate Texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping/filtering options (on currently bound texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height,
                                    &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}
