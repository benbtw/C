#pragma once

#ifndef DYNAM_H
#define DYNAM_H
#include <stdlib.h>
#include <stdio.h>

// simple integer vector

typedef struct vector
{
    size_t size; // size of array/vector
    int *data; // data array 
} vector;

vector *initVector(size_t size); // init the vector
void deleteVector(vector *v); // delete vector / free the memory
size_t resizeVector(vector *v, size_t newSize); // resize vector / increase size
int getValue(vector *v, size_t n); // get value at vector[n]
void setValue(vector *v, size_t n, int x); // set value at vector[n]

#endif