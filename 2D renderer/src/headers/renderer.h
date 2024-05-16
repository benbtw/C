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
void setWindowIcon(GLFWwindow *window, const char *path);

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

#endif