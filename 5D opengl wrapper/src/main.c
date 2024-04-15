#include "renderer.h"
#include "physics.h"

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

    bRect rects[2] = {{200, 200, 64, 64, 1.0f, 0.0f, 0.0f, 1.0f},
                      {400, 200, 64, 64, 0.0f, 0.0f, 1.0f, 1.0f}};

    CollisionSide cs;

    glClearColor(0.5, 0.5, 0.5, 1);

    float currentFrame = 0;
    float deltaTime = 0;
    float lastFrame = 0;
    int speed = 300;

    float gravity = -4.0f;
    int jumpTimer = 0;
    bool isGrounded = false;
    bool isJumping = false;

    while (Running(window))
    {
        if (!isGrounded)
            rects[0].y -= gravity;

        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (deltaTime > 0.16)
            deltaTime = 0.16;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            Move(&rects[0].x, &rects[0].y, -speed, 0, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            Move(&rects[0].x, &rects[0].y, speed, 0, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            Move(&rects[0].x, &rects[0].y, 0, -speed, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            Move(&rects[0].x, &rects[0].y, 0, speed, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded)
            isJumping = true;

        // jumpSpeed = speed at which you go up, maxTimer = how long for the jump "animation"
        jump(&rects[0], &gravity, &jumpTimer, 30, 1000, &isJumping, deltaTime);

        cs = checkCollision(&rects[0], &rects[1]);

        if (rects[0].y >= 500)
        {
            isGrounded = true;
            isJumping = false;
        }
        else
            isGrounded = false;

        switch (cs)
        {
        case COLLISION_TOP:
            // Handle collision with the top side
            rects[0].y = rects[1].y + rects[1].h; // Adjust y position to stop at the bottom side
            break;
        case COLLISION_BOTTOM:
            // Handle collision with the bottom side
            rects[0].y = rects[1].y - rects[0].h; // Adjust y position to stop at the top side
            break;
        case COLLISION_LEFT:
            // Handle collision with the left side
            rects[0].x = rects[1].x + rects[1].w; // Adjust x position to stop at the right side
            break;
        case COLLISION_RIGHT:
            // Handle collision with the right side
            rects[0].x = rects[1].x - rects[0].w; // Adjust x position to stop at the left side
            break;
        case NO_COLLISION:
            // No collision
            break;
        }

        clearRenderer();

        // rendering functions go here
        drawRects(rects, 2);

        glfwSwapBuffers(window);

        rendererPollEvents(window);
    }

    cleanRenderer(window);
    return 0;
}