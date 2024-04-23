#pragma once

#ifndef RENDERING_H
#define RENDERING_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "physics.h"

////////////////////////////////////////
// window handling
////////////////////////////////////////

void rendererInit();
GLFWwindow *initWindow(const char *title, int w, int h);
bool Running(GLFWwindow *window);
void cleanRenderer(GLFWwindow *window);
void clearRenderer();

// "input handling" (not full input just some helper stuff)
void rendererPollEvents(GLFWwindow *window);
extern bool escapeCanClose;

////////////////////////////////////////
// 2D rendering functions
////////////////////////////////////////

void drawTriangle(bTriangle triangle);
void drawTriangles(bTriangle *triangles, int size);

void drawRect(bRect rect);
void drawRects(bRect *rects, int size);

void drawPoint(bPoint point);
void drawPoints(bPoint *points, int size);

void drawLine(bLine line);
void drawLines(bLine *lines, int size);

void drawImage(bImage image);
void drawImages(bImage *images, int size);

////////////////////////////////////////
// 3D rendering
////////////////////////////////////////

// most likely pass in a model, projection a maybe a bCube struct?
// view might not need to be passed since its basically the camera
// however model and projection are basically essential to seeing and changing the object
// ie. void drawCube(bCube cube, mat4 model, mat4 projection); probably every shape will be like this
// do batch rendering for drawing multiple cubes (and other 3D objects) probably
void drawCube(bCube cube, mat4 model, mat4 view, mat4 projection, bColor color);
void drawCubes(bCube *cubes, int cubeSize, mat4 model, mat4 view, mat4 projection);

#endif