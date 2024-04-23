#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdbool.h>
#include <math.h>

typedef struct bColor{
    float r, g, b, a;
} bColor;

////////////////////////////////////////
// 2D rendering shapes/objects
////////////////////////////////////////

typedef struct bRect
{
    int x, y;
    float w, h;
    float r, g, b;
    float alpha;
} bRect;

typedef struct bLine
{
    int x1, y1;
    int x2, y2;
    int width;
    float r, g, b;
    float alpha;
} bLine;

typedef struct bPoint
{
    int x, y;
    float size;
    float r, g, b;
    float alpha;
} bPoint;

typedef struct bImage
{
    const char *fileName;
    int x, y;
    int width, height;
    int sX, sY, sW, sH; // source x, y, w, h for spritesheets
    int imageW, imageH; // needed for spritesheets and in general now maybe,
                        // i should change due to the declaration being messy as fuck
                        /*
                            ie. (this displays the entire image)
                            bImage image;
                            image.fileName = "src/PlayerSheet.png";
                            image.x = 100;
                            image.y = 100;
                            image.width = 198;
                            image.height = 288;
                            image.sX = 0;
                            image.sY = 0;
                            image.sW = 198;
                            image.sH = 288;
                            image.imageW = 198;
                            image.imageH = 288;
                        */
} bImage;

typedef struct bTriangle
{
    int x, y;
    float w, h;
    float r, g, b;
    float alpha;
} bTriangle;

////////////////////////////////////////
// 2D physics stuff
////////////////////////////////////////

// Vectors
typedef struct Vector2i
{
    int x, y;
} Vector2i;

typedef struct Vector2f
{
    float x, y;
} Vector2f;

// Collisions
typedef enum
{
    NO_COLLISION,
    COLLISION_TOP,
    COLLISION_BOTTOM,
    COLLISION_LEFT,
    COLLISION_RIGHT
} CollisionSide;

CollisionSide checkCollision(const bRect *rect1, const bRect *rect2);

// Movement
void Move(int *x, int *y, int speedX, int speedY, float dt);
void jump(bRect *rect, float *gravity, int *jumpTimer, int maxTimer, int jumpSpeed, bool *isJumping, float deltaTime);

/////////////////////////////////////////
// 3D Physics stuff
////////////////////////////////////////

typedef enum cameraDirections{
    CAMERA_UP,
    CAMERA_DOWN,
    CAMERA_LEFT,
    CAMERA_RIGHT,
    CAMERA_NONE
} cameraDirections;

typedef struct Camera
{
    vec3 position;
    vec3 front;
    vec3 up;
    float horizontal;
    float vertical;
    float speed;
    float sensitivity;
} Camera;

void moveCamera(cameraDirections direction, float cameraSpeed, vec3 *cameraPos, vec3 *cameraFront, vec3 *cameraUp);
void moveCameraMouse(double xpos, double ypos, float *horizontal, float *vertical, vec3 *cameraFront);
void updateCameraPos(mat4 *view, vec3 *cameraPos, vec3 *cameraFront, vec3 *cameraUp);
void setFov(float FOV, mat4 *projection, int screenWidth, int screenHeight);

////////////////////////////////////////
// other 3D stuff
////////////////////////////////////////

typedef struct bCube
{
    float x, y, z;
    float w, h;
    float angle;
    float r, g, b, a;
} bCube;

#endif