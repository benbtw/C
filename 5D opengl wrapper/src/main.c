#include "renderer.h"

int main()
{
    // init GLFW with error handling
    rendererInit();

    // put window extra window hints here
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // creates window in modern opengl 3.3 takes care of error handling
    GLFWwindow *window = initWindow("Renderer Stuff", 960, 640);
    glfwSwapInterval(1); // vsync, will need to add deltaTime support

    escapeCanClose = true; // set if you want escape to close the window

    bRect rect = {200, 200, 64, 64, 1.0f, 0.0f, 0.0f, 1.0f};

    glClearColor(0.5, 0.5, 0.5, 1);

    float currentFrame = 0;
    float deltaTime = 0;
    float lastFrame = 0;

    while (Running(window))
    {

        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (deltaTime > 0.16)
            deltaTime = 0.16;

        clearRenderer();

        // rendering functions go here
        drawRect(rect);

        glfwSwapBuffers(window);

        rendererPollEvents(window);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            rect.x -= 5 * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            rect.x += 5 * deltaTime + 1; // + 1 to fix weird no movement?
    }

    cleanRenderer(window);
    return 0;
}