#include "renderer.h"

int main()
{
    // init GLFW with error handling
    rendererInit();

    //put window extra window hints here
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // creates window in modern opengl 3.3 takes care of error handling
    GLFWwindow *window = initWindow("Renderer Stuff", 960, 640);

    escapeCanClose = true; // set if you want escape to close the window
    bImage images[2] = {
        {"C:/Users/newmi/OneDrive/Documents/C/C/5D opengl wrapper/src/mcIcon.png", 100, 100, 128, 128},
        {"C:/Users/newmi/OneDrive/Documents/C/C/5D opengl wrapper/src/mcIcon.png", 200, 300, 128, 128}};

    glClearColor(0.5, 0.5, 0.5, 1);
    while (Running(window))
    {
        clearRenderer();

        // rendering functions go here
        drawImages(images, 2);

        glfwSwapBuffers(window);

        rendererPollEvents(window);
    }
    
    cleanRenderer(window);
    return 0;
}