#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <math.h>
#include "renderer.h"

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

// Vectors
typedef struct Vector3i
{
    int x, y, z;
} Vector3i;

typedef struct Vector3f
{
    float x, y, z;
} Vector3f;

#endif