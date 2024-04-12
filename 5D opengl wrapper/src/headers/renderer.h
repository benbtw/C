#pragma once

#ifndef RENDERING_H
#define RENDERING_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

// window handling
void rendererInit();
GLFWwindow *initWindow(const char *title, int w, int h);
bool Running(GLFWwindow *window);
void cleanRenderer(GLFWwindow *window);
void clearRenderer();

// "input handling" (not full input just some helper stuff)
void rendererPollEvents(GLFWwindow *window);
extern bool escapeCanClose;

// 2D rendering shapes/objects

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
} bImage;

typedef struct bTriangle
{
    int x, y;
    float w, h;
    float r, g, b;
    float alpha;
} bTriangle;

// 2D rendering functions
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