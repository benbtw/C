#include "physics.h"

// Function to check for collision between two rectangles
CollisionSide checkCollision(const bRect *rect1, const bRect *rect2)
{
    // Check if rect1's right edge is to the right of rect2's left edge
    bool xOverlap = rect1->x < rect2->x + rect2->w && rect1->x + rect1->w > rect2->x;

    // Check if rect1's bottom edge is below rect2's top edge
    bool yOverlap = rect1->y < rect2->y + rect2->h && rect1->y + rect1->h > rect2->y;

    // If there is overlap along both axes, the rectangles intersect
    if (xOverlap && yOverlap)
    {
        // Calculate overlap on each side
        float overlapTop = rect2->y + rect2->h - rect1->y;
        float overlapBottom = rect1->y + rect1->h - rect2->y;
        float overlapLeft = rect2->x + rect2->w - rect1->x;
        float overlapRight = rect1->x + rect1->w - rect2->x;

        // Determine which side has the least overlap
        float minOverlap = fminf(fminf(overlapTop, overlapBottom), fminf(overlapLeft, overlapRight));

        if (minOverlap == overlapTop)
            return COLLISION_TOP;
        else if (minOverlap == overlapBottom)
            return COLLISION_BOTTOM;
        else if (minOverlap == overlapLeft)
            return COLLISION_LEFT;
        else
            return COLLISION_RIGHT;
    }

    // No collision
    return NO_COLLISION;
}

// Increase x, y with speed and using a velocity vector
void Move(int *x, int *y, int speedX, int speedY, float dt)
{
    // need to add plus one when the speed is positive due to some weird bug with deltaTime,
    // so it can match the same speed when it is negative
    if (speedX > 0)
        *x += speedX * dt + 1;
    if (speedY > 0)
        *y += speedY * dt + 1;
    if (speedX < 0 || speedY < 0)
    {
        *x += speedX * dt;
        *y += speedY * dt;
    }
}

void jump(bRect *rect, float *gravity, int *jumpTimer, int maxTimer, int jumpSpeed, bool *isJumping, float deltaTime)
{
    // Apply jumping logic
    if (*isJumping)
    {
        *gravity = 0;                     // Set gravity to 0 while jumping
        rect->y -= jumpSpeed * deltaTime; // Simulate upward movement
        (*jumpTimer)++;
        if (*jumpTimer == maxTimer)
        {
            *jumpTimer = 0;
            *gravity = -4.0f;   // Reset gravity after jump is complete
            *isJumping = false; // Reset jump flag
        }
    }
}

void moveCamera(cameraDirections direction, float cameraSpeed, vec3 *cameraPos, vec3 *cameraFront, vec3 *cameraUp)
{
    switch (direction)
    {
    case CAMERA_UP:
        glm_vec3_add(*cameraPos, (vec3){cameraFront[0][0] * cameraSpeed, cameraFront[0][1] * cameraSpeed, cameraFront[0][2] * cameraSpeed}, *cameraPos);
        break;
    case CAMERA_DOWN:
        glm_vec3_sub(*cameraPos, (vec3){cameraFront[0][0] * cameraSpeed, cameraFront[0][1] * cameraSpeed, cameraFront[0][2] * cameraSpeed}, *cameraPos);
        break;
    case CAMERA_LEFT:
        vec3 left;
        glm_vec3_cross(*cameraFront, *cameraUp, left);
        glm_normalize_to(left, left);
        glm_vec3_scale(left, cameraSpeed, left);
        glm_vec3_sub(*cameraPos, left, *cameraPos);
        break;
    case CAMERA_RIGHT:
        vec3 right;
        glm_vec3_cross(*cameraFront, *cameraUp, right);
        glm_normalize_to(right, right);
        glm_vec3_scale(right, cameraSpeed, right);
        glm_vec3_add(*cameraPos, right, *cameraPos);
        break;
    case CAMERA_NONE:
        break;
    }
}

float lastX = 960.0f / 2.0f;
float lastY = 640.0f / 2.0f;
bool firstMouse = true;

void moveCameraMouse(double xpos, double ypos, float *horizontal, float *vertical, vec3 *cameraFront)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    *horizontal += xoffset;
    *vertical += yoffset;

    // Clamp pitch to prevent the camera from flipping upside down
    if (*vertical > 89.0f)
        *vertical = 89.0f;
    if (*vertical < -89.0f)
        *vertical = -89.0f;

    // Update camera front vector based on yaw and pitch
    vec3 front;
    front[0] = cos(glm_rad(*horizontal)) * cos(glm_rad(*vertical));
    front[1] = sin(glm_rad(*vertical));
    front[2] = sin(glm_rad(*horizontal)) * cos(glm_rad(*vertical));
    glm_normalize_to(front, *cameraFront);
}

void updateCameraPos(mat4 *view, vec3 *cameraPos, vec3 *cameraFront, vec3 *cameraUp)
{
    glm_lookat(*cameraPos, (vec3){cameraPos[0][0] + cameraFront[0][0], cameraPos[0][1] + cameraFront[0][1], cameraPos[0][2] + cameraFront[0][2]}, *cameraUp, *view);
}

// Update projection matrix based on window size and FOV
void setFov(float FOV, mat4 *projection, int screenWidth, int screenHeight) {
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    glm_perspective(glm_rad(FOV), aspectRatio, 0.1f, 100.0f, *projection);
}