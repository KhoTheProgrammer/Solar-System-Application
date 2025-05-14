#include "Planet/Planet.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Timer/Timer.h"
using namespace std;

// Viewport dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera Settings
const float radius = 3.0f;
float camX = sin(glfwGetTime()) * radius;
float camZ = cos(glfwGetTime()) * radius;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Variables for controlling mouse functionality
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
float fov = 45.0f;

// DeltaTime variables
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Lighting Variables
glm::vec3 lightPos(0.0, 0.5, 0.0);
glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
glm::vec3 yellowColor(1.0f, 1.0f, 0.5f);
glm::vec3 blueColor(0.0f, 0.0f, 1.0f);
glm::vec3 lightDir(0.1f, 0.2f, 0.1f);

// Function prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *wimdow, Planet &moon, Planet &sun);
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System App", NULL, NULL);
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

    // Timer object for getting elapsedtime
    Timer timer;

    // Shader object
    Shader ourShader("shader.vs ", "shader.fs");

    // Define planets
    Planet sun(0.5f, 0.0f, 0.0f, 1.0f, "PlanetTextureMaps/sunmap.jpg", true);
    Planet earth(0.2f, 1.0f, 1.0f, 2.0f, "PlanetTextureMaps/earthmap1k.jpg");
    Planet moon(0.1f, 0.5f, 3.0f, 2.0f, "PlanetTextureMaps/moonmap1k.jpg");
    Planet jupiter(0.3f, 3.0f, 0.50f, 2.0f, "PlanetTextureMaps/jupitermap.jpg");
    Planet mars(0.15, 2.0, 2.0f, 2.0f, "PlanetTextureMaps/marsmap1k.jpg");
    Planet neptune(0.25, 4.0, 1.5f, 2.0f, "PlanetTextureMaps/neptunemap.jpg");

    // initialize the ParentModelMatrix of moon planet to a 4x4 identity matrix
    moon.setParentModelMatrix(glm::mat4(1.0f));

    // Earth is the parent of the moon(orbit around it)
    moon.setHasParent(true);

    ourShader.use();

    // Applying textures to planets
    ourShader.setInt("material.diffuse", 0);
    ourShader.setInt("material.specular", 1);

    while (!glfwWindowShouldClose(window))
    {
        // Process input
        processInput(window, moon, sun);

        // Clear buffers with a black color as the background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate new deltaTime
        float currentFrame = timer.getElapsedTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // View Matrix
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Projecttion Matrix
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        ourShader.use();

        // Setting projection and view uniforms to projection and view matrix respectively
        ourShader.setMat4("view", view); 
        ourShader.setMat4("projection", projection);

        // Setting default values fornviewpos and material shininess
        ourShader.setVec3("viewPos", cameraPos);
        ourShader.setFloat("material.shininess", 32.0f);

        /* For the directional and point light, the sun must not be affected, but all other planets. so first, i will set the lighting properties so that
        the sun is not affected and then resetting them so that they can now affect all other planets*/

        // Not affecting the sun
        // Directional Light
        ourShader.setVec3("dirLight.ambient", yellowColor);
        ourShader.setVec3("dirLight.diffuse", yellowColor);
        ourShader.setVec3("dirLight.specular", yellowColor);
        ourShader.setVec3("dirLight.direction", lightDir);

        // Point Light
        ourShader.setVec3("pointLight.position", lightPos);
        ourShader.setVec3("pointLight.ambient", blueColor);
        ourShader.setVec3("pointLight.diffuse", blueColor);
        ourShader.setVec3("pointLight.specular", blueColor);
        ourShader.setFloat("pointLight.constant", 1.0f);
        ourShader.setFloat("pointLight.linear", 0.09f);
        ourShader.setFloat("pointLight.quadratic", 0.032f);

        // Draw Sun
        sun.update(deltaTime);
        sun.drawPlanet(ourShader);

        // Resseting all lighting properties
        ourShader.use();

        // Affecting all other planets
        // Directional Light
        ourShader.setVec3("dirLight.ambient", yellowColor * 0.2f);
        ourShader.setVec3("dirLight.diffuse", yellowColor * 0.4f);
        ourShader.setVec3("dirLight.specular", yellowColor * 0.5f);
        ourShader.setVec3("dirLight.direction", lightDir);

        // Point Light
        ourShader.setVec3("pointLight.position", lightPos);
        ourShader.setVec3("pointLight.ambient", blueColor * 0.1f);
        ourShader.setVec3("pointLight.diffuse", blueColor * 0.8f);
        ourShader.setVec3("pointLight.specular", blueColor * 1.0f);
        ourShader.setFloat("pointLight.constant", 1.0f);
        ourShader.setFloat("pointLight.linear", 0.09f);
        ourShader.setFloat("pointLight.quadratic", 0.032f);

        // Draw Earth
        earth.update(deltaTime);
        earth.drawPlanet(ourShader);

        // Draw Moon
        moon.setParentModelMatrix(earth.getParentModelMatrix()); // Set the parentModelMatrix to the modelmartix of aerth inorder to orbit around it not the sun
        moon.update(deltaTime);
        moon.drawPlanet(ourShader);

        // Draw Jupiter
        jupiter.update(deltaTime);
        jupiter.drawPlanet(ourShader);

        // Draw Mars
        mars.update(deltaTime);
        mars.drawPlanet(ourShader);

        // Draw Neptune
        neptune.update(deltaTime);
        neptune.drawPlanet(ourShader);

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
void processInput(GLFWwindow *window, Planet &moon, Planet &sun)
{
    // Close the application when ESC is pressed
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

    // Move to the Left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) *
                     cameraSpeed;

    // Move to the Right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) *
                     cameraSpeed;

    // Decrease the orbit speed of moon
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        moon.decreaseMoonSpeed();

    // Increase the orbit speed of moon
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        moon.increaseMoonSpeed();

    // Increase the rotation speed of sun
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        sun.increaseSunSpeed();

    // Decrease the rotation speed of sun
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        sun.decreaseSunSpeed();
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

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

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
