#include "renderer.h"
#include "physics.h"

const int scWidth = 960;
const int scHeight = 640;

Camera camera = {
    .position = {0.0f, 0.5f, 2.0f},
    .front = {0.0f, 0.0f, -1.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .horizontal = -90.0f,
    .vertical = 0.0f,
    .speed = 0.02f,
    .sensitivity = 0.05f};

void handleCameraInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

int main()
{
    // init GLFW with error handling
    rendererInit();

    // put window extra window hints here
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // creates window in modern opengl 3.3 takes care of error handling
    GLFWwindow *window = initWindow("Renderer Stuff", scWidth, scHeight);
    glfwSwapInterval(1); // vsync

    escapeCanClose = true; // set if you want escape to close the window

    glClearColor(0.5, 0.5, 0.5, 1);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // keeps mouse locked onto window alt+tab gets the cursor out

    float currentFrame = 0;
    float deltaTime = 0;
    float lastFrame = 0;

    mat4 cubeModel;
    bCube cubes[2] = {{1.0f, 2.0f, 1.0f, 64.0f, 42.0f, 20.0f, 1.0f, 0.0f, 0.0f, 1.0f}, 
                        {100.0f, 200.0f, 1.0f, 64.0f, 42.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f}};

    mat4 projection;
    mat4 view;

    while (Running(window))
    {

        rendererPollEvents(window);

        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (deltaTime > 0.16)
            deltaTime = 0.16;

        handleCameraInput(window);

        updateCameraPos(&view, &camera.position, &camera.front, &camera.up);
        setFov(60.0f, &projection, scWidth, scHeight);

        clearRenderer();

        // rendering functions go here
        drawCubes(cubes, 2, cubeModel, view, projection);

        glfwSwapBuffers(window);

        
    }

    cleanRenderer(window);
    return 0;
}

// Function to handle camera movement
void handleCameraInput(GLFWwindow *window)
{
    // Forward/backward movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveCamera(CAMERA_UP, camera.speed, &camera.position, &camera.front, &camera.up);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveCamera(CAMERA_DOWN, camera.speed, &camera.position, &camera.front, &camera.up);

    // Strafing movement
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        moveCamera(CAMERA_LEFT, camera.speed, &camera.position, &camera.front, &camera.up);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        moveCamera(CAMERA_RIGHT, camera.speed, &camera.position, &camera.front, &camera.up);
    }
}

// Function to handle mouse movement
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    moveCameraMouse(xpos, ypos, &camera.horizontal, &camera.vertical, &camera.front);
}