#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

// 2D physics stuff
// Collisions
bool doRectsCollide(bRect rect, bRect rect2);
bool rectLeftSide(bRect rect, bRect rect2);
bool rectRightSide(bRect rect, bRect rect2);
bool rectTopSide(bRect rect, bRect rect2);
bool rectBottomSide(bRect rect, bRect rect2);

// movement and jump stuff
void move(float x, float y);
void jump(float jumpHeight, float jumpSpeed);

#endif